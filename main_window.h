#ifndef _MAIN_WINDOW_INCLUDED
#define _MAIN_WINDOW_INCLUDED

#include <gtk/gtk.h>
#include "message.h"
#include "common_list.h"


/*
 *	for GtkListStore* store, tree-view
 */
enum {
	USER_NAME_COLUMN = 0,
	IP_COLUMN,
	NBR_COLUMNS
};

/*
 *	for GtkListStore* file_list_store
 */
enum {
	FILE_NAME_COLUMN = 0,
	FILE_SIZE_COLUMN,
	FILE_NBR_COLUMNS
};

typedef struct main_app_window_t {
	list_t* history_list;
	list_node_t* current_node;
	//GtkWidget* prev_button;
	//GtkWidget* next_button;

	GtkWidget* main_window;
	GtkWidget* scrolled_window;
	GtkWidget* hbox_of_vbox_tree_view;

	// the left partition
	GtkWidget* vbox_of_left;

	GtkWidget* msg_list_text_view;
	GtkWidget* name_label;
	GtkWidget* msg_line_text_view;
	GtkWidget* hbox_of_button;
	GtkWidget* send_button;
	GtkWidget* close_button;
	// face
	GtkWidget* face_button;
	GtkWidget* face_window;

	// the right partiton
	GtkWidget* user_list_tree_view;
	GtkListStore* user_list_store;

	// files 
	GtkWidget* file_window;
	GtkWidget* file_list_tree_view;
	GtkListStore* file_list_store;
} main_app_window_t;

void create_main_app_window(main_app_window_t* main_app_window);

void create_main_window(main_app_window_t* main_app_window);
void init_hbox_for_main_window(main_app_window_t* main_app_window);
void init_vbox_of_left(main_app_window_t* main_app_window);
void init_user_list_tree_view(main_app_window_t* main_app_window);

void display_msg(struct msg_pack_t msg, main_app_window_t* main_app_window);

// name and IP
void add_user(struct msg_pack_t msg, main_app_window_t* main_app_window);
void delete_user(const gchar* dst_name, main_app_window_t* main_app_window);

void send_msg_for_send_button(GtkWidget* widget, GtkWidget* text_view);

gboolean safe_quit(GtkWidget* widget, gpointer data);

// the history record function
void init_history(main_app_window_t* main_app_window);
void load_history_from_file(list_t* list);
void write_history_to_file(list_t* list);
// 2 hot-keys
void prev_history(GtkAccelGroup* accel_group, GObject* arg1, guint arg2, GdkModifierType arg3, gpointer user_data);
void next_history(GtkAccelGroup* accel_group, GObject* arg1, guint arg2, GdkModifierType arg3, gpointer user_data);


// face 
void create_face_window(main_app_window_t* main_app_window);
void show_or_close_face_window(GtkWidget* widget, main_app_window_t* main_app_window);
void hide_face_window(GtkWidget* widget, GdkEvent* event, gpointer data);
GtkWidget* create_image(gchar* filename, int left, int top, GtkWidget* container);
void add_code_to_msg_line(GtkWidget* widget, GdkEvent* event, gchar* filename);

// file window
void hide_file_window(GtkWidget* widget, GdkEvent* event, gpointer data);

// file transgering
void show_user_share(GtkWidget* treeview, GtkTreePath* path, GtkTreeViewColumn* col, gpointer data);
void download_file_info_list(int file_downloader, main_app_window_t* main_app_window);// NOTE
void download_file(GtkWidget* widget, main_app_window_t* main_app_window);

#endif
