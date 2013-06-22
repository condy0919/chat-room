#include "recv_thread.h"
#include "gui.h"
#include <arpa/inet.h>
#include <netdb.h>

int start_recv_message_thread()
{
	extern pthread_t id;
	pthread_attr_t attr;
	int s;

	pthread_attr_init(&attr);
	s = pthread_create(&id, &attr, recv_message, NULL);

	return s;
}

static inline int has_same_ip(struct msg_pack_t msg)
{
	char host[256];
	struct hostent* pp;

	gethostname(host, sizeof(host));
	pp = gethostbyname(host);
	int i = 0;
	struct in_addr in;
	while (pp->h_addr_list[i] != NULL) {
		in.s_addr = *(unsigned int*)(pp->h_addr_list[i]);
		++i;
	}
	if (msg.content.address.sin_addr.s_addr == in.s_addr)
		return 1;
	return 0;
}

void* recv_message(void* arg)
{
	extern struct user_info_t* user;
	extern GtkWidget* msg_list;
	extern GString* user_name;
	//extern GtkListStore* store;

	struct msg_pack_t msg;
	struct sockaddr_in dst_address;
	//int socklen = sizeof(dst_address);

	while (1) {
		recv_msg(user, &msg, &dst_address);

		switch (msg.msg_type) {
		case LOGIN:// add the user && return the self info
			// same IP 
			if (has_same_ip(msg))
				break;
			add_user(msg.user_name);

			if (msg.flag & 1)
				break;

			dst_address = msg.content.address;

			strcpy(msg.content.msg, msg.user_name);
			strcat(msg.content.msg, " login");

			gdk_threads_enter();
			display_msg(msg, msg_list);
			gdk_threads_leave();

			
			strcpy(msg.user_name, user_name->str);
			msg.flag |= 1;
			int s = sendto(user->sender_socket, &msg, sizeof(msg), 0, 
					(struct sockaddr*)&dst_address, sizeof(dst_address));// NOTICE
			if (s < 0)
				perror("send to");
			break;
		case QUIT:// remove the user
			strcpy(msg.content.msg, msg.user_name);
			strcat(msg.content.msg, " quit");

			gdk_threads_enter();
			display_msg(msg, msg_list);
			delete_user(msg.user_name);
			gdk_threads_leave();
			break;
		case CHAT:// show msg in the msg_list
			gdk_threads_enter();
			display_msg(msg, msg_list);
			gdk_threads_leave();
			break;
		}
	}
}


