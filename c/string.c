#include "string.h"

#include <stdlib.h>
#include <string.h>

#define MIN(a, b) (a < b ? a : b)

static inline char *get_str(Gstring *str) {
  if (Gstring_smallp(str)) {
    return str->small;
  }
  return str->s;
}

static inline void add_null(Gstring *ss) {
  char *buf = get_str(ss);
  buf[ss->len] = '\0';
}

bool Gstring_smallp(Gstring *s) { return s->len < GSTRING_SMALL_SIZE; }
u32 Gstring_length(Gstring *s) { return s->len; }

Gstring Gstring_create(const char *s, u32 len) {
  Gstring ss = {0};
  /* We can use the local buffer */
  if (len < GSTRING_SMALL_SIZE) {
    strncpy(ss.small, s, GSTRING_SMALL_SIZE); /* this inserts a null byte */
  } else {
    ss.s = malloc(len + 1);
    ss.size = len + 1;
    strncpy(ss.s, s, len);
  }
  ss.len = len;
  add_null(&ss);
  return ss;
}

void Gstring_destroy(Gstring *s) {
  if (!Gstring_smallp(s)) {
    free(s->s);
  }
  *s = (Gstring){0};
}

const char *Gstring_get_str(Gstring *s) {
  if (Gstring_smallp(s))
    return s->small;
  return s->s;
}

void Gstring_append(Gstring *dst, const char *s, u32 len) {
  size_t end_size = dst->len + len + 1;

  bool is_small = Gstring_smallp(dst);

  if (end_size <= GSTRING_SMALL_SIZE) {
    strncat(dst->small, s, len);
  } else if (end_size <= dst->size) {
    strncat(dst->s, s, len);
  } else {
    /* reallocate strategy */
    if (dst->size == 0)
      dst->size = end_size;
    else if (dst->size * 2 <= end_size)
      dst->size = end_size;
    else
      dst->size = dst->size * 2;

    if (is_small) {
      dst->s = malloc(dst->size);
      strncpy(dst->s, dst->small, dst->len);
    } else {
      dst->s = realloc(dst->s, dst->size);
    }
    strncpy(dst->s + dst->len, s, len);
  }

  dst->len = end_size - 1;
  add_null(dst);
}

i32 Gstring_compare(Gstring *gs, const char *s) {
  return strncmp(Gstring_get_str(gs), s, MIN(Gstring_length(gs), strlen(s)));
}

#undef MIN
