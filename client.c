#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include "header.h"

int main()
{
    struct sockaddr_in serv;

    int sd = socket(AF_INET, SOCK_STREAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(5000);

    int ret = connect(sd, (void *)&serv, sizeof(serv));
    if (ret < 0)
    {
        printf("Error while connecting to server!\n");
        return -1;
    }
    if (ret == 0)
    {
        printf("You are successfully connected to server!\n");
        printf("Enter any one of the following choice:\n");
        printf("Login as:\n");
        printf("1. Normal User\n");
        printf("2. Joint Account User\n");
        printf("3. Administrator\n");
        printf("Your choice: ");
        int choice, type, j = 0;
        scanf("%d", &type);
        write(sd, &type, sizeof(type));
        if (type == 1 || type == 2)
        {
            int accId;
            char pswd[7];
            int x;
            printf("Enter accountId: ");
            scanf("%d", &accId);
            printf("Enter accountPswd: ");
            scanf("%s", pswd);
            struct buf buff;
            buff.accId = accId;
            strcpy(buff.pswd, pswd);
            write(sd, &buff, sizeof(buff));
            read(sd, &x, sizeof(int));
            if (x == 0)
            {
                while (1)
                {
                    j++;
                    if (j == 1)
                    {
                        if (type == 1)
                            printf("You are now logged in as Normal User!\n\n");
                        else
                            printf("You are now logged in as Joint account user!\n\n");
                        printf("Enter any one of the following choice:\n");
                        printf("1. Deposit\n");
                        printf("2. Withdraw\n");
                        printf("3. Balance Enquiry\n");
                        printf("4. Password change\n");
                        printf("5. View Details\n");
                        printf("6. Exit\n\n");
                        printf("Your choice: ");
                    }

                    scanf("%d", &choice);
                    write(sd, &choice, sizeof(int));
                    if (choice == 1)
                    {
                        float amt;
                        printf("Enter the amount you want to deposit: ");
                        scanf("%f", &amt);
                        write(sd, &amt, sizeof(float));
                    }
                    else if (choice == 2)
                    {
                        float amt;
                        printf("Enter the amount you want to withdraw: ");
                        scanf("%f", &amt);
                        write(sd, &amt, sizeof(float));
                        read(sd, &ret, sizeof(int));
                        if (ret == -1)
                            printf("Error while withdrawing\n");
                    }
                    else if (choice == 3)
                    {
                        float bal;
                        read(sd, &bal, sizeof(float));
                        printf("Balance: %f\n", bal);
                    }
                    else if (choice == 4)
                    {
                        char newP[7];
                        printf("Enter new passsword: ");
                        scanf("%s", newP);
                        write(sd, &newP, sizeof(newP));
                    }
                    else if (choice == 5)
                    {
                        struct newAcc na;
                        read(sd, &na, sizeof(na));
                        printf("Account ID: %d", na.accId);
                        printf(" Account Balance: %f", na.balance);
                        if (na.custId != -1)
                            printf(" Customer ID: %d\n", na.custId);
                        if (na.custId1 != -1 && na.custId2 != -1)
                        {
                            printf(" Customer1 ID: %d", na.custId1);
                            printf(" Customer2 ID: %d\n", na.custId2);
                        }
                    }
                    else
                        exit(1);
                    printf("Your next choice: ");
                }
            }
            else
                printf("Incorrect login details!\n");
        }

        else if (type == 3)
        {
            int j = 0;
            int adminId;
            char pswd[7];
            printf("Enter adminId: ");
            scanf("%d", &adminId);
            printf("Enter adminPswd: ");
            scanf("%s", pswd);
            struct buf buff;
            buff.accId = adminId;
            strcpy(buff.pswd, pswd);
            write(sd, &buff, sizeof(struct buf));
            int x;
            read(sd, &x, sizeof(int));
            if (x == 0)
            {
                while (1)
                {
                    j++;
                    if (j == 1)
                    {
                        printf("You are now logged in as Admin!\n\n");
                        printf("Enter any one of the following choice:\n");
                        printf("1. Add account\n");
                        printf("2. Delect account\n");
                        printf("3. Modify account\n");
                        printf("4. Search account\n");
                        printf("5. Exit\n\n");
                        printf("Your choice: ");
                    }
                    scanf("%d", &choice);
                    write(sd, &choice, sizeof(int));
                    if (choice == 1)
                    {
                        struct Account pb;
                        float bal;
                        int id, accId;
                        char pswd[7];
                        printf("Enter customer id: ");
                        scanf("%d", &id);
                        printf("Enter account id: ");
                        scanf("%d", &accId);
                        printf("Enter new account's pswd: ");
                        scanf("%s", pswd);
                        printf("Enter new account's balance: ");
                        scanf("%f", &bal);
                        pb.accId = accId;
                        pb.custId = id;
                        pb.balance = bal;
                        strcpy(pb.accPswd, pswd);
                        write(sd, &pb, sizeof(struct Account));
                    }
                    else if (choice == 2)
                    {
                        int id;
                        printf("Enter account's id that you want to delete: ");
                        scanf("%d", &id);
                        write(sd, &id, sizeof(int));
                    }
                    else if (choice == 3)
                    {
                        struct id_bal ib;
                        int id;
                        float bal;
                        printf("Enter accounts id: ");
                        scanf("%d", &id);
                        printf("Enter new accounts balance: ");
                        scanf("%f", &bal);
                        ib.bal = bal;
                        ib.id = id;
                        write(sd, &ib, sizeof(struct id_bal));
                    }
                    else if (choice == 4)
                    {
                        int id;
                        float bal;
                        printf("Enter account id: ");
                        scanf("%d", &id);
                        write(sd, &id, sizeof(int));
                        printf("Enter any one of the following choice:\n");
                        printf("1. Search Accounts Balance\n");
                        printf("2. Search Accounts Customer ID\n");
                        int choice2;
                        printf("Your choice: ");
                        scanf("%d", &choice2);
                        write(sd, &choice2, sizeof(int));
                        if (choice2 == 1)
                        {
                            read(sd, &bal, sizeof(float));
                            if(bal == -1) printf("Account doesn't exist\n");
                            else    printf("Balance: %f\n", bal);
                        }
                        else if (choice2 == 2)
                        {
                            int custId;
                            read(sd, &custId, sizeof(int));
                            if(custId <= 0) printf("Account doesn't exist\n");
                            else printf("Customer ID: %d\n", custId);
                        }
                    }

                    else
                        exit(1);

                    printf("Your next choice: ");
                }
            }
            else
                printf("Invalid login credentials!\n");
        }
    }
    return 0;
}