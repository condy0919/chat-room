#include <gtk/gtk.h>
#include <glib.h>
#include "gui.h"

GString* user_name;
GtkWidget* main_app_window;

int main(int argc, char* argv[])
{
	GtkWidget* login_window;

	gtk_init(&argc, &argv);

	login_window = create_login_window();


	gtk_main();

	return 0;
}
