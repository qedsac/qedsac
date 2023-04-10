/*
 * settings.h -- declarations for Settings class
 *               (available run-time options)
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStandardPaths>
#include <QString>

class Settings
{
public:
    static void load_settings();
    static void save_settings();

    static void set_stop_bell(bool value) { _stop_bell = value; }
    static void set_real_time(bool value) { _real_time = value; }
    static void set_sound(bool value) { _sound = value; }
    static void set_hints(bool value) { _hints = value; }
    static void set_short_tanks(bool value) { _short_tanks = value; }
    static void set_open_recent(bool value)
        { _open_recent = value; }
    static void set_ignore_case(bool value) { _ignore_case = value; }
    static void set_teleprinter_sound(bool value)
        { _teleprinter_sound = value; }
    static void set_digital_clock(bool value) { _digital_clock = value; }

    static void set_literal_output(bool value) { _literal_output = value; }

    static void set_long_tank(int value) { _long_tank = value; }

    static void set_st_suppress(bool value) { _st_suppress = value; }

    static void set_initial_orders(int value) { _initial_orders = value; }
    static void set_scale_factor(int value) { _scale_factor = value; }
    static void set_edsac_font_size(int value) { _edsac_font_size = value; }
    static void set_edit_font_size(int value) { _edit_font_size = value; }
    static void set_edit_font_name(const QString& value)
        { _edit_font_name = value; }

    static bool stop_bell() { return _stop_bell; }
    static bool real_time() { return _real_time; }
    static bool sound() { return _sound; }
    static bool hints() { return _hints; }
    static bool short_tanks() { return _short_tanks; }
    static bool open_recent() { return _open_recent; }
    static bool ignore_case() { return _ignore_case; }
    static bool teleprinter_sound() { return _teleprinter_sound; }
    static bool digital_clock() { return _digital_clock; }

    static bool literal_output() { return _literal_output; }

    // currently displayed  long tank
    static int long_tank() { return _long_tank; }

    // for avoiding short tank repainting
    static bool st_suppress() { return _st_suppress; }

    static int initial_orders() { return _initial_orders; }
    static int scale_factor() { return _scale_factor; }
    static int edsac_font_size() { return _edsac_font_size; }
    static int edit_font_size() { return _edit_font_size; }
    static QString edit_font_name() { return _edit_font_name; }
private:
    static bool
        _stop_bell,
        _real_time,
        _sound,
        _hints,
        _short_tanks,
        _open_recent,
        _ignore_case,
        _teleprinter_sound,
        _digital_clock,
        _literal_output,
        _st_suppress;
    static int _long_tank;
    static int
        _initial_orders,
        _scale_factor,
        _edsac_font_size,
        _edit_font_size;
    static QString _edit_font_name;
};

const QString DEFAULT_MONOSPACED_FONT = 
    #if defined(_WIN32) or defined(_WIN64)
        "Courier New";
    #elif defined(__APPLE__)
        "Monaco";
    #else
        "Monospace";
    #endif

const QString LIBRARY_DIRECTORY = DATA_DIR "/Edsac Tapes";
const QString SAVE_DIRECTORY
    = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

#endif
