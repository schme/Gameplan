#ifndef GAMEPLAN_COMMON_H_
#define GAMEPLAN_COMMON_H_

#ifdef NDEBUG
#ifndef TRACE_ENABLE
#define TRACE_ENABLE 0
#endif
#else
#ifndef TRACE_ENABLE
#define TRACE_ENABLE 1
#endif
#endif

#define TRACE_PRINT(F, X)                                                      \
  do {                                                                         \
    if (TRACE_ENABLE)                                                          \
      fprintf(stderr, "%s:%lu: " F "\n", __func__, __LINE__ + 0UL, X);         \
  } while (0)

#endif
