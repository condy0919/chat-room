#ifndef _GUI_INCLUDED
#define _GUI_INCLUDED

#include <gtk/gtk.h>
#include <glib.h>
#include "message.h"

GtkWidget* create_login_window();
GtkWidget* create_main_window();

//	the basic for login_window
GtkWidget* create_login_dialog();

//  the menu of main_app_window
void create_menu_for_hbox(GtkWidget* hbox);

//	init the left part of main_app_window about vbox
void init_msg_part(GtkWidget* vbox_left);

//	init the right part of main_app_window about scrolled_win
void init_user_list_part(GtkWidget* scrolled_window);


GtkWidget* init_command_buttons(GtkWidget* widget);

//	print the msg in the msg_list window
void display_msg(struct msg_pack_t msg, GtkWidget* text_view);

// add user to the store
void add_user(char* name);

// delete the user from the user_list
void delete_user(const gchar* name);
#endif

