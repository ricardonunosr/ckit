#pragma once

#include <stdint.h>
#include <stddef.h>

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((U64)n) << 30)
#define Terabytes(n)  (((U64)n) << 40)

// Linear Allocator
typedef struct Arena Arena;
struct Arena{
  unsigned char* mem_base;
  size_t mem_len;
  size_t curr_offset;
};

Arena* arena_alloc(size_t capacity);
void arena_release(Arena* arena);

void* arena_push(Arena* arena, size_t size);
void* arena_push_align(Arena* arena, size_t size, size_t align);

// Pool Allocator
typedef struct PoolFreeNode PoolFreeNode;
struct PoolFreeNode{
  PoolFreeNode *next;
};

typedef struct Pool Pool;
struct Pool{
  unsigned char* mem_base;
  size_t mem_len;
  size_t chunk_size;

  PoolFreeNode* head;
};

Pool* pool_init(size_t capacity, size_t chunk_size);
void *pool_alloc(Pool *p);
void pool_free(Pool *p, void *ptr);
void pool_free_all(Pool *p);