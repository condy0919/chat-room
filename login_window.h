#ifndef _LOGIN_WINDOW_INCLUDED
#define _LOGIN_WINDOW_INCLUDED

#include <gtk/gtk.h>

typedef struct login_window_t {
	GtkWidget* login_dialog;
	GtkWidget* name_label;
	GtkWidget* name_entry;
	GtkWidget* login_button;
} login_window_t;


void create_login_window(login_window_t* login_window);

void create_login_dialog(login_window_t* login_window);
void create_name_label(login_window_t* login_window);
void create_name_entry(login_window_t* login_window);
void create_login_button(login_window_t* login_window);

// signal for login button click
void login(GtkWidget* widget, gpointer data);

#endif
