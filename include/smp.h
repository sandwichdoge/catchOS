#ifndef INCLUDE_SMP_H
#define INCLUDE_SMP_H

void smp_init();
void smp_redirect_external_irq(uint8_t irq, uint8_t dest_cpu);
uint8_t smp_get_cpu_id();

#endif
