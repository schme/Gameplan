#ifndef GAMEPLAN_MEMORY_H_
#define GAMEPLAN_MEMORY_H_

#include "gameplan.h"

typedef struct GStack GStack;
struct GStack {
  gexp bottom, top;
  size_t len;
};

typedef struct GCallstack GCallstack;
struct GCallstack {
  GStack stack;
};

typedef struct GHeap GHeap;
struct GHeap {
  gexp begin, next;
  size_t allocated_size; /* how many gexp_structs is the heap overall */
};

GHeap memory_create_heap(size_t len);
GStack memory_create_stack(size_t len);
gexp memory_get_next(GHeap *heap);

char *memory_string_alloc(size_t bytes);

#endif
