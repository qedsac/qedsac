/*
 * clock.h -- declarations for Clock class
 *            (clock in display window)
 */
#ifndef CLOCK_H
#define CLOCK_H

#include <QWidget>

#include "attributes.h"

class Clock : public QWidget
{
public:
    const TICKS TICKS_PER_SECOND = 10000;

    Clock(QWidget *parent = nullptr) : QWidget(parent) {}

    void reset() { curr_time = 0; update(); }
    void advance(TICKS increment);
protected:
    void paintEvent(QPaintEvent *e);
private:
    TICKS curr_time = 0;
};

#endif
