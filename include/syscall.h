#ifndef INCLUDE_SYSCALL_H
#define INCLUDE_SYSCALL_H
// Global keyboard press handler. Userland will register with this.
void (*_kb_handler_cb)(unsigned char c);

// Subscribe to keyboard events.
void syscall_register_kb_handler(void (*kb_handler)(unsigned char c));
#endif
