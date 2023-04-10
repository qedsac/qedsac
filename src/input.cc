/*
 * input.cc -- definitions for Tape_Reader class
 *             (EDSAC input device)
 */
#include <QString>

#include "input.h"

#include "editor.h"
#include "error.h"
#include "settings.h"

// "load" tape (current edit window) on tape reader
bool Tape_Reader::set_up_tape()
{
    Edit_Window *w = Edit_Window::current_window();
    if (w == nullptr) {
        Error::beep();
        Error::error("Please load a program");
        return false;
    }
    tape = w->text();
    curr_pos = tape.begin();
    curr_line = 1;
    return true;
}

// "perforator" codes
static const QString LETTERS = "PQWERTYUIOJπS"  "ZK*.FθDΦHNMΔLXGABCV";
static const QString FIGURES = "0123456789J#\"" "§(*.F@D!+-'&%XG:?()";

// note that FIGURES includes alternate chars for Greek letters, as
// well as perforator codes in WWG1951 but not in Tutorial Guide Table 2
// (WWG has single quote for both S and M in figure shift, so we
// substitute a double quote for S here)

// get next character from current "tape"
// (ignore whitespace and comments)
char Tape_Reader::next_char()
{
    // skip whitespace
    // should unmatched ']' be an error? -> yes, for now
    while (curr_pos != tape.end() and (curr_pos->isSpace()
        or *curr_pos == '['))
        // behavior in original ignores unmatched ']':
        //or *curr_pos == '[' or *curr_pos == ']'))
    {
        if (*curr_pos == '\n') { ++curr_line; }
        else if (*curr_pos == '[') {    // start of comment
            while (*(++curr_pos) != ']') {
                if (*curr_pos == '\n') { ++curr_line; }
            }
        }
        ++curr_pos;
    }
    if (curr_pos == tape.end()) {   // unexpected EOF
        Error::beep();
        Error::error("End of input tape encountered");
        return ENDFILE;
    }

    // convert next input character into appropriate perforator code
    QChar ch = *curr_pos++;
	// check letter codes...
    int punch = LETTERS.indexOf(ch, 0, Settings::ignore_case()
                                           ? Qt::CaseInsensitive
                                           : Qt::CaseSensitive);
	// then figure codes
    if (punch < 0) { punch = FIGURES.indexOf(ch); }
    if (punch < 0) {    // invalid char
        Error::beep();
        Error::error("Illegal character on line number "
                        + QString::number(curr_line) + "\n['"
                        + ch + "' Unicode value U+"
                        + QString("%1")
                            .arg(ch.unicode(),4,16,QChar('0')).toUpper()
                        + "]");
        return INVALID;
    }
    return punch;
}
