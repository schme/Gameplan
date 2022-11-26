#ifndef GAMEPLAN_H_
#define GAMEPLAN_H_

#include "string.h"

#include "types.h"

typedef struct Gexp_struct *gexp;

typedef enum Gopcode
{
  GEXP_OP_PLUS,
  GEXP_OP_MINUS,
  GEXP_OP_MUL,
  GEXP_OP_COUNT,
} Gopcode;

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
    fixnum fixnum;
    flonum flonum;
    Gstring symbol;
    struct {
      gexp car, cdr;
    } pair;
  } val;
} Gexp_struct;

_Static_assert(sizeof(Gexp_struct) <= 32, "gexp_struct size over a limit!");

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
static inline gexp Gexp_void(gexp e) {
  e->tag = GEXP_TYPE_VOID;
  return e;
}

static inline gexp Gexp_car(gexp e) { return e->val.pair.car; }
static inline gexp Gexp_cdr(gexp e) { return e->val.pair.cdr; }
static inline fixnum Gexp_unbox_fixnum(gexp e) { return e->val.fixnum; }
static inline flonum Gexp_unbox_flonum(gexp e) { return e->val.flonum; }
static inline Gstring Gexp_unbox_symbol(gexp e) { return e->val.symbol; }
static inline Gopcode Gexp_unbox_proc(gexp e) { return e->val.fixnum; }

/* Predicate functions */
static inline _Bool Gvoidp(gexp e) { return e->tag == GEXP_TYPE_VOID; }
static inline _Bool Gpairp(gexp e) { return e->tag == GEXP_TYPE_PAIR; }
static inline _Bool Gatomp(gexp e) { return !Gpairp(e) && !Gvoidp(e); }
static inline _Bool Gfixnump(gexp e) { return e->tag == GEXP_TYPE_FIXNUM; }
static inline _Bool Gflonump(gexp e) { return e->tag == GEXP_TYPE_FLONUM; }
static inline _Bool Gnumberp(gexp e) { return Gfixnump(e) || Gflonump(e); }
static inline _Bool Gsymbolp(gexp e) { return e->tag == GEXP_TYPE_SYMBOL; }
static inline _Bool Gprocp(gexp e) { return e->tag == GEXP_TYPE_PROCEDURE; }

#define DEBUG_VERBOSE
#endif
