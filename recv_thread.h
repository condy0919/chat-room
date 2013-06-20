#ifndef _RECV_THREAD_INCLUDED
#define _RECV_THREAD_INCLUDED

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "user.h"
#include "message.h"

// front of thread
int start_recv_message_thread();

// bg
void* recv_message(void* arg);

#endif
