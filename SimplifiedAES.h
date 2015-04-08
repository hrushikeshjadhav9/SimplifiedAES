#include <stdint.h>

#ifndef _SIMPLIFIEDAES_H_
#define _SIMPLIFIEDAES_H_


typedef uint8_t State[2];
typedef uint8_t ByteState[4];
typedef uint8_t Key[2];
typedef Key ExpandedKey[3];

extern const uint8_t S_BOX[];
extern const uint8_t INVERSE_S_BOX[];
extern const uint8_t MIXCOLUMN_MATRIX[];
extern const uint8_t INVERSE_MIXCOLUMN_MATRIX[];
extern const uint8_t RC[];

extern void ShiftRow(ByteState);
extern void AddRoundKey(ByteState, Key);
extern void KeyExpand(Key, ExpandedKey);
extern void MixColumn(ByteState, const uint8_t*);
extern void StateExpand(State, ByteState);
extern void StatePack(ByteState, State);
extern void NibbleSub(ByteState, const uint8_t*);

extern void* SAES_Encrypt(void*, size_t, Key);
extern void* SAES_Decrypt(void*, size_t, Key);


#endif // _SIMPLIFIEDAES_H_
