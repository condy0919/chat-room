#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>
#include "gui.h"
#include "user.h"


GString* user_name;
GtkWidget* main_app_window;
struct user_info_t* user;


// the main window's widgets
GtkWidget* msg_list;
GtkWidget* tree_view;
GtkListStore* store;

// the accel group
GtkAccelGroup* accel_group;

// the thread info
pthread_t id;


int main(int argc, char* argv[])
{
	GtkWidget* login_window;

	gdk_threads_init();
	gtk_init(&argc, &argv);

	login_window = create_login_window();

	/*
	if (!g_thread_supported())
		g_thread_init(NULL);
	*/
	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	return 0;
}
