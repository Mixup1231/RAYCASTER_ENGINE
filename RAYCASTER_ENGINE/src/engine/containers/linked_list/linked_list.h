#pragma once
#include <stddef.h>
#include <inttypes.h>

typedef struct listNode {
	struct listNode* next;
	void* item;
} ListNode;

ListNode* list_node_create(size_t item_size);
void list_node_append(ListNode* node, size_t count, size_t item_size);
ListNode* list_node_get(const ListNode* node, size_t index);
void list_node_remove(ListNode* node, size_t index);
void list_node_free(ListNode* node);

typedef struct linkedList {
	ListNode* root;
	size_t length;
	size_t item_size;
} LinkedList;

LinkedList* linked_list_create(size_t length, size_t item_size);
void linked_list_append(LinkedList* list, size_t count);
void* linked_list_get(const LinkedList* list, size_t index);
void linked_list_remove(LinkedList* list, size_t index);
void linked_list_free(LinkedList* list);