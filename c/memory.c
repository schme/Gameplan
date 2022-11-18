#include "memory.h"

#include <assert.h>
#include <stdlib.h>

/* len: size in gexp_structs */
GHeap memory_create_heap(size_t len) {
  GHeap heap;
  heap.begin = calloc(len, sizeof(Gexp_struct));
  heap.next = heap.begin;
  heap.allocated_size = len;
  return heap;
}

/* len: size in gexp_structs */
GStack memory_create_stack(size_t len) {
  GStack stack;
  stack.bottom = calloc(len, sizeof(Gexp_struct));
  stack.top = stack.bottom;
  stack.len = len;
  return stack;
}

gexp memory_get_next(GHeap *heap) {
  assert(heap->next < heap->begin + heap->allocated_size && "Out of memory!");
  gexp ret = heap->next++;
  return ret;
}

char *memory_string_alloc(size_t bytes) {
  return malloc(bytes);
}
