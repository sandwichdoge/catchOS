#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H
void outb(unsigned short port, unsigned char data);
void outw(unsigned short port, unsigned short data);
unsigned char inb(unsigned short port);
void io_wait(void);
#endif
