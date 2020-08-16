#include "utils/spinlock.h"
#include "utils/string.h"
#include "utils/atomic.h"
#include "cpu.h"
#include "builddef.h"

public
inline void spinlock_init(struct spinlock* p) {
    _memset(p, 0, sizeof(*p));
}

public
inline void spinlock_lock(struct spinlock* p) {
    size_t eflags = get_flags_reg();
    p->is_irq_enabled_when_locked = eflags & CPU_EFLAGS_IF;
    if (p->is_irq_enabled_when_locked) {
        asm("cli");
    }
    while (!atomic_compare_exchange(&p->locked, 0, 1))
        ;
}

public
inline void spinlock_unlock(struct spinlock* p) {
    atomic_store(&p->locked, 0);
    if (p->is_irq_enabled_when_locked) {
        asm("sti");
    }
}
