## 6. Datastructures
Datastructures used by the kernel.

### 6.1 Primitives
Primitive datastructures take advantage of target architecture and does not depend on any multitasking implementation.

#### 6.1.1 Spinlocks
We can do as below. This code is safe for global use. Interrupts are automatically turned off/on inside lock function.
```
spinlock_init(&lock);
...
spinlock_lock(&lock);
// Critical section
...
spinlock_unlock(&lock);
```
IMPORTANT: If a spinlock is used in an interrupt handler, we must first disable _local_ irqs, 
otherwise we might get deadlock if the same interrupt happens on the same CPU:
```
spin_lock(&lock);
	...
		<- interrupt comes in:
			spin_lock(&lock);
```

#### 6.1.2 Primitive Semaphores
Based off atomic operations. Only use this in interrupt handlers because it's based off spinlocks and is very costly.

#### 6.1.3 Readers/Writers Locks
- Many readers may read an object at the same time concurrently.
- A reader may not read if a writer is in progress.
- A writer may not write if a reader is in progress.
- A writer may not write of another writer is writing.
This is also a primitive datastructure. Only use it in interrupt handlers. If you want to implement a readers/writers lock for 
user applications, use general Semaphores in "General Synchronization Structures".

Interrupts are automatically saved and restored in R/W locks.

```
rwlock_read_acquire(&lock);
// Read something
rwlock_read_release(&lock);
...
rwlock_write_acquire(&lock);
// Modify something
rwlock_write_release(&lock);
```


### 6.2 General Synchronization Structures

#### 6.2.1 Semaphores
After MMU and Tasks have been initialized, we may use semaphores to synchronize between tasks. Do not use this in tasks.c and sem.c. Do not 
use this in interrupt handlers because it does not clear CPU interrupt flag which may cause deadlocks.
