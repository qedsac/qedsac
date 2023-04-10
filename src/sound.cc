/*
 * sound.cc -- definitions for audio functions
 */
#include <QSound>
#include <QThread>

#include "sound.h"

#include "settings.h"

// play a specified sound file, then wait a specified # of ms.
void play_sound(const QString& name, int delay)
{
    if (Settings::sound()) {
        QSound::play(name);
        QThread::msleep(delay);
    }
}
