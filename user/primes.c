#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

int first_prime(int lpipe[2], int *first_data){
    if (read(lpipe[RD], first_data, INT_LEN) == INT_LEN)
    {
        printf("prime %d\n", *first_data);
        return 0;
    }
    return -1;
}

void transmit_data(int rpipe[2], int lpipe[2], int first_data){
    int data;
    while (read(lpipe[RD], &data, INT_LEN) == INT_LEN)
    {
        if (data % first_data)
            write(rpipe[WR], &data, INT_LEN);
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

void primes(int lpipe[2]){
    int first_data;
    if (first_prime(lpipe, &first_data) == 0){
        int rpipe[2];
        pipe(rpipe);
        transmit_data(rpipe,lpipe,first_data);
        if (fork() == 0)
        {
            primes(rpipe);
        } else{
            close(rpipe[RD]);
            wait((int *) 0);
        }
    }
    exit(0);
}

int
main()
{
    int p[2];
    pipe(p);
    for (int i  = 2; i < 36; i++)   
        write(p[WR], &i, INT_LEN);

    close(p[WR]);
    if (fork() == 0){
        primes(p);
    } else{
        close(p[RD]);
        wait( (int *) 0);
    }
    exit(0);
}