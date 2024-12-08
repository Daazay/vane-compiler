#pragma once

typedef unsigned char       u8;
typedef char                i8;
typedef unsigned short      u16;
typedef short               i16;
typedef unsigned long       u32;
typedef long                i32;
typedef unsigned long long  u64;
typedef long long           i64;

typedef u8                  bool;

#define true    ((bool)(1 == 1))
#define false   ((bool)(1 == 0))

#ifndef NULL
#define NULL 0
#endif

static_assert(sizeof(u8)  == 1, "expected 1 byte");
static_assert(sizeof(i8)  == 1, "expected 1 byte");
static_assert(sizeof(u16) == 2, "expected 2 bytes");
static_assert(sizeof(i16) == 2, "expected 2 bytes");
static_assert(sizeof(u32) == 4, "expected 4 bytes");
static_assert(sizeof(i32) == 4, "expected 4 bytes");
static_assert(sizeof(u64) == 8, "expected 8 bytes");
static_assert(sizeof(i64) == 8, "expected 8 bytes");

#define KB ((u64)1024)
#define MB ((u64)(KB * 1024))

#define MAX_STREAM_CHUNK_CAPACITY 1 * KB
#define MIN_STREAM_CHUNK_CAPACITY 8

#define DEFAULT_VECTOR_CAPACITY 8

void error_exit(i32 code);

void error_exit_with_msg(i32 code, const char* msg, ...);