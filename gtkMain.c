#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>

#include "login_window.h"
#include "main_window.h"
#include "user.h"


GString* user_name;
main_app_window_t main_app_window;
struct user_info_t* user;

// the thread info
pthread_t id;// recv_message thread
pthread_t spyon_id;


int main(int argc, char* argv[])
{
	login_window_t login_window;

	gdk_threads_init();
	gtk_init(&argc, &argv);

	create_login_window(&login_window);

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	return 0;
}

