#ifndef GAMEPLAN_STRING_H_
#define GAMEPLAN_STRING_H_

#include "types.h"

#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def GSTRING_SMALL_SIZE
 * Size of the inner buffer. Play with this to optimize for use case
 */
#ifndef GSTRING_SMALL_SIZE
#define GSTRING_SMALL_SIZE 7
#endif

#ifndef GSTRING_SHORT_NAMES
#define GSTRING_SHORT_NAMES 1
#endif

/**
 * \struct Gstring
 * String type with a small internal buffer and dynamic allocation.
 *
 * Will store a null terminated byte even in the smaller buffer. Doesn't
 * assume a null terminated byte as input, but guarantees one when accessing.
 * */
typedef struct Gstring Gstring;
struct Gstring {
  u32 len;  /* without null termination */
  u32 size; /* allocated size */
  char small[GSTRING_SMALL_SIZE];
  char *s;
};

/**
 * Create a string of length len (excluding null terminator)
 *
 * Allocates if the small buffer is not enough to fit the string.
 * @param s Initial string value. If not needed, struct can be initialized with {0}
 * @param len Length of the string, excluding null terminator.
 * @return New Gstring struct with the initial values set.
 */
Gstring Gstring_create_len(const char *s, u32 len);
/**
 * Create a string from a null terminated C-string
 */
static inline Gstring Gstring_create(const char *s)
{
  return Gstring_create_len(s, strlen(s));
}
/**
 * Set everything to zero. If needed, deallocate. Should be called for every
 * Gstring created.
 */
void Gstring_destroy(Gstring *gs);
/**
 * Get the underlying string buffer
 */
const char *Gstring_get_str(Gstring *gs);
/**
 * If the Gstring fits into the small buffer
 */
bool Gstring_smallp(Gstring *gs);
/**
 * Append char buffer of length len (non-null terminated) to Gstring.
 *
 * Allocates or reallocates if required.
 */
void Gstring_append_len(Gstring *gs, const char *s, u32 len);
/**
 * Append null terminated c-string
 */
static inline void Gstring_append(Gstring *gs, const char *s) {
  Gstring_append_len(gs, s, strlen(s));
}
/**
 * Length of the string (without null terminator)
 */
u32 Gstring_length(Gstring *gs);
/**
 * Lexical comparison with a C-string
 */
i32 Gstring_compare(Gstring *gs, const char *s); 


#if GSTRING_SHORT_NAMES
/* Function shortening wrapper */
static inline Gstring Gstr(const char *s) {return Gstring_create(s);}
static inline Gstring Gstrl(const char *s, u32 len) {return Gstring_create_len(s, len);}
static inline void Gstrdstr(Gstring *gs) {Gstring_destroy(gs);}
static inline const char* Gstrget(Gstring *gs) {return Gstring_get_str(gs);}
static inline bool Gstrsmall(Gstring *gs) {return Gstring_smallp(gs);}
static inline void Gstrcatl(Gstring *gs, const char *s, u32 len) {Gstring_append_len(gs, s, len);}
static inline void Gstrcat(Gstring *gs, const char *s) {Gstring_append(gs, s);}
static inline u32 Gstrlen(Gstring *gs) {return Gstring_length(gs);}
static inline i32 Gstrcmp(Gstring *gs, const char *s) {return Gstring_compare(gs,s);}
#endif


#ifdef __cplusplus
}
#endif
#endif
