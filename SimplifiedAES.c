#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "SimplifiedAES.h"


/* S-Box */
const uint8_t S_BOX[] =
{
    0x9, 0x4, 0xA, 0xB,
    0xD, 0x1, 0x8, 0x5,
    0x6, 0x2, 0x0, 0x3,
    0xC, 0xE, 0xF, 0x7
};

/* Inverse S-Box */
const uint8_t INVERSE_S_BOX[] =
{
    0xA, 0x5, 0x9, 0xB,
    0x1, 0x7, 0x8, 0xF,
    0x6, 0x0, 0x2, 0x3,
    0xC, 0x4, 0xD, 0xE
};

const uint8_t MIXCOLUMN_MATRIX[] =
{
    1, 4,
    4, 1
};

const uint8_t INVERSE_MIXCOLUMN_MATRIX[] =
{
    9, 2,
    2, 9
};

/* Round Constants used in key expansion algorithm */
const uint8_t RC[] = {0x80, 0x30};

const uint8_t MULTIPLY_TABLE[16][16] =
{
    {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF},
    {0x0, 0x2, 0x4, 0x6, 0x8, 0xA, 0xC, 0xE, 0x3, 0x1, 0x7, 0x5, 0xB, 0x9, 0xF, 0xD},
    {0x0, 0x3, 0x6, 0x5, 0xC, 0xF, 0xA, 0x9, 0xB, 0x8, 0xD, 0xE, 0x7, 0x4, 0x1, 0x2},
    {0x0, 0x4, 0x8, 0xC, 0x3, 0x7, 0xB, 0xF, 0x6, 0x2, 0xE, 0xA, 0x5, 0x1, 0xD, 0x9},
    {0x0, 0x5, 0xA, 0xF, 0x7, 0x2, 0xD, 0x8, 0xE, 0xB, 0x4, 0x1, 0x9, 0xC, 0x3, 0x6},
    {0x0, 0x6, 0xC, 0xA, 0xB, 0xD, 0x7, 0x1, 0x5, 0x3, 0x9, 0xF, 0xE, 0x8, 0x2, 0x4},
    {0x0, 0x7, 0xE, 0x9, 0xF, 0x8, 0x1, 0x6, 0xD, 0xA, 0x3, 0x4, 0x2, 0x5, 0xC, 0xB},
    {0x0, 0x8, 0x3, 0xB, 0x6, 0xE, 0x5, 0xD, 0xC, 0x4, 0xF, 0x7, 0xA, 0x2, 0x9, 0x1},
    {0x0, 0x9, 0x1, 0x8, 0x2, 0xB, 0x3, 0xA, 0x4, 0xD, 0x5, 0xC, 0x6, 0xF, 0x7, 0xE},
    {0x0, 0xA, 0x7, 0xD, 0xE, 0x4, 0x9, 0x3, 0xF, 0x5, 0x8, 0x2, 0x1, 0xB, 0x6, 0xC},
    {0x0, 0xB, 0x5, 0xE, 0xA, 0x1, 0xF, 0x4, 0x7, 0xC, 0x2, 0x9, 0xD, 0x6, 0x8, 0x3},
    {0x0, 0xC, 0xB, 0x7, 0x5, 0x9, 0xE, 0x2, 0xA, 0x6, 0x1, 0xD, 0xF, 0x3, 0x4, 0x8},
    {0x0, 0xD, 0x9, 0x4, 0x1, 0xC, 0x8, 0x5, 0x2, 0xF, 0xB, 0x6, 0x3, 0xE, 0xA, 0x7},
    {0x0, 0xE, 0xF, 0x1, 0xD, 0x3, 0x2, 0xC, 0x9, 0x7, 0x6, 0x8, 0x4, 0xA, 0xB, 0x5},
    {0x0, 0xF, 0xD, 0x2, 0x9, 0x6, 0x4, 0xB, 0x1, 0xE, 0xC, 0x3, 0x8, 0x7, 0x5, 0xA}
};

/*
 * ShiftRow: Swap ByteState[1] and ByteState[3]
 */
void ShiftRow(ByteState bs)
{
    bs[1] ^= bs[3];
    bs[3] ^= bs[1];
    bs[1] ^= bs[3];
}

/*
 * NibbleSub: Apply a nibble substitute use given matrix
 */
void NibbleSub(ByteState bs, const uint8_t* matrix)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        bs[i] = matrix[bs[i]];
    }
}

/*
 * StateExpand: Expand a 2x2 nibble matrix (2byte) to 2x2 byte matrix (4byte)
 */
void StateExpand(State s, ByteState bs)
{
    int i;

    for (i = 0; i < 2; i++)
    {
        bs[i * 2 + 1] = s[i] >> 4;
        bs[i * 2] = s[i] & 0xF;
    }
}

/*
 * StateExpand: Pack a 2x2 byte matrix (4byte) to 2x2 nibble matrix (2byte)
 */
void StatePack(ByteState bs, State s)
{
    int i;

    for (i = 0; i < 2; i++)
    {
        s[i] = bs[i * 2 + 1] << 4 | bs[i * 2];
    }
}

/*
 * AddRoundKey: Apply a matrix addition(XOR) to State with k
 */
void AddRoundKey(ByteState bs, Key k)
{
    ByteState ek;
    uint32_t* p1;
    uint32_t* p2;

    p1 = (uint32_t*)bs;
    p2 = (uint32_t*)ek;

    StateExpand(k, ek);
    *p1 ^= *p2;
}

/* g: Function used in KeyExpand() */
uint8_t g(uint8_t w)
{
    static uint8_t round = 0;

    // RCon ^ NibbleSub(NibbleRot(w)) == RCon ^ NibbleRot(NibbleSub(w))
    return (S_BOX[w >> 4] | S_BOX[w & 0xF] << 4) ^ RC[round++ % 2];
}

/*
 * KeyExpand: Expand a 2bytes key to 6byte for encryption/decryption
 */
void KeyExpand(Key k, ExpandedKey ek)
{
    int i;

    memcpy(ek[0], k, 2);

    for (i = 1; i < 3; i++)
    {
        ek[i][0] = ek[i - 1][0] ^ g(ek[i - 1][1]);
        ek[i][1] = ek[i][0] ^ ek[i - 1][1];
    }
}

uint8_t Multiply(uint8_t a, uint8_t b)
{
    return MULTIPLY_TABLE[a][b];
}

void MixColumn(ByteState bs, const uint8_t* matrix)
{
    ByteState tmp;

    tmp[0] = Multiply(bs[0], matrix[0]) ^ Multiply(bs[2], matrix[1]);
    tmp[1] = Multiply(bs[1], matrix[0]) ^ Multiply(bs[3], matrix[1]);
    tmp[2] = Multiply(bs[0], matrix[2]) ^ Multiply(bs[2], matrix[3]);
    tmp[3] = Multiply(bs[1], matrix[2]) ^ Multiply(bs[3], matrix[3]);

    memcpy(bs, tmp, 4);
}

/*
 * S-AES (Simplified AES) encryption algorithm
 *
 * Route: Ak0 -> NS -> SR -> MC -> Ak1 -> NS -> SR -> Ak2
 *
 * NOTE: NibbleSub uses S_BOX
 *       MixColumn uses MIXCOLUMN_MATRIX
 *       Input data will be padded to (floor(length / 2) + 1) * 2 bytes
 *       Output data length equal to Input with padding
 */
void* SAES_Encrypt(void* data, size_t length, Key k)
{
    State s;
    ByteState bs;
    ExpandedKey ek;

    size_t memsize;
    uint8_t* head;
    uint8_t* curs;
    uint8_t* tail;
    int i;
    int padding;

    padding = 2 - length % 2;

    // Data length must be mod 2
    memsize = length + padding;

    head = (uint8_t*)malloc(memsize);
    curs = head;
    tail = head + memsize;

    // Copy original data
    memcpy(curs, data, length);

    // Padding
    memset(head + length, padding, padding);

    KeyExpand(k, ek);

    while (curs != tail)
    {
        // Load 2 bytes into state
        memcpy(s, curs, 2);

        StateExpand(s, bs);

        for (i = 0; i < 2; i++)
        {
            AddRoundKey(bs, ek[i]);
            NibbleSub(bs, S_BOX);
            ShiftRow(bs);
            if (i == 0)
            {
                MixColumn(bs, MIXCOLUMN_MATRIX);
            }
        }
        AddRoundKey(bs, ek[2]);

        StatePack(bs, s);

        // Replace original data
        memcpy(curs, s, 2);

        // Next block
        curs += 2;
    }

    return head;
}

/*
 * S-AES (Simplified AES) decryption algorithm
 *
 * Route: Ak0 <- NS <- SR <- MC <- Ak1 <- NS <- SR <- Ak2
 *
 * NOTE: NibbleSub uses INVERSE_S_BOX
 *       MixColumn uses INVERSE_MIXCOLUMN_MATRIX
 *       Padding in Output data will be automatically truncated,
 *       so Output data length equal to Input data length
 */
void* SAES_Decrypt(void* data, size_t length, Key k)
{
    State s;
    ByteState bs;
    ExpandedKey ek;

    uint8_t* head;
    uint8_t* curs;
    uint8_t* tail;
    int i;
    int padding;

    head = (uint8_t*)malloc(length);
    curs = head;
    tail = head + length;

    // Copy original data
    memcpy(head, data, length);

    KeyExpand(k, ek);

    while (curs != tail)
    {
        // Load 2 bytes into state
        memcpy(s, curs, 2);

        StateExpand(s, bs);
        KeyExpand(k, ek);

        for (i = 2; i > 0; i--)
        {
            AddRoundKey(bs, ek[i]);
            if (i == 1)
            {
                MixColumn(bs, INVERSE_MIXCOLUMN_MATRIX);
            }
            ShiftRow(bs);
            NibbleSub(bs, INVERSE_S_BOX);
        }
        AddRoundKey(bs, ek[0]);

        StatePack(bs, s);

        // Replace original data
        memcpy(curs, s, 2);

        // Next block
        curs += 2;
    }

    // Get padding length from last data block
    padding = head[length - 1];

    // Truncate padding
    realloc(head, length - padding);

    return head;
}


