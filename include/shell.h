#ifndef INCLUDE_SHELL_H
#define INCLUDE_SHELL_H
void shell_handle_keypress(unsigned char ascii);
void shell_init(void);
void shell_setpos(unsigned int scrpos);
void shell_cout(const char* str, unsigned int len);
void shell_cin(char* out);
void shell_main();
#endif
