#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include "header.h"

int verifyUser(int accId, char *pswd, int type)
{
    if (type == 1)
    {
        int fd = open("account.txt", O_RDONLY);
        while (1)
        {
            struct Account a;
            if (read(fd, &a, sizeof(struct Account)) == 0)
                break;
            if (a.accId == accId)
            {
                int res = strcmp(a.accPswd, pswd);
                if (res == 0)
                    return 0;
                else
                    return -1;
            }
        }
    }
    else if (type == 2)
    {
        int fd = open("Jointacc.txt", O_RDONLY);
        while (1)
        {
            struct jointAccount a;
            if (read(fd, &a, sizeof(struct jointAccount)) == 0)
                break;
            if (a.accId == accId)
            {
                int res = strcmp(a.accPswd, pswd);
                if (res == 0)
                    return 0;
                else
                    return -1;
            }
        }
    }
    return -1;
}

int verifyAdmin(int id, char *pswd)
{
    int fd = open("admin.txt", O_RDONLY);
    while (1)
    {
        struct admin a;
        if (read(fd, &a, sizeof(struct Account)) == 0)
            break;
        if (a.id == id)
        {
            int res = strcmp(a.pswd, pswd);
            if (res == 0)
                return 0;
            else
                return -1;
        }
    }
    return -1;
}

void Deposit(float amount, int accId, int accountType)
{
    if (accountType == 1)
    {
        int fd = open("account.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);
        while (1)
        {
            struct Account a;
            if (read(fd, &a, sizeof(a)) == 0)
                break;
            if (a.accId == accId)
            {
                a.balance += amount;
                lseek(fd, -1 * sizeof(struct Account), SEEK_CUR); 
                write(fd, &a, sizeof(a));
                break;
            }
        }
        close(fd);
    }
    else
    {
        int fd = open("Jointacc.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);

        while (1)
        {
            struct jointAccount ja;
            if (read(fd, &ja, sizeof(struct jointAccount)) == 0)
                break;
            if (ja.accId == accId)
            {
                struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = lseek(fd, -1 * sizeof(struct jointAccount), SEEK_CUR); 
                lock.l_len = sizeof(struct jointAccount);
                lock.l_pid = getpid();
                fcntl(fd, F_SETLKW, &lock);
                ja.balance += amount;
                lseek(fd, 0, SEEK_CUR); 
                write(fd, &ja, sizeof(struct jointAccount));
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                break;
            }
        }
    }
}

int Withdraw(float amount, int accId, int accountType)
{
    if (accountType == 1)
    {
        int fd = open("account.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);
        while (1)
        {
            struct Account a;
            if (read(fd, &a, sizeof(a)) == 0)
                break;
            if (a.accId == accId)
            {
                if (a.balance - amount >= 0)
                {
                    a.balance -= amount;
                    lseek(fd, -1 * sizeof(struct Account), SEEK_CUR); 
                    write(fd, &a, sizeof(a));
                    break;
                }
                else
                    return -1;
            }
        }
        return 0;
        close(fd);
    }
    else
    {
        int fd = open("Jointacc.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);

        while (1)
        {
            struct jointAccount ja;
            if (read(fd, &ja, sizeof(struct jointAccount)) == 0)
                break;
            if (ja.accId == accId)
            {
                struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = lseek(fd, -1 * sizeof(struct jointAccount), SEEK_CUR);
                lock.l_len = sizeof(struct jointAccount);
                lock.l_pid = getpid();
                fcntl(fd, F_SETLKW, &lock);
                ja.balance -= amount;
                lseek(fd, 0, SEEK_CUR); 
                write(fd, &ja, sizeof(struct jointAccount));
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                break;
            }
        }
    }
}

float balanceEnquiry(int accId, int accountType)
{
    float bal = -1;
    if (accountType == 1)
    {
        int fd = open("account.txt", O_RDONLY);
        lseek(fd, 0, SEEK_SET);
        while (1)
        {
            struct Account a;
            if (read(fd, &a, sizeof(a)) == 0)
                break;
            if (a.accId == accId)
            {
                bal = a.balance;
                break;
            }
        }
        close(fd);
    }
    else
    {
        int fd = open("Jointacc.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);

        while (1)
        {
            struct jointAccount ja;
            if (read(fd, &ja, sizeof(struct jointAccount)) == 0)
                break;
            if (ja.accId == accId)
            {
                struct flock lock;
                lock.l_type = F_RDLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = lseek(fd, -1 * sizeof(struct jointAccount), SEEK_CUR);
                lock.l_len = sizeof(struct jointAccount);
                lock.l_pid = getpid();
                fcntl(fd, F_SETLKW, &lock);
                bal = ja.balance;
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                break;
            }
        }
    }
}

void changePassword(int accId, char *newPswd, int accountType)
{
    if (accountType == 1)
    {
        int fd = open("account.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);
        while (1)
        {
            struct Account a;
            if (read(fd, &a, sizeof(a)) == 0)
                break;
            if (a.accId == accId)
            {
                strcpy(a.accPswd, newPswd);
                lseek(fd, -1 * sizeof(struct Account), SEEK_CUR); 
                write(fd, &a, sizeof(a));
                break;
            }
        }
        close(fd);
    }
    else
    {
        int fd = open("Jointacc.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);

        while (1)
        {
            struct jointAccount ja;
            if (read(fd, &ja, sizeof(struct jointAccount)) == 0)
                break;
            if (ja.accId == accId)
            {
                struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = lseek(fd, -1 * sizeof(struct jointAccount), SEEK_CUR); 
                lock.l_len = sizeof(struct jointAccount);
                lock.l_pid = getpid();
                fcntl(fd, F_SETLKW, &lock);
                strcpy(ja.accPswd, newPswd);
                lseek(fd, 0, SEEK_CUR); 
                write(fd, &ja, sizeof(struct jointAccount));
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                break;
            }
        }
    }
}

struct newAcc viewDetails(int accId, int accountType)
{
    struct newAcc na;
    if (accountType == 1)
    {
        int fd = open("account.txt", O_RDONLY);
        lseek(fd, 0, SEEK_SET);
        while (1)
        {
            struct Account a;
            if (read(fd, &a, sizeof(a)) == 0)
                break;
            if (a.accId == accId)
            {
                na.accId = accId;
                na.custId = a.custId;
                na.custId1 = -1;
                na.custId2 = -1;
                na.balance = a.balance;
                return na;
            }
        }
        close(fd);
    }
    else
    {
        int fd = open("Jointacc.txt", O_RDWR);
        lseek(fd, 0, SEEK_SET);

        while (1)
        {
            struct jointAccount ja;
            if (read(fd, &ja, sizeof(struct jointAccount)) == 0)
                break;
            if (ja.accId == accId)
            {
                struct flock lock;
                lock.l_type = F_RDLCK;
                lock.l_whence = SEEK_SET;
                lock.l_start = lseek(fd, -1 * sizeof(struct jointAccount), SEEK_CUR);
                lock.l_len = sizeof(struct Account);
                lock.l_pid = getpid();
                fcntl(fd, F_SETLKW, &lock);
                na.accId = ja.accId;
                na.balance = ja.balance;
                na.custId = -1;
                na.custId1 = ja.custId1;
                na.custId2 = ja.custId2;
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                return na;
            }
        }
    }
}

void addAccount(int custId, int accId, char *pswd, float bal)
{
    struct Account a;
    int fd = open("account.txt", O_RDWR);
    while(read(fd, &a, sizeof(struct Account)))
    {
        if(a.accId == accId) return;
    }
    struct Account a1;
    lseek(fd, 0, SEEK_END);
    a1.custId = custId;
    a1.accId = accId;
    strcpy(a1.accPswd, pswd);
    a1.balance = bal;
    write(fd, &a1, sizeof(a1));
}

void removeAccount(int accId)
{
    int fd = open("account.txt", O_RDWR);
    struct Account a;
    while (read(fd, &a, sizeof(a)))
    {
        if (a.accId == accId)
        {
            a.accId = -1; // it means invalid account i.e. account doesn't exist
            a.custId = -1;
            a.balance = -1;
            strcpy(a.accPswd, "");
            lseek(fd, -1 * sizeof(struct Account), SEEK_CUR);
            write(fd, &a, sizeof(a));
            break;
        }
    }
}

void modifyAccBal(int accId, float newBal)
{
    int fd = open("account.txt", O_RDWR);
    struct Account a;
    while (read(fd, &a, sizeof(a)))
    {
        if (a.accId == accId)
        {
            a.balance = newBal;
            lseek(fd, -1 * sizeof(struct Account), SEEK_CUR);
            write(fd, &a, sizeof(a));
            break;
        }
    }
}

float searchAccBal(int accId)
{
    int fd = open("account.txt", O_RDWR);
    struct Account a;
    while (read(fd, &a, sizeof(a)))
    {
        if (a.accId == accId)
        {
            return a.balance;
            break;
        }
    }
    return -1;
}
int searchCustId(int accId)
{
    int fd = open("account.txt", O_RDWR);
    struct Account a;
    while (read(fd, &a, sizeof(a)))
    {
        if (a.accId == accId)
        {
            return a.custId;
            break;
        }
    }
    return -1;
}

int main()
{
    struct sockaddr_in serv, cli;

    int sd = socket(AF_INET, SOCK_STREAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(5000);

    bind(sd, (void *)&serv, sizeof(serv));
    listen(sd, 5);
    int type;
    int sizeCli = sizeof(cli);
    while (1)
    {
        int nsd = accept(sd, (void *)&cli, &sizeCli);
        if (!fork())
        {
            close(sd);
            read(nsd, &type, sizeof(type));
            if (type == 1 || type == 2)
            {
                struct buf buff;
                read(nsd, &buff, sizeof(struct buf));
                int x = verifyUser(buff.accId, buff.pswd, type);
                write(nsd, &x, sizeof(int));
                if (x == 0)
                {
                    while (1)
                    {
                        int choice;
                        read(nsd, &choice, sizeof(int));
                        if (choice == 1)
                        {
                            float amount;
                            read(nsd, &amount, sizeof(int));
                            Deposit(amount, buff.accId, type);
                        }
                        else if (choice == 2)
                        {
                            float amount;
                            read(nsd, &amount, sizeof(int));
                            int ret = Withdraw(amount, buff.accId, type);
                            write(nsd, &ret, sizeof(int));
                        }
                        else if (choice == 3)
                        {
                            float bal = balanceEnquiry(buff.accId, type);
                            write(nsd, &bal, sizeof(float));
                        }
                        else if (choice == 4)
                        {
                            char newP[7];
                            read(nsd, &newP, sizeof(newP));
                            changePassword(buff.accId, newP, type);
                        }
                        else if (choice == 5)
                        {
                            struct newAcc na = viewDetails(buff.accId, type);
                            write(nsd, &na, sizeof(struct newAcc));
                        }
                    }
                }
            }

            else if (type == 3)
            {
                struct buf buff;
                struct Account buf2;
                read(nsd, &buff, sizeof(struct buf));
                int x = verifyAdmin(buff.accId, buff.pswd);
                write(nsd, &x, sizeof(int));
                if (x == 0)
                {
                    while (1)
                    {
                        int choice;
                        read(nsd, &choice, sizeof(int));
                        if (choice == 1)
                        {
                            read(nsd, &buf2, sizeof(struct Account));
                            addAccount(buf2.custId, buf2.accId, buf2.accPswd, buf2.balance);
                        }
                        else if (choice == 2)
                        {
                            int id;
                            read(nsd, &id, sizeof(int));
                            removeAccount(id);
                        }
                        else if (choice == 3)
                        {
                            struct id_bal ib;
                            read(nsd, &ib, sizeof(ib));
                            modifyAccBal(ib.id, ib.bal);
                        }
                        else if (choice == 4)
                        {
                            int id, choice2;
                            read(nsd, &id, sizeof(int));
                            read(nsd, &choice2, sizeof(int));
                            if (choice2 == 1)
                            {
                                float bal = searchAccBal(id);
                                write(nsd, &bal, sizeof(float));
                            }
                            else if (choice2 == 2)
                            {
                                int custId = searchCustId(id);
                                write(nsd, &custId, sizeof(int));
                            }
                        }
                    }
                }
            }
            exit(1);
        }
        else
            close(nsd);
    }
}