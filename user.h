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

#include <sys/select.h>

#include <dirent.h>
#include <sys/stat.h>

#include <netdb.h>

#include "file_pack.h"
#include "message.h"

#define SERV_PORT		29000

enum { SEND_FILE_LIST, SEND_FILE };// for file transfering

struct user_info_t {
	int sender_socket;

	// for file's sharing and downloading
	int file_sharer;// server
	int file_downloader;// client
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


/*
 *	file transfer
 */

// check whether there is a msg
void* spyon(void* file_sharer);

void send_file_list(int file_sharer);
void traverse_file(char* path, int file_sharer, void (*send_file_info)(int, struct file_info_t*));
void send_file_info(int file_sharer, struct file_info_t* file_info);

void send_file(int file_sharer, char* file_name);
void send_file_part(int file_sharer, struct file_pack_t* file_part);

// NOTE
void get_file_info();
void get_file();

#endif
