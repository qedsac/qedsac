/*
 * tank.cc -- definitions for Tank class
 *            (EDSAC storage tanks)
 */
#include "tank.h"

#include "attributes.h"

Tank::Tank(unsigned nwords)
    : num_words(nwords)
{
    data = new WORD[nwords];
}
