/*
 * dial.h -- declarations for Dial_Button class
 *           (dial in display window)
 */
#ifndef DIAL_H
#define DIAL_H

#include <QFont>
#include <QPushButton>

class Dial_Button : public QPushButton
{
public:
    Dial_Button(char d, QWidget *parent = nullptr);
    static void initialize_dial_digit_font();  // can't do this at compile time
protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
private:
    static QFont dial_font;
    char digit;
};

#endif
