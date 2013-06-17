#include <gtk/gtk.h>

void check_button_callback(GtkWidget* widget, gpointer data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		g_print("oh, you pressed check button.\n");
	} else {
		; // do nothing
	}
}

int main(int argc, char* argv[])
{
	GtkWidget* window;
	GtkWidget* check_button;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy",
			G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "delete_event",
			G_CALLBACK(gtk_main_quit), window);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	check_button =  gtk_check_button_new_with_label("first check button");
	gtk_container_add(GTK_CONTAINER(window), check_button);
	g_signal_connect(G_OBJECT(check_button), "toggled",
			G_CALLBACK(check_button_callback), NULL);

	gtk_widget_show(check_button);
	gtk_widget_show(window);

	gtk_main();

	return 0;
}
