#ifndef INCLUDE_SMP_H
#define INCLUDE_SMP_H

void smp_init();
void smp_redirect_external_irq(uint8_t irq, uint8_t dest_cpu);

// Get total number of CPUs in system.
uint8_t smp_get_cpu_count();

// Get local CPU's ID
uint8_t smp_get_cpu_id();

#endif
