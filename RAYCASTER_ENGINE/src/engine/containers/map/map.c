#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "map.h"


size_t hash_uint(const uint32_t* key) {
	assert(key);
	return (size_t)CANTOUR_PAIR(key[0], key[0]);
}

size_t hash_size(const size_t* key) {
	assert(key);
	return (size_t)CANTOUR_PAIR(key[0], key[0]);
}

size_t hash_uint_pair(const size_t* key) {
	assert(key);
	return (size_t)CANTOUR_PAIR(key[0], key[1]);
}

size_t hash_uint_triplet(const size_t* key) {
	assert(key);
	return (size_t)CANTOUR_PAIR((size_t)CANTOUR_PAIR(key[0], key[1]), key[2]);
}

size_t hash_string(const char* string) {
	static const size_t p = 31;
	assert(string);

	size_t length = strlen(string);
	size_t total = 0;
	for (size_t i = 0; i < length; i++)
		total = p * total + string[i];

	return total;
}



void key_value_init(KeyValue* key_value, size_t pre_modulus, size_t key, size_t item_size) {
	assert(key_value);

	key_value->pre_modulus = pre_modulus;
	key_value->key = key;

	key_value->item = malloc(item_size);
	assert(key_value->item);
}



Map* map_create(size_t item_size, hash* hashing_callback) {
	Map* map = malloc(sizeof(Map));
	assert(map);

	for (size_t i = 0; i < MAP_LENGTH; i++)
		map->items[i] = NULL;

	map->item_size = item_size;
	map->hashing_callback = hashing_callback;
	map->length = 0;

	return map;
}

void map_insert(Map* map, void* key, void* item) {
	assert(map && key && item && map->length < MAP_LENGTH);

	size_t pre_modulus = map->hashing_callback(key);
	size_t key_hash = pre_modulus % MAP_LENGTH;

	if (map->items[key_hash] == NULL)
		map->items[key_hash] = linked_list_create(1, sizeof(KeyValue));

	LinkedList* list = map->items[key_hash];
	for (ListNode* i = list->root; i != NULL; i = i->next)
		if (((KeyValue*)i->item)->pre_modulus == pre_modulus)
			assert(true && "Item is already contained!");

	linked_list_append(list, 1);
	KeyValue* key_value = (KeyValue*)linked_list_get(list, list->length - 1);
	key_value_init(key_value, pre_modulus, key_hash, map->item_size);
	memcpy(key_value->item, item, map->item_size);
	map->length++;
}

void map_remove(Map* map, void* key) {
	assert(map && key && map->length < MAP_LENGTH);

	size_t pre_modulus = map->hashing_callback(key);
	size_t key_hash = pre_modulus % MAP_LENGTH;

	LinkedList* list = map->items[key_hash];
	assert(list && "Key not yet mapped!");

	size_t index = 0;
	for (ListNode* i = list->root; i != NULL; i = i->next, index++)
		if (((KeyValue*)i->item)->pre_modulus == pre_modulus)
			free(((KeyValue*)i->item)->item);

	linked_list_remove(list, index);
	if (list->length == 1) {
		linked_list_free(list);
		map->items[key_hash] = NULL;
	}
}

void* map_get(const Map* map, void* key) {
	assert(map && key && map->length < MAP_LENGTH);

	size_t pre_modulus = map->hashing_callback(key);
	size_t key_hash = pre_modulus % MAP_LENGTH;

	LinkedList* list = map->items[key_hash];
	assert(list && "Key not yet mapped!");

	ListNode* i = list->root;
	for (; i != NULL; i = i->next)
		if (((KeyValue*)i->item)->pre_modulus == pre_modulus)
			return ((KeyValue*)i->item)->item;
	
	return NULL;
}

bool map_contains(const Map* map, void* key) {
	assert(map && key && map->length < MAP_LENGTH);

	size_t pre_modulus = map->hashing_callback(key);
	size_t key_hash = pre_modulus % MAP_LENGTH;

	LinkedList* list = map->items[key_hash];
	if (!list) return false;

	ListNode* i = list->root;
	for (; i != NULL; i = i->next)
		if (((KeyValue*)i->item)->pre_modulus == pre_modulus)
			return true;

	return false;
}

void map_free(Map* map) {
	assert(map);

	for (size_t i = 0; i < MAP_LENGTH; i++)
		if (map->items[i]) {
			for (ListNode* node = map->items[i]->root; node != NULL; node = node->next)
				if (((KeyValue*)node->item)->item)
					free(((KeyValue*)node->item)->item);
			linked_list_free(map->items[i]);
		}
}
