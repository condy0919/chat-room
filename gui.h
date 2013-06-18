#ifndef _GUI_INCLUDED
#define _GUI_INCLUDED

#include <gtk/gtk.h>
#include <glib.h>

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


inline static GtkWidget* init_command_buttons();

/*
 *	SIGNAL callback
 */

//	the button clicked signal callback function
void pre_login(GtkWidget* widget, gpointer object);

/*
 *	EVENT callback
 */

#endif

