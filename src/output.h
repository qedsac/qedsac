/*
 * output.h -- declarations for Teleprinter class
 *             (EDSAC output window)
 */
#ifndef OUTPUT_H
#define OUTPUT_H

#include <QPlainTextEdit>

class Teleprinter : public QPlainTextEdit 
{
public:
    static const char LINEFEED;

    Teleprinter(QWidget *parent = nullptr)
        : QPlainTextEdit(parent) { setOverwriteMode(true); }
    
    void do_print(char c);
protected:
    // the following prevent clicks from interfering w/output
    void mousePressEvent(QMouseEvent *e) {}
    void mouseReleaseEvent(QMouseEvent *e) {}
    void mouseDoubleClickEvent(QMouseEvent *e) {}
private:
    enum Shift_Mode {LETTERS = 0, FIGURES = 1};
    Shift_Mode shift = LETTERS;         // letter shift at start
};

#endif
