#ifndef _MESSAGE_INCLUDED
#define _MESSAGE_INCLUDED

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
// DEBUG
#include <gtk/gtk.h>

#define USER_NAME_LENGTH	40
#define MSG_MAX_LENGTH		512

enum { LOGIN, QUIT, CHAT };


struct msg_pack_t {
	char user_name[USER_NAME_LENGTH];
	unsigned char msg_type;// LOGIN, QUIT or CHAT
	unsigned char flag;
	/*
	 *	sender:0	------->	receiver:0
	 *							|
	 *	sender:1,OK	<-------	| set it to 1
	 */
	struct {
		struct sockaddr_in address;
		char msg[MSG_MAX_LENGTH];
	} content;
};

inline static void print(struct msg_pack_t* msg)
{
	g_print("%d ", msg->msg_type);
	if (msg->msg_type == LOGIN)
		g_print("%s logined\n", msg->user_name);
	else if (msg->msg_type == QUIT)
		g_print("%s quited\n", msg->user_name);
	else if (msg->msg_type == CHAT)
		g_print("%s\n", msg->content.msg);
	else
		g_print("unknown type\n");
}

#endif
