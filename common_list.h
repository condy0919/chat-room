#ifndef _COMMON_LIST_INCLUDED
#define _COMMON_LIST_INCLUDED

#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct list_node_t {
	void* data;
	struct list_node_t* next;
	struct list_node_t* prev;
} list_node_t;

typedef struct list_t {
	int item_size;
	int list_size;
	list_node_t head;
} list_t;


list_t* create_list(int _item_size);

void insert_node(list_t* list, void* data);

list_node_t* find_node(list_t* list, void* data, int (*cmp)(const void*, const void*));

void remove_node(list_t* list, list_node_t* node);

void destroy_list(list_t* list);


#endif
