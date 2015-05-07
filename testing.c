#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "SimplifiedAES.h"

void MixColumn_testing()
{
    State s;
    ByteState bs;

    unsigned int i;
    int err;

    err = 0;

    for (i = 0; i <= 0xFFFF; i++)
    {
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
}

void AddRoundKey_testing()
{
    State s;
    ByteState bs;
    Key k;

    unsigned int i;
    uint16_t _k;
    int err;

    err = 0;

    for (i = 0; i <= 0xFFFF; i++)
    {
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
}

void NibbleSub_testing()
{
    State s;
    ByteState bs;

    unsigned int i;
    int err;

    err = 0;

    for (i = 0; i <= 0xFFFF; i++)
    {
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
}

void ShiftRow_testing()
{
    State s;
    ByteState bs;

    unsigned int i;
    int err;

    err = 0;

    for (i = 0; i <= 0xFFFF; i++)
    {
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
}

void Intergration_testing()
{
    Key k;

    unsigned int i;
    uint16_t _k;
    uint8_t* buffer1;
    uint8_t* buffer2;
    size_t out_length;
    int err;

    err = 0;

    for (i = 0; i <= 0xFFFF; i++)
    {
        _k = rand();

        // Key
        memcpy(k, &_k, 2);

        // Load data
        buffer1 = (uint8_t*)malloc(2);
        memcpy(buffer1, &i, 2);

        // Encryption
        buffer2 = SAES_Encrypt(buffer1, 2, k, &out_length);
        free(buffer1);


        // Decryption
        buffer1 = SAES_Decrypt(buffer2, 4, k, &out_length);

        if (*(uint16_t*)buffer1 != i)
        {
            printf("%04X Not match\n", i);
        }
    }

    if (!err)
    {
        printf("[Integration testing] passed\n");
    }
}

void self_testing()
{
    MixColumn_testing();
    AddRoundKey_testing();
    NibbleSub_testing();
    ShiftRow_testing();

    Intergration_testing();
}
