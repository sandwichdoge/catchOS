#include "sched.h"
#include "tasks.h"
#include "builddef.h"

struct task_struct* _current;

public
void sched_init() {
    // Allocate for default scheduler task.

}

private
void sched_switch_to(struct task_struct* next) {
    /*
    - Thread A enters function.
    - Context-switch happens and becomes Thread B.
    - Exit function as thread B.
    - Maybe switch page tables as well in the future?
    */
    if (_current == next) return;
    struct task_struct* prev = _current;
    _current = next;
    cpu_switch_to(prev, next);
}
