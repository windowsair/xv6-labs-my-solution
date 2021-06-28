#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 1. Read parameters from the argv of the main function
// 2. After this append the parameters read from stdin(0) (Read only on line, ending with \n)

char * parameter[128] = { 0 };
int cnt = 0;


void copy_parameter(char *left, char *right) {
    int str_length = right - left + 1;
    parameter[cnt] = malloc(str_length + 1);

    char *p = parameter[cnt];
    memmove(p, left, str_length);
    p[str_length] = '\0';

    cnt++;
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs parameter\n");
        exit(1);
    }
    // step1
    for (int i = 1; i < argc; i++) {
        printf("[%s]\n", argv[i]);
        parameter[cnt++] = argv[i];
    }

    // step2

    // read stdin
    char * buf = malloc(1023 + 1);
    char * p = buf;
    int n = 0;
    while (read(0, p, 1)) {
        if (*p++ == '\n') {
            break;
        }

        if (++n > 1023) {
            fprintf(2, "[Error] parameter too long.\n");
            exit(1);
        }
    }
    *p = '\0';

    char *left = 0;
    char *right = 0;
    p = buf;
    // split blank
    while (n--) {
        if (left == 0 && *p != ' ' && *p != '\t') {
            left = p;
        }

        if (left != 0 && (*p == ' ' || *p == '\t')) {
            right = p - 1;
            copy_parameter(left, right); // length: (right - left + 1) + 1('\0')
            // clean
            left = 0;
        }
        p++;
    }
    if (left != 0) {
        right = p - 1;
        copy_parameter(left, right);
    }

    // step3: exec!
    int pid = fork();
    if (pid == 0) {
        // child
        printf("going\n");
        for (int i = 0; i < 10; i++) {
            printf("[[%s]]\n", parameter[i]);
        }
        exec(parameter[0], parameter);
        fprintf(2,"[ERROR] exec failed!\n");
        exit(1);
    } else {
        wait(0);
        exit(0);
    }
    return 0;
}

