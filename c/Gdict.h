#ifndef GAMEPLAN_DICT_H_
#define GAMEPLAN_DICT_H_

#include "types.h"

#ifndef DICT_MAX
#define DICT_MAX 1000
#endif

typedef struct Gdict Gdict;
struct Gdict
{
  u32 size;
  void *data[DICT_MAX];
};

Gdict *Gdict_create(void);
void Gdict_destroy(Gdict *d);
void *Gdict_get(Gdict *d, const char *s);
void Gdict_set(Gdict *d, const char *s, void* val);

#endif

