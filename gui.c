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


// get the user_name, create the main_window
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

	main_app_window = create_main_window();

	gtk_widget_destroy(parent);
}

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


GtkWidget* create_main_window()
{
	GtkWidget* main_window;
	GtkWidget* hbox;
	GtkWidget* send_button;

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(main_window), "destroy",
			G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(main_window), "delete_event",
			G_CALLBACK(gtk_main_quit), main_window);
	gtk_widget_show(main_window);

	// create the whole menu
	hbox = create_menu_for_hbox();
	gtk_container_add(GTK_CONTAINER(main_window), hbox);

	gtk_widget_show_all(main_window);

	return main_window;
}


GtkWidget* create_menu_for_hbox()
{
	GtkWidget* hbox_ret;
	GtkWidget* vbox_left;
	GtkWidget* scrolled_window;

	hbox_ret = gtk_hbox_new(FALSE, 3);

	vbox_left = init_msg_part();
	scrolled_window = init_user_list_part();

	gtk_box_pack_start(GTK_BOX(hbox_ret), vbox_left, 
			TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox_ret), scrolled_window,
			TRUE, TRUE, 0);

	return hbox_ret;
}

GtkWidget* init_user_list_part()
{

}

GtkWidget* init_msg_part()
{
	extern GString* user_name;
	GtkWidget* vbox_ret;

	GtkWidget* msg_list_text;
	GtkWidget* user_name_label;
	GtkWidget* msg_line_text;
	GtkWidget* h_button_box;

	GtkWidget* scrolled_win_msg_list_text;
	GtkWidget* scrolled_win_msg_line_text;

	// the msg_list part
	scrolled_win_msg_list_text = 
		gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_msg_list_text),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win_msg_list_text)
			, msg_list_text);
	gtk_box_pack_start(GTK_BOX(vbox_ret), 
			scrolled_win_msg_list_text, TRUE, TRUE, 0);

	// the user_name label part
	user_name_label = gtk_label_new(user_name->str);
	gtk_box_pack_start(GTK_BOX(vbox_ret),
			user_name_label, TRUE, TRUE, 0);

	// the msg_line inputing part
	scrolled_win_msg_line_text = 
		gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_msg_line_text),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win_msg_line_text)
			, msg_line_text);
	gtk_box_pack_start(GTK_BOX(vbox_ret),
			scrolled_win_msg_line_text, TRUE, TRUE, 0);
	
	// the hbox_button
	h_button_box = init_command_buttons();
	gtk_box_pack_start(GTK_BOX(vbox_ret),
			h_button_box, TRUE, TRUE, 0);

	return vbox_ret;
}

inline static GtkWidget* init_command_buttons()
{
	GtkWidget* hbox_ret;
	GtkWidget* send_button;
	GtkWidget* close_button;

	hbox_ret = gtk_hbox_new(FALSE, 3);

	send_button = gtk_button_new_with_label("send");
	gtk_box_pack_start(GTK_BOX(hbox_ret), send_button, 
			TRUE, TRUE, 0);
	// TODO: add the signal functiom to connect with the bg
	
	close_button = gtk_button_new_with_label("close");
	gtk_box_pack_start(GTK_BOX(hbox_ret), close_button,
			TRUE, TRUE, 0);
	// TODO: add the singal function to close all the port and file

	return hbox_ret;
}

