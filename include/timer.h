#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H
int timer_init();
// Get clock tick count from PIT.
unsigned int getticks();
void delay(unsigned int ms);
#endif
