#ifndef GAMEPLAN_H_
#define GAMEPLAN_H_

#include "Gstring.h"

#include "types.h"

typedef struct Gexp_struct *gexp;
extern gexp obj_true;
extern gexp obj_false;
extern gexp obj_null;

typedef enum Gopcode
{
  GEXP_OP_IF,
  GEXP_OP_DEFINE,
  GEXP_OP_PLUS,
  GEXP_OP_MINUS,
  GEXP_OP_MUL,
  GEXP_OP_DIV,
  GEXP_OP_COUNT,
} Gopcode;

typedef enum Gexp_type {
  GEXP_TYPE_NULL,
  GEXP_TYPE_PAIR,
  GEXP_TYPE_FIXNUM,
  GEXP_TYPE_FLONUM,
  GEXP_TYPE_SYMBOL,
  GEXP_TYPE_BOOL,
  GEXP_TYPE_PROCEDURE,
} GType;

typedef u32 gexp_tag;
typedef struct Gexp_struct {
  gexp_tag tag;
  union {
    fixnum fixnum;
    flonum flonum;
    Gstring symbol;
    char boolean;
    struct {
      gexp car, cdr;
    } pair;
  } val;
} Gexp_struct;

_Static_assert(sizeof(Gexp_struct) == 32, "gexp_struct size changed!");

static inline gexp Gexp_flonum(gexp e, flonum val) {
  e->tag = GEXP_TYPE_FLONUM;
  e->val.flonum = val;
  return e;
}
static inline gexp Gexp_fixnum(gexp e, fixnum val) {
  e->tag = GEXP_TYPE_FIXNUM;
  e->val.fixnum = val;
  return e;
}
static inline gexp Gexp_symbol(gexp e, Gstring val) {
  e->tag = GEXP_TYPE_SYMBOL;
  e->val.symbol = val;
  return e;
}
static inline gexp Gexp_pair(gexp e, gexp car, gexp cdr) {
  e->tag = GEXP_TYPE_PAIR;
  e->val.pair.car = car;
  e->val.pair.cdr = cdr;
  return e;
}
static inline gexp Gexp_proc(gexp e, Gopcode code) {
  e->tag = GEXP_TYPE_PROCEDURE;
  e->val.fixnum = code;
  return e;
}
static inline gexp Gexp_bool(gexp e, char val) {
  e->tag = GEXP_TYPE_BOOL;
  e->val.boolean = val;
  return e;
}
static inline gexp Gexp_null(gexp e) {
  e->tag = GEXP_TYPE_NULL;
  return e;
}

#define Gcar(e) ((e)->val.pair.car)
#define Gcdr(e) ((e)->val.pair.cdr)
#define Gfixnum(e) ((e)->val.fixnum)
#define Gflonum(e) ((e)->val.flonum)
#define Gsymbol(e) ((e)->val.symbol)
#define Gproc(e) ((Gopcode)((e)->val.fixnum))

/* Predicate functions */
static inline _Bool Gnullp(gexp e) { return e == obj_null; }
static inline _Bool Gpairp(gexp e) { return e->tag == GEXP_TYPE_PAIR; }
static inline _Bool Gatomp(gexp e) { return !Gpairp(e) && !Gnullp(e); }
static inline _Bool Gfixnump(gexp e) { return e->tag == GEXP_TYPE_FIXNUM; }
static inline _Bool Gflonump(gexp e) { return e->tag == GEXP_TYPE_FLONUM; }
static inline _Bool Gnumberp(gexp e) { return Gfixnump(e) || Gflonump(e); }
static inline _Bool Gsymbolp(gexp e) { return e->tag == GEXP_TYPE_SYMBOL; }
static inline _Bool Gprocp(gexp e) { return e->tag == GEXP_TYPE_PROCEDURE; }
static inline _Bool Gboolp(gexp e) { return e == obj_false || e == obj_true; }
static inline _Bool Gfalsep(gexp e) { return e == obj_false; }
static inline _Bool Gtruep(gexp e) { return !Gfalsep(e); }

#define DEBUG_VERBOSE 0
#define TRACE_ENABLE 1
#endif
