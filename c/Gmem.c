#include "Gmem.h"

#include <assert.h>
#include <stdlib.h>

/* len: size in gexp_structs */
Gheap Gheap_create(size_t len) {
  Gheap heap;
  heap.begin = calloc(len, sizeof(Gexp_struct));
  heap.next = heap.begin;
  heap.size = len;
  return heap;
}

/* len: size in gexp_structs */
Gstack Gstack_create(size_t len) {
  Gstack stack;
  stack.bottom = calloc(len, sizeof(gexp));
  stack.top = stack.bottom;
  stack.size = len;
  return stack;
}

gexp Gheap_alloc(Gheap *heap) {
  assert(heap->next < heap->begin + heap->size && "Out of memory!");
  gexp ret = heap->next++;
  return ret;
}

void Gstack_push(Gstack *stack, gexp val)
{
  assert(stack->top < stack->bottom + stack->size && "Out of stack!");
  *stack->top = val;
  stack->top++;
}

gexp Gstack_pop(Gstack *stack) {
  assert(stack->top > stack->bottom && "Popping an empty stack!");
  gexp ret = *(--stack->top);
  return ret;
}

gexp Gstack_peek(Gstack *stack) {
  if (stack->top <= stack->bottom) return 0;
  return *stack->top;
}

bool Gstack_empty(Gstack *stack) {
  return stack->bottom == stack->top;
}
