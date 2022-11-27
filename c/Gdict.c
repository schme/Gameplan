#include "Gdict.h"
#include <stdlib.h>

/* sdbm hash */
static i32 get_hash(const char *s) {
  u32 hash_code = 0;

  for (i32 counter = 0; s[counter] != '\0'; counter++) {
    hash_code = s[counter] + (hash_code << 6) + (hash_code << 16) - hash_code;
  }
  return hash_code % DICT_MAX;
}

Gdict *Gdict_create(void) {
  Gdict *d = calloc(1, sizeof(Gdict));
  return d;
}
void Gdict_destroy(Gdict *d) { free(d); }
void *Gdict_get(Gdict *d, const char *s) { return d->data[get_hash(s)]; }
void Gdict_set(Gdict *d, const char *s, void *val) { d->data[get_hash(s)] = val; }
