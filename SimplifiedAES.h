#include <stdint.h>

#ifndef _SIMPLIFIEDAES_H_
#define _SIMPLIFIEDAES_H_


typedef uint8_t State[2];
typedef uint8_t ByteState[4];
typedef uint8_t Key[2];
typedef Key ExpandedKey[3];

extern uint8_t S_BOX[];
extern uint8_t INVERSE_S_BOX[];
extern uint8_t MIXCOLUMN_MATRIX[];
extern uint8_t INVERSE_MIXCOLUMN_MATRIX[];
extern uint8_t RC[];

void ShiftRow(ByteState);

#endif // _SIMPLIFIEDAES_H_
