#ifndef INCLUDE_SHELL_H
#define INCLUDE_SHELL_H
void shell_handle_keypress(unsigned char ascii);
void shell_init(void);
void shell_print(const char* str, unsigned int len);
#endif
