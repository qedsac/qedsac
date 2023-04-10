/*
 * error.h -- declarations for Error namespace
 *            (error reporting functions)
 */
#ifndef ERROR_H
#define ERROR_H

#include <QMessageBox>
#include <QSound>
#include <QString>

namespace Error {

    inline
    QMessageBox::StandardButton internal_error(const QString& msg)
    {   return QMessageBox::critical(nullptr, "Internal Error", msg,
            QMessageBox::Abort | QMessageBox::Ignore,
            QMessageBox::Abort);
    }

    inline
    QMessageBox::StandardButton error(const QString& msg)
        { return QMessageBox::critical(nullptr, "Error", msg); }

    // using "play_sound" causes extra, unnecessary dependencies
    inline void beep() { QSound::play(":/Beep.wav"); }

};

#endif
