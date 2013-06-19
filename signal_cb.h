#ifndef _SIGNAL_CB_INCLUDED
#define _SIGNAL_CB_INCLUDED

#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "user.h"
#include "message.h"

// create the main_window, and send the LOGIN msg to all 
void pre_login(GtkWidget* widget, gpointer data);

// send_button's callback function.
void broadcast_message(GtkWidget* widget, GtkWidget* entry);

// close_button's callback function. also can be used to quit program
void safe_quit(GtkWidget* widget, gpointer data);
#endif
