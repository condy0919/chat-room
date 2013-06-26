#include "common_list.h"

list_t* create_list(int _item_size)
{
	list_t* list = malloc(sizeof(list_t));

	list->item_size = _item_size;
	list->list_size = 0;
	list->head.next = list->head.prev = &list->head;

	return list;
}

void insert_node(list_t* list, void* data)
{
	list_node_t* p;

	p = malloc(sizeof(list_node_t));
	p->data = malloc(list->item_size);
	memcpy(p->data, data, list->item_size);

	p->next = list->head.next;
	list->head.next->prev = p;
	list->head.next = p;
	p->prev = &list->head;
	++list->list_size;
}

list_node_t* find_node(list_t* list, void* data, int (*cmp)(const void*, const void*))
{
	list_node_t* p;

	p = list->head.next;

	while (p != &list->head) {
		if (cmp(p->data, data) == 0)
			return p;
		p = p->next;
	}
	return NULL;
}

void remove_node(list_t* list, list_node_t* node)
{
	list_node_t* front;
	list_node_t* back;

	front = node->prev;
	back = node->next;

	front->next = back;
	back->prev = front;
	free(node->data);
	free(node);

	--list->list_size;
}

void destroy_list(list_t* list)
{
	list_node_t* node;

	while (list->list_size > 0)
		remove_node(list, list->head.next);
	free(list);
}

