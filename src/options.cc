/*
 * options.cc -- definitions for Options class
 *               (Options window dialog)
 */
#include <QGroupBox>
#include <QLabel>
#include <QFontDatabase>
#include <QPushButton>
#include <QSettings>
#include <QStringList>

#include "options.h"

#include "display.h"
#include "editor.h"
#include "settings.h"

Options::Options(QWidget *parent)
    : QDialog(parent)
{
    // match layout of original implementation
    setFixedSize(353, 402);
    setWindowTitle("Edsac Options");

    // "Initial Orders" section
    QGroupBox *order_group = new QGroupBox("Initial Orders", this);
    order_group->setGeometry(8, 8, 152, 72);
    for (int i = 0; i < sizeof(orders)/sizeof(orders[0]); i++) {
        orders[i] =
            new QRadioButton("Initial Orders "
                                + QString::number(i+1), order_group);
        orders[i]->move(6, 26 + 22*i);
    }

    // "Toolbar" section
    QGroupBox *toolbar = new QGroupBox("Toolbar", this);
    toolbar->setGeometry(176, 8, 168, 120);
    sound = new QCheckBox("Sound", toolbar);
    sound->move(6, 26);
    hints = new QCheckBox("Hints", toolbar);
    hints->move(6, 49);
    real_time = new QCheckBox("Real Time", toolbar);
    real_time->move(6, 72);
    short_tanks = new QCheckBox("Short Tanks", toolbar);
    short_tanks->move(6, 95);

    // "Other" section
    QGroupBox *other = new QGroupBox("Other", this);
    other->setGeometry(8, 88, 160, 176);
    ignore_case = new QCheckBox("Ignore Case", other);
    ignore_case->setGeometry(6, 26, 146, 30);
    teleprinter_sound = new QCheckBox("Teleprinter Sound", other);
    teleprinter_sound->setGeometry(6, 56, 146, 30);
    open_recent = new QCheckBox("Open Recent", other);
    open_recent->setGeometry(6, 86, 146, 30);
    digital_clock = new QCheckBox("Digital Clock", other);
    digital_clock->setGeometry(6, 116, 146, 30);
    stop_bell = new QCheckBox("Stop Bell", other);
    stop_bell->setGeometry(6, 146, 146, 30);

    // "Editor Font" section
    QGroupBox *edit_font = new QGroupBox("Editor Font", this);
    edit_font->setGeometry(8, 272, 336, 72);
    QLabel *size_label = new QLabel("Size:", edit_font);
    size_label->setGeometry(140, 33, 40, 26);
    size_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    edit_font_size = new QSpinBox(edit_font);
    edit_font_size->setGeometry(181, 33, 56, 26);
    edit_font_size->setRange(6, 18);
    edit_font_size->setFocusPolicy(Qt::NoFocus);
    edit_font_size->setAlignment(Qt::AlignHCenter);
    edit_font_name = new QComboBox(edit_font);
    edit_font_name->setGeometry(8, 31, 128, 30);
    QPushButton *edit_font_apply = new QPushButton("Apply", edit_font);
    edit_font_apply->setGeometry(248, 33, 75, 26);
    connect(edit_font_apply, &QPushButton::clicked,
        [this]() {
            current_edit_font_size = edit_font_size->value();
            current_edit_font_name = edit_font_name->currentText();
            Settings::set_edit_font_size(current_edit_font_size);
            Settings::set_edit_font_name(current_edit_font_name);

            // reset edit windows appropriately
            Edit_Window::reset_font(QFont(current_edit_font_name,
                                          current_edit_font_size));
        });

    // "Resize Edsac" section
    QGroupBox *resize = new QGroupBox("Resize Edsac", this);
    resize->setGeometry(175, 136, 168, 128);
    QLabel *sf_label = new QLabel("Scale Factor:", resize);
    sf_label->setGeometry(8, 36, 85, 26);
    QLabel *font_label = new QLabel("Font Size:", resize);
    font_label->setGeometry(8, 60, 72, 26);
    scale_factor = new QComboBox(resize);
    scale_factor->setGeometry(96, 36, 62, 26);
    edit_font_size->setAlignment(Qt::AlignHCenter);
    QStringList scales = { "83", "100", "133", "150", "200" };
    scale_factor->addItems(scales);
    edsac_font_size = new QSpinBox(resize);
    edsac_font_size->setGeometry(96, 60, 62, 26);
    edsac_font_size->setRange(6, 18);
    edsac_font_size->setFocusPolicy(Qt::NoFocus);
    QPushButton *resize_apply = new QPushButton("Apply", resize);
    resize_apply->setGeometry(40, 96, 72, 25);
    connect(resize_apply, &QPushButton::clicked,
        [this]() {
            current_scale_factor = scale_factor->currentText().toInt();
            current_edsac_font_size = edsac_font_size->value();
            Settings::set_scale_factor(current_scale_factor);
            Settings::set_edsac_font_size(current_edsac_font_size);

            display->set_scale(current_scale_factor);
            display->set_output_font_size(current_edsac_font_size);
        });

    // buttons (OK, Save, Cancel)
    QPushButton *ok_button = new QPushButton("OK", this);
    ok_button->setGeometry(24, 360, 72, 25);
    connect(ok_button, &QPushButton::clicked, this, &Options::accept);

    QPushButton *save_button = new QPushButton("Save", this);
    save_button->setGeometry(136, 360, 75, 25);
    connect(save_button, &QPushButton::clicked, this, &Options::save_options);

    QPushButton *cancel_button = new QPushButton("Cancel", this);
    cancel_button->setGeometry(248, 360, 75, 25);
    connect(cancel_button, &QPushButton::clicked, this, &Options::reject);

    populate();
}

// redefined exec slot -- populate data first
int Options::exec()
{
    populate();
    return QDialog::exec();
}

// redefined accept slot -- set global options first
void Options::accept()
{
    if (Settings::digital_clock() != digital_clock->isChecked()) {
        Settings::set_digital_clock(digital_clock->isChecked());
        display->update();
    }

    Settings::set_stop_bell(stop_bell->isChecked());
    Settings::set_sound(sound->isChecked());
    Settings::set_hints(hints->isChecked());
    Settings::set_real_time(real_time->isChecked());
    Settings::set_short_tanks(short_tanks->isChecked());
    // note conversion of bool to int in setting initial orders number:
    Settings::set_initial_orders(orders[0]->isChecked()
                                    + 2*orders[1]->isChecked());
    Settings::set_open_recent(open_recent->isChecked());
    Settings::set_ignore_case(ignore_case->isChecked());
    Settings::set_teleprinter_sound(teleprinter_sound->isChecked());
    Settings::set_scale_factor(current_scale_factor);
    Settings::set_edsac_font_size(current_edsac_font_size);
    Settings::set_edit_font_size(current_edit_font_size);
    Settings::set_edit_font_name(current_edit_font_name);

    display->update();

    done(QDialog::Accepted);
}

// populate dialog with current settings
void Options::populate()
{
    orders[Settings::initial_orders() - 1]->setChecked(true);
    sound->setChecked(Settings::sound());
    hints->setChecked(Settings::hints());
    real_time->setChecked(Settings::real_time());
    short_tanks->setChecked(Settings::short_tanks());

    ignore_case->setChecked(Settings::ignore_case());
    teleprinter_sound->setChecked(Settings::teleprinter_sound());
    open_recent->setChecked(Settings::open_recent());
    digital_clock->setChecked(Settings::digital_clock());
    stop_bell->setChecked(Settings::stop_bell());
    
    current_edit_font_name = Settings::edit_font_name();
    current_edit_font_size = Settings::edit_font_size();
    edit_font_size->setValue(current_edit_font_size);
    QFontDatabase qdb;
    QStringList font_list = qdb.families();
    edit_font_name->addItems(font_list);
    edit_font_name->setCurrentText(current_edit_font_name);

    current_scale_factor = Settings::scale_factor();
    current_edsac_font_size = Settings::edsac_font_size();
    scale_factor->setCurrentText(QString::number(current_scale_factor));
    edsac_font_size->setValue(current_edsac_font_size);
}

// note that scale_factor, edsac_font, and edit_font stuff is
// saved ONLY if corresponding apply button was previously pressed
void Options::save_options() const
{
    QSettings settings;
    settings.setValue("StartUp/AutoOpenRecent", open_recent->isChecked());
    settings.setValue("ToolBarPresets/Sound", sound->isChecked());
    settings.setValue("ToolBarPresets/RealTime", real_time->isChecked());
    settings.setValue("ToolBarPresets/Hints", hints->isChecked());
    settings.setValue("ToolBarPresets/ShortTanks", short_tanks->isChecked());
    settings.setValue("Other/Stop_bell", stop_bell->isChecked());
    settings.setValue("Other/IgnoreCase", ignore_case->isChecked());
    settings.setValue("Other/Teleprinter", teleprinter_sound->isChecked());
    settings.setValue("Other/DigitalClock", digital_clock->isChecked());
        // note conversion of bool to int in setting initial orders number
    settings.setValue("Other/InitialOrders", orders[0]->isChecked()
                                                + 2*orders[1]->isChecked());
    settings.setValue("EdsacFormScaleFactor/SF", current_scale_factor);
    settings.setValue("EdsacFormScaleFactor/FontSize", current_edsac_font_size);
    settings.setValue("Font/FontSize", current_edit_font_size);
    settings.setValue("Font/FontName", current_edit_font_name);

    settings.sync();    // probably not necessary, but some folks recommend
}
