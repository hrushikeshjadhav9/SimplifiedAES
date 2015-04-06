#include <stdio.h>
#include <string.h>
#include <time.h>

#include "SimplifiedAES.h"



int main(int argc, char* argv[])
{
    testing();
}

// Testing
void testing()
{
    State s;
    ByteState bs;
    Key k;
    ExpandedKey ek;
    unsigned int i;
    uint16_t _k;
    uint16_t data;
    int err;
    int st, et;

    // MixColumn test
    for (i = 0; i <= 0xFFFF; i++)
    {
        err = 0;
        memcpy(s, &i, 2);
        StateExpand(s, bs);
        MixColumn(bs, MIXCOLUMN_MATRIX);
        MixColumn(bs, INVERSE_MIXCOLUMN_MATRIX);
        StatePack(bs, s);
        if (*(uint16_t*)s != i)
        {
            printf("Error at %04X\n", i);
            err++;
        }
    }

    if (!err)
    {
        printf("[Module testing] MixColumn() passed\n");
    }

    // AddRoundKey test
    for (i = 0; i <= 0xFFFF; i++)
    {
        err = 0;
        memcpy(s, &i, 2);
        StateExpand(s, bs);
        srand(time(NULL));
        _k = rand();
        memcpy(k, &_k, 2);

        AddRoundKey(bs, k);
        AddRoundKey(bs, k);
        StatePack(bs, s);
        if (*(uint16_t*)s != i)
        {
            printf("Error at %04X\n", i);
            err++;
        }
    }

    if (!err)
    {
        printf("[Module testing] AddRoundKey() passed\n");
    }

    // NibbleSub test
    for (i = 0; i <= 0xFFFF; i++)
    {
        err = 0;
        memcpy(s, &i, 2);
        StateExpand(s, bs);

        NibbleSub(bs, S_BOX);
        NibbleSub(bs, INVERSE_S_BOX);
        StatePack(bs, s);
        if (*(uint16_t*)s != i)
        {
            printf("Error at %04X\n", i);
            err++;
        }
    }

    if (!err)
    {
        printf("[Module testing] NibbleSub() passed\n");
    }

    // ShiftRow test
    for (i = 0; i <= 0xFFFF; i++)
    {
        err = 0;
        memcpy(s, &i, 2);
        StateExpand(s, bs);

        ShiftRow(bs);
        ShiftRow(bs);
        StatePack(bs, s);
        if (*(uint16_t*)s != i)
        {
            printf("Error at %04X\n", i);
            err++;
        }
    }

    if (!err)
    {
        printf("[Module testing] ShiftRow() passed\n");
    }

    // Encryption and Decryption test
    for (i = 0; i <= 0xFFFF; i++)
    {
        err = 0;
        data = i;
        _k = rand();
        memcpy(k, &_k, 2);

//        printf("Data: %04X\t", data);
        Encrypt(&data, k);
//        printf("Encrypted: %04X\t", data);
        Decrypt(&data, k);
//        printf("Decrypted: %04X\t", data);
        if (data != i)
        {
            printf("%04X Not match\n", i);
        }
//        else
//        {
//            printf("Passed\n");
//        }
    }

    if (!err)
    {
        printf("[Integration testing] passed\n");
    }
}
