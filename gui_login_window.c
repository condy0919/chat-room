#include "gui.h"
#include <gdk/gdk.h>


GtkWidget* create_login_window()
{
	GtkWidget* login_dialog;
	GtkWidget* login_button;
	GtkWidget* label;
	GtkWidget* entry;
	static GtkWidget* object[3];// transfer the login_dialog and entry

	login_dialog = create_login_dialog();

	label = gtk_label_new("username");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(login_dialog)->vbox),
			label, FALSE, TRUE, 5);
	gtk_widget_show(label);

	entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(login_dialog)->vbox),
			entry, FALSE, TRUE, 5);
	gtk_widget_set_can_focus(entry, TRUE);
	gtk_widget_grab_focus(entry);
	gtk_widget_show(entry);


	login_button = gtk_button_new_with_label("login");
	gtk_box_pack_start(
			GTK_BOX(GTK_DIALOG(login_dialog)->action_area),
			login_button, FALSE, TRUE, 10);
	// set the value for signal
	object[0] = login_dialog;
	object[1] = entry;
	object[2] = login_button;
	g_signal_connect(G_OBJECT(login_button), "clicked",
			G_CALLBACK(pre_login), object);
	gtk_widget_show(login_button);

	return login_dialog;
}

// the base of login_window
GtkWidget* create_login_dialog()
{
	GtkWidget* login_dialog;

	login_dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(login_dialog), "login");
	//gtk_window_set_default_size(GTK_WINDOW(login_dialog),
	//		150, 120);
	g_signal_connect(G_OBJECT(login_dialog), "destroy",
			G_CALLBACK(gtk_widget_destroy), login_dialog);
	g_signal_connect(G_OBJECT(login_dialog), "delete_event",
			G_CALLBACK(gtk_widget_destroy), login_dialog);
	gtk_window_set_resizable(GTK_WINDOW(login_dialog), FALSE);
	gtk_widget_show(login_dialog);

	return login_dialog;
}

// check, get the user_name, create the main_window
void pre_login(GtkWidget* widget, gpointer object)
{
	extern GString* user_name;
	extern GtkWidget* main_app_window;
	
	GtkWidget* parent = ((GtkWidget* *)object)[0];
	GtkWidget* entry = ((GtkWidget* *)object)[1];
	GtkWidget* message_dialog;
	const gchar* name;
	gint len;

	len = gtk_entry_get_text_length(GTK_ENTRY(entry));
	if (len == 0) {
		message_dialog = gtk_message_dialog_new(
				GTK_WINDOW(parent),	GTK_DIALOG_MODAL, 
				GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
				"please input username");
		gtk_window_set_title(GTK_WINDOW(message_dialog), "Error");
		gtk_dialog_run(GTK_DIALOG(message_dialog));
		gtk_widget_destroy(message_dialog);

		return;
	}

	name = gtk_entry_get_text(GTK_ENTRY(entry));
	user_name = g_string_new(name);

	// it connect the login with main_app_window
	main_app_window = create_main_window();

	gtk_widget_destroy(parent);
}

