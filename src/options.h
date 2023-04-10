/*
 * options.h -- declarations for Options class
 *              (Options window dialog)
 */
#ifndef OPTIONS_H
#define OPTIONS_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>

class Options : public QDialog
{
public:
    Options(QWidget *parent = nullptr);

    // redefined slots
    int exec();
    void accept();
private:
    void populate();
    void save_options() const;

    // "Initial Orders" section
    QRadioButton *orders[2];

    // "Toolbar" section
    QCheckBox *sound;
    QCheckBox *hints;
    QCheckBox *real_time;
    QCheckBox *short_tanks;

    // "Other" section
    QCheckBox *ignore_case;
    QCheckBox *teleprinter_sound;
    QCheckBox *open_recent;
    QCheckBox *digital_clock;
    QCheckBox *stop_bell;

    // "Editor Font" section
    QComboBox *edit_font_name;
    QSpinBox *edit_font_size;

    // "Resize Edsac" section
    QComboBox *scale_factor;
    QSpinBox *edsac_font_size;

    // needed to keep "Apply" and "Save" buttons distinct
    QString current_edit_font_name;
    int current_edit_font_size;
    int current_scale_factor;
    int current_edsac_font_size;
};

#endif
