#ifndef GAMEPLAN_DICT_H_
#define GAMEPLAN_DICT_H_

#include "types.h"

#ifndef DICT_MAX
#define DICT_MAX 1000
#endif

typedef struct Dictionary
{
  void *data[DICT_MAX];
  u32 size;
} Dictionary;

Dictionary *create_dict();
void destroy_dict(Dictionary *d);
void *dict_get(Dictionary *d, char *s);
void dict_set(Dictionary *d, char *s, void* val);

#endif

