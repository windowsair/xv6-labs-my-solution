# Foreword

For people who are not familiar with *nix programming (like me), they may encounter many problems in this lab. This lab can also be done in operating systems such as Linux. I also did a simple debugging of the program using GDB, although it was mentioned in lecture 3.

# sleep

Don't forget to check the number of parameters. Try to observe the difference between the actual delay and reality.

# pingpong

Don't forget to close unused file descriptors. Without hints, noobs will likely not realize this and observe the weird bugs in the program.

# primes

This part makes me really frustrated. Never lose enthusiasm! Here are some tips:

1. Figure out what to do. Create some child processes, and perform the judgment, transmission and output of prime numbers on the child processed as needed.
2. Figure out how to create a child process. Nested creation? Created only in the parent process? It depends on you. Note that the number of child processes that xv6 can create is limited.
3. Closing the pipe on the write end allows `read` function to get a return value of `0`, which helps control the survival of the child process.
4. Ensure that `wait` is used correctly. Try to modify wait, you may get approximately correct output.

# find

The key is the distinction between file names and path names. `.` and `..` are the two names worthy of attention. `user/ls.c` shows how to read paths and files. You can make `find` work quickly through a simple recursive method.

# xargs

```bash
$ echo 123 | xargs echo 456
456 123
$
```

For those who have never used `xargs` before, the brief version of xargs implements the following functions:


1. Read parameters from the argv of the main function. `{"echo", "456"}"`
2. Append the parameters read from stdin(0) (Read only on line, ending with '\n') `{"123"}`
3. Exec it! `{"echo", "456", "123", 0}`

If you are familiar with C, especially the pointer part, then this lab will be relatively easy.