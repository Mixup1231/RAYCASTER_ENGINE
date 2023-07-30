#pragma once
#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

#include "../linked_list/linked_list.h"

#define CANTOUR_PAIR(a, b)  (0.5 * (a + b) * (a + b + 1) + b)
#define MAP_LENGTH 1000

size_t hash_uint(const uint32_t* key);
size_t hash_size(const size_t* key);
size_t hash_uint_pair(const size_t* key);
size_t hash_uint_triplet(const size_t* key);
size_t hash_string(const char* string);

typedef struct keyValue {
	size_t pre_modulus;
	size_t key;
	void* item;
} KeyValue;

void key_value_init(KeyValue* key_value, size_t pre_modulus, size_t key, size_t item_size);

typedef size_t hash(void*);

typedef struct map {
	LinkedList* items[MAP_LENGTH];
	size_t item_size;
	size_t length;
	hash* hashing_callback;
} Map;

Map* map_create(size_t item_size, hash* hashing_callback);
void map_insert(Map* map, void* key, void* item);
void map_remove(Map* map, void* key);
void* map_get(const Map* map, void* key);
bool map_contains(const Map* map, void* key);
void map_free(Map* map);