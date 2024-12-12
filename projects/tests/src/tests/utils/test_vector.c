#include "utest/utest.h"

#include "vanec/utils/vector.h"

struct Vecu8Fixture {
    Vector vec;
};

UTEST_F_SETUP(Vecu8Fixture) {
    utest_fixture->vec = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(u8), NULL, false);

    ASSERT_NE(utest_fixture->vec.items, NULL);
    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY);
    ASSERT_EQ(utest_fixture->vec.items_count, 0);
    ASSERT_EQ(utest_fixture->vec.item_size, sizeof(u8));
    ASSERT_EQ(utest_fixture->vec.free, NULL);
    ASSERT_EQ(utest_fixture->vec.is_ptr, false);
}

UTEST_F_TEARDOWN(Vecu8Fixture) {
    vector_free(&utest_fixture->vec);

    ASSERT_EQ(utest_fixture->vec.items, NULL);
    ASSERT_EQ(utest_fixture->vec.capacity, 0);
    ASSERT_EQ(utest_fixture->vec.items_count, 0);
}

UTEST_F(Vecu8Fixture, push_back) {
    u8 value = 0;
    vector_push_back(&utest_fixture->vec, &value);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY);
    ASSERT_EQ(utest_fixture->vec.items_count, 1);
    ASSERT_EQ(value, *(u8*)vector_get_ref(&utest_fixture->vec, 0));
}

UTEST_F(Vecu8Fixture, push_insert_at_back) {
    u8 values[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    vector_insert(&utest_fixture->vec, (u8*)utest_fixture->vec.items + utest_fixture->vec.items_count * utest_fixture->vec.item_size, values, 8);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY * 2);
    ASSERT_EQ(utest_fixture->vec.items_count, 8);

    ASSERT_EQ(values[0], *(u8*)vector_get_ref(&utest_fixture->vec, 0));
    ASSERT_EQ(values[1], *(u8*)vector_get_ref(&utest_fixture->vec, 1));
    ASSERT_EQ(values[2], *(u8*)vector_get_ref(&utest_fixture->vec, 2));
    ASSERT_EQ(values[3], *(u8*)vector_get_ref(&utest_fixture->vec, 3));
    ASSERT_EQ(values[4], *(u8*)vector_get_ref(&utest_fixture->vec, 4));
    ASSERT_EQ(values[5], *(u8*)vector_get_ref(&utest_fixture->vec, 5));
    ASSERT_EQ(values[6], *(u8*)vector_get_ref(&utest_fixture->vec, 6));
    ASSERT_EQ(values[7], *(u8*)vector_get_ref(&utest_fixture->vec, 7));
}

UTEST_F(Vecu8Fixture, push_front) {
    u8 value = 13;
    vector_push_back(&utest_fixture->vec, &value);

    value = 7;
    vector_push_front(&utest_fixture->vec, &value);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY);
    ASSERT_EQ(utest_fixture->vec.items_count, 2);
    ASSERT_EQ(value, *(u8*)vector_get_ref(&utest_fixture->vec, 0));
}

UTEST_F(Vecu8Fixture, push_insert_at_front) {
    u8 value = 13;
    vector_push_back(&utest_fixture->vec, &value);

    u8 values[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    vector_insert(&utest_fixture->vec, utest_fixture->vec.items, values, 8);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY * 2);
    ASSERT_EQ(utest_fixture->vec.items_count, 9);

    ASSERT_EQ(values[0], *(u8*)vector_get_ref(&utest_fixture->vec, 0));
    ASSERT_EQ(values[1], *(u8*)vector_get_ref(&utest_fixture->vec, 1));
    ASSERT_EQ(values[2], *(u8*)vector_get_ref(&utest_fixture->vec, 2));
    ASSERT_EQ(values[3], *(u8*)vector_get_ref(&utest_fixture->vec, 3));
    ASSERT_EQ(values[4], *(u8*)vector_get_ref(&utest_fixture->vec, 4));
    ASSERT_EQ(values[5], *(u8*)vector_get_ref(&utest_fixture->vec, 5));
    ASSERT_EQ(values[6], *(u8*)vector_get_ref(&utest_fixture->vec, 6));
    ASSERT_EQ(values[7], *(u8*)vector_get_ref(&utest_fixture->vec, 7));
}

struct Obj {
    u64 a;
    u64 b;
    u64 c;
};

#define ALLOC_OBJ(obj, _a, _b, _c)  \
obj = malloc(sizeof(struct Obj));   \
ASSERT_NE(obj, NULL);               \
obj->a = _a;                        \
obj->b = _b;                        \
obj->c = _c

void obj_free(struct Obj* obj) {
    free(obj);
}

struct VecObjFixture {
    Vector vec;
};

UTEST_F_SETUP(VecObjFixture) {
    utest_fixture->vec = vector_create(DEFAULT_VECTOR_CAPACITY, sizeof(struct Obj*), &obj_free, true);

    ASSERT_NE(utest_fixture->vec.items, NULL);
    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY);
    ASSERT_EQ(utest_fixture->vec.items_count, 0);
    ASSERT_EQ(utest_fixture->vec.item_size, sizeof(struct Obj*));
    ASSERT_EQ(utest_fixture->vec.free, &obj_free);
    ASSERT_EQ(utest_fixture->vec.is_ptr, true);
}

UTEST_F_TEARDOWN(VecObjFixture) {
    vector_free(&utest_fixture->vec);

    ASSERT_EQ(utest_fixture->vec.items, NULL);
    ASSERT_EQ(utest_fixture->vec.capacity, 0);
    ASSERT_EQ(utest_fixture->vec.items_count, 0);
}

UTEST_F(VecObjFixture, push_back) {
    struct Obj* value = NULL;
    ALLOC_OBJ(value, 13, 15, 17);
    vector_push_back(&utest_fixture->vec, &value);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY);
    ASSERT_EQ(utest_fixture->vec.items_count, 1);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 0), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->a, value->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->b, value->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->c, value->c);
}

UTEST_F(VecObjFixture, push_insert_at_back) {
    struct Obj* value1 = NULL;
    ALLOC_OBJ(value1, 1, 2, 3);
    struct Obj* value2 = NULL;
    ALLOC_OBJ(value2, 4, 5, 6);
    struct Obj* value3 = NULL;
    ALLOC_OBJ(value3, 7, 8, 9);
    struct Obj* value4 = NULL;
    ALLOC_OBJ(value4, 10, 11, 12);
    struct Obj* value5 = NULL;
    ALLOC_OBJ(value5, 13, 14, 15);
    struct Obj* value6 = NULL;
    ALLOC_OBJ(value6, 16, 17, 18);
    struct Obj* value7 = NULL;
    ALLOC_OBJ(value7, 19, 20, 21);
    struct Obj* value8 = NULL;
    ALLOC_OBJ(value8, 22, 23, 24);

    struct Obj* values[] = {
        value1,
        value2,
        value3,
        value4,
        value5,
        value6,
        value7,
        value8,
    };

    vector_insert(&utest_fixture->vec, (u8*)utest_fixture->vec.items + utest_fixture->vec.items_count * utest_fixture->vec.item_size, values, 8);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY * 2);
    ASSERT_EQ(utest_fixture->vec.items_count, 8);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 0), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->a, values[0]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->b, values[0]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->c, values[0]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 1), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 1))->a, values[1]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 1))->b, values[1]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 1))->c, values[1]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 2), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 2))->a, values[2]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 2))->b, values[2]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 2))->c, values[2]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 3), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 3))->a, values[3]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 3))->b, values[3]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 3))->c, values[3]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 4), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 4))->a, values[4]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 4))->b, values[4]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 4))->c, values[4]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 5), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 5))->a, values[5]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 5))->b, values[5]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 5))->c, values[5]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 6), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 6))->a, values[6]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 6))->b, values[6]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 6))->c, values[6]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 7), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 7))->a, values[7]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 7))->b, values[7]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 7))->c, values[7]->c);
}

UTEST_F(VecObjFixture, push_front) {
    struct Obj* value = NULL;
    ALLOC_OBJ(value, 13, 15, 17);
    vector_push_back(&utest_fixture->vec, &value);

    ALLOC_OBJ(value, 16, 75, 42);
    vector_push_front(&utest_fixture->vec, &value);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY);
    ASSERT_EQ(utest_fixture->vec.items_count, 2);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 0), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->a, value->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->b, value->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->c, value->c);
}

UTEST_F(VecObjFixture, push_insert_at_front) {
    struct Obj* value = NULL;
    ALLOC_OBJ(value, 13, 15, 17);
    vector_push_back(&utest_fixture->vec, &value);

    struct Obj* value1 = NULL;
    ALLOC_OBJ(value1, 1, 2, 3);
    struct Obj* value2 = NULL;
    ALLOC_OBJ(value2, 4, 5, 6);
    struct Obj* value3 = NULL;
    ALLOC_OBJ(value3, 7, 8, 9);
    struct Obj* value4 = NULL;
    ALLOC_OBJ(value4, 10, 11, 12);
    struct Obj* value5 = NULL;
    ALLOC_OBJ(value5, 13, 14, 15);
    struct Obj* value6 = NULL;
    ALLOC_OBJ(value6, 16, 17, 18);
    struct Obj* value7 = NULL;
    ALLOC_OBJ(value7, 19, 20, 21);
    struct Obj* value8 = NULL;
    ALLOC_OBJ(value8, 22, 23, 24);

    struct Obj* values[] = {
        value1,
        value2,
        value3,
        value4,
        value5,
        value6,
        value7,
        value8,
    };

    vector_insert(&utest_fixture->vec, utest_fixture->vec.items, values, 8);

    ASSERT_EQ(utest_fixture->vec.capacity, DEFAULT_VECTOR_CAPACITY * 2);
    ASSERT_EQ(utest_fixture->vec.items_count, 9);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 0), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->a, values[0]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->b, values[0]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 0))->c, values[0]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 1), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 1))->a, values[1]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 1))->b, values[1]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 1))->c, values[1]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 2), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 2))->a, values[2]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 2))->b, values[2]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 2))->c, values[2]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 3), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 3))->a, values[3]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 3))->b, values[3]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 3))->c, values[3]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 4), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 4))->a, values[4]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 4))->b, values[4]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 4))->c, values[4]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 5), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 5))->a, values[5]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 5))->b, values[5]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 5))->c, values[5]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 6), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 6))->a, values[6]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 6))->b, values[6]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 6))->c, values[6]->c);

    ASSERT_NE((struct Obj*)vector_get_ref(&utest_fixture->vec, 7), NULL);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 7))->a, values[7]->a);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 7))->b, values[7]->b);
    ASSERT_EQ(((struct Obj*)vector_get_ref(&utest_fixture->vec, 7))->c, values[7]->c);
}