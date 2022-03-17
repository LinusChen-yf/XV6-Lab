#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define RD 0
#define WR 1

int
main()
{
    char buf = 'p';
    int pipe_c2p[2], pipe_p2c[2];
    pipe(pipe_c2p);
    pipe(pipe_p2c);

    int pid = fork();
    int exit_status = 0;
    if (pid < 0)
    {
        fprintf(2, "fork() error!\n");
        close(pipe_c2p[RD]);
        close(pipe_c2p[WR]);
        close(pipe_p2c[RD]);
        close(pipe_p2c[WR]);
        exit_status =1;
    } else if (pid == 0){ // child process
        close(pipe_c2p[RD]);
        close(pipe_p2c[WR]);

        if (read(pipe_p2c[RD], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "parent to child read error!\n");
            exit_status = 1;
        }
        else {
            fprintf(1, "%d: received ping\n", getpid());
        }
        
        if (write(pipe_c2p[WR], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "child to parent write error!\n");
            exit_status = 1;
        }

        close(pipe_c2p[WR]);
        close(pipe_p2c[RD]);
    } else{ // parent process
        close(pipe_p2c[RD]);
        close(pipe_c2p[WR]);

        if (write(pipe_p2c[WR], &buf, sizeof(char)) != sizeof(char)){
            fprintf(2, "parent to child write error!\n");
            exit_status = 1;
        }

        if (read(pipe_c2p[RD], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "child to parent read error!\n");
            exit_status = 1;
        }
        else {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(pipe_p2c[WR]);
        close(pipe_c2p[RD]);
    }
    exit(exit_status);
}