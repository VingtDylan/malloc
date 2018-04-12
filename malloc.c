#include "malloc.h"

#include <stdlib.h>

//=========================================================
// Your implementations HERE
//=========================================================
void* do_malloc(size_t size) {
    return malloc(size);
}

void do_free(void *ptr) {
    free(ptr);
}
