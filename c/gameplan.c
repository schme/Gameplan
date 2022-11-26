#include "gameplan.h"

#include "common.h"
#include "dictionary.h"
#include "memory.h"
#include "string.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GP_UNUSED(x) ((void)(x))
#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline void gexp_error(const char *msg) { fprintf(stderr, "%s\n", msg); }
Gstring gp_gexptostr(gexp e);

static gexp Gmul_op(gexp *gsp);

typedef gexp (*Gfunction)(gexp *gsp);
static Gfunction op_table[GEXP_OP_COUNT] = {
    0,
    0,
    &Gmul_op,
};

/* For now, one global environment for everything */
static Gheap top_heap;
static Gstack top_stack;
static Gdict *top_env;

typedef struct {
  size_t len;
  char *tokens[];
} TokenStorage;

typedef struct {
  size_t len;
  char **tokens;
} TokenView;

static inline TokenView make_token_view(TokenStorage *tokens) {
  return (TokenView){tokens->len, tokens->tokens};
}
static inline const char *token_get(const TokenView *tokens) {
  return tokens->tokens[0];
}
static inline const char *token_pop(TokenView *tokens) {
  if (tokens->len == 0) {
    return 0;
  }
  const char *next = token_get(tokens);
  tokens->len--;
  tokens->tokens++;
  return next;
}

TokenStorage *gp_tokenize(const char *program) {
  GP_UNUSED(program);

  TokenStorage *tokens = malloc(sizeof(TokenStorage) + sizeof(char *) * 9);
  tokens->len = 9;
  char *tokes[9] = {"(", "*", "pi", "(", "*", "5", "5.000000000", ")", ")"};

  memcpy(tokens->tokens, tokes, tokens->len * sizeof(char *));
  return tokens;
}

static inline bool gp_parse_fixval(const char *token, int token_length,
                                   gexp atom) {
  char *end = 0;
  i64 fixval = strtol(token, &end, 0);
  if (end == token + token_length) {
    Gexp_fixnum(atom, fixval);
    return true;
  }
  return false;
}

static inline bool gp_parse_flonum(const char *token, int token_length,
                                   gexp atom) {
  char *end = 0;
  f64 floval = strtod(token, &end);
  if (end == token + token_length) {
    Gexp_flonum(atom, floval);
    return true;
  }
  return false;
}

gexp gp_build_atom(const char *token) {
  gexp atom = Gheap_alloc(&top_heap);
  int token_length = strlen(token);

  if (gp_parse_fixval(token, token_length, atom))
    return atom;

  if (gp_parse_flonum(token, token_length, atom))
    return atom;

  /* symbol */
  Gexp_symbol(atom, Gstr(token, token_length));

  return atom;
}

gexp gp_parse_expr(TokenView *tokens) {
  gexp start = 0;
  gexp current = 0;
  gexp prev = 0;

  if (tokens->len <= 0)
    return 0;

  do {

#ifdef DEBUG_VERBOSE
    for (size_t i = 0; i < tokens->len; ++i) {
      printf("%s ", tokens->tokens[i]);
    }
    printf("\n");
#endif

    const char *token = token_pop(tokens);

    current = Gheap_alloc(&top_heap);
    if (token[0] == ')') {
      Gexp_void(current);

      if (!prev) {
        gexp_error("Syntax error!: ')'");
        return 0;
      }

      assert(Gpairp(prev));
      gexp prev_car = Gexp_car(prev);
      prev_car = current;
      return start;

    } else if (token[0] == '(') {
      Gexp_pair(current, gp_parse_expr(tokens), 0);

    } else {
      /* atom */
      gexp atom = gp_build_atom(token);
      Gexp_pair(current, atom, 0);
    }

    if (!start)
      start = current;

    if (prev)
      prev->val.pair.cdr = current;

    prev = current;
    current = 0;

  } while (tokens->len > 0);

  return start;
}

gexp gp_parse(TokenStorage *tokens) {
  TokenView tk = make_token_view(tokens);
  gexp exp = gp_parse_expr(&tk);

  return exp;
}

gexp Gread(const char *program) { return gp_parse(gp_tokenize(program)); }

gexp Gmul_op(gexp *stackptr) {
  flonum floval = 1;
  fixnum fixval = 1;
  bool decay_to_flonum = false;
  gexp n = 0;
  /* Unwind stack until we hit the proc itself, pop it and write result */
  while ((n = Gstack_pop(&top_stack)) != *stackptr) {
    assert(!Gstack_empty(&top_stack));

    /* Here we do a sort of exact-inexact decay, where if there is any flonum,
     * the value will become flonum (inexact) */
    if (Gfixnump(n)) {
      fixnum val = Gexp_unbox_fixnum(n);
      if (decay_to_flonum)
        floval *= val;
      else
        fixval *= val;
    } else if (Gflonump(n)) {
      if (!decay_to_flonum) {
        decay_to_flonum = true;
        floval = fixval;
      }
      floval *= Gexp_unbox_flonum(n);
    } else {
      gexp_error("Gmul_op: Syntax error!");
    }
  }

  gexp ret = Gheap_alloc(&top_heap);
  return decay_to_flonum ? Gexp_flonum(ret, floval) : Gexp_fixnum(ret, fixval);
}

static void debug_print_stack(Gstack *stack) {
  gexp *bottom = stack->bottom;
  gexp *top = stack->top;
  while (bottom != top) {
    Gstring e = gp_gexptostr(*bottom++);
    printf("%s ", Gstrget(&e));
  }
  printf("\n");
}

gexp Geval(gexp e) {
#ifdef DEBUG_VERBOSE
  debug_print_stack(&top_stack);
#endif
  if (!e) {
    return 0;
  }

  if (Gpairp(e)) {
    gexp car = Gexp_car(e);
    gexp car_ret = 0;
    if (car) {
      car_ret = Geval(car);
    }

    gexp *gsp = 0;
    gsp = top_stack.top - 1;

    gexp cdr = Gexp_cdr(e);
    if (cdr) {
      Geval(cdr);
    }

    if (car_ret && Gprocp(car_ret)) {
      gexp ret = op_table[Gexp_unbox_proc(car_ret)](gsp);
      Gstack_push(&top_stack, ret);
      return ret;

    } else {
      return car_ret;
    }

  } else if (Gvoidp(e)) {
    return 0;

  } else if (Gnumberp(e)) {
    Gstack_push(&top_stack, e);

  } else if (Gsymbolp(e)) {
    Gstring gs = Gexp_unbox_symbol(e);
    gexp ret = Gdict_get(top_env, Gstrget(&gs));
    return Geval(ret);

  } else if (Gprocp(e)) {
    Gstack_push(&top_stack, e);
  }

  return e;
}

Gstring gp_gexptostr(gexp e) {
#define GSTR_NUM_BUFSIZE 21
  Gstring s = {0};

  if (Gpairp(e)) {
    gexp car = e->val.pair.car;
    if (car) {
      if (!Gatomp(car)) {
        s = Gstr("(", 1);
      }
      Gstring next = gp_gexptostr(car);
      Gstrcat(&s, Gstrget(&next), Gstrlen(&next));
      Gstrdstr(&next);
    }

    gexp cdr = e->val.pair.cdr;
    if (cdr) {
      Gstring next = gp_gexptostr(cdr);
      Gstrcat(&s, Gstrget(&next), Gstrlen(&next));
      Gstrdstr(&next);
    }
  } else if (Gvoidp(e)) {
    s = Gstr(")", 1);

  } else if (Gfixnump(e)) {
    char buf[GSTR_NUM_BUFSIZE];
    snprintf(buf, GSTR_NUM_BUFSIZE, "%ld", e->val.fixnum);
    s = Gstr(buf, strlen(buf));

  } else if (Gflonump(e)) {
    char buf[GSTR_NUM_BUFSIZE];
    snprintf(buf, GSTR_NUM_BUFSIZE, "%g", e->val.flonum);
    s = Gstr(buf, strlen(buf));

  } else {
    /* symbol */
    s = Gstr(Gstrget(&e->val.symbol), Gstrlen(&e->val.symbol));
  }

  return s;
#undef GSTR_NUM_BUFSIZE
}

Gstring gp_read_and_eval(const char *program) {

  gexp ast = Gread(program);
  if (!ast)
    return Gstr("Error!", 6);

  gexp ret = Geval(ast);
  return gp_gexptostr(ret);
}

int main(int argc, char *argv[static argc + 1]) {
  GP_UNUSED(argc);
  GP_UNUSED(argv);

  const char *program = "(begin (define r 10.23) (* pi (* r r)))";
  GP_UNUSED(program);

  const char *small_program = "(* 5 5))";
  const char *input = small_program;

  top_heap = Gheap_create(1024);
  top_stack = Gstack_create(128);
  top_env = Gdict_create();

  gexp e = Gheap_alloc(&top_heap);
  Gexp_flonum(e, M_PI);
  Gdict_set(top_env, "pi", e);

  e = Gheap_alloc(&top_heap);
  Gexp_proc(e, GEXP_OP_MUL);
  Gdict_set(top_env, "*", e);

  Gstring ret = gp_read_and_eval(input);
  printf("%s\n", Gstrget(&ret));
  return 0;
}
