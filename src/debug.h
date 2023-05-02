/*
 * debug.h -- declarations for Debug namespace
 *            (debugging functions, obviously)
 */
#ifndef DEBUG_H
#define DEBUG_H

#include <QString>

#include "settings.h"

namespace Debug {

    inline
    void debug(const QString& msg) { qInfo(msg.toStdString().c_str()); }

    inline  // useful because QString has no "boolalpha" conversion
    QString as_str(bool b) { return b ? "true" : "false"; }

};

#endif
