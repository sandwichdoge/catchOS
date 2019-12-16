/* This is a way to let userland applications (such as shell)
 * subscribe to appropriate interrupts (such as keyboard press).
 * */

#include "syscall.h"

void syscall_register_kb_handler(void (*kb_handler)(unsigned char c)) {
    _kb_handler_cb = kb_handler;
}
