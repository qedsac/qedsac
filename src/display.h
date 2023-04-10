/*
 * display.h -- declarations for Display class
 *              (main display window -- EDSAC controls & output)
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <QCloseEvent>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QWidget>

#include "attributes.h"

class Display_Tube;
class Teleprinter;
class Clock;
class Light;
class Dial_Button;

// short tank names
enum Short_Tank { ACC, MCAND, MPLIER, SCT, ORDER, NUM_REGS, STORE=-1 };

class Display : public QWidget
{
private:
    friend class Edsac;
    friend void print_char(char);
public:
    Display(QWidget *parent = nullptr);
    void set_scale(int new_scale);

    void set_program_name(const QString& s)
        { output_title->setText("Output From: " + s); }
    void set_output_font_size(int size);
    void update_tube(Short_Tank st, ADDR a = ALL_LINES);
    void set_stop_light(bool on);

    void force_close() { done = true; close(); }

    // slots
    void print_output();
    void save_output();
    void discard_output();
    void append_linefeed();
    void reset_clock();
    void lt_up()   { lt_sel->stepUp(); }
    void lt_down() { lt_sel->stepDown(); }
protected:
    void closeEvent(QCloseEvent *e)
    { if (not done) { e->ignore(); }   // ignore titlebar close button
      else {
          QSettings settings;
          settings.setValue("EdsacFormPlacement/Position", pos());
      }
    }
private:
    QLabel *background;

    bool done = false;    // see close_event() and force_close(), above

    // control buttons
    QPushButton *clear;
    QPushButton *reset;
    QPushButton *start;
    QPushButton *stop;
    QPushButton *single;

    // output (teleprinter) window
    QLabel *output_title;
    Teleprinter *output;

    // long tank selector
    QSpinBox *lt_sel;

    // clock
    Clock *clock;

    // telephone dial buttons
    Dial_Button *d[10];

    // short tank displays
    Display_Tube *monitor[Short_Tank::NUM_REGS];

    // long tank display
    Display_Tube *store;

    // stop light
    Light *stop_light;
};

extern Display *display;

#endif
