#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char * argcv[]) {
  int pid;
  int fd[4]; // use 2 pipe
  // 0 : id 1 read
  // 1 : id 1 write

  // 2 : id 2 read
  // 3 : id 2 write
  char c = 'c';

  pipe(&fd[0]);
  pipe(&fd[2]);

  pid = fork();
  if (pid == 0) {
    // child
    close(fd[1]);

    close(fd[2]);
    read(fd[0], &c, 1);
    printf("%d: received ping\n", getpid());
    write(fd[3], &c, 1);
    exit(0);
  } else {
    // parent
    close(fd[0]);
    close(fd[3]);
    write(fd[1], &c, 1);
    read(fd[2], &c, 1);
    printf("%d: received pong\n", getpid());
    exit(0);
  }

  return 0;
}