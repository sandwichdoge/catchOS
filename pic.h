#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H
void pic_ack(unsigned int pic_int_num);
void pic_remap(int offset1, int offset2);
#endif
