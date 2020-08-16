#ifndef INCLUDE_SPINLOCK_H
#define INCLUDE_SPINLOCK_H

struct spinlock {
    volatile int locked;
    int is_irq_enabled_when_locked;
};

void spinlock_init(struct spinlock* p);
void spinlock_lock(struct spinlock* p);
void spinlock_unlock(struct spinlock* p);

#endif
