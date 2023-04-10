/*
 * tube.h -- declarations for Tube class
 *           (EDSAC display tubes)
 */
#ifndef TUBE_H
#define TUBE_H

#include <QCursor>
#include <QWidget>

#include "attributes.h"

class Tank;

// flags to designate which hints provided for each tube
enum Hints {
    AS_INTEGER = 0x01,
    AS_ORDER   = 0x02,
    AS_FRAC    = 0x04,
    LABEL      = 0x08,
};

class Display_Tube : public QWidget
{
public:
    Display_Tube(Tank *t, unsigned nlines, unsigned nbits,
                 bool is_shrt, QWidget *parent = nullptr,
                 const QString& lbl = "WORD", unsigned  h = 0);
    void set_tank(int i);   // set which long tank is to be displayed
protected:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
private:
    Tank *data;
    unsigned num_lines,     // # of lines
             num_bits;      // # of bits/line
    bool is_short;          // short tanks (registers) may be turned off 
                            // long tanks (store) always on;
    QString label;          // label for hints              
    unsigned hint_flags;    // relevant hint flags
};

#endif
