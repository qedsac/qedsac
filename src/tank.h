/*
 * tank.h -- declarations for Tank class
 *           (EDSAC storage tanks)
 */
#ifndef TANK_H
#define TANK_H

#include "attributes.h"

class Tank
{
public:
    Tank() { data = nullptr; }
    Tank(unsigned nwords);

    WORD& operator[](int i) { return data[i]; }
    void clear() { for (auto p = begin(); p != end(); p++) { *p = 0; } }

    typedef WORD *iterator;
    iterator begin(unsigned n = 0) const { return data + n; }
    iterator end()                 const { return data + num_words; }
private:
    unsigned num_words;     // # of words in tank
    WORD *data;             // LSW first
};

// functions for EDSAC data manipulation

// access short word at offset in specified tank
inline WORD word_at(Tank *tank, unsigned offset)
    { return *tank->begin(offset); }

// access long word at offset in specified tank
//inline LONGWORD long_word_at(Tank *tank, unsigned offset)
//{
//    WORD *p = tank->begin(offset & ~0x1);
//    return (LONGWORD(*p) << LOWORD_BITS) + *(p+1);
//}

// store short word at offset in specified tank
inline void store_word_at(Tank *tank, unsigned offset, WORD w)
    { (*tank)[offset] = w; }

// store long word at offset in specified tank
//inline void store_word_at(Tank *tank, unsigned offset, WORD low, WORD high)
//{
//    offset &= ~0x1;
//    (*tank)[offset] = low;
//    (*tank)[offset+1] = high;
//}

#endif
