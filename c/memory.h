#ifndef GAMEPLAN_MEMORY_H_
#define GAMEPLAN_MEMORY_H_

#include "gameplan.h"

typedef struct Gstack Gstack;
struct Gstack {
  gexp *bottom, *top;
  size_t size; /* how many gexp pointers is the heap */
};

typedef struct Gcallstack Gcallstack;
struct Gcallstack {
  Gstack stack;
};

typedef struct Gheap Gheap;
struct Gheap {
  gexp begin, next;
  size_t size; /* how many gexp_structs is the heap overall */
};

Gheap Gheap_create(size_t len);
gexp Gheap_alloc(Gheap *heap);

Gstack Gstack_create(size_t len);
void Gstack_push(Gstack *stack, gexp val);
gexp Gstack_pop(Gstack *stack);
gexp Gstack_peek(Gstack *stack);
bool Gstack_empty(Gstack *stack);

#endif
