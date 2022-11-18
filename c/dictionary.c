#include "dictionary.h"
#include <stdlib.h>

/* sdbm hash */
i32 get_hash(char *s) {
  u32 hash_code = 0;

  for (i32 counter = 0; s[counter] != '\0'; counter++) {
    hash_code = s[counter] + (hash_code << 6) + (hash_code << 16) - hash_code;
  }
  return hash_code % DICT_MAX;
}

Dictionary *create_dict() {
  Dictionary *d = calloc(1, sizeof(Dictionary));
  return d;
}

void destroy_dict(Dictionary *d) { free(d); }
void *dict_get(Dictionary *d, char *s) { return d->data[get_hash(s)]; }
void dict_set(Dictionary *d, char *s, void *val) { d->data[get_hash(s)] = val; }
