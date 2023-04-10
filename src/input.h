/*
 * input.h -- declarations for Tape_Reader class
 *            (EDSAC input device)
 */
#ifndef INPUT_H
#define INPUT_H

#include <QObject>

class Tape_Reader : public QObject
{
public:
    static const char ENDFILE = char(-1);
    static const char INVALID = char(-2);

    bool set_up_tape();
    char next_char();
private:
    QString tape;
    QString::iterator curr_pos;
    int curr_line;
};

#endif
