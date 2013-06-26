#ifndef _USER_INCLUDED
#define _USER_INCLUDED

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "message.h"

#define SERV_PORT		29000

struct user_info_t {
	int sender_socket;
	//int receiver_socket;// used to send/receive file, NOTE
};

// initialize the user's sender and receiver
struct user_info_t* create_user_info();

// broadcast the msg
int send_msg(struct user_info_t* user, struct msg_pack_t* msg);

// receive msg
int recv_msg(struct user_info_t* user, struct msg_pack_t* msg,
		struct sockaddr_in* sender_addr);

// clear all data
void clear_user_info(struct user_info_t* user);

#endif
