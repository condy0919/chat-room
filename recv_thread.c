#include "recv_thread.h"
#include "gui.h"

int start_recv_message_thread()
{
	pthread_t id;
	pthread_attr_t attr;
	int s;

	pthread_attr_init(&attr);
	s = pthread_create(&id, &attr, (void* (*)(void*))recv_message, NULL);

	return s;
}

void* recv_message(void* arg)
{
	extern struct user_info_t* user;
	extern GtkWidget* msg_list;
	extern GString* user_name;
	extern GtkListStore* store;

	struct msg_pack_t msg;
	struct sockaddr_in dst_address;
	//int socklen = sizeof(dst_address);

	while (1) {
		ssize_t len = recvfrom(user->receiver_socket, &msg,
				sizeof(msg), 0, NULL, NULL);

		if (len < 0) {
			g_print("receive error. it will continue...\n");
			continue;
		} 

		switch (msg.msg_type) {
		case LOGIN:// add the user && return the self info
			add_user(msg.user_name);

			if (msg.flag == 1)
				break;
			msg.flag = 1;
			strcpy(msg.content.msg, msg.user_name);
			strcat(msg.content.msg, "login");

			gdk_threads_enter();
			display_msg(msg, msg_list);
			gdk_threads_leave();

			strcpy(msg.user_name, user_name->str);
			dst_address = msg.content.address;
			sendto(user->sender_socket, &msg, sizeof(msg), 0, 
					(struct sockaddr*)&dst_address, sizeof(dst_address));// NOTICE
			break;
		case QUIT:// remove the user
			gdk_threads_enter();
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


