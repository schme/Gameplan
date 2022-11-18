#ifndef GAMEPLAN_STRING_H_
#define GAMEPLAN_STRING_H_

#include "types.h"

#include <stdbool.h>

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
 * @param s Initial string value. If not needed, struct can be initialized
 * normally.
 * @param len Length of the string, excluding null terminator.
 * @return New Gstring struct with the initial values set.
 */
Gstring Gstring_create(const char *s, u32 len);

/**
 * Set everything to zero. If needed, deallocate.
 */
void Gstring_destroy(Gstring *s);

/**
 * Get the underlying string buffer
 */
const char *Gstring_get_str(Gstring *s);

/**
 * Append char buffer of length len (non-null terminated) to Gstring.
 *
 * Allocates or reallocates if required.
 */
void Gstring_append(Gstring *dst, const char *s, u32 len);

/**
 * If the Gstring fits into the small buffer
 */
bool Gstring_smallp(Gstring *s);

/**
 * Length of the string (without null terminator)
 */
u32 Gstring_length(Gstring *s);

#if GSTRING_SHORT_NAMES
/* Function shortening wrapper */
static inline Gstring Gstr(const char *s, u32 len) {return Gstring_create(s, len);}
static inline void Gstrdstr(Gstring *s) {Gstring_destroy(s);}
static inline void Gstrcat(Gstring *dst, const char *s, u32 len) {Gstring_append(dst, s, len);}
static inline u32 Gstrlen(Gstring *s) {return Gstring_length(s);}
static inline const char* Gstrget(Gstring *s) {return Gstring_get_str(s);}
static inline bool Gstrsmall(Gstring *s) {return Gstring_smallp(s);}
#endif


#ifdef __cplusplus
}
#endif
#endif
