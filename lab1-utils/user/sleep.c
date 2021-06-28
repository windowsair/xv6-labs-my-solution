#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
  int i;
  // In the test case, it is assumed that the parameters are legal.
  if (argc != 2) {
    fprintf(2, "Usage: sleep [time to sleep: Unit 100ms]\n");
    exit(1);
  }

  i = atoi(argv[1]);
  sleep(i);

  exit(0);

}