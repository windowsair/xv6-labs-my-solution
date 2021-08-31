# Foreword


# Uthread

What we have to do is the same as what `swtch` does in the kernel. Similarly, `thread_switch` needs to save/restore only the callee-save registers, so we can reuse the code in `swtch.s`.

Another question is the operation that needs to be considered when creating a thread. From the code point of view, when the thread is switched, the `ret` assembly instruction will cause the `PC` register to jump to the address saved in the `ra` register. Therefore, in order to ensure that the thread enters the entry point when switching for the first time, we should modify the `ra` in the save structure to the entry point when creating the thread, which is the corresponding function pointer address.

In addition, the thread needs to have its own stack, which has been given in the thread structure, and we need to specify the `sp` register. Note that the RISC-V stack is extended from the high address to the low address, so the `sp` register needs to be set to the highest address of the thread stack.


```cpp
t->context.ra = (uint64)func;
t->context.sp = (uint64)&t->stack[STACK_SIZE];
```


# Using threads

By reading the code, it is not difficult to find possible race problems in the `put` operation. One of the most straightforward ways to ensure that only one `put` operation can be executed at the same time is through a `mutex`:

```cpp
pthread_mutex_lock(&lock);       // acquire lock
put(keys[b*n + i], n);
pthread_mutex_unlock(&lock);     // release lock
```

You can see that this is similar to `acquire` and `release` in xv6 (actually not the same). This is actually a coarse-grained locking operation. To speed up this process, we can consider adding locks to each hash bucket individually.

```cpp
void put() {
    int i = key % NBUCKET;

    pthread_mutex_lock(&lock[i]);
    // ... put operation
    pthread_mutex_unlock(&lock[i]);
}

```

# Barrier

`pthread_cond_wait` and `pthread_cond_broadcast` are analogous to `sleep` and `wakeup` in xv6, which have similar semantics. They need to be used with `mutex`.

> pthread_cond_wait releases the mutex when called, and re-acquires the mutex before returning.

```cpp
void wait_thread() {
    pthread_mutex_lock(&mutex_lock);
    //   -----< mutex_lock locked.
    pthread_cond_wait(&cond, &mutex_lock); // unlock mutx_lock, then go to sleep
    //   -----< mutex_lock locked.

    // do something...

    pthread_mutex_unlock(&mutex_lock);
}


void notify_thread() {
    // ...
    pthread_cond_broadcast(&cond); // wake up every thread sleeping on cond
}

```