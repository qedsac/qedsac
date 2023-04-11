/*
 * display.cc -- definitions for Display class
 *               (main display window -- EDSAC controls & output)
 */
#include <QGuiApplication>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QScreen>
#include <QSettings>
#include <QTextStream>

#include "display.h"

#include "clock.h"
#include "dial.h"
#include "edsac.h"
#include "error.h"
#include "layout.h"
#include "light.h"
#include "menu.h"
#include "output.h"
#include "settings.h"
#include "sound.h"
#include "tube.h"

Display *display;

Display::Display(QWidget *parent)
    : QWidget(parent)
{
    // title bar & background image
    setWindowTitle("Edsac (Qt)");

    // supposed to remove most title-bar buttons (doesn't work on Linux/LXDE)
    setWindowFlags(Qt::CustomizeWindowHint
                    | Qt::WindowMinimizeButtonHint);
    background = new QLabel(this);

    // control buttons
    clear = new QPushButton("Clear", this);
    connect(clear, &QPushButton::clicked, edsac, &Edsac::clear);
    reset = new QPushButton("Reset", this);
    connect(reset, &QPushButton::clicked, edsac, &Edsac::reset);
    start = new QPushButton("Start", this);
    connect(start, &QPushButton::clicked, edsac, &Edsac::start);
    stop = new QPushButton("Stop", this);
    connect(stop, &QPushButton::clicked, edsac, &Edsac::stop);
    single = new QPushButton("Single E.P.", this);
    connect(single, &QPushButton::clicked, edsac, &Edsac::single);
    auto pal = stop->palette();
    pal.setColor(QPalette::ButtonText, Qt::darkRed);
    stop->setPalette(pal);

    // program output window (teleprinter)
    output_title = new QLabel("Output from: No Program", this);
    output_title->setAlignment(Qt::AlignCenter);
    output_title->setAutoFillBackground(true);
    pal = output_title->palette();
    pal.setColor(output_title->backgroundRole(), Qt::darkBlue);
    pal.setColor(output_title->foregroundRole(), Qt::white);
    output_title->setPalette(pal);

    output = new Teleprinter(this);

    // long tank selector
    lt_sel = new QSpinBox(this);
    lt_sel->setAlignment(Qt::AlignCenter);
    lt_sel->setRange(0, NUM_LONG_TANKS - 1);
    lt_sel->setFocusPolicy(Qt::NoFocus);
    pal = lt_sel->palette();
    pal.setColor(QPalette::Base, Qt::black);
    pal.setColor(QPalette::Highlight, Qt::black);
    pal.setColor(QPalette::Text, Qt::green);
    pal.setColor(QPalette::HighlightedText, Qt::green);
    lt_sel->setPalette(pal);
    connect(lt_sel, QOverload<int>::of(&QSpinBox::valueChanged),
        [this](int n) {
            store->set_tank(n);
            Settings::set_long_tank(n);
        } );

    // clock
    clock = new Clock(this);

    // dial buttons
    Dial_Button::initialize_dial_digit_font();
    for (int i = 1; i <= 10; i++) {
        d[i % 10] = new Dial_Button('0' + i % 10, this);
        connect(d[i % 10], &Dial_Button::clicked,
            [i]() { edsac->dial_digit(i); } );
    }

    // short tank displays
    monitor[ACC] = new Display_Tube(edsac->accumulator(), 1, 71, true,
                                    this, "ACCUMULATOR",
                                    AS_INTEGER+AS_FRAC+LABEL);
    monitor[MCAND] = new Display_Tube(edsac->multiplicand(), 1, 35, true,
                                    this, "MULTIPLICAND",
                                    AS_INTEGER+AS_FRAC+LABEL);
    monitor[MPLIER] = new Display_Tube(edsac->multiplier(), 1, 35, true,
                                    this, "MULTIPLIER",
                                    AS_INTEGER+AS_FRAC+LABEL);
    monitor[SCT] = new Display_Tube(edsac->sc_tank(), 1, 10, true,
                                    this, "SEQUENCE CONTROL TANK",
                                    AS_INTEGER);
    monitor[ORDER] = new Display_Tube(edsac->order_tank(), 1, 17, true,
                                    this, "ORDER TANK", AS_ORDER);

    // long tank display
    store = new Display_Tube(edsac->store_tank(0), 16, 35, false, this);

    // stop light
    stop_light = new Light(this);

    // scale the display window (and its widgets) correctly
    set_scale(Settings::scale_factor());

    // place menu in its most recent position
    QRect screen_area = QGuiApplication::primaryScreen()->availableGeometry();
    QPoint default_pos(screen_area.x() + (screen_area.width() - width())/ 2,
                       screen_area.y() + (screen_area.height() - height())/ 2);
    QSettings settings;
    move(settings.value("EdsacFormPlacement/Position", default_pos).toPoint());
}

// make sure everyting is scaled correctly
void Display::set_scale(int new_scale)
{
    // background
    QString bkgnd = ":/Ed%1.png";
    QPixmap pixmap(bkgnd.arg(new_scale));
    setFixedSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
    resize(pixmap.width(), pixmap.height());
    setFixedSize(QSize(width(), height()));
    background->resize(pixmap.width(), pixmap.height());
    background->setPixmap(pixmap);

using namespace Layout;

    // control buttons
    clear->setGeometry(DIMEN.value(new_scale).clear);
    QFont f = clear->font(); f.setBold(true); clear->setFont(f);
    f.setPixelSize(SIZE.value(new_scale).button_font);
    clear->setFont(f);

    reset->setGeometry(DIMEN.value(new_scale).reset);
    reset->setFont(f);

    start->setGeometry(DIMEN.value(new_scale).start);
    start->setFont(f);

    stop->setGeometry(DIMEN.value(new_scale).stop);
    stop->setFont(f);

    single->setGeometry(DIMEN.value(new_scale).single);
    single->setFont(f);

    // output window
    //    title
    f = output_title->font();
    f.setPixelSize(SIZE.value(new_scale).output_title_font);
    output_title->setFont(f);
    output_title->setGeometry(DIMEN.value(new_scale).output_title);

    //    text
    output->setGeometry(DIMEN.value(new_scale).output);
    output->setFont(QFont(DEFAULT_MONOSPACED_FONT,
                    Settings::edsac_font_size()));
    output->setReadOnly(true);
    output->setLineWrapMode(QPlainTextEdit::NoWrap);

    // long tank selector
    f = lt_sel->font();
    f.setPixelSize(SIZE.value(new_scale).lt_sel_font);
    lt_sel->setFont(f);
    lt_sel->setGeometry(DIMEN.value(new_scale).lt_sel);

    // clock face
    clock->setGeometry(DIMEN.value(new_scale).clock);

    // dial buttons
    // [setGeometry() doesn't resize font (& won't send resizeEvent)!]
    f = d[0]->font(); f.setBold(true);
    f.setPixelSize(SIZE.value(Settings::scale_factor()).dial_font);
    for (int i = 0; i < 10; i++) {
        d[i]->setFont(f);
        d[i]->setGeometry(DIMEN.value(Settings::scale_factor()).dial[i]);
    }

    // short tank displays
    monitor[ACC]->setGeometry(DIMEN.value(new_scale).acc);
    monitor[MCAND]->setGeometry(DIMEN.value(new_scale).mcand);
    monitor[MPLIER]->setGeometry(DIMEN.value(new_scale).mplier);
    monitor[SCT]->setGeometry(DIMEN.value(new_scale).sct);
    monitor[ORDER]->setGeometry(DIMEN.value(new_scale).order);

    // long tank display
    store->setGeometry(DIMEN.value(new_scale).store);

    // stop light
    stop_light->setGeometry(DIMEN.value(new_scale).stoplight);
}

// change size of teleprinter font
void Display::set_output_font_size(int size)
{
    output->setFont(QFont(DEFAULT_MONOSPACED_FONT, size));
}

// line on store tube containing address a
static inline int tube_line(ADDR a) { return (a & OFFSET_MASK) / 2; }

// update display for specific line on specified tube
// (a parameter only meaningful for STORE tube)
// -- do nothing if short tanks not displayed
//    or address is not currently shown on STORE tube
void Display::update_tube(Short_Tank st, ADDR a)
{
    if (st == STORE) {
        int t = tank_num(a);
        if (t != Settings::long_tank()) { return; }
        QRect r = Layout::DIMEN.value(Settings::scale_factor()).store;
        int dy = Layout::SIZE.value(Settings::scale_factor()).bit_dy;
        store->repaint(0, (STORE_DISPLAY_LINES - 1 - tube_line(a)) * dy,
                       r.width(), dy);
    } else if (not Settings::st_suppress()) {
        monitor[st]->repaint();
    }
}

// turn stop light on (or off)
void Display::set_stop_light(bool on)
{
    stop_light->set_light(on);
}

// print contents of teleprinter output window
void Display::print_output()
{
    QPrinter printer;
    QPrintDialog dialog(&printer);
    if (dialog.exec() == QDialog::Accepted) {
        output->print(&printer);
    }
}

// save contents of teleprinter output window to a file
void Display::save_output()
{
    // get file name via dialog
    QSettings settings;
    QString save_loc = settings.value("FileDialog/SaveLocation",
                                      SAVE_DIRECTORY).toString();
    QString filename = save_loc + "/Edsac Output.txt";
    QFileDialog dialog(nullptr, "Save Edsac Output As",
                       filename, "Text Files (*.txt);;All Files (*)");
    dialog.setOption(QFileDialog::DontUseNativeDialog);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setSidebarUrls(dialog.sidebarUrls()
                    << QUrl::fromLocalFile(SAVE_DIRECTORY));
    if (dialog.exec() == QDialog::Accepted) {
        filename = dialog.selectedFiles().front();
        settings.setValue("FileDialog/SaveLocation",
                          dialog.directory().absolutePath());
    } else { return; }  // dialog cancelled -> don't bother

    // save data to file
    QFile f(filename);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate
                                    | QIODevice::Text))
    {
        QTextStream out(&f);
        out << output->toPlainText();
        if (out.status() == QTextStream::WriteFailed) {
            Error::error("Write to " + filename + " failed");
        }
    } else {
        Error::error("Unable to save " + filename);
    }
}

// clear contents of teleprinter output window
void Display::discard_output()
{
    output->clear();
    menu->enable(PRINT_OUTPUT, false);
    menu->enable(SAVE_OUTPUT, false);
    menu->enable(DISCARD_OUTPUT, false);
    menu->enable(DISCARD_OUTPUT_TOOL, false);
}

// append a linefeed to teleprinter output
void Display::append_linefeed()
{
    output->do_print(Teleprinter::LINEFEED);
}

// reset clock to zero
void Display::reset_clock()
{
    clock->reset();
}
