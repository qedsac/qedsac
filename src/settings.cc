/*
 * settings.cc -- definitions for Settings class
 *                (available run-time options)
 */
#include <QSettings>

#include "settings.h"

bool Settings::_stop_bell = false;
bool Settings::_real_time = false;
bool Settings::_sound = true;
bool Settings::_hints = false;
bool Settings::_short_tanks = true;
bool Settings::_open_recent = true;
bool Settings::_ignore_case = true;
bool Settings::_teleprinter_sound = false;
bool Settings::_digital_clock = false;

bool Settings::_literal_output = false;

bool Settings::_st_suppress = false;

int Settings::_long_tank = 0;

int Settings::_initial_orders = 2;
int Settings::_scale_factor = 100;
int Settings::_edsac_font_size = 9;
int Settings::_edit_font_size = 9;
QString Settings::_edit_font_name = DEFAULT_MONOSPACED_FONT;

// load the saved settings from previous runs
void Settings::load_settings()
{
    QSettings settings;
    _open_recent =
        settings.value("StartUp/AutoOpenRecent", _open_recent).toBool();
    _sound =
        settings.value("ToolBarPresets/Sound", _sound).toBool();
    _real_time =
        settings.value("ToolBarPresets/RealTime", _real_time).toBool();
    _hints =
        settings.value("ToolBarPresets/Hints", _hints).toBool();
    _short_tanks =
        settings.value("ToolBarPresets/ShortTanks", _short_tanks).toBool();
    _st_suppress = not _short_tanks;
    _stop_bell =
        settings.value("Other/Stop_bell", _stop_bell).toBool();
    _ignore_case =
        settings.value("Other/IgnoreCase", _ignore_case).toBool();
    _teleprinter_sound =
        settings.value("Other/Teleprinter", _teleprinter_sound).toBool();
    _digital_clock =
        settings.value("Other/DigitalClock", _digital_clock).toBool();
    _initial_orders =
        settings.value("Other/InitialOrders", _initial_orders).toInt();
    _scale_factor =
        settings.value("EdsacFormScaleFactor/SF", _scale_factor).toInt();
    _edsac_font_size =
        settings.value("EdsacFormScaleFactor/FontSize",
            _edsac_font_size).toInt();
    _edit_font_size =
        settings.value("Font/FontSize", _edit_font_size).toInt();
    _edit_font_name =
        settings.value("Font/FontName", _edit_font_name).toString();
}

// not currently used -- should we remove it?
void Settings::save_settings()
{
    QSettings settings;
    settings.setValue("StartUp/AutoOpenRecent", _open_recent);
    settings.setValue("ToolBarPresets/Sound", _sound);
    settings.setValue("ToolBarPresets/RealTime", _real_time);
    settings.setValue("ToolBarPresets/Hints", _hints);
    settings.setValue("ToolBarPresets/ShortTanks", _short_tanks);
    settings.setValue("Other/Stop_bell", _stop_bell);
    settings.setValue("Other/IgnoreCase", _ignore_case);
    settings.setValue("Other/Teleprinter", _teleprinter_sound);
    settings.setValue("Other/DigitalClock", _digital_clock);
    settings.setValue("Other/InitialOrders", _initial_orders);
    settings.setValue("EdsacFormScaleFactor/SF", _scale_factor);
    settings.setValue("EdsacFormScaleFactor/FontSize", _edsac_font_size);
    settings.setValue("Font/FontSize", _edit_font_size);
    settings.setValue("Font/FontName", _edit_font_name);

    settings.sync();    // probably not necessary, but some folks recommend
}
