#include "gui.h"
#include "message.h"
#include "signal_cb.h"
#include <stdlib.h>
#include <gdk/gdk.h>


GtkWidget* create_main_window()
{
	extern GString* user_name;
	extern struct user_info_t* user;

	GtkWidget* main_window;
	GtkWidget* hbox;
	struct msg_pack_t msg;

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(main_window), 
			600, 450);
	gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
	g_signal_connect(G_OBJECT(main_window), "destroy",
			G_CALLBACK(safe_quit), NULL);
	g_signal_connect(G_OBJECT(main_window), "delete_event",
			G_CALLBACK(safe_quit), main_window);
	gtk_widget_show(main_window);

	// create the whole menu
	hbox = gtk_hbox_new(FALSE, 4);
	gtk_container_add(GTK_CONTAINER(main_window), hbox);
	create_menu_for_hbox(hbox);

	gtk_widget_show_all(main_window);

	return main_window;
}



void create_menu_for_hbox(GtkWidget* hbox)
{
	GtkWidget* vbox_left;
	GtkWidget* scrolled_window;

	vbox_left = gtk_vbox_new(FALSE, 4);
	gtk_box_pack_start(GTK_BOX(hbox), vbox_left, 
			FALSE, FALSE, 0);
	init_msg_part(vbox_left);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), scrolled_window,
			FALSE, FALSE, 0);
	init_user_list_part(scrolled_window);

}


void init_user_list_part(GtkWidget* scrolled_window)
{
	GtkWidget* tree_view;

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	tree_view = gtk_tree_view_new();
	gtk_widget_set_size_request(tree_view, 150, 450);// Note
	gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
}


void init_msg_part(GtkWidget* vbox_left)
{
	extern GString* user_name;

	GtkWidget* msg_list_text;
	GtkWidget* user_name_label;
	GtkWidget* msg_line_text;
	GtkWidget* h_button_box;

	GtkWidget* scrolled_win_for_msg_list;
	GtkWidget* scrolled_win_for_msg_line;

	// init the text view
	msg_list_text = gtk_text_view_new();
	msg_line_text = gtk_text_view_new();

	// the msg_list part
	scrolled_win_for_msg_list = 
		gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_win_for_msg_list, 450, 250);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_for_msg_list),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win_for_msg_list)
			, msg_list_text);
	gtk_box_pack_start(GTK_BOX(vbox_left), 
			scrolled_win_for_msg_list, FALSE, FALSE, 0);

	// the user_name label part
	user_name_label = gtk_label_new(user_name->str);// this may not work in Gtk+3.0
	gtk_widget_set_size_request(user_name_label, 450, 25);
	gtk_box_pack_start(GTK_BOX(vbox_left),
			user_name_label, FALSE, FALSE, 0);

	// the msg_line inputing part
	scrolled_win_for_msg_line = 
		gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_win_for_msg_line, 450, 140);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_for_msg_line),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win_for_msg_line)
			, msg_line_text);
	gtk_box_pack_start(GTK_BOX(vbox_left),
			scrolled_win_for_msg_line, FALSE, FALSE, 0);
	
	// the hbox_button, and the msg_line_text for the signal callback function
	h_button_box = init_command_buttons(msg_line_text);
	gtk_box_pack_start(GTK_BOX(vbox_left), h_button_box, TRUE, FALSE, 0);
}


GtkWidget* init_command_buttons(GtkWidget* text_view)
{
	GtkWidget* hbox_ret;
	GtkWidget* send_button;
	GtkWidget* close_button;

	hbox_ret = gtk_hbox_new(FALSE, 3);

	send_button = gtk_button_new_with_label("send");
	gtk_box_pack_start(GTK_BOX(hbox_ret), send_button, 
			TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(send_button), "clicked",
			G_CALLBACK(broadcast_message), text_view);
	
	close_button = gtk_button_new_with_label("close");
	gtk_box_pack_start(GTK_BOX(hbox_ret), close_button,
			TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(close_button), "clicked",
			G_CALLBACK(safe_quit), NULL);

	return hbox_ret;
}


