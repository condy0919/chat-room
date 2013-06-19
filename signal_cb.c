#include "signal_cb.h"
#include "gui.h"


// check, get the user_name, create the main_window,
// and broadcast the LOGIN msg
void pre_login(GtkWidget* widget, gpointer object)
{
	extern GString* user_name;
	extern GtkWidget* main_app_window;
	extern struct user_info_t* user;

	GtkWidget* parent = ((GtkWidget* *)object)[0];
	GtkWidget* entry = ((GtkWidget* *)object)[1];
	GtkWidget* message_dialog;
	struct msg_pack_t msg;
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

	// create user and send broadcast msg
	user = create_user_info();

	strcpy(msg.user_name, user_name->str);
	msg.msg_type = LOGIN;
	msg.flag = 0;
	send_msg(user, &msg);
	
// DEBUG
	print(&msg);

	gtk_widget_destroy(parent);
}


void broadcast_message(GtkWidget* widget, GtkWidget* text_view)
{
	extern struct user_info_t* user;
	extern GString* user_name;

	GtkTextBuffer* buffer;
	GtkTextIter start, end;
	struct msg_pack_t msg;
	gint char_count;
	gchar* text;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	char_count = gtk_text_buffer_get_char_count(GTK_TEXT_BUFFER(buffer));

	if (char_count < 1)
		return;

	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);

	strcpy(msg.user_name, user_name->str);
	strcpy(msg.msg, text);
	msg.msg_type = CHAT;
	msg.flag = 0;

	send_msg(user, &msg);
// DEBUG
	print(&msg);

	buffer = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), buffer);
}


void safe_quit(GtkWidget* widget, gpointer data)
{
	extern struct user_info_t* user;
	extern GString* user_name;

	struct msg_pack_t msg;

	strcpy(msg.user_name, user_name->str);
	msg.msg_type = QUIT;
	msg.flag = 0;

	send_msg(user, &msg);
// DEBUG
	print(&msg);

	if (user)
		clear_user_info(user);
	
	gtk_main_quit();
}


