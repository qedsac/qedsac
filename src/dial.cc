/*
 * dial.cc -- definitions for Dial_Button class
 *            (dial in display window)
 */
#include <QApplication>
#include <QFontDatabase>
#include <QMessageBox>
#include <QString>

#include "dial.h"

#include "error.h"

QFont Dial_Button::dial_font;

Dial_Button::Dial_Button(char d, QWidget *parent)
    : QPushButton(QChar(d), parent), digit(d)
{
    setFont(dial_font);
    QPalette pal = palette();
    pal.setColor(QPalette::Button, Qt::black);  // color when pressed
    pal.setColor(QPalette::ButtonText, Qt::lightGray);
    setPalette(pal);
    setFlat(true);
}

// load and set up dial button font
void Dial_Button::initialize_dial_digit_font()
{
    QString font_family;
    int id = QFontDatabase::addApplicationFont(":/dial-digits.ttf");
    if (id == -1) {  // Should never happen -- but just in case...
        if (Error::internal_error("Unable to load dial font")
            == QMessageBox::Abort) { exit(1); }
        // use default application font when ignoring error
        font_family = QApplication::font().family();
    } else {
        font_family = QFontDatabase::applicationFontFamilies(id).value(0);
    }

    dial_font = QFont(font_family);
}

// highlight button when mouse is over it
void Dial_Button::enterEvent(QEvent *e)
{
    QPalette pal = palette();
    pal.setColor(QPalette::ButtonText, Qt::white);
    setPalette(pal);
}

// turn off highlighting when mouse leaves
void Dial_Button::leaveEvent(QEvent *e)
{
    QPalette pal = palette();
    pal.setColor(QPalette::ButtonText, Qt::lightGray);
    setPalette(pal);
}
