#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H
void pic_ack(unsigned int pic_int_num);
void pic_remap(int offset1, int offset2);
void pic_init(void);
void pic_enable_irq(unsigned int irq_no);
#endif
