#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXID 6


int is_prime(int num, int child_id) {
  if (num == 2)
    return 1;
  for (int i = child_id * 6; i < child_id * 6 + 6 && i < num; i++) {
    if (i == 1 || i == 0) {
      continue;
    }
    if (num % i == 0) {
      if (num == 2) {
        printf("no!!");
      }
      return 0;
    }
  }
  return 1;
}


void create_process(int id, int * fd) {
  int child_fd[2];
  int pid;
  int num;

  close(fd[1]); // close write first

  if (id == MAXID) {
    // Just read.
    // Prime numbers will be output at the end of the right side of the pipeline
    while (read(fd[0], &num, 4)) {
      if (is_prime(num, MAXID)) {
        printf("prime %d\n", num);
      }
    }
    exit(0);
  } else {
    // self
    pipe(&child_fd[0]);

    pid = fork();

    if (pid == 0) {
      // new child
      create_process(id + 1, &child_fd[0]);
    } else {
      // self
      close(child_fd[0]);
      sleep(1);
      while(read(fd[0], &num, 4)) {
        if (is_prime(num, id)) {
          write(child_fd[1], &num, 4); // pass to next
        }
      }

      close(child_fd[1]); // like top father we do
      wait(0);
      exit(0);
    }

  }

}


int main(int argc, char *argv[]) {
  int pid;
  int fd[2];

  pipe(&fd[0]);
  pid = fork();

  if (pid == 0) {
    // child
    create_process(0, &fd[0]);
  } else {
    // top father
    // This is a convention, the program will behave strangely if we don’t do this
    close(fd[0]);

    for (int i = 2; i <= 35; i++) {
      if (write(fd[1], &i, 4) != 4) {
        fprintf(2, "[Error] Failed to write %d\n", i);
        exit(1);
      }
    }

    close(fd[1]);
    // When we close the write end, the read end will get a return value of zero.
    wait(0);
    exit(0);
  }

  return 0;
}