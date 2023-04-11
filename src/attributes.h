/*
 * attributes.h -- attributes of EDSAC words, orders, etc.
 */
#ifndef ATTRIB_H
#define ATTRIB_H

#include <cstdint>                 // for [u]int..._t
#include <cmath>                   // for log2()

typedef uint32_t TICKS;            // clock ticks (10000/sec)

typedef uint16_t ADDR;             // 11 bits/address
const ADDR ALL_LINES = ~0;         // for displaying all lines in a tube

typedef uint32_t WORD;             // 17-18 bits/word
typedef int32_t SIGNED_WORD;
typedef uint64_t LONGWORD;         // 35 bits/long word
typedef int64_t SIGNED_LONGWORD;

// data attributes
const unsigned HIWORD_BITS = 17;                    // # of bits/word
const WORD MAX_HIWORD = ~(~WORD(0) << HIWORD_BITS); // mask for usable bits

const WORD SIGN_BIT = WORD(1) << (HIWORD_BITS-1);   // most sig. bit
const WORD SANDWICH_BIT = SIGN_BIT << 1;            // bit between words

const unsigned LOWORD_BITS = HIWORD_BITS + 1;       // reg size + sandwich
const WORD MAX_LOWORD = MAX_HIWORD | SANDWICH_BIT;  // mask w/ sandwich bit

const unsigned CHAR_BITS = 5;                       // 5 bits/char
const WORD MAX_CHAR = (~(~WORD(0) << CHAR_BITS));   // char mask

// order code arranged so that func codes are alphabetic chars
const unsigned FUNC_BITS = CHAR_BITS;
const WORD MAX_FUNC = MAX_CHAR;

const unsigned ADDR_BITS = 11;                      // 11 bits
const WORD MAX_ADDR = (~(WORD(~0) << ADDR_BITS));   // address mask

const unsigned FLAG_BITS = 1;                       // 1 bit
const WORD MAX_FLAG = (~(~WORD(0) << FLAG_BITS));   // flag mask

// architecture attributes
const unsigned NUM_LONG_TANKS = 32;
const unsigned LONG_TANK_WORDS = 32;
const unsigned STORE_DISPLAY_LINES = LONG_TANK_WORDS / 2;

const unsigned TANK_BITS = log2(NUM_LONG_TANKS);
const WORD TANK_MASK = ~(WORD(~0) << TANK_BITS);

const unsigned OFFSET_BITS = log2(LONG_TANK_WORDS);
const WORD OFFSET_MASK = ~(WORD(~0) << OFFSET_BITS);

inline int tank_num(ADDR a) { return (a >> OFFSET_BITS) & TANK_MASK; }

// useful functions
const unsigned LONGWORD_BITS = LOWORD_BITS + HIWORD_BITS;
const LONGWORD LONG_SIGN_BIT                    // most sig. dword bit
        = LONGWORD(1) << (LONGWORD_BITS - 1);
const LONGWORD MAX_LONGWORD                     // mask for usable bits
        = ~(~LONGWORD(0) << LONGWORD_BITS);

// note that WORD and LONGWORD may actually be the same type
// (assume that no word ever has more bits than it should)
inline LONGWORD long_word(WORD low, WORD high)
    { return (LONGWORD(high) << LOWORD_BITS) + (low); }
    //{ return (LONGWORD(high & MAX_HIWORD) << LOWORD_BITS)
    //          + (low & MAX_LOWORD); }

inline SIGNED_WORD sign_extend(WORD w)
    { return (w & (MAX_HIWORD >> 1)) - (w & SIGN_BIT); }
inline SIGNED_LONGWORD sign_extend_dword(LONGWORD w)
    { return (w & (MAX_LONGWORD >> 1)) - (w & LONG_SIGN_BIT); }
inline SIGNED_LONGWORD sign_extend(WORD low, WORD high)
    { return sign_extend_dword(long_word(low, high)); }

#endif
