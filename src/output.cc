/*
 * output.cc -- definitions for Teleprinter class
 *              (EDSAC output window)
 */
#include <QString>

#include "output.h"

#include "display.h"
#include "menu.h"
#include "settings.h"
#include "sound.h"

// translation table for EDSAC teleprinter characters
static const QString TRANSTAB[2] = {
    "PQWERTYUIOJf" "S"  "ZKlxFrD HNMnLXGABCV",      // letter shift
    "0123456789bf" "\"" "+(lx$r; £,.n)/#-?:=",      // figure shift
};

const char Teleprinter::LINEFEED = TRANSTAB[0].indexOf('n');

static const QString LITERAL_CHARS ="PQWERTYUIOJ#SZK*.F@D!HNM&LXGABCV";

// print specified EDSAC character on the teleprinter
void Teleprinter::do_print(char c)
{   
    if (Settings::literal_output()) {
        static int columns = 0;
        ensureCursorVisible();
        insertPlainText(LITERAL_CHARS[c]);
        columns = (columns + 1) % 4;
        if (columns == 0) { insertPlainText("\n"); }
		menu->enable(PRINT_OUTPUT, true);
		menu->enable(SAVE_OUTPUT, true);
		menu->enable(DISCARD_OUTPUT, true);
		menu->enable(DISCARD_OUTPUT_TOOL, true);
    } else {
        QChar qc = TRANSTAB[shift][c];
        if (qc.isLower()) {
            int col = textCursor().columnNumber();
            switch (qc.toLatin1()) {
                case 'f':       // figure shift
                    shift = FIGURES;
                    break;
                case 'l':       // letter shift
                    shift = LETTERS;
                    break;
                case 'n':       // linefeed ("\n")
                    if (Settings::teleprinter_sound()) {
                        play_sound(":/Linefeed.wav", 100);
                    }
                    moveCursor(QTextCursor::EndOfLine);
                    insertPlainText("\n");
                    for (int i = 0; i < col; i++) { insertPlainText(" "); }
                    ensureCursorVisible();
                    break;
                case 'r':       // carriage return ("\r")
                    if (Settings::teleprinter_sound()) {
                        play_sound(":/C_return.wav", 100);
                    }
                    moveCursor(QTextCursor::StartOfLine);
                    ensureCursorVisible();
                    break;
                case 'b':       // bell
                    if (Settings::teleprinter_sound()) {
                        play_sound(":/TeleprinterBell.wav");
                    }
                    break;
                case 'x':       // "no effect"
                    break;
            };
        } else {
            if (Settings::teleprinter_sound()) {
                play_sound(qc == ' ' ? ":/Space.wav" : ":/Print.wav", 100);
            }
            moveCursor(QTextCursor::Right, QTextCursor::KeepAnchor);
            insertPlainText(qc);
            ensureCursorVisible();
            menu->enable(PRINT_OUTPUT, true);
            menu->enable(SAVE_OUTPUT, true);
            menu->enable(DISCARD_OUTPUT, true);
            menu->enable(DISCARD_OUTPUT_TOOL, true);
        }
    }
    repaint();
}
