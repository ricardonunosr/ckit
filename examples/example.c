#include <stdio.h>
#include "mem.h"

int main(int argc, char** argv)
{
    Arena* arena = arena_alloc(Megabytes(500));
    arena_push(arena,sizeof(int));
    printf("Hello World!\n");
    return 0;
}
