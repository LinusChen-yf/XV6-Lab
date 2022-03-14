#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int 
main()
{
    int pid;
    pid = fork();

    if (pid == 0)
    {
        close(1);
        open("output.txt", O_WRONLY | O_CREATE);
        char *argv[] = {"echo", "this", "is", "rediect", "echo", 0};
        exec("echo", argv);
        printf("exec failed!\n");
    } else {
        wait((int *) 0);
    }
    exit(0);
}