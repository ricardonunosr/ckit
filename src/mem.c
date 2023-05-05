#include "mem.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

//------------------------------------------------------------
// Linear Allocator

Arena* arena_alloc(size_t capacity)
{
  Arena* arena = (Arena*)malloc(capacity);
  arena->mem_base = (unsigned char*)(arena + sizeof(arena));
  arena->mem_len = capacity;
  arena->curr_offset = sizeof(arena);
  return arena;
}

static int is_power_of_two(uintptr_t x)
{
  return (x&(x-1)) == 0;
}

static uintptr_t align_forward(uintptr_t ptr, size_t align)
{
  uintptr_t p, a, modulo;
  assert(is_power_of_two(align) && 
         "Alignment is not a power of two");

  p = ptr;
  a = (uintptr_t)align;
  // Same as (p % a) but faster as 'a' is a power of two
  modulo = p & (a-1);
  if(modulo != 0)
  {
    // If 'p' address is not aligned, push the address to the
    // next value which is aligned
    p += a - modulo;
  }
  return p;
}

void* arena_push_align(Arena* arena, size_t size, size_t align)
{
  uintptr_t curr_ptr = (uintptr_t)arena->mem_base + (uintptr_t)arena->curr_offset;
  uintptr_t offset = align_forward(curr_ptr, align);
  offset -= (uintptr_t)arena->mem_base;

  if( offset + size <= arena->mem_len)
  {
    void* memory = &arena->mem_base[offset];
    arena->curr_offset = offset + size;
    // Zero new memory by default
    memset(memory, 0, size);

    return memory;
  }
  return NULL;
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void*))
#endif

void* arena_push(Arena* arena, uint64_t size)
{
  return arena_push_align(arena, size, DEFAULT_ALIGNMENT);
}

//------------------------------------------------------------
// Pool Allocator

uintptr_t align_forward_uintptr(uintptr_t ptr, uintptr_t align) {
	uintptr_t a, p, modulo;

	assert(is_power_of_two(align));

	a = align;
	p = ptr;
	modulo = p & (a-1);
	if (modulo != 0) {
		p += a - modulo;
	}
	return p;
}

size_t align_forward_size(size_t ptr, size_t align) {
	size_t a, p, modulo;

	assert(is_power_of_two((uintptr_t)align));

	a = align;
	p = ptr;
	modulo = p & (a-1);
	if (modulo != 0) {
		p += a - modulo;
	}
	return p;
}

Pool* pool_init(size_t capacity, size_t chunk_size)
{
  Pool* p = (Pool*)malloc(capacity);
  int chunk_alignment = 8;

  // Align backing buffer to the specified chunk alignment
	uintptr_t initial_start = (uintptr_t)p;
	uintptr_t start = align_forward_uintptr(initial_start, (uintptr_t)chunk_alignment);
	capacity -= (size_t)(start-initial_start);

	// Align chunk size up to the required chunk_alignment
	chunk_size = align_forward_size(chunk_size, chunk_alignment);

	// Assert that the parameters passed are valid
	assert(chunk_size >= sizeof(PoolFreeNode) &&
	       "Chunk size is too small");
	assert(capacity >= chunk_size &&
	       "Backing buffer length is smaller than the chunk size");

	// Store the adjusted parameters
	p->mem_base = (unsigned char *)p;
	p->mem_len = capacity;
	p->chunk_size = chunk_size;
	p->head = NULL; // Free List Head

	// Set up the free list for free chunks
	pool_free_all(p);
  return p;
}

void *pool_alloc(Pool *p) {
	// Get latest free node
	PoolFreeNode *node = p->head;

	if (node == NULL) {
		assert(0 && "Pool allocator has no free memory");
		return NULL;
	}

	// Pop free node
	p->head = p->head->next;

	// Zero memory by default
	return memset(node, 0, p->chunk_size);
}

void pool_free(Pool *p, void *ptr) {
	PoolFreeNode *node;

	void *start = p->mem_base;
	void *end = &p->mem_base[p->mem_len];

	if (ptr == NULL) {
		// Ignore NULL pointers
		return;
	}

	if (!(start <= ptr && ptr < end)) {
		assert(0 && "Memory is out of bounds of the buffer in this pool");
		return;
	}

	// Push free node
	node = (PoolFreeNode *)ptr;
	node->next = p->head;
	p->head = node;
}

void pool_free_all(Pool *p) {
	size_t chunk_count = p->mem_len / p->chunk_size;
	size_t i;

	// Set all chunks to be free
	for (i = 0; i < chunk_count; i++) {
		void *ptr = &p->mem_base[i * p->chunk_size];
		PoolFreeNode *node = (PoolFreeNode *)ptr;
		// Push free node onto thte free list
		node->next = p->head;
		p->head = node;
	}
}