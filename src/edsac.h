/*
 * edsac.h -- declarations for Edsac class
 *            (actual EDSAC simulation)
 */
#ifndef EDSAC_H
#define EDSAC_H

#include <QObject>

#include "attributes.h"
#include "tank.h"

class Tape_Reader;

class Edsac : public QObject
{
public:
    // operations used when multiplying
    enum Op_Spec { ADDITION = 0, SUBTRACTION = 1 };

    Edsac();

    void clear();
    void start();
    void reset();
    void stop();
    void single();
    void kill();

    void dial_digit(unsigned i);

    Tank *accumulator() { return &acc; }
    Tank *multiplicand() { return &mcand; }
    Tank *multiplier() { return &mplier; }
    Tank *sc_tank() { return &sct; }
    Tank *order_tank() { return &order; }
    Tank *store_tank(unsigned n = 0) { return &store[n]; }
private:
    typedef void (Edsac::*EXEC_FUNC)(ADDR, bool);

    // ensure long word address is even
    static void adjust_addr(ADDR& a, bool lflag)
        { a &= ~ADDR(lflag); }

    void execute();
    void exec_order();
    void clear_regs()
        { acc.clear(); mcand.clear(); mplier.clear(); sct.clear();
          order.clear(); }
    void load_initial_orders();
    WORD get_word(ADDR a);
    void store_word(ADDR a, WORD value);
    void load_mcand(ADDR a, bool lflag);

    // operation functions
    void add(ADDR a, bool lflag);               // A
    void subtract(ADDR a, bool lflag);          // S
    void load_multiplier(ADDR a, bool lflag);   // H
    void mult_add(ADDR a, bool lflag);          // V
    void mult_subtract(ADDR a, bool lflag);     // N
    void transfer(ADDR a, bool lflag);          // T
    void u_transfer(ADDR a, bool lflag);        // U
    void collate(ADDR a, bool lflag);           // C
    void rshift(ADDR a, bool lflag);            // R
    void lshift(ADDR a, bool lflag);            // L
    void pos_branch(ADDR a, bool lflag);        // E
    void neg_branch(ADDR a, bool lflag);        // F
    void read(ADDR a, bool lflag);              // I
    void print(ADDR a, bool lflag);             // O
    void print_check(ADDR a, bool lflag);       // F
    void no_op(ADDR a, bool lflag);             // X
    void roundoff(ADDR a, bool lflag);          // Y
    void halt(ADDR a, bool lflag);              // Z
    void invalid(ADDR a, bool lflag);           // everything else

    // helper func for mult_add/mult_subtract
    void mult_op(ADDR a, bool lflag, Op_Spec op);

    // jump table
    static const EXEC_FUNC optab[];

    // registers & store
    Tank acc;
    Tank mcand;
    Tank mplier;
    Tank sct;
    Tank order;
    Tank store[NUM_LONG_TANKS];

    // flags
    volatile bool running = false;
    volatile bool waiting = false;
    volatile bool halted = true;        // may be redundant
    volatile bool reset_enabled = false;
    volatile bool loading = false;

    Tape_Reader *input;
    TICKS excess = 0;       // for adjusting "real time" calculations
    char last_printed = 0;
};

extern Edsac *edsac;    // single simulator object

#endif
