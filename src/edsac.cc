/*
 * edsac.cc -- definitions for Edsac class
 *             (actual EDSAC simulation)
 */
#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>

#include "edsac.h"

#include "attributes.h"
#include "clock.h"
#include "display.h"
#include "error.h"
#include "input.h"
#include "output.h"
#include "settings.h"
#include "sound.h"
#include "tank.h"

Edsac *edsac = new Edsac();

Edsac::Edsac()
{
    // short tanks
    acc = Tank(4); acc.clear();
    mcand = Tank(2); mcand.clear();
    mplier = Tank(2); mplier.clear();
    sct = Tank(1); sct.clear();
    order = Tank(1); order.clear();

    // long tanks
    for (int i = 0; i < NUM_LONG_TANKS; i++) {
        store[i] = Tank(LONG_TANK_WORDS);
        store[i].clear();
    }

    // tape reader
    input = new Tape_Reader();
}

// clear the store and registers
void Edsac::clear()
{
    if (running or loading) { Error::beep(); return; }
    display->set_stop_light(false);
    clear_regs();
    for (int i = 0; i < NUM_LONG_TANKS; i++) {
        store[i].clear();
    }
    display->update();
}

// start execution of the current program
// after loading the initial orders
void Edsac::start()
{
    if (running or loading) { Error::beep(); return; }
    if (not input->set_up_tape()) { return; }

    display->set_stop_light(false);
    load_initial_orders();
    clear_regs();
    display->update();
    running = true;
    halted = false;
    execute();
}

// continue execution, starting at the current order
void Edsac::reset()
{
    if (running or not reset_enabled) { Error::beep(); return; }
    display->set_stop_light(false);
    running = true;
    halted = false;
    execute();
}

// stop execution of a running program
void Edsac::stop()
{   
    if (running) { halt(0,0); }
    else if (loading) { Error::beep(); }
}

// start execution of a single order
void Edsac::single()
{
    if (running or loading) { Error::beep(); return; }
    else if (waiting) { return; }      // race condition w/ multiple clicks?
    display->set_stop_light(false);
    exec_order();
}

// kill the currently running program (if any)
void Edsac::kill()
{
    if (running) { running = waiting = false; }
}

// process dialed digit n
void Edsac::dial_digit(unsigned n)
{   
    if (running or not reset_enabled) { Error::beep(); return; }

    acc[3] += 2 *n;
    acc[3] &= MAX_HIWORD;
    display->update_tube(ACC);
    play_sound(":/Dial" + QString::number(n % 10) + ".wav");
    
    QElapsedTimer t;    // QThread::msleep doesn't do the job here.
    int delay = 1000 + 100 * n;
    t.start();
    while (not t.hasExpired(delay)) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    }

    reset();
}

// execute the current program
void Edsac::execute()
{
    display->set_stop_light(false);
    running = true;     // redundant, but helps readability
    halted = false;     // redundant, but helps readability
    while (running) { exec_order(); 
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    }
}

// approximate timings for orders (10 TICKs == 1 millisecond)
static const TICKS timings[] = {
    15,         // P -- [invalid]
    15,         // Q -- [invalid]
    15,         // W -- [invalid]
    15,         // E -- branch positive
    15,         // R -- right shift
    15,         // T -- transfer, clear acc.
    15,         // Y -- round-off
    15,         // U -- transfer, don't clear acc.
    200+15,     // I -- input
    1500+15,    // O -- output
    15,         // J -- [invalid]
    15,         // pi -- [invalid]
    15,         // S -- subtract
    15,         // Z -- halt
    15,         // K -- [invalid]
    15,         // erase -- [invalid]
    15,         // blank -- [invalid]
    15,         // F -- print check
    15,         // theta -- [invalid]
    15,         // D -- [invalid]
    15,         // phi -- [invalid]
    15,         // H -- load multiplier
    60,         // N -- multiply & subtract
    15,         // M -- [invalid]
    15,         // delta -- [invalid]
    15,         // L -- left shift
    15,         // X -- no operation
    15,         // G -- branch negative
    15,         // A -- add
    15,         // B -- [invalid]
    15,         // C -- collate (bitwise and)
    60,         // V -- multiply & add
};

// execute a single order
void Edsac::exec_order()
{
    // fetch order
    WORD curr_order = get_word(sct[0]);
    sct[0] = (sct[0] + 1) & MAX_ADDR; display->update_tube(SCT);
    order[0] = curr_order; display->update_tube(ORDER);
    bool lflag = (curr_order & MAX_FLAG) != 0;
    curr_order >>= FLAG_BITS;
    unsigned addr = (curr_order & MAX_ADDR);
    curr_order >>= ADDR_BITS;
    char func = (curr_order & MAX_FUNC);

    // set up timing info if running in "real time" mode
    if (Settings::real_time()) {
        waiting = true;
        TICKS how_long = timings[func] + excess;
        excess = how_long % 10;
        QTimer::singleShot(how_long/10, this, [this]() { waiting = false; });
    }

    // execute order
    halted = false;       // redundant?
    (this->*optab[func])(addr, lflag);
    display->clock->advance(timings[func]);

    // pause appropriately, if running in "real time" mode
    if (Settings::real_time()) {
        while (waiting) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
        }
    }
}

// punch code for given letter
inline char code(QChar c)
    { return QString("PQWERTYUIOJ#SZK*.F@D!HNM&LXGABCV").indexOf(c); }

// load the initial orders into the store from the uniselectors
void Edsac::load_initial_orders()
{
    struct Order { 
        signed char func;
        unsigned addr;
        bool flag;
    };

    static const int NUM_SWITCHES = 41;
    static const Order uniselectors[2][NUM_SWITCHES+1] = {
        {   // original initial orders
            {code('T')},     {code('H'), 2},    {code('T')},
            {code('E'), 6},  {code('P'), 1},    {code('P'), 5},
            {code('T')},     {code('I')},       {code('A')},
            {code('R'), 16}, {code('T'), 0, 1}, {code('I'), 2},
            {code('A'), 2},  {code('S'), 5},    {code('E'), 21},
            {code('T'), 3},  {code('V'), 1},    {code('L'), 8},
            {code('A'), 2},  {code('T'), 1},    {code('E'), 11},
            {code('R'), 4},  {code('A'), 1},    {code('L'), 0, 1},
            {code('A')},     {code('T'), 31},   {code('A'), 25},
            {code('A'), 4},  {code('U'), 25},   {code('S'), 31},
            {code('G'), 6},  {-1}
        },
        {   // final form of initial orders
            {code('T')},       {code('E'), 20},    {code('P'), 1},
            {code('U'), 2},    {code('A'), 39},    {code('R'), 4},
            {code('V')},       {code('L'), 8},     {code('T')},
            {code('I'), 1},    {code('A'), 1},     {code('S'), 39},
            {code('G'), 4},    {code('L'), 0, 1},  {code('S'), 39},
            {code('E'), 17},   {code('S'), 7},     {code('A'), 35},
            {code('T'), 20},   {code('A')},        {code('H'), 8},
            {code('A'), 40},   {code('T'), 43},    {code('A'), 22},
            {code('A'), 2},    {code('T'), 22},    {code('E'), 34},
            {code('A'), 43},   {code('E'), 8},     {code('A'), 42},
            {code('A'), 40},   {code('E'), 25},    {code('A'), 22},
            {code('T'), 42},   {code('I'), 40, 1}, {code('A'), 40, 1},
            {code('R'), 16},   {code('T'), 40, 1}, {code('E'), 8},
            {code('P'), 5, 1}, {code('P'), 0, 1},  {-1}
        }
    };
    loading = true;
    int orders = Settings::initial_orders() - 1;
    for (int i = 0; uniselectors[orders][i].func != -1; i++) {
        store_word(i, (uniselectors[orders][i].func << (ADDR_BITS + FLAG_BITS))
                      + (uniselectors[orders][i].addr << FLAG_BITS)
                      + (uniselectors[orders][i].flag));
        display->repaint();     // d->update() doesn't work here

        // 10 loads/sec in real time (less 1ms for overhead)
        if (Settings::real_time()) { QThread::msleep(99); }
        display->clock->advance(1000);
    }
    loading = false;
}

// get the word at store location (a)
inline WORD Edsac::get_word(ADDR a)
{
    int t = tank_num(a);
    return word_at(&store[t], a & OFFSET_MASK);
}

// set value as the contents of store location (a)
// (value assumed already appropriately masked)
inline void Edsac::store_word(ADDR a, WORD value)
{
    int t = tank_num(a);
    store_word_at(&store[t], a & OFFSET_MASK, value);
}

// load the multiplicand register with the contents of the (possibly
// long) word in location (a)
inline void Edsac::load_mcand(ADDR a, bool lflag)
{
    adjust_addr(a, lflag);
    if (lflag) {
        mcand[0] = get_word(a++);
    } else { mcand[0] = 0; }
    mcand[1] = get_word(a);
    display->update_tube(MCAND);
}

//---------- individual order functions ----------
// add the (possibly long) number in location (a) to the accumulator
void Edsac::add(ADDR a, bool lflag)
{
    load_mcand(a, lflag);
    acc[2] += mcand[0];
    if (acc[2] > MAX_LOWORD) {
        acc[2] &= MAX_LOWORD;
        ++acc[3];
    }
    acc[3] += mcand[1];
    acc[3] &= MAX_HIWORD;
    display->update_tube(ACC);
}

// subtract the (possibly long) number in location (a) from the accumulator
void Edsac::subtract(ADDR a, bool lflag)
{
    load_mcand(a, lflag);
    acc[2] -= mcand[0];
    if (acc[2] > MAX_LOWORD) {
        --acc[3];
        acc[2] &= MAX_LOWORD;
    }
    acc[3] -= mcand[1];
    acc[3] &= MAX_HIWORD;
    display->update_tube(ACC);
}

// load the (possibly long) number in location (a) into the multiplier register
void Edsac::load_multiplier(ADDR a, bool lflag)
{
    adjust_addr(a, lflag);
    if (lflag) {
        mplier[0] = get_word(a++);
    } else { mplier[0] = 0; }
    mplier[1] = get_word(a);
    display->update_tube(MPLIER);
}

static void shift_left(WORD [], int);
static void shift_right(WORD [], int);

// shift the contents of tank t one bit to the left
static inline void shift_left(Tank& t)
    { shift_left(t.begin(), t.end() - t.begin()); }

// shift the contents of tank t one bit to the right (arithmetic)
static inline void shift_right(Tank& t)
    { shift_right(t.begin(), t.end() - t.begin()); }

// negate (2's complement) the content of the specified number of
// words at dest
static void negate(WORD dest[], int num_words)
{
    for (int i = 0; i < num_words; i++) {
        dest[i] = ~dest[i] & MAX_LOWORD;
    }
    ++dest[0];
    for (int i = 0; i < num_words - 1 and dest[i] > MAX_LOWORD; i++) {
        dest[i] &= MAX_LOWORD;
        ++dest[i+1];
    }
    dest[num_words - 1] &= MAX_HIWORD;
}

// add the contents of src to the dest tank
// (the src array and the dest tank must be the same size)
static void add_to(WORD src[], Tank& dest)
{
    int num_words = dest.end() - dest.begin();
    for (int i = 0; i < num_words; i++) {
        dest[i] += src[i];
        if (dest[i] > MAX_LOWORD and i < num_words - 1) {
            dest[i] &= MAX_LOWORD;
            ++dest[i+1];
        }
    }
    dest[num_words - 1] &= MAX_HIWORD;
}

// subtract the contents of src from the dest tank
// (the src array and the dest tank must be the same size)
static void subtract_from(WORD src[], Tank& dest)
{
    int num_words = dest.end() - dest.begin();
    for (int i = 0; i < num_words; i++) {
        dest[i] -= src[i];
        if (dest[i] > MAX_LOWORD and i < num_words - 1) {
            dest[i] &= MAX_LOWORD;
            --dest[i+1];
        }
    }
    dest[num_words - 1] &= MAX_HIWORD;
}

// returns the opposite multiplication operation (addition/subtraction)
// (the type conversions generate no code, but inform the reader -- and
// the compiler -- that we know what we're doing)
static inline Edsac::Op_Spec operator not(Edsac::Op_Spec op)
    { return Edsac::Op_Spec(not bool(op)); }

typedef void (*OPERATION)(WORD [], Tank&);
OPERATION add_op[] = { add_to, subtract_from };

// multiply the (possibly long) number in location (a) by the number
// in the multiplier register and add the product to the accumulator
void Edsac::mult_add(ADDR a, bool lflag)
{
    mult_op(a, lflag, ADDITION);
}

// multiply the (possibly long) number in location (a) by the number
// in the multiplier register and subtract the product from the accumulator
void Edsac::mult_subtract(ADDR a, bool lflag)
{
    mult_op(a, lflag, SUBTRACTION);
}

// transfer the contents of the accumulator to (possibly) long)
// location (a), and clear the accumulator
void Edsac::transfer(ADDR a, bool lflag)
{
    u_transfer(a, lflag);
    acc.clear();
    display->update_tube(ACC);
}

// transfer the contents of the accumulator to (possibly) long)
// location (a), but do not clear the accumulator
void Edsac::u_transfer(ADDR a, bool lflag)
{
    adjust_addr(a, lflag);
    if (lflag) { store_word(a++, acc[2]); }
    store_word(a, acc[3]);
    display->update_tube(STORE, a);
}

// "collate" (bitwise and) the (possibly long) number in location (a)
// with the number in the multiplier register, and add the result into
// the accumulator
void Edsac::collate(ADDR a, bool lflag)
{
    load_mcand(a, lflag);
    acc[2] += mcand[0] & mplier[0];
    if (acc[2] > MAX_LOWORD) {
        acc[2] &= MAX_LOWORD;
        ++acc[3];
    }
    acc[3] += mcand[1] & mplier[1];
    acc[3] &= MAX_HIWORD;
    display->update_tube(ACC);
}

// compute the "control word" value for a shift order
// [direction must be code('L') or code('R')]
static inline WORD control_word(char direction, ADDR a, bool lflag)
{
    return WORD(code(direction)) << (ADDR_BITS + FLAG_BITS)
        | (a << FLAG_BITS) | lflag;
}

// compute the number of bits to shift, given the "control word"
// (specified by the position of the rightmost 1-bit in the word)
static int shift_length(WORD control)
{
    int result = 1;
    while ((control & 0x1) == 0) {
        ++result;
        control >>= 1;
    }
    return result;
}

// perform a 1-bit arithmetic right shift on a register value with the
// specified number of words
static void shift_right(WORD reg[], int num_words)
{
    reg[num_words-1] |= (reg[num_words-1] & SIGN_BIT) << 1;
    WORD next_bit = 0;
    for (int i = 0; i < num_words; i++) {
        reg[i] >>= 1;
        if (i != num_words - 1) {
            reg[i] |= (reg[i+1] & 0x1) << (LOWORD_BITS - 1);
        }
    }
}

// shift the contents of the accumulator the appropriate number of
// bits to the right (arithmetic)
void Edsac::rshift(ADDR a, bool lflag)
{
    int nbits = shift_length(control_word('R', a, lflag));
    for (int i = 0; i < nbits; i++) {
        shift_right(acc);
    }
    display->update_tube(ACC);
}

// perform a 1-bit left shift on a register value with the
// specified number of words
static void shift_left(WORD reg[], int num_words)
{
    bool prev_bit = false;
    for (int i = 0; i < num_words; i++) {
        reg[i] = (reg[i] << 1) | prev_bit;
        prev_bit = (reg[i] & (SANDWICH_BIT << 1)) != 0;
        reg[i] &= MAX_LOWORD;
    }
    reg[num_words - 1] &= MAX_HIWORD;
}

// shift the contents of the accumulator the appropriate number of
// bits to the left
void Edsac::lshift(ADDR a, bool lflag)
{
    int nbits = shift_length(control_word('L', a, lflag));
    for (int i = 0; i < nbits; i++) {
        shift_left(acc);
    }
    display->update_tube(ACC);
}

// branch to location (a) if the accumulator contents are greater than
// or equal to zero
void Edsac::pos_branch(ADDR a, bool lflag)
{
    if ((acc[3] & SIGN_BIT) == 0) { sct[0] = a; }
}

// branch to location (a) if the accumulator contents are less than zero
void Edsac::neg_branch(ADDR a, bool lflag)
{
    if ((acc[3] & SIGN_BIT) != 0) { sct[0] = a; }
}

// read the next character from the "input tape" and place the code
// (multiplied by 2⁻¹⁶) in (possibly long) location (a)
void Edsac::read(ADDR a, bool lflag)
{
    adjust_addr(a, lflag);
    if (lflag) {
        // lo word gets zero
        store_word(a++, 0);
    }
    char ch = input->next_char();
    if (ch == Tape_Reader::ENDFILE or ch == Tape_Reader::INVALID) {
        running = false;
        halted = true;
        reset_enabled = (ch == Tape_Reader::ENDFILE);
        --sct[0]; display->update_tube(SCT);
        return;
    }
    store_word(a, ch);
    display->update_tube(STORE, a);
}

// print the character in the five most significant bits of (possibly
// long) word location (a) on the teleprinter
void Edsac::print(ADDR a, bool lflag)
{
    // hi in odd; lo in even
    // odd a --> lflag irrelevant (hi bits already in odd loc)
    // even a --> add lflag to get actual addr
    if ((a & 0x1) == 0) { a += lflag; }
    char c = (get_word(a) >> (HIWORD_BITS - CHAR_BITS)) & MAX_CHAR;
    display->output->do_print(c);
    last_printed = c;
    display->update_tube(ACC);
}

// place the last printed character in the five most significant bits
// of the (possibly long) word at location (a)
void Edsac::print_check(ADDR a, bool lflag)
{
    adjust_addr(a, lflag);
    if (lflag) { store_word(a++, 0); }
    store_word(a,last_printed << (ADDR_BITS + FLAG_BITS)); 
    display->update_tube(STORE, a);
}

// do nothing
void Edsac::no_op(ADDR a, bool lflag)
{
}

// roundoff accumulator to 34 bits
// (i.e., add 2⁻³⁵ to the accumulator)
void Edsac::roundoff(ADDR a, bool lflag)
{
    acc[1] += SANDWICH_BIT;
    if (acc[1] > MAX_LOWORD) {
        acc[1] &= MAX_LOWORD;
        if (++acc[2] > MAX_LOWORD) {
            acc[2] &= MAX_LOWORD;
            ++acc[3];
            acc[3] &= MAX_HIWORD;
        }
    }
    display->update_tube(ACC);
}

// stop the machine and ring the warning bell
void Edsac::halt(ADDR a, bool lflag)
{
    if (Settings::stop_bell()) { play_sound(":/Bell.wav"); }
    display->set_stop_light(true);
    reset_enabled = halted = true;
    running = false;
}

// invalid order -- stop the machine
void Edsac::invalid(ADDR a, bool lflag)
{
    halted = true;
    reset_enabled = running = false;
}
//---------- end of individual order functions ----------

// multiply the (possibly long) number in location (a) by the number
// in the multiplier register and perform the specified operation wrt
// the accumulator (helper function for mult_add/mult_subtract)
void Edsac::mult_op(ADDR a, bool lflag, Op_Spec op)
{
    load_mcand(a, lflag);
    LONGWORD x_mcand = long_word(mcand[0], mcand[1]);
    if ((x_mcand & LONG_SIGN_BIT) != 0) {
        x_mcand = -x_mcand & MAX_LONGWORD;
        op = not op;
    }
    WORD x_mplier[4] = {};
    // multiplier to use is value shifted left twice
    x_mplier[0] = mplier[0]; x_mplier[1] = mplier[1];
    if ((x_mplier[1] & SIGN_BIT) != 0) {
        negate(x_mplier, 2);
        op = not op;
    }
    shift_left(x_mplier, 4); shift_left(x_mplier, 4);
    for (int i = 0; i < 35; i++) {
        if ((x_mcand & 0x1) != 0) {
            // add to accumulator
            add_op[op](x_mplier, acc);
            //display->update_tube(ACC);
        }
        x_mcand >>= 1;
        shift_left(x_mplier, 4);
    }
    display->update_tube(ACC);
    mcand.clear();
    display->update_tube(MCAND);
}

// jump table for Edsac order functions
const Edsac::EXEC_FUNC Edsac::optab[] = {
    &Edsac::invalid,            // P
    &Edsac::invalid,            // Q
    &Edsac::invalid,            // W
    &Edsac::pos_branch,         // E
    &Edsac::rshift,             // R
    &Edsac::transfer,           // T
    &Edsac::roundoff,           // Y
    &Edsac::u_transfer,         // U
    &Edsac::read,               // I
    &Edsac::print,              // O
    &Edsac::invalid,            // J
    &Edsac::invalid,            // pi
    &Edsac::subtract,           // S
    &Edsac::halt,               // Z
    &Edsac::invalid,            // K
    &Edsac::invalid,            // erase
    &Edsac::invalid,            // blank
    &Edsac::print_check,        // F
    &Edsac::invalid,            // theta
    &Edsac::invalid,            // D
    &Edsac::invalid,            // phi
    &Edsac::load_multiplier,    // H
    &Edsac::mult_subtract,      // N
    &Edsac::invalid,            // M
    &Edsac::invalid,            // delta
    &Edsac::lshift,             // L
    &Edsac::no_op,              // X
    &Edsac::neg_branch,         // G
    &Edsac::add,                // A
    &Edsac::invalid,            // B
    &Edsac::collate,            // C
    &Edsac::mult_add            // V
};
