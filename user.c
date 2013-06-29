#include "user.h"
#include <arpa/inet.h>

static inline void p_error(char* err)
{
	perror(err);
	exit(1);
}

struct user_info_t* create_user_info()
{
	struct user_info_t* user;
	struct sockaddr_in server_addr;
	const int on = 1;
	int s;


	extern pthread_t spyon_id;

	pthread_attr_t attr;

	user = malloc(sizeof(*user));

	user->sender_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (user->sender_socket < 0)
		p_error("sender socket");

	s = setsockopt(user->sender_socket, SOL_SOCKET, SO_BROADCAST, &on, sizeof(int));
	if (s < 0)
		p_error("setsockopt for sender");

	s = setsockopt(user->sender_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	if (s < 0)
		p_error("setsockopt for sender");


	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERV_PORT);
	if (bind(user->sender_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		p_error("bind");
	// sender done, UDP

	user->file_downloader = socket(AF_INET, SOCK_STREAM, 0);
	if (user->file_downloader < 0)
		p_error("file downloader socket");

	

	user->file_sharer = socket(AF_INET, SOCK_STREAM, 0);
	if (user->file_sharer < 0)
		p_error("file_sharer socket");

	// get host ip
	char host[256];
	struct hostent* pp;
	struct in_addr addr;

	gethostname(host, sizeof(host));
	pp = gethostbyname(host);
	int i = 0;
	while (pp->h_addr_list[i] != NULL) {
		addr.s_addr = *(unsigned int*)(pp->h_addr_list[i]);
		++i;
	}
// DEBUG
	g_print("debug by user.c%s\n", inet_ntoa(addr));

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr = addr;
	server_addr.sin_port = htons(SERV_PORT);
	if (bind(user->file_sharer, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		p_error("bind");

	if (listen(user->file_sharer, 20) < 0)
		p_error("listen");

	s = pthread_attr_init(&attr);
	if (s < 0)
		p_error("pthread_attr_init");

	/*
	s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (s < 0)
		p_error("pthread_attr_setdetachstate");
	*/

	s = pthread_create(&spyon_id, &attr, spyon, &user->file_sharer);
	if (s != 0)
		p_error("pthread_create");

	return user;
}


int send_msg(struct user_info_t* user, struct msg_pack_t* msg)
{
	struct sockaddr_in server_addr;
	socklen_t dest_len = sizeof(server_addr);
	int s;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	server_addr.sin_port = htons(SERV_PORT);

	s = sendto(user->sender_socket, msg, sizeof(*msg), 0, 
			(struct sockaddr*)&server_addr, dest_len);

	if (s < 0)
		perror("sendto");
	return s;
}

int recv_msg(struct user_info_t* user, struct msg_pack_t* msg,
		struct sockaddr_in* sender_addr)
{
	int s;
	socklen_t len;

	len = sizeof(struct sockaddr_in);
	s = recvfrom(user->sender_socket, msg, sizeof(*msg),
			0, (struct sockaddr*)sender_addr, &len);

	if (s < 0)
		perror("recvfrom");
	return s;
}

void clear_user_info(struct user_info_t* user)
{
	close(user->sender_socket);
	free(user);
}

/*
 *	file transfering
 */
void* spyon(void* sharer)
{
	struct sockaddr_in dst_addr;
	socklen_t socklen = sizeof(dst_addr);
	int file_sharer, s;
	int client_fd;
	ssize_t data_len;
	int choice;

	//pthread_attr_t attr;

	file_sharer = *(int*)sharer;

	client_fd = accept(file_sharer, (struct sockaddr*)&dst_addr, &socklen);
	if (client_fd < 0)
		p_error("accept");

	//pthread_attr_init(&attr);
	//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	//s = pthread_create(&recv_id, &attr, spyon, sharer);
	//if (s < 0)
	//	p_error("pthread_create");


	data_len = recv(file_sharer, &choice, sizeof(choice), 0);
	if (data_len <= 0)
		p_error("recv");

	char name[1024];
	int file_name_len;
	switch (choice) {
	case SEND_FILE_LIST:
		send_file_list(file_sharer);
		break;
	case SEND_FILE:
		file_name_len = recv(file_sharer, name, sizeof(name), 0);
		send_file(file_sharer, name);
		break;
	default:
		break;
	}

	return NULL;
}


/*
 *	traverse the 'share' directory and send the files' name and size
 */
void send_file_list(int file_sharer)
{
	struct file_info_t file_info;

	traverse_file("share", file_sharer, send_file_info);

	file_info.file_size = -1;// the end of sending
	send_file_info(file_sharer, &file_info);
}

void traverse_file(char* path, int file_sharer, void (*send_file_info)(int, struct file_info_t*))
{
	DIR* dp;
	struct dirent* entry;
	struct stat statbuf;
	struct file_info_t file_info;

	if ((dp = opendir(path)) == NULL) {
		fprintf(stderr, "can't open direnry %s\n", path);
		return;
	}

	chdir(path);
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			//traverse_file(entry->d_name, file_sharer, send_file_info);// not support sub-directory
		} else {
			file_info.file_size = statbuf.st_size;
			strcpy(file_info.file_name, entry->d_name);
			send_file_info(file_sharer, &file_info);
		}
	}
	
	chdir("..");
	closedir(dp);
}

void send_file_info(int file_sharer, struct file_info_t* file_info)
{
	ssize_t s;

	s = send(file_sharer, file_info, sizeof(*file_info), 0);
	if (s < 0)
		perror("send");

}

void send_file(int file_sharer, char* file_name)
{
	FILE* fp;
	struct file_pack_t file_part;
	struct stat statbuf;
	unsigned char buf[MAX_FILE_PART_LENGTH];
	int i = 0;
	ssize_t read_len;

	chdir("share");
	fp = fopen(file_name, "r");
	if (fp == NULL)
		return;

	lstat(file_name, &statbuf);// get the property of this file

	unsigned int time = statbuf.st_size / MAX_FILE_PART_LENGTH;
	unsigned int last_file_part_size = statbuf.st_size % MAX_FILE_PART_LENGTH;

	while ((read_len = fread(buf, sizeof(buf), 1, fp)) == sizeof(buf)) {
		file_part.order = i++;
		file_part.length = sizeof(buf);
		memcpy(file_part.file_content, buf, sizeof(buf));
		send_file_part(file_sharer, &file_part);
	}
	// the remaining part
	if (last_file_part_size > 0) {
		file_part.order = i++;
		file_part.length = read_len;
		memcpy(file_part.file_content, buf, read_len);
		send_file_part(file_sharer, &file_part);
	}

	// send the end pack
	file_part.order = file_part.length = -1;// end sign
	memset(file_part.file_content, 0, MAX_FILE_PART_LENGTH);
	send_file_part(file_sharer, &file_part);

	chdir("..");
}

void send_file_part(int file_sharer, struct file_pack_t* file_part)
{
	ssize_t s;

	s = send(file_sharer, file_part, sizeof(*file_part), 0);
	if (s < 0)
		perror("send");
}


/*
 *	
 */
//void get_file_info(
		

