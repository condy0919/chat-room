#include "gui.h"
#include "message.h"
#include "signal_cb.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

/*
 *	for GtkListStore* store
 */
enum {
	USER_NAME_COLUMN = 0,
	//IP_COLUMN,
	NBR_COLUMNS
};


GtkWidget* create_main_window()
{
	//extern GString* user_name;
	//extern struct user_info_t* user;
	extern GtkAccelGroup* accel_group;

	GtkWidget* main_window;
	GtkWidget* hbox;
	//struct msg_pack_t msg;

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(main_window), 
			600, 450);
	gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
	g_signal_connect(G_OBJECT(main_window), "destroy",
			G_CALLBACK(safe_quit), NULL);
	//g_signal_connect(G_OBJECT(main_window), "delete_event",
	//		G_CALLBACK(safe_quit), main_window);
	gtk_widget_show(main_window);


	// create the accelerator group
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(main_window), accel_group);


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
	extern GtkWidget* tree_view;
	extern GtkListStore* store;

	GtkTreeViewColumn* column;
	GtkCellRenderer* renderer;


	store = gtk_list_store_new(NBR_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);

	/* TEST 
	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
			USER_NAME_COLUMN, "abc",
			IP_COLUMN, "192.168.1.1",
			-1);
	*/

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_widget_set_size_request(tree_view, 150, 450);
	gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

	// add name column
	renderer = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);
	column = gtk_tree_view_column_new_with_attributes("name",
			renderer, "text", USER_NAME_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	/*
	column = gtk_tree_view_column_new_with_attributes("ip",
			renderer, "text", IP_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
	*/
}


void init_msg_part(GtkWidget* vbox_left)
{
	extern GString* user_name;
	extern GtkWidget* msg_list;

	GtkWidget* user_name_label;
	GtkWidget* msg_line_text;
	GtkWidget* h_button_box;

	GtkWidget* scrolled_win_for_msg_list;
	GtkWidget* scrolled_win_for_msg_line;

	// init the text view
	msg_list = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(msg_list), FALSE);

	msg_line_text = gtk_text_view_new();
	gtk_widget_set_can_focus(msg_line_text, TRUE);
	gtk_widget_grab_focus(msg_line_text);


	// the msg_list part
	scrolled_win_for_msg_list = 
		gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_win_for_msg_list, 450, 250);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_for_msg_list),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win_for_msg_list)
			, msg_list);
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
	extern GtkAccelGroup* accel_group;

	GtkWidget* hbox_ret;
	GtkWidget* send_button;
	GtkWidget* close_button;

	hbox_ret = gtk_hbox_new(FALSE, 3);

	send_button = gtk_button_new_with_label("send");
	gtk_box_pack_start(GTK_BOX(hbox_ret), send_button, 
			TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(send_button), "clicked",
			G_CALLBACK(send_msg_for_send_button), text_view);
	// add the hot-key
	gtk_widget_add_accelerator(send_button, "clicked", accel_group, 
			GDK_KEY_Return, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	
	close_button = gtk_button_new_with_label("close");
	gtk_box_pack_start(GTK_BOX(hbox_ret), close_button,
			TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(close_button), "clicked",
			G_CALLBACK(safe_quit), NULL);

	return hbox_ret;
}


void display_msg(struct msg_pack_t msg, GtkWidget* text_view)
{
	extern GtkWidget* msg_list;

	GtkTextMark* text_mark_log;// for automatic scrolling
	GtkTextBuffer* buffer;
	GtkTextIter iter;
	const gchar* str;

	// display name
	str = msg.user_name;
	char* br1 = "[";
	char* br2 = "]";
	char* colon_tab = ":\t";

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	gtk_text_buffer_get_end_iter(buffer, &iter);

	text_mark_log = gtk_text_buffer_create_mark(buffer, NULL, &iter, 1);

	gtk_text_buffer_insert(buffer, &iter, br1, strlen(br1));
	gtk_text_buffer_insert(buffer, &iter, str, strlen(str));
	gtk_text_buffer_insert(buffer, &iter, br2, strlen(br2));
	gtk_text_buffer_insert(buffer, &iter, colon_tab, strlen(colon_tab));

	// display msg content
	str = msg.content.msg;
	char* endl = "\n";

	gtk_text_buffer_insert(buffer, &iter, str, strlen(str));
	gtk_text_buffer_insert(buffer, &iter, endl, strlen(endl));
	gtk_text_buffer_insert(buffer, &iter, endl, strlen(endl));

	// scrolle the text_view_interact
	gtk_text_buffer_move_mark(buffer, text_mark_log, &iter);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(msg_list), text_mark_log, 0, 1, 1, 1);
}

// to user-list tree-view
void add_user(char* name)
{
	extern GtkListStore* store;

	GtkTreeIter iter;

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, USER_NAME_COLUMN, name, -1);
}


// from user-list tree-view
void delete_user(const gchar* dst_name)
{
	extern GtkListStore* store;

	GtkTreeIter iter;
	gboolean vaild;

	vaild = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
	while (vaild) {
		gchar* name;
		
		gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, USER_NAME_COLUMN, &name, -1);
		if (strcmp(dst_name, name) == 0) {
			gtk_list_store_remove(GTK_LIST_STORE(store), &iter);
			break;
		}
		vaild = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
	}
}


// arrow up to see the iterate the former command

