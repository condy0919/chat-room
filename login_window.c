#include <netdb.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include "login_window.h"
#include "user.h"
#include "message.h"
#include "main_window.h"
#include "recv_thread.h"


// check, get the user_name, create the main_window,
// and broadcast the LOGIN msg, start thread about recv_message
void login(GtkWidget* widget, gpointer object)
{
	extern GString* user_name;
	extern main_app_window_t main_app_window;
	extern struct user_info_t* user;

	login_window_t* login_window = (login_window_t*)object;
	GtkWidget* entry = login_window->name_entry;
	GtkWidget* parent = login_window->login_dialog;

	GtkWidget* message_dialog;
	struct msg_pack_t msg;
	const gchar* name;
	gint len;

	len = gtk_entry_get_text_length(GTK_ENTRY(entry));
	if (len == 0) {
		message_dialog = gtk_message_dialog_new(GTK_WINDOW(parent),	GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
				"please input username");
		gtk_window_set_title(GTK_WINDOW(message_dialog), "Error");
		gtk_dialog_run(GTK_DIALOG(message_dialog));
		gtk_widget_destroy(message_dialog);

		return;
	}

	name = gtk_entry_get_text(GTK_ENTRY(entry));
	user_name = g_string_new(name);

	// it connect the login with main_app_window
	create_main_app_window(&main_app_window);

	// create user and send broadcast msg
	user = create_user_info();

	// get the user's info
	memset(&msg.content, 0, sizeof(msg.content));
	strcpy(msg.user_name, user_name->str);
	strcpy(msg.content.msg, user_name->str);
	strcat(msg.content.msg, " login");
	msg.msg_type = LOGIN;
	msg.flag = 0;

	// get IP address
	char host[256];
	struct hostent* pp;

	gethostname(host, sizeof(host));
	pp = gethostbyname(host);
	int i = 0;
	while (pp->h_addr_list[i] != NULL) {
		struct in_addr in;

		in.s_addr = *(unsigned int*)(pp->h_addr_list[i]);
		msg.content.address.sin_addr = in;
		char* hostaddress = inet_ntoa(in);
		g_print("%s\n", hostaddress);

		++i;
	}
	// add the userself to user_list
	add_user(msg, &main_app_window);
	display_msg(msg, &main_app_window);


	start_recv_message_thread(&main_app_window);

	msg.content.address.sin_family = AF_INET;
	msg.content.address.sin_port = htons(SERV_PORT);
	send_msg(user, &msg);// local info 

	// the initialization of history
	init_history(&main_app_window);
	load_history_from_file(main_app_window.history_list);

// DEBUG
	print(&msg);

	gtk_widget_destroy(login_window->login_dialog);
}



// the base of login_window
void create_login_dialog(login_window_t* login_window)
{
	login_window->login_dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(login_window->login_dialog), "login");
	//gtk_window_set_default_size(GTK_WINDOW(login_dialog),
	//		150, 120);
	g_signal_connect(G_OBJECT(login_window->login_dialog), "destroy", G_CALLBACK(gtk_widget_destroy), login_window->login_dialog);
	g_signal_connect(G_OBJECT(login_window->login_dialog), "delete_event",
			G_CALLBACK(gtk_widget_destroy), login_window->login_dialog);
	gtk_window_set_resizable(GTK_WINDOW(login_window->login_dialog), FALSE);

	gtk_widget_show(login_window->login_dialog);
}


void create_login_window(login_window_t* login_window)
{
	create_login_dialog(login_window);

	create_name_label(login_window);

	create_name_entry(login_window);

	create_login_button(login_window);
}


void create_name_label(login_window_t* login_window)
{
	login_window->name_label = gtk_label_new("username");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(login_window->login_dialog)->vbox),
			login_window->name_label, FALSE, TRUE, 5);
	gtk_widget_show(login_window->name_label);
}


void create_name_entry(login_window_t* login_window)
{
	login_window->name_entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(login_window->login_dialog)->vbox),
			login_window->name_entry, FALSE, TRUE, 5);
	gtk_widget_set_can_focus(login_window->name_entry, TRUE);
	gtk_widget_grab_focus(login_window->name_entry);
	gtk_widget_show(login_window->name_entry);
}


void create_login_button(login_window_t* login_window)
{
	login_window->login_button = gtk_button_new_with_label("login");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(login_window->login_dialog)->action_area), login_window->login_button, FALSE, TRUE, 10);
	g_signal_connect(G_OBJECT(login_window->login_button), "clicked", G_CALLBACK(login), login_window);//NOTE
	gtk_widget_show(login_window->login_button);
}

