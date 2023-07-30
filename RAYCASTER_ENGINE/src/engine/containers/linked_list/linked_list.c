#include <stdlib.h>
#include <assert.h>

#include "linked_list.h"


ListNode* list_node_create(size_t item_size) {
	ListNode* node;

	node = malloc(sizeof(ListNode));
	assert(node);

	node->item = malloc(item_size);
	assert(node->item);

	node->next = NULL;

	return node;
}

void list_node_append(ListNode* node, size_t count, size_t item_size) {
	assert(node);

	for (ListNode* i = node; count > 0; i = i->next)
		if (i->next == NULL) {
			i->next = list_node_create(item_size);
			count--;
		}
}

ListNode* list_node_get(const ListNode* node, size_t index) {
	assert(node);

	ListNode* i = node;
	for (; i != NULL && index > 0; i = i->next, index--);
	assert(!index && "node index out of range!");

	return i;
}

void list_node_remove(ListNode* root, size_t index) {
	ListNode* prev = list_node_get(root, index - 1);
	ListNode* node = list_node_get(prev, 1);
	ListNode* next = list_node_get(node, 1);

	if (node->item != NULL)
		free(node->item);
	free(node);

	prev->next = next;
}

void list_node_free(ListNode* node) {
	assert(node);

	if (node->next)
		list_node_free(node->next);
	if (node->item)
		free(node->item);
	free(node);
}



LinkedList* linked_list_create(size_t length, size_t item_size) {
	LinkedList* list = malloc(sizeof(LinkedList));
	assert(list);

	list->length = length;
	list->item_size = item_size;

	list->root = list_node_create(item_size);
	list_node_append(list->root, length - 1, item_size);

	return list;
}

void linked_list_append(LinkedList* list, size_t count) {
	assert(list);

	list_node_append(list->root, count, list->item_size);
	list->length++;
}

void* linked_list_get(const LinkedList* list, size_t index) {
	assert(list);
	assert(0 <= index && index < list->length && "list index out of range!");

	return list_node_get(list->root, index)->item;
}

void linked_list_remove(LinkedList* list, size_t index) {
	assert(list);
	assert(0 <= index && index < list->length && "list index out of range!");

	list_node_remove(list->root, index);
	list->length--;
}

void linked_list_free(LinkedList* list) {
	assert(list);

	list_node_free(list->root);
}
