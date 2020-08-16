## 6. Datastructures
Datastructures used by the kernel.

### 6.1 Spinlocks
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
