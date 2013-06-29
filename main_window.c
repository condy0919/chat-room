#include <gdk/gdkkeysyms.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <arpa/inet.h>


#include "user.h"
#include "main_window.h"

gboolean safe_quit(GtkWidget* widget, gpointer data)
{
	extern struct user_info_t* user;
	extern GString* user_name;
	extern pthread_t id;
	extern pthread_t spyon_id;
	extern main_app_window_t main_app_window;

	struct msg_pack_t msg;

	strcpy(msg.user_name, user_name->str);
	msg.msg_type = QUIT;
	msg.flag = 0;

	send_msg(user, &msg);
// DEBUG
	print(&msg);

	pthread_cancel(id);
	pthread_cancel(spyon_id);

	// save the history 
	if (main_app_window.history_list->list_size > 0)
		write_history_to_file(main_app_window.history_list);

	if (user)
		clear_user_info(user);
	destroy_list(main_app_window.history_list);
	
	gtk_main_quit();

	return FALSE;
}


void send_msg_for_send_button(GtkWidget* widget, GtkWidget* text_view)
{
	extern struct user_info_t* user;
	extern GString* user_name;
	extern main_app_window_t main_app_window;

	GtkTextBuffer* buffer;
	GtkTextIter start, end;
	struct msg_pack_t msg;
	gint char_count;
	gchar* text;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	char_count = gtk_text_buffer_get_char_count(GTK_TEXT_BUFFER(buffer));

	if (char_count < 1 || char_count > MSG_MAX_LENGTH - 1)
		return;

	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);

	// insert the msg into history_list
	insert_node(main_app_window.history_list, text);
	main_app_window.current_node = main_app_window.history_list->head.next;// reset the current to the first node

	strcpy(msg.user_name, user_name->str);
	strcpy(msg.content.msg, text);
	msg.msg_type = CHAT;
	msg.flag = 0;

	send_msg(user, &msg);
	
// DEBUG
	print(&msg);

	// clear the input msg_line
	buffer = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_view), buffer);

	gtk_widget_grab_focus(text_view);
}


void create_main_app_window(main_app_window_t* main_app_window)
{
	create_main_window(main_app_window);

	init_hbox_for_main_window(main_app_window);

	gtk_widget_show_all(main_app_window->main_window);
	
	// for the face window, it should be hiding
	gtk_widget_hide(main_app_window->face_window);
	gtk_widget_hide(main_app_window->file_window);
}


void create_main_window(main_app_window_t* main_app_window)
{
	main_app_window->main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(main_app_window->main_window), "chat room");
	gtk_window_set_position(GTK_WINDOW(main_app_window->main_window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(main_app_window->main_window), 600, 450);
	gtk_window_set_resizable(GTK_WINDOW(main_app_window->main_window), FALSE);
	//g_signal_connect(G_OBJECT(main_app_window->main_window), "destroy", G_CALLBACK(safe_quit), NULL);
	g_signal_connect(G_OBJECT(main_app_window->main_window), "delete_event", G_CALLBACK(safe_quit), main_app_window->main_window);
	gtk_widget_show(main_app_window->main_window);
}


void init_hbox_for_main_window(main_app_window_t* main_app_window)
{
	main_app_window->hbox_of_vbox_tree_view = gtk_hbox_new(FALSE, 3);
	gtk_container_add(GTK_CONTAINER(main_app_window->main_window), main_app_window->hbox_of_vbox_tree_view);

	main_app_window->vbox_of_left = gtk_vbox_new(FALSE, 4);
	gtk_box_pack_start(GTK_BOX(main_app_window->hbox_of_vbox_tree_view), main_app_window->vbox_of_left, FALSE, FALSE, 0);

	// for the user-list-tree-view
	main_app_window->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(main_app_window->scrolled_window), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(main_app_window->hbox_of_vbox_tree_view), main_app_window->scrolled_window, FALSE, FALSE, 0);


	init_vbox_of_left(main_app_window);
	init_user_list_tree_view(main_app_window);
}


void init_vbox_of_left(main_app_window_t* main_app_window)
{
	extern GString* user_name;

	GtkAccelGroup* accel_group;
	GtkWidget* scrolled_win_for_msg_list;
	GtkWidget* scrolled_win_for_msg_line;
	GtkWidget* hbox_of_label_button;


	// Init the text view
	main_app_window->msg_list_text_view = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(main_app_window->msg_list_text_view), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(main_app_window->msg_list_text_view), FALSE);

	main_app_window->msg_line_text_view = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(main_app_window->msg_line_text_view), GTK_WRAP_WORD_CHAR);
	gtk_widget_set_can_focus(main_app_window->msg_line_text_view, TRUE);
	gtk_widget_grab_focus(main_app_window->msg_line_text_view);


	// the msg_list part
	scrolled_win_for_msg_list = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_win_for_msg_list, 450, 250);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_for_msg_list), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win_for_msg_list), main_app_window->msg_list_text_view);
	gtk_box_pack_start(GTK_BOX(main_app_window->vbox_of_left), scrolled_win_for_msg_list, FALSE, FALSE, 0);

	// the user_name label and 2 buttons about history skimming 
	hbox_of_label_button = gtk_hbox_new(FALSE, 4);
	main_app_window->name_label = gtk_label_new(user_name->str);
	gtk_box_pack_start(GTK_BOX(hbox_of_label_button), main_app_window->name_label, FALSE, FALSE, 0);

	// the prev button, add the hot-key Control-P to show the prev msg
	/*
	main_app_window->prev_button = gtk_button_new_with_label("prev");
	gtk_box_pack_start(GTK_BOX(hbox_of_label_button), main_app_window->prev_button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(main_app_window->prev_button), "clicked", G_CALLBACK(prev_history), main_app_window);

	accel_group = gtk_accel_group_new();
	gtk_widget_add_accelerator(main_app_window->prev_button, "clicked", 
			accel_group, GDK_KEY_P, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_window_add_accel_group(GTK_WINDOW(main_app_window->main_window), accel_group);
	*/
	accel_group = gtk_accel_group_new();
	GClosure* closure = g_cclosure_new(G_CALLBACK(prev_history), main_app_window, NULL);
	gtk_accel_group_connect(accel_group, GDK_KEY_P, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, closure);
	gtk_window_add_accel_group(GTK_WINDOW(main_app_window->main_window), accel_group);


	// the next button, add the hot-key Control-N to show the next msg
	/*
	main_app_window->next_button = gtk_button_new_with_label("next");
	gtk_box_pack_start(GTK_BOX(hbox_of_label_button), main_app_window->next_button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(main_app_window->next_button), "clicked", G_CALLBACK(next_history), main_app_window);

	accel_group = gtk_accel_group_new();
	gtk_widget_add_accelerator(main_app_window->next_button, "clicked",
			accel_group, GDK_KEY_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_window_add_accel_group(GTK_WINDOW(main_app_window->main_window), accel_group);
	*/

	accel_group = gtk_accel_group_new();
	closure = g_cclosure_new(G_CALLBACK(next_history), main_app_window, NULL);
	gtk_accel_group_connect(accel_group, GDK_KEY_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, closure);
	gtk_window_add_accel_group(GTK_WINDOW(main_app_window->main_window), accel_group);

	// the face window
	create_face_window(main_app_window);
	main_app_window->face_button = gtk_button_new_with_label("face");
	g_signal_connect(G_OBJECT(main_app_window->face_button), "clicked", 
			G_CALLBACK(show_or_close_face_window), main_app_window);
	gtk_box_pack_start(GTK_BOX(hbox_of_label_button), main_app_window->face_button, TRUE, TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(main_app_window->vbox_of_left), hbox_of_label_button, FALSE, FALSE, 0);
	

	// the msg_line inputing part
	scrolled_win_for_msg_line = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled_win_for_msg_line, 450, 140);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win_for_msg_line), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_win_for_msg_line), main_app_window->msg_line_text_view);
	gtk_box_pack_start(GTK_BOX(main_app_window->vbox_of_left), scrolled_win_for_msg_line, FALSE, FALSE, 0);
	
	// the hbox contains send/close buttons
	main_app_window->hbox_of_button = gtk_hbox_new(FALSE, 4);
	gtk_box_pack_start(GTK_BOX(main_app_window->vbox_of_left), main_app_window->hbox_of_button, TRUE, FALSE, 0);

	// the send/close button
	main_app_window->send_button = gtk_button_new_with_label("send");
	gtk_box_pack_start(GTK_BOX(main_app_window->hbox_of_button), main_app_window->send_button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(main_app_window->send_button), "clicked", G_CALLBACK(send_msg_for_send_button), 
			main_app_window->msg_line_text_view);
	// add the hot-key, Control-Enter to send msg
	accel_group = gtk_accel_group_new();
	gtk_widget_add_accelerator(main_app_window->send_button, "clicked",
			accel_group, GDK_KEY_Return, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_window_add_accel_group(GTK_WINDOW(main_app_window->main_window), accel_group);
	
	main_app_window->close_button = gtk_button_new_with_label("close");
	gtk_box_pack_start(GTK_BOX(main_app_window->hbox_of_button), main_app_window->close_button, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(main_app_window->close_button), "clicked", G_CALLBACK(safe_quit), NULL);
}

void show_or_close_face_window(GtkWidget* widget, main_app_window_t* main_app_window)
{
	if (gtk_widget_get_visible(main_app_window->face_window))
		gtk_widget_hide(main_app_window->face_window);
	else
		gtk_widget_show(main_app_window->face_window);
}

void create_face_window(main_app_window_t* main_app_window)
{
	GtkWidget* table;
	GtkWidget* pic;
	int i, j;
	gchar* filename;
	int count = 0;

	main_app_window->face_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(main_app_window->face_window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(main_app_window->face_window), 4);

	g_signal_connect(G_OBJECT(main_app_window->face_window), "delete_event", G_CALLBACK(hide_face_window), NULL);
	table = gtk_table_new(5, 6, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(table), 4);
	gtk_table_set_row_spacings(GTK_TABLE(table), 2);
	gtk_table_set_col_spacings(GTK_TABLE(table), 2);
	gtk_container_add(GTK_CONTAINER(main_app_window->face_window), table);

	for (i = 0; i < 5; ++i)
		for (j = 0; j < 6; ++j) {
			filename = g_strdup_printf("./rc/%d%d.gif", i, j);
			pic = create_image(filename, j, i, table);
			//g_free(filename);
		}
	gtk_widget_show_all(main_app_window->face_window);
}


void hide_face_window(GtkWidget* widget, GdkEvent* event, gpointer data)
{
	gtk_widget_hide(widget);
}


GtkWidget* create_image(gchar* filename, int left, int top, GtkWidget* container)
{
	GtkWidget* image;
	GtkWidget* frame;

	image = gtk_image_new_from_file(filename);
	frame = gtk_frame_new(NULL);

	GtkWidget* ebox = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(frame), image);
	gtk_container_add(GTK_CONTAINER(ebox), frame);

	gtk_widget_set_events(ebox, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(G_OBJECT(ebox), "button_press_event", G_CALLBACK(add_code_to_msg_line), filename);
	gtk_table_attach_defaults(GTK_TABLE(container), ebox, left, left + 1, top, top + 1);

	return image;
}

void add_code_to_msg_line(GtkWidget* widget, GdkEvent* event, gchar* filename)
{
	extern main_app_window_t main_app_window;
	GtkWidget* msg_line;
	GtkTextBuffer* buffer;
	GtkTextIter end;
	gchar str[3] = { filename[5], filename[6], '\0' };

	msg_line = main_app_window.msg_line_text_view;
	
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(msg_line));
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, "/", 1);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &end, str, strlen(str));


	gtk_text_view_set_buffer(GTK_TEXT_VIEW(msg_line), GTK_TEXT_BUFFER(buffer));
}


void init_user_list_tree_view(main_app_window_t* main_app_window)
{
	GtkTreeViewColumn* column;
	GtkCellRenderer* renderer;

	main_app_window->user_list_store = gtk_list_store_new(NBR_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);

	// create the tree view of user list
	main_app_window->user_list_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(main_app_window->user_list_store));
	gtk_widget_set_size_request(main_app_window->user_list_tree_view, 150, 450);
	gtk_container_add(GTK_CONTAINER(main_app_window->scrolled_window), main_app_window->user_list_tree_view);
	g_signal_connect(main_app_window->user_list_tree_view, "row-activated", G_CALLBACK(show_user_share), main_app_window);

	// create file window
	main_app_window->file_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	main_app_window->file_list_store = gtk_list_store_new(FILE_NBR_COLUMNS, G_TYPE_STRING, G_TYPE_INT);
	GtkWidget* scrolled_window_for_file_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window_for_file_window), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(main_app_window->file_window), scrolled_window_for_file_window);
	g_signal_connect(G_OBJECT(main_app_window->file_window), "delete_event", G_CALLBACK(hide_file_window), NULL); 
			

	// create the file-list tree view
	main_app_window->file_list_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(main_app_window->file_list_store));
	gtk_container_add(GTK_CONTAINER(scrolled_window_for_file_window), main_app_window->file_list_tree_view);
	g_signal_connect(G_OBJECT(main_app_window->file_list_tree_view), "row-activated", G_CALLBACK(download_file), main_app_window);


	// add name column
	renderer = gtk_cell_renderer_text_new();
	g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);

	// the column of name;
	column = gtk_tree_view_column_new_with_attributes("name", renderer, "text", USER_NAME_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(main_app_window->user_list_tree_view), column);

	// the column of IP
	column = gtk_tree_view_column_new_with_attributes("IP", renderer, "text", IP_COLUMN, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(main_app_window->user_list_tree_view), column);
}

// event function for hiding the file_window
void hide_file_window(GtkWidget* widget, GdkEvent* event, gpointer data)
{
	gtk_widget_hide(widget);
}

void download_file(GtkWidget* widget, main_app_window_t* main_app_window)
{

}

void show_user_share(GtkWidget* treeview, GtkTreePath* path, GtkTreeViewColumn* col, gpointer data)
{
	extern struct user_info_t* user;

	GtkTreeModel* model;
	GtkTreeIter iter;
	main_app_window_t* main_app_window = data;
	int s;

// DEBUG
	g_print("a row has been double-clicked\n");
	gtk_widget_show(main_app_window->file_window);

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	if (gtk_tree_model_get_iter(model, &iter, path)) {
		gchar* name;
		gchar* ip;
		gtk_tree_model_get(model, &iter, USER_NAME_COLUMN, &name, -1);
		gtk_tree_model_get(model, &iter, IP_COLUMN, &ip, -1);

		// connect downloader with sharer
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERV_PORT);
		server_addr.sin_addr.s_addr = inet_addr(ip);
		s = connect(user->file_downloader, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if (s < 0)
			perror("connect");

		download_file_info_list(user->file_downloader, main_app_window);

		g_free(name);
		g_free(ip);
	}
}

// download the file info list, and show these in the file list window
void download_file_info_list(int file_downloader, main_app_window_t* main_app_window)
{
	// send the ask for sending file info list
	int choice = 0;
	int s;
	struct file_info_t file_info;

	// for showing
	GtkTreeIter iter;
	GtkListStore* store = main_app_window->file_list_store;


	// ask for file list
	choice = SEND_FILE_LIST;
	s = send(file_downloader, &choice, sizeof(choice), 0);
	if (s < 0)
		perror("download file info list send");

	// receive the file info (file name, size)
	while (1) {
		s = recv(file_downloader, &file_info, sizeof(file_info), 0);
		if (s < 0)
			perror("download file info list recv");

		if (file_info.file_size == -1)
			break;
		// show file info in the file window's treeview
		char* file_name;
		int file_size;

		file_name = file_info.file_name;
		file_size = file_info.file_size;

		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, FILE_NAME_COLUMN, file_name, FILE_SIZE_COLUMN, file_size, -1);// NOTE
	}
}


void display_msg(struct msg_pack_t msg, main_app_window_t* main_app_window)
{
	GtkTextMark* text_mark_log;// for automatic scrolling
	GtkTextBuffer* buffer;
	GtkWidget* msg_list;
	GtkTextIter end, start;
	const gchar* str;
	int i, front;

	// show the time
	time_t cur_time;

	// display name
	str = msg.user_name;
	char* br1 = "[";
	char* br2 = "]";
	char* colon_tab = ":\t";

	msg_list = main_app_window->msg_list_text_view;
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(msg_list));
	gtk_text_buffer_get_end_iter(buffer, &end);

	// log the text's position
	text_mark_log = gtk_text_buffer_create_mark(buffer, NULL, &end, 1);

	gtk_text_buffer_insert(buffer, &end, br1, strlen(br1));
	gtk_text_buffer_insert(buffer, &end, str, strlen(str));
	gtk_text_buffer_insert(buffer, &end, br2, strlen(br2));
	gtk_text_buffer_insert(buffer, &end, colon_tab, strlen(colon_tab));
	cur_time = time(0);
	gtk_text_buffer_insert(buffer, &end, ctime(&cur_time), strlen(ctime(&cur_time)));

	// display msg content, parse the msg 
	str = msg.content.msg;
	char* endl = "\n";

	i = front = 0;
	while (i < strlen(str)) {
		if (str[i] == '/' && i + 1 < strlen(str) && isdigit(str[i + 1]) && i + 2 < strlen(str) && isdigit(str[i + 2])) {
			// insert the front characters
			gtk_text_buffer_insert(buffer, &end, &str[front], i - front);

			gchar* filename = g_strdup_printf("./rc/%c%c.gif", str[i + 1], str[i + 2]);
			GdkPixbuf* pic;

			pic = gdk_pixbuf_new_from_file(filename, NULL);
			if (pic != NULL) {
				gtk_text_buffer_insert_pixbuf(buffer, &end, pic);
			} else {
				++i;
				continue;
			}

			i += 3;
			front = i;
		} else {
			//gtk_text_buffer_insert(buffer, &end, &str[i], 1);
			++i;
		}
	}
	// insert the remaining characters
	gtk_text_buffer_insert(buffer, &end, &str[front], i - front);

	//gtk_text_buffer_insert(buffer, &end, str, strlen(str));
	gtk_text_buffer_insert(buffer, &end, endl, strlen(endl));
	gtk_text_buffer_insert(buffer, &end, endl, strlen(endl));

	gtk_text_view_set_buffer(GTK_TEXT_VIEW(msg_list), buffer);

	// scrolle the text_view_interact
	gtk_text_buffer_move_mark(buffer, text_mark_log, &end);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(msg_list), text_mark_log, 0, 1, 1, 1);
}


void add_user(struct msg_pack_t msg, main_app_window_t* main_app_window)
{
	GtkTreeIter iter;
	GtkListStore* store = main_app_window->user_list_store;
	char* name;
	char* ip;

	name =  msg.user_name;
	ip = inet_ntoa(msg.content.address.sin_addr);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, USER_NAME_COLUMN, name, IP_COLUMN, ip, -1);// NOTE
}


void delete_user(const gchar* dst_name, main_app_window_t* main_app_window)
{
	GtkListStore* store;

	GtkTreeIter iter;
	gboolean vaild;

	store = main_app_window->user_list_store;
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

// the history function
void init_history(main_app_window_t* main_app_window)
{
	main_app_window->history_list = create_list(MSG_MAX_LENGTH);
	main_app_window->history_list->head.data = "";
	main_app_window->current_node = main_app_window->history_list->head.next;
}

// load history from the specified file
void load_history_from_file(list_t* list)
{
	FILE* in;
	char buf[MSG_MAX_LENGTH];

	in = fopen("chatroom.history", "r");
	if (in == NULL)
		return;

	while (fscanf(in, "%s", buf) > 0) {
		insert_node(list, buf);
	}
}

// wirte history to specified file
void write_history_to_file(list_t* list)
{
	FILE* out;
	list_node_t* p;
	
	out = fopen("chatroom.history", "w");
	if (out == NULL)
		return;

	p = list->head.next;
	while (p != &list->head) {
		fprintf(out, "%s\n", p->data);
		p = p->next;
	}
}

// move the current node to the prev
void prev_history(GtkAccelGroup* accel_group, GObject* arg1, guint arg2, GdkModifierType arg3, gpointer user_data)
{
	GtkTextBuffer* buffer;
	GtkWidget* msg_list;
	GtkTextIter end/*, start*/;
	char* str;
	main_app_window_t* main_app_window = user_data;

	msg_list = main_app_window->msg_line_text_view;
	//buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(msg_list));
	buffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_get_end_iter(buffer, &end);
	//gtk_text_buffer_get_start_iter(buffer, &start);
	//gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &start, &end);

	str = main_app_window->current_node->data;// get the string

	gtk_text_buffer_insert(buffer, &end, str, strlen(str));
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(msg_list), buffer);
	main_app_window->current_node = main_app_window->current_node->next;
}

// move the current node to the next
void next_history(GtkAccelGroup* accel_group, GObject* arg1, guint arg2, GdkModifierType arg3, gpointer user_data)
{
	GtkTextBuffer* buffer;
	GtkWidget* msg_list;
	GtkTextIter iter;
	char* str;
	main_app_window_t* main_app_window = user_data;

	msg_list = main_app_window->msg_line_text_view;
	buffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_get_end_iter(buffer, &iter);

	str = main_app_window->current_node->data;// get the string

	gtk_text_buffer_insert(buffer, &iter, str, strlen(str));
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(msg_list), buffer);
	main_app_window->current_node = main_app_window->current_node->prev;
}


