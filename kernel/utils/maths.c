#include "utils/maths.h"

unsigned int ceiling(unsigned int dividend, unsigned int divisor) { return (dividend + (divisor / 2)) / divisor; }