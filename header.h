struct custInfo{
    int custId;
};

struct Account{
    int custId;
    int accId;
    float balance;
    char accPswd[7];
};

struct jointAccount{
    int custId1, custId2;
    int accId;
    float balance;
    char accPswd[7];
};

struct admin{
    int id;
    char pswd[7];
};

struct buf{
    int accId;
    char pswd[7];
};

struct newAcc{
    int custId;
    int custId1;
    int custId2;
    float balance;
    int accId;
};

struct id_bal{
    int id;
    float bal;
};