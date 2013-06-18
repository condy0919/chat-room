#ifndef _MESSAGE_INCLUDED
#define _MESSAGE_INCLUDED

#include <sys/socket.h>
#include <unistd.h>

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
	union {
		struct sockarr_in address;
		char msg_content[MSG_MAX_LENGTH];
	} content;
};


#endif
