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

    return 0;
}
