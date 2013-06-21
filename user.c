#include "user.h"
#include <arpa/inet.h>

struct user_info_t* create_user_info()
{
	struct user_info_t* user;
	struct sockaddr_in server_addr;
	const int on = 1;
	int s;

	user = malloc(sizeof(*user));

	user->sender_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (user->sender_socket < 0) {
		perror("sender socket");
		exit(1);
	}
	s = setsockopt(user->sender_socket, SOL_SOCKET,
			SO_BROADCAST, &on, sizeof(int));
	if (s < 0) {
		perror("setsockopt for sender");
		exit(1);
	}


	user->receiver_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (user->receiver_socket < 0) {
		perror("receiver socket");
		exit(1);
	}
	s = setsockopt(user->receiver_socket, SOL_SOCKET,
			SO_REUSEADDR, &on, sizeof(int));
	if (s < 0) {
		perror("setsockopt for receiver");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERV_PORT);

	if (bind(user->receiver_socket, (struct sockaddr*)&server_addr,
				sizeof(server_addr)) < 0) {
		perror("bind");
		exit(1);
	}

	return user;
}


int send_msg(struct user_info_t* user, struct msg_pack_t* msg)
{
	struct sockaddr_in server_addr;
	socklen_t dest_len = sizeof(server_addr);
	int s;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "255.255.255.255", &server_addr.sin_addr);// NOTE: 
	//server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
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
	s = recvfrom(user->receiver_socket, msg, sizeof(*msg),
			0, (struct sockaddr*)sender_addr, &len);

	if (s < 0)
		perror("recvfrom");
	return s;
}

void clear_user_info(struct user_info_t* user)
{
	close(user->receiver_socket);
	close(user->sender_socket);
	free(user);
}

