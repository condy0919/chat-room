#include "recv_thread.h"
#include "main_window.h"
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>

int start_recv_message_thread(main_app_window_t* main_app_window)
{
	extern pthread_t id;
	pthread_attr_t attr;
	int s;

	pthread_attr_init(&attr);
	s = pthread_create(&id, &attr, recv_message, main_app_window);

	if (s != 0)
		perror("pthread_create");

	return s;
}

static inline void get_ip_address(struct in_addr* addr)
{
	char host[256];
	struct hostent* pp;

	gethostname(host, sizeof(host));
	pp = gethostbyname(host);
	int i = 0;
	while (pp->h_addr_list[i] != NULL) {
		addr->s_addr = *(unsigned int*)(pp->h_addr_list[i]);
		++i;
	}
}

static inline int has_same_ip(struct msg_pack_t msg)
{
	struct in_addr in;

	get_ip_address(&in);
	if (msg.content.address.sin_addr.s_addr == in.s_addr)
		return 1;
	return 0;
}

void* recv_message(void* arg)
{
	extern struct user_info_t* user;
	extern GString* user_name;

	GtkWidget* msg_list = ((main_app_window_t*)arg)->msg_list_text_view;
	struct msg_pack_t msg;
	struct sockaddr_in dst_address;

	while (1) {
		recv_msg(user, &msg, &dst_address);

		switch (msg.msg_type) {
		case LOGIN:// add the user && return the self info
			// same IP 
			if (has_same_ip(msg))
				break;
			add_user(msg, (main_app_window_t*)arg);

			// flag means the msg has been received by another user
			if (msg.flag & 1)
				break;

			//dst_address = msg.content.address;

			strcpy(msg.content.msg, msg.user_name);
			strcat(msg.content.msg, " login");

			gdk_threads_enter();
			display_msg(msg, (main_app_window_t*)arg);
			gdk_threads_leave();

			// get local ip
			struct in_addr in;
			get_ip_address(&in);
			msg.content.address.sin_addr = in;

			strcpy(msg.user_name, user_name->str);
			msg.flag |= 1;
			int s = sendto(user->sender_socket, &msg, sizeof(msg), 0, 
					(struct sockaddr*)&dst_address, sizeof(dst_address));// NOTICE
			if (s < 0)
				perror("sendto");
			break;
		case QUIT:// remove the user
			strcpy(msg.content.msg, msg.user_name);
			strcat(msg.content.msg, " quit");

			gdk_threads_enter();
			display_msg(msg, (main_app_window_t*)arg);
			delete_user(msg.user_name, (main_app_window_t*)arg);
			gdk_threads_leave();
			break;
		case CHAT:// show msg in the msg_list
			gdk_threads_enter();
			display_msg(msg, (main_app_window_t*)arg);
			gdk_threads_leave();
			break;
		}
	}
}


