#pragma once

#include "vanec/utils/defines.h"

typedef struct {
    void* items;
    void (*free)(void*);
    u64 item_size;
    u64 items_count;
    u64 capacity;
    bool is_ptr;
} Vector;

Vector vector_create(const u64 capacity, const u64 item_size, const void(*free)(void*), const bool is_ptr);

void vector_free(Vector* vector);

void vector_clear(Vector* vector);

void vector_push_back(Vector* vector, const void* item);

void vector_push_front(Vector* vector, const void* item);

void vector_insert(Vector* vector, const void* where, const void* items, const u64 count);

const void* vector_get_ref(const Vector* vector, const u64 index);