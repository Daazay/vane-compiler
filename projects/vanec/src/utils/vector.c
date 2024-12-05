#include "vanec/utils/vector.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

static void vector_resize(Vector* vector, const u64 capacity) {
    assert(vector != NULL);
    assert(capacity > vector->capacity);

    void* items = realloc(vector->items, capacity * vector->item_size);
    assert(items != NULL);

    vector->items = items;
    vector->capacity = capacity;
}

Vector vector_create(const u64 capacity, const u64 item_size, const void(*free)(void*), const bool is_ptr) {
    assert(capacity > 0 && item_size > 0);

    void* items = malloc(capacity * item_size);
    assert(items != NULL);

    return (Vector) {
        .items = items,
            .capacity = capacity,
            .items_count = 0,
            .item_size = item_size,
            .free = free,
            .is_ptr = is_ptr,
    };
}

void vector_free(Vector* vector) {
    assert(vector != NULL);

    if (vector->free != NULL) {
        for (u64 i = 0; i < vector->items_count; ++i) {
            vector->free(vector_get_ref(vector, i));
        }
    }

    free(vector->items);

    vector->items = NULL;
    vector->free = NULL;
    vector->item_size = 0;
    vector->capacity = 0;
    vector->items_count = 0;
    vector->is_ptr = false;
}

void vector_push_back(Vector* vector, const void* item) {
    assert(vector != NULL);
    assert(item != NULL);

    if (vector->items_count + 1 >= vector->capacity) {
        vector_resize(vector, vector->capacity * 2);
    }

    memcpy((u8*)vector->items + vector->items_count * vector->item_size, item, vector->item_size);

    ++vector->items_count;
}

void vector_push_front(Vector* vector, const void* item) {
    assert(vector != NULL);
    assert(item != NULL);

    if (vector->items_count + 1 >= vector->capacity) {
        vector_resize(vector, vector->capacity * 2);
    }

    memcpy((u8*)vector->items + vector->item_size, vector->items, vector->items_count * vector->item_size);
    memcpy((u8*)vector->items, item, vector->item_size);

    ++vector->items_count;
}

void vector_insert(Vector* vector, const void* where, const void* items, const u64 count) {
    assert(vector != NULL);
    assert(items != NULL && count != 0);
    assert(where != NULL && ((u8*)where >= (u8*)vector->items && (u8*)where <= (u8*)vector->items + (vector->items_count * vector->item_size)));
    
    const u64 offset = (u8*)where - (u8*)vector->items;

    if (vector->items_count + count >= vector->capacity) {
        u64 capacity = vector->capacity;
        while (capacity <= vector->items_count + count) {
            capacity *= 2;
        }
        vector_resize(vector, capacity);
    }

    memcpy((u8*)vector->items + offset + (count * vector->item_size), (u8*)vector->items + offset, (vector->items_count * vector->item_size) - offset);
    memcpy((u8*)vector->items + offset, items, count * vector->item_size);

    vector->items_count += count;
}

const void* vector_get_ref(const Vector* vector, const u64 index) {
    assert(vector != NULL);
    assert(index < vector->items_count);

    void* item = (u8*)vector->items + index * vector->item_size;
    return vector->is_ptr ? *(void**)item : item;
}