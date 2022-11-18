#include "gameplan.h"

#include "common.h"
#include "dictionary.h"
#include "memory.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GP_UNUSED(x) ((void)(x))
#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))

static inline void gexp_error(const char *msg) { fprintf(stderr, "%s\n", msg); }

/* For now, one global environment for everything */
static GHeap top_heap;
static GStack top_stack;
static Env top_env;

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
  char *tokes[9] = {"(", "*", "pi", "(", "*", "5", "5.0", ")", ")"};

  memcpy(tokens->tokens, tokes, tokens->len * sizeof(char *));
  return tokens;
}

static inline bool gp_parse_fixval(const char *token, int token_length, gexp atom) {
  char *end = 0;
  i64 fixval = strtol(token, &end, 0);
  if (end == token + token_length) {
    TRACE_PRINT("Fixnum! %ld", fixval);
    atom->tag = GEXP_TYPE_FIXNUM;
    atom->val.fixnum = fixval;
    return true;
  }
  return false;
}

static inline bool gp_parse_flonum(const char *token, int token_length, gexp atom) {
  char *end = 0;
  f64 floval = strtod(token, &end);
  if (end == token + token_length) {
    TRACE_PRINT("Flonum! %g", floval);
    atom->tag = GEXP_TYPE_FLONUM;
    atom->val.flonum = floval;
    return true;
  }
  return false;
}

gexp gp_build_atom(const char *token) {
  gexp atom = memory_get_next(&top_heap);
  int token_length = strlen(token);

  if (gp_parse_fixval(token, token_length, atom))
    return atom;

  if (gp_parse_flonum(token, token_length, atom))
    return atom;

  /* symbol */
  atom->tag = GEXP_TYPE_SYMBOL;
  atom->val.symbol = Gstr(token, token_length);

  TRACE_PRINT("Symbol! %s", Gstring_get_str(&atom->val.symbol));
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
    size_t token_length = strlen(token);

    if (token[0] == ')') {
      current = memory_get_next(&top_heap);
      current->tag = GEXP_TYPE_VOID;

      if (!prev) {
        gexp_error("Syntax error!: ')'");
        return 0;
      }

      assert(Gpairp(prev));
      prev->val.pair.cdr = current;
      return start;

    } else if (token[0] == '(') {
      current = memory_get_next(&top_heap);
      current->tag = GEXP_TYPE_PAIR;
      current->val.pair.car = gp_parse_expr(tokens);

    } else {

      /* atom */
      current = memory_get_next(&top_heap);
      current->tag = GEXP_TYPE_PAIR;

      gexp atom = gp_build_atom(token);
      current->val.pair.car = atom;
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

gexp Geval(gexp e) {
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

  top_heap = memory_create_heap(1024);
  top_stack = memory_create_stack(128);

  Gstring ret = gp_read_and_eval(input);
  printf("%s\n", Gstrget(&ret));
  return 0;
}
