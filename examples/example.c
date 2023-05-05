#include <stdio.h>
#include <stdlib.h>

#include "timer.c"
#include "mem.h"

#define ITERATIONS 1000

int main(void)
{
    init_timer_system();
    Arena* arena = arena_alloc(Megabytes(2));
    for(int i=0; i<ITERATIONS; i++)
    {
      int* number = arena_push(arena,sizeof(int));
      *(number) = i;
    }
    uint64_t elapsed_time_arena = time_in_nanos();

    init_timer_system();
    for(int i=0; i<ITERATIONS; i++)
    {
      int* number = malloc(sizeof(int));
      *(number) = i;
    }
    uint64_t elapsed_time_malloc = time_in_nanos();

    printf("arena time elapsed: %lld ns\n",elapsed_time_arena);
    printf("malloc time elapsed: %lld ns\n",elapsed_time_malloc);

    init_timer_system();
    Pool* p = pool_init(Megabytes(2),64);
    void *a, *b, *c, *d, *e, *f;

    a = pool_alloc(&p);
    b = pool_alloc(&p);
    c = pool_alloc(&p);
    d = pool_alloc(&p);
    e = pool_alloc(&p);
    f = pool_alloc(&p);

    pool_free(&p, f);
    pool_free(&p, c);
    pool_free(&p, b);
    pool_free(&p, d);

    d = pool_alloc(&p);

    pool_free(&p, a);

    a = pool_alloc(&p);

    pool_free(&p, e);
    pool_free(&p, a);
    pool_free(&p, d);

    return 0;
}
