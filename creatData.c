#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "header.h"

int main()
{
    int fd = open("custInfo.txt", O_CREAT | O_RDWR, 0744);
    if (fd == -1)
    {
        printf("Error while opening the file!\n");
        exit(1);
    }
    
    for (int i = 1; i < 6; i++)
    {
        struct custInfo c;
        c.custId = i;
        lseek(fd, 0, SEEK_END);
        write(fd, &c, sizeof(c));
    }

    close(fd);

    fd = open("account.txt", O_CREAT | O_RDWR, 0744);
    if (fd == -1)
    {
        printf("Error while opening the file!\n");
        exit(1);
    }

    char accpswd[5][7] = {"accnt1", "accnt2", "accnt3", "accnt4", "accnt5"};
    for (int i = 1; i < 6; i++)
    {
        struct Account a;
        strcpy(a.accPswd, accpswd[i - 1]);
        a.accId = i;
        a.balance = i * 10000;
        a.custId = i;
        lseek(fd, 0, SEEK_END);
        write(fd, &a, sizeof(a));
    }

    close(fd);

    fd = open("Jointacc.txt", O_CREAT | O_RDWR, 0744);
    if (fd == -1)
    {
        printf("Error while opening the file!\n");
        exit(1);
    }

    struct jointAccount ja;
    ja.custId1 = 1;
    ja.custId2 = 2;
    ja.accId = 1;
    ja.balance = 100000;

    strcpy(ja.accPswd, "acnt12");
    write(fd, &ja, sizeof(ja));

    close(fd);

    fd = open("admin.txt", O_CREAT | O_RDWR, 0744);
    if (fd == -1)
    {
        printf("Error while opening the file!\n");
        exit(1);
    }

    struct admin a;
    a.id = 1;
    strcpy(a.pswd, "admin1");
    write(fd, &a, sizeof(a));
    
    close(fd);
}