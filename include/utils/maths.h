#ifndef INCLUDE_MATHS_UTILS_H
#define INCLUDE_MATHS_UTILS_H
// Decrease n if it's bigger than 0.
#define DEC_TIL_ZERO(n) (--n < 0 ? 0 : n)

unsigned int ceiling(unsigned int dividend, unsigned int divisor);

#endif
