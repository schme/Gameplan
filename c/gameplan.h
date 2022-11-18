#ifndef GAMEPLAN_H_
#define GAMEPLAN_H_

#include "string.h"

#include "types.h"

typedef struct Gexp_struct *gexp;

typedef enum Gexp_type {
  GEXP_TYPE_VOID,
  GEXP_TYPE_PAIR,
  GEXP_TYPE_FIXNUM,
  GEXP_TYPE_FLONUM,
  GEXP_TYPE_SYMBOL,
  GEXP_TYPE_PROCEDURE,
} GType;

typedef u32 gexp_tag;
typedef struct Gexp_struct {
  gexp_tag tag;
  union {
    i64 fixnum;
    f64 flonum;
    Gstring symbol;
    struct {
      gexp car, cdr;
    } pair;
  } val;
} Gexp_struct;

_Static_assert(sizeof(Gexp_struct) <= 32, "gexp_struct size over a limit!");

static inline _Bool Gvoidp(gexp e) { return e->tag == GEXP_TYPE_VOID; }
static inline _Bool Gpairp(gexp e) { return e->tag == GEXP_TYPE_PAIR; }
static inline _Bool Gatomp(gexp e) { return !Gpairp(e) && !Gvoidp(e); }
static inline _Bool Gfixnump(gexp e) { return e->tag == GEXP_TYPE_FIXNUM; }
static inline _Bool Gflonump(gexp e) { return e->tag == GEXP_TYPE_FLONUM; }
static inline _Bool Gnumberp(gexp e) { return Gfixnump(e) || Gflonump(e); }

typedef struct Dictionary Env;

#define DEBUG_VERBOSE
#endif
