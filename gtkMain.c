#include <gtk/gtk.h>
#include <glib.h>
#include "gui.h"
#include "user.h"


GString* user_name;
GtkWidget* main_app_window;
struct user_info_t* user;

int main(int argc, char* argv[])
{
	GtkWidget* login_window;

	gtk_init(&argc, &argv);

	login_window = create_login_window();


	gtk_main();

	return 0;
}
