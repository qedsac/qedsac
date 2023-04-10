/*
 * menu.cc -- definitions for Menu class
 *            (main menu/toolbar window)
 */
#include <QAction>
#include <QClipboard>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QPrinter>
#include <QPrintDialog>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>
#include <QTextStream>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

#include "menu.h"

#include "about.h"
#include "display.h"
#include "editor.h"
#include "error.h"
#include "edsac.h"
#include "options.h"
#include "settings.h"

// document locations
static const QString TUTORIAL_URL = "file://" DOC_DIR "/EdsacTG.pdf";
    //"https://www.dcs.warwick.ac.uk/~edsac/Software/EdsacTG.pdf";
static const QString EDSACDOC_URL = "file://" DOC_DIR "/EdsacDoc.pdf";
    //"https://www.dcs.warwick.ac.uk/~edsac/Software/EdsacDoc.pdf";
static const QString WWG1951_URL = "file://" DOC_DIR "/WWG1951.pdf";

Menu *menu;     // single menu/toolbar window for simulator
                // (initialized in main)

// get next non-blank, non-comment line from input stream
// (for reading config files -- like catalog.ini)
static QString next_line(QTextStream& in)
{
    QString result;
    do {
        result = in.readLine().simplified();
    } while (result.length() == 0 or result[0] == '#');
    return result;
}

// get filename using "Open File" dialog
static QString get_filename(const QString& caption)
{
    // find out where to look for files
    QSettings settings;
    QString open_loc = settings.value("FileDialog/OpenLocation",
                                      LIBRARY_DIRECTORY).toString();

    // use dialog to pick filename; empty string if dialog canceled
    QFileDialog dialog(menu, caption, open_loc,
                        "Text Files (*.txt);;All Files (*)");
    dialog.setOption(QFileDialog::DontUseNativeDialog);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setSidebarUrls(dialog.sidebarUrls()
                << QUrl::fromLocalFile(LIBRARY_DIRECTORY));

    QString result;
    if (dialog.exec() == QDialog::Accepted) {
        result = dialog.selectedFiles().front();        // only one!
        QFileInfo finfo(result);
        settings.setValue("FileDialog/OpenLocation", finfo.path());
    }

    return result;
}

// Note that Menu constructor actually builds whole GUI
Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
{
    // permanent dialogs
    about_dialog = new About();
    options_dialog = new Options();

    // title bar
    setWindowTitle("The Edsac Simulator (Qt)");
    setAttribute(Qt::WA_AlwaysShowToolTips);
    // set flags so that maximize not possible (doesn't work on MacOS)
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);

    // load previously saved settings
    Settings::load_settings();

    // menu bar
    QMenuBar *main_menu = menuBar();

    // "File" menu
    QMenu *file_menu = main_menu->addMenu("&File");

    items[Conditional::NEW] = new QAction("&New", this);
    items[Conditional::NEW]->setShortcut(QKeySequence::New);
    items[Conditional::NEW]->setShortcutContext(Qt::ApplicationShortcut);
    file_menu->addAction(items[Conditional::NEW]);
    std::function<void()> new_action=
        []() {
            Edit_Window *window = new Edit_Window();
            window->show();
        };
    connect(items[Conditional::NEW], &QAction::triggered, new_action);

    items[Conditional::OPEN] = new QAction("&Open", this);
    items[Conditional::OPEN]->setShortcut(QKeySequence::Open);
    items[Conditional::OPEN]->setShortcutContext(Qt::ApplicationShortcut);
    file_menu->addAction(items[Conditional::OPEN]);
    std::function<void()> open_action = 
        []() { Edit_Window::open_file(get_filename("Open File")); };
    connect(items[Conditional::OPEN], &QAction::triggered, open_action);

    open_recent = file_menu->addMenu("Open &Recent");
    for (int i = 0; i < MAX_RECENT; i++) {
        QAction *a = new QAction(this);
        connect(a, &QAction::triggered,
            [this,a]() {
                if (QFileInfo::exists(a->text())) {
                    Edit_Window::open_file(a->text());
                    adjust_recent(a->text());
                } else {    // file no longer exists!
                    Error::error("Cannot find file:  " + a->text());
                }
            });
        // no recent files by default
        a->setVisible(false);
        recent_files.append(a);
        open_recent->addAction(a);
    }

    items[Conditional::INSERT] = new QAction("&Insert...", this);
    items[Conditional::INSERT]->setEnabled(false);
    file_menu->addAction(items[Conditional::INSERT]);
    connect(items[Conditional::INSERT], &QAction::triggered,
        []() {
            QString filename = get_filename("Insert File");
            if (not filename.isEmpty()) {   // read file and insert contents
                Edit_Window::current_window()->insert_file(filename);
            }
        });

    file_menu->addSeparator();

    items[Conditional::SAVE] = new QAction("&Save", this);
    items[Conditional::SAVE]->setShortcut(QKeySequence::Save);
    items[Conditional::SAVE]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::SAVE]->setEnabled(false);
    file_menu->addAction(items[Conditional::SAVE]);
    std::function<void()> save_action = 
        []() { Edit_Window::current_window()->save_file(false); };
    connect(items[Conditional::SAVE], &QAction::triggered, save_action);

    items[Conditional::SAVE_AS] = new QAction("Save &As...", this);
    items[Conditional::SAVE_AS]->setEnabled(false);
    file_menu->addAction(items[Conditional::SAVE_AS]);
    connect(items[Conditional::SAVE_AS], &QAction::triggered,
        []() { Edit_Window::current_window()->save_file(true); });

    items[Conditional::CLOSE] = new QAction("&Close", this);
    items[Conditional::CLOSE]->setShortcut(QKeySequence::Close);
    items[Conditional::CLOSE]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::CLOSE]->setEnabled(false);
    file_menu->addAction(items[Conditional::CLOSE]);
    connect(items[Conditional::CLOSE], &QAction::triggered,
        []() { Edit_Window::current_window()->close(); });

    file_menu->addSeparator();

    items[Conditional::PRINT] = new QAction("&Print", this);
    items[Conditional::PRINT]->setShortcut(QKeySequence::Print);
    items[Conditional::PRINT]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::PRINT]->setEnabled(false);
    file_menu->addAction(items[Conditional::PRINT]);
    std::function<void()> print_action = 
        []() {
            QPrinter printer;
            QPrintDialog dialog(&printer);
            if (dialog.exec() == QDialog::Accepted) {
                Edit_Window::current_window()->contents()->print(&printer);
            }
        };
    connect(items[Conditional::PRINT], &QAction::triggered, print_action);

    file_menu->addSeparator();

    QAction *quit = new QAction("E&xit", this);
    quit->setShortcut(QKeySequence::Quit);
    quit->setShortcutContext(Qt::ApplicationShortcut);
    file_menu->addAction(quit);
    connect(quit, &QAction::triggered, this, &Menu::close);

    // "Edit" menu
    QMenu *edit_menu = main_menu->addMenu("&Edit");

    items[Conditional::UNDO] = new QAction("&Undo", this);
    items[Conditional::UNDO]->setShortcut(QKeySequence::Undo);
    items[Conditional::UNDO]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::UNDO]->setEnabled(false);
    edit_menu->addAction(items[Conditional::UNDO]);
    std::function<void()> undo_action = 
        []() { Edit_Window::current_window()->contents()->undo(); };
    connect(items[Conditional::UNDO], &QAction::triggered, undo_action);

    items[Conditional::REDO] = new QAction("&Redo", this);
    items[Conditional::REDO]->setShortcut(QKeySequence::Redo);
    items[Conditional::REDO]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::REDO]->setEnabled(false);
    edit_menu->addAction(items[Conditional::REDO]);
    std::function<void()> redo_action = 
        []() { Edit_Window::current_window()->contents()->redo(); };
    connect(items[Conditional::REDO], &QAction::triggered, redo_action);

    edit_menu->addSeparator();

    items[Conditional::CUT] = new QAction("Cu&t", this);
    items[Conditional::CUT]->setShortcut(QKeySequence::Cut);
    items[Conditional::CUT]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::CUT]->setEnabled(false);
    edit_menu->addAction(items[Conditional::CUT]);
    connect(items[Conditional::CUT], &QAction::triggered,
        []() { Edit_Window::current_window()->contents()->cut(); });

    items[Conditional::COPY] = new QAction("&Copy", this);
    items[Conditional::COPY]->setShortcut(QKeySequence::Copy);
    items[Conditional::COPY]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::COPY]->setEnabled(false);
    edit_menu->addAction(items[Conditional::COPY]);
    connect(items[Conditional::COPY], &QAction::triggered,
        []() { Edit_Window::current_window()->contents()->copy(); });

    items[Conditional::PASTE] = new QAction("&Paste", this);
    items[Conditional::PASTE]->setShortcut(QKeySequence::Paste);
    items[Conditional::PASTE]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::PASTE]->setEnabled(false);
    edit_menu->addAction(items[Conditional::PASTE]);
    connect(items[Conditional::PASTE], &QAction::triggered,
        []() { Edit_Window::current_window()->contents()->paste(); });
    connect(clipboard, &QClipboard::changed,
        [this](QClipboard::Mode m) {
            if (m != QClipboard::Clipboard) { return; }
            enable(Conditional::PASTE,
                Edit_Window::current_window() != nullptr
                    and Edit_Window::current_window()->writeable()
                    and not clipboard->text().isEmpty());
        });

    items[Conditional::DELETE] = new QAction("&Delete", this);
    items[Conditional::DELETE]->setShortcut(QKeySequence::Delete);
    items[Conditional::DELETE]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::DELETE]->setEnabled(false);
    edit_menu->addAction(items[Conditional::DELETE]);
    connect(items[Conditional::DELETE], &QAction::triggered,
        []() {
            Edit_Window::current_window()->contents()
                ->textCursor().removeSelectedText();
        });

    edit_menu->addSeparator();

    items[Conditional::SELECT_ALL] = new QAction("Select &All", this);
    items[Conditional::SELECT_ALL]->setShortcut(QKeySequence::SelectAll);
    items[Conditional::SELECT_ALL]->setShortcutContext(Qt::ApplicationShortcut);
    items[Conditional::SELECT_ALL]->setEnabled(false);
    edit_menu->addAction(items[Conditional::SELECT_ALL]);
    connect(items[Conditional::SELECT_ALL], &QAction::triggered,
        []() { Edit_Window::current_window()->contents()->selectAll(); });

    // "Library" menu
    QMenu *library_menu = main_menu->addMenu("&Library");
    // (menu is populated by tape catalog, below)

    // "Edsac" menu
    QMenu *edsac_menu = main_menu->addMenu("E&dsac");

    // initial orders chooser
    QActionGroup *order_group = new QActionGroup(edsac_menu);
    order_group->setExclusive(true);
    QAction *orders[2];
    orders[0]
        = order_group->addAction(edsac_menu->addAction("Initial Orders &1"));
    orders[0]->setCheckable(true);
    orders[1]
        = order_group->addAction(edsac_menu->addAction("Initial Orders &2"));
    orders[1]->setCheckable(true);
    orders[Settings::initial_orders()-1]->setChecked(true);
    // connections deferred until "initial" combo box defined

    edsac_menu->addSeparator();

    // construct display here so connections don't need lamdas
    display = new Display();
    items[PRINT_OUTPUT] = edsac_menu->addAction("&Print Edsac Output");
    items[PRINT_OUTPUT]->setEnabled(false);
    connect(items[PRINT_OUTPUT], &QAction::triggered,
            display, &Display::print_output);
    items[SAVE_OUTPUT] = edsac_menu->addAction("&Save Edsac Output As...");
    items[SAVE_OUTPUT]->setEnabled(false);
    connect(items[SAVE_OUTPUT], &QAction::triggered, 
            display, &Display::save_output);
    items[DISCARD_OUTPUT] = edsac_menu->addAction("&Discard Edsac Output");
    items[DISCARD_OUTPUT]->setEnabled(false);
    connect(items[DISCARD_OUTPUT],  &QAction::triggered, 
            display, &Display::discard_output);
    QAction *literal_output = edsac_menu->addAction("&Literal Output");
    literal_output->setCheckable(true);
    connect(literal_output, &QAction::toggled,
        [](bool checked) { Settings::set_literal_output(checked); } );

    edsac_menu->addSeparator();

    // "Console Command" submenu
    QMenu *console_command = edsac_menu->addMenu("Console Command");
    QAction *cc_clear = console_command->addAction("&Clear");
    connect(cc_clear, &QAction::triggered, edsac, &Edsac::clear);
    cc_clear->setShortcut(tr("Ctrl+Shift+C"));
    cc_clear->setShortcutContext(Qt::ApplicationShortcut);
    QAction *cc_start = console_command->addAction("&Start");
    connect(cc_start, &QAction::triggered, edsac, &Edsac::start);
    cc_start->setShortcut(tr("Ctrl+Shift+S"));
    cc_start->setShortcutContext(Qt::ApplicationShortcut);
    QAction *cc_reset = console_command->addAction("&Reset");
    connect(cc_reset, &QAction::triggered, edsac, &Edsac::reset);
    cc_reset->setShortcut(tr("Ctrl+Shift+R"));
    cc_reset->setShortcutContext(Qt::ApplicationShortcut);
    QAction *cc_single_ep = console_command->addAction("Single &E.P.");
    connect(cc_single_ep, &QAction::triggered, edsac, &Edsac::single);
    cc_single_ep->setShortcut(tr("Ctrl+Shift+X"));
    cc_single_ep->setShortcutContext(Qt::ApplicationShortcut);
    QAction *cc_stop = console_command->addAction("Sto&p");
    connect(cc_stop, &QAction::triggered, edsac, &Edsac::stop);
    cc_stop->setShortcut(tr("Ctrl+Shift+Q"));
    cc_stop->setShortcutContext(Qt::ApplicationShortcut);
    console_command->addSeparator();
    QAction *cc_lt_up = console_command->addAction("Long Tank &Up");
    connect(cc_lt_up, &QAction::triggered,
        display, &Display::lt_up);
    cc_lt_up->setShortcut(tr("Ctrl+Shift+U"));
    cc_lt_up->setShortcutContext(Qt::ApplicationShortcut);
    QAction *cc_lt_down = console_command->addAction("Long Tank &Down");
    connect(cc_lt_down, &QAction::triggered,
        display, &Display::lt_down);
    cc_lt_down->setShortcut(tr("Ctrl+Shift+D"));
    cc_lt_down->setShortcutContext(Qt::ApplicationShortcut);

    // "Dial" submenu
    // should we use an array & loop for dial_digits?
    // (and does it matter now?)
    QMenu *dial_digits = edsac_menu->addMenu("Dial");
    QAction *digit_0 = dial_digits->addAction("\"&0\"");
    connect(digit_0, &QAction::triggered, []() { edsac->dial_digit(10); });
    digit_0->setShortcut(tr("F10"));
    digit_0->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_1 = dial_digits->addAction("\"&1\"");
    connect(digit_1, &QAction::triggered, []() { edsac->dial_digit(1); });
    digit_1->setShortcut(tr("F1"));
    digit_1->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_2 = dial_digits->addAction("\"&2\"");
    connect(digit_2, &QAction::triggered, []() { edsac->dial_digit(2); });
    digit_2->setShortcut(tr("F2"));
    digit_2->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_3 = dial_digits->addAction("\"&3\"");
    connect(digit_3, &QAction::triggered, []() { edsac->dial_digit(3); });
    digit_3->setShortcut(tr("F3"));
    digit_3->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_4 = dial_digits->addAction("\"&4\"");
    connect(digit_4, &QAction::triggered, []() { edsac->dial_digit(4); });
    digit_4->setShortcut(tr("F4"));
    digit_4->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_5 = dial_digits->addAction("\"&5\"");
    connect(digit_5, &QAction::triggered, []() { edsac->dial_digit(5); });
    digit_5->setShortcut(tr("F5"));
    digit_5->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_6 = dial_digits->addAction("\"&6\"");
    connect(digit_6, &QAction::triggered, []() { edsac->dial_digit(6); });
    digit_6->setShortcut(tr("F6"));
    digit_6->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_7 = dial_digits->addAction("\"&7\"");
    connect(digit_7, &QAction::triggered, []() { edsac->dial_digit(7); });
    digit_7->setShortcut(tr("F7"));
    digit_7->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_8 = dial_digits->addAction("\"&8\"");
    connect(digit_8, &QAction::triggered, []() { edsac->dial_digit(8); });
    digit_8->setShortcut(tr("F8"));
    digit_8->setShortcutContext(Qt::ApplicationShortcut);
    QAction *digit_9 = dial_digits->addAction("\"&9\"");
    connect(digit_9, &QAction::triggered, []() { edsac->dial_digit(9); });
    digit_9->setShortcut(tr("F9"));
    digit_9->setShortcutContext(Qt::ApplicationShortcut);
    
    edsac_menu->addSeparator();

    // "Options" item
    QAction *show_options = edsac_menu->addAction("&Options...");
    connect(show_options, &QAction::triggered,
            [this]() {
                if (options_dialog->exec() == QDialog::Accepted) {
                    sync_options_bar();
                }
            });

    // "Window" menu
    window_menu = main_menu->addMenu("&Window");
    window_group = new QActionGroup(window_menu);
    window_group->setExclusive(true);
    // menu is populated as windows are opened & closed

    // "Help" menu
    QMenu *help_menu = main_menu->addMenu("&Help");
    QAction *tutorial = help_menu->addAction("&Tutorial Guide");
    connect(tutorial, &QAction::triggered,
            []() { QDesktopServices::openUrl(QUrl(TUTORIAL_URL)); });
    QAction *docs = help_menu->addAction("&Documentation");
    connect(docs, &QAction::triggered,
            []() { QDesktopServices::openUrl(QUrl(EDSACDOC_URL)); });
    QAction *wwg = help_menu->addAction("&WWG 1951");
    connect(wwg, &QAction::triggered,
            []() { QDesktopServices::openUrl(QUrl(WWG1951_URL)); });
#if !defined(__APPLE__) // MacOS moves "About" menu elsewhere,
                        // so separator is superfluous
    help_menu->addSeparator();
#endif
    QAction *about = help_menu->addAction("&About");
    connect(about, &QAction::triggered, about_dialog, &About::exec);

    // layout boxes
    QWidget *window = new QWidget(this);
    QHBoxLayout *outer = new QHBoxLayout();     // overall container
    QVBoxLayout *left = new QVBoxLayout(),      // left side  (actions/options)
                *right = new QVBoxLayout();     // right side (subr library)
    QHBoxLayout *options = new QHBoxLayout();   // option choices (left side)

    // left side
    // tool bar
    QToolBar *toolbar = new QToolBar();
	toolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QAction *tmp;

    // "New" tool
    toolbar->setIconSize(QSize(16,16));
    tmp = toolbar->addAction(QIcon(QPixmap(":/new.png")), "New");
    connect(tmp, &QAction::triggered, new_action);

    // "Open" tool
    tmp = toolbar->addAction(QIcon(QPixmap(":/open.png")), "Open");
    connect(tmp, &QAction::triggered, open_action);

    items[Conditional::SAVE_TOOL]
        = toolbar->addAction(QIcon(QPixmap(":/save.png")), "Save");
    items[Conditional::SAVE_TOOL]->setEnabled(false);
    connect(items[Conditional::SAVE_TOOL], &QAction::triggered, save_action);

    items[Conditional::PRINT_TOOL]
        = toolbar->addAction(QIcon(QPixmap(":/print.png")), "Print");
    items[Conditional::PRINT_TOOL]->setEnabled(false);
    connect(items[Conditional::PRINT_TOOL], &QAction::triggered, print_action);

    // "Options" tool
    tmp = toolbar->addAction(QIcon(QPixmap(":/settings.png")), "Options");
    connect(tmp, &QAction::triggered,
            [this]() {
                if (options_dialog->exec() == QDialog::Accepted) {
                    sync_options_bar();
                }
            });

    // "Zeroize Clock" tool
    tmp = toolbar->addAction(QIcon(QPixmap(":/clock.png")), "Zeroize Clock");
    connect(tmp, &QAction::triggered, display, &Display::reset_clock);

    items[DISCARD_OUTPUT_TOOL]
        = toolbar->addAction(QIcon(QPixmap(":/discard.png")), "Discard Output");
    items[DISCARD_OUTPUT_TOOL]->setEnabled(false);
    connect(items[DISCARD_OUTPUT_TOOL], &QAction::triggered,
        display, &Display::discard_output);
    tmp = toolbar->addAction(QIcon(QPixmap(":/linefeed.png")), "Linefeed");
    connect(tmp, &QAction::triggered, display, &Display::append_linefeed);

    items[Conditional::UNDO_TOOL]
        = toolbar->addAction(QIcon(QPixmap(":/tundo.png")), "Undo");
    items[Conditional::UNDO_TOOL]->setEnabled(false);
    connect(items[Conditional::UNDO_TOOL], &QAction::triggered, undo_action);

    items[Conditional::REDO_TOOL]
        = toolbar->addAction(QIcon(QPixmap(":/tredo.png")), "Redo");
    items[Conditional::REDO_TOOL]->setEnabled(false);
    connect(items[Conditional::REDO_TOOL], &QAction::triggered, redo_action);

    // "Find" tool
    tmp = toolbar->addWidget(new QLabel("Find:"));
    QLineEdit *search_box = new QLineEdit("");
    search_box->setFixedSize(QSize(80, 16));
    tmp = toolbar->addWidget(search_box);
    tmp = toolbar->addAction(QIcon(QPixmap(":/down.png")), "Next");
    connect(tmp, &QAction::triggered,
        [search_box]() {
            if (Edit_Window::current_window() == nullptr
                or not Edit_Window::current_window()->find(search_box->text()))
            {
                Error::beep();
            }
        });
    tmp = toolbar->addAction(QIcon(QPixmap(":/up.png")), "Previous");
    connect(tmp, &QAction::triggered,
        [search_box]() {
            if (Edit_Window::current_window() == nullptr
                or not Edit_Window::current_window() ->find(search_box->text(),
                    QTextDocument::FindBackward))
            {
                Error::beep();
            }
        });

    // "Tutorial Guide" tool
    tmp = toolbar->addAction(QIcon(QPixmap(":/edtut.png")), "Tutorial Guide");
    connect(tmp, &QAction::triggered,
            []() { QDesktopServices::openUrl(QUrl(TUTORIAL_URL)); });

    // "Documentation" tool
    tmp = toolbar->addAction(QIcon(QPixmap(":/eddoc.png")), "Documentation");
    connect(tmp, &QAction::triggered,
            []() { QDesktopServices::openUrl(QUrl(EDSACDOC_URL)); });
    left->addWidget(toolbar);

    // separator
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    left->addWidget(line);

    // options bar
    //    initial orders chooser
    initial = new QComboBox();
    QFont f = initial->font();
    f.setPointSize(9);          // should we use pixel size instead?
    initial->setFont(f);        // (point size seems to work best)
    initial->addItem("Initial Orders 1");
    initial->addItem("Initial Orders 2");
    options->addWidget(initial);
    connect(initial, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [orders](int index) {
                orders[index]->activate(QAction::Trigger);
                Settings::set_initial_orders(index + 1);
            });
    // deferred connections from orders[] array
    for (int i = 0; i < sizeof(orders)/sizeof(orders[0]); i++) {
        connect(orders[i], &QAction::triggered,
            [this,i](bool checked) {
                initial->setCurrentIndex(i);
                Settings::set_initial_orders(i + 1);
            });
    }

    bell = new QCheckBox("Stop Bell");
    bell->setFont(f);
    options->addWidget(bell);
    connect(bell, &QCheckBox::stateChanged,
        [this]() { Settings::set_stop_bell(bell->isChecked()); } );

    time = new QCheckBox("Real Time");
    time->setFont(f);
    options->addWidget(time);
    connect(time, &QCheckBox::stateChanged,
        [this]() { Settings::set_real_time(time->isChecked()); } );

    sound = new QCheckBox("Sound");
    sound->setFont(f);
    options->addWidget(sound);
    connect(sound, &QCheckBox::stateChanged,
        [this]() { Settings::set_sound(sound->isChecked()); } );

    hints = new QCheckBox("Hints");
    hints->setFont(f);
    options->addWidget(hints);
    connect(hints, &QCheckBox::stateChanged,
        [this]() { Settings::set_hints(hints->isChecked()); } );

    tanks = new QCheckBox("Short Tanks");
    tanks->setFont(f);
    options->addWidget(tanks);
    connect(tanks, &QCheckBox::stateChanged,
        [this]() {
            Settings::set_short_tanks(tanks->isChecked());
            display->update();
            // set flag to avoid updating short tanks, if appropriate
            Settings::set_st_suppress(not tanks->isChecked());
        } );

    left->addLayout(options);
    sync_options_bar();

    // status line (for hints)
    status = new QStatusBar();
    {   // slightly smaller font to fit more info
        QFont g(f);
        g.setPointSize(g.pointSize() - 1);
        status->setFont(g);
    }
    QFrame *s_frame = new QFrame(status);
    s_frame->setFrameShape(QFrame::Panel);
    s_frame->setFrameShadow(QFrame::Sunken);
    s_frame->setLineWidth(3);
    s_frame->setMidLineWidth(1);
    s_frame->setGeometry(status->frameGeometry());
    left->addWidget(status);

    // right side
    QLabel *rtitle = new QLabel("Subroutine Library");
    rtitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    right->addWidget(rtitle);
    right->setMargin(0);
    right->setContentsMargins(QMargins(0,0,0,0));
    right->setSpacing(0);
    // subroutine library tabs
    QTabWidget *tabs = new QTabWidget();
    tabs->setFont(f);
    tabs->setUsesScrollButtons(false);
    right->addWidget(tabs);

    // subroutine library info contained in "catalog.ini" resource
    QFile sc(":/tapes/catalog.ini");
    if (not sc.open(QIODevice::ReadOnly | QIODevice::Text)) {
        catalog_error("Unable to find subroutine catalog");
    } else {
        QTextStream subr_catalog(&sc);
        QIcon tape_icon(":/tape.png");

        // get category names
        QString line = next_line(subr_catalog);
        if (line != "[Categories]") {
            catalog_error("First line (\"" + line
                            + "\")\n    should be \"[Categories]\"");
        }
        QStringList categories;
        line = next_line(subr_catalog);   // "num=" line
        int num = line.mid(line.indexOf('=')+1).toInt();
        line = next_line(subr_catalog);  // "default=" line
        QString default_tab = line.mid(line.indexOf('=')+1).simplified();
        // read all the names listed in the "Categories" section
        for (int i = 0; i < num; i++) {
            line = next_line(subr_catalog);
            int eq_pos = line.indexOf('=');
            categories.append(line.mid(eq_pos+1).simplified());
            if (line.left(eq_pos).simplified() == default_tab) {
                default_tab = categories.last();
            }
        }

        // process categories in their "Categories" section order
        for (auto p = categories.begin(); p != categories.end(); p++) {
            // category header
            line = next_line(subr_catalog);
            if (line != ('[' + *p + ']')) {
                catalog_error("Category header (\"" + line
                            + "\")\n    should be \"[" + *p + "]\"");
            }

            // add category submenu to "Library" menu
            QMenu *menu = library_menu->addMenu(*p);
            // add category tab to "Subroutine Library" panel
            QWidget *curr_tab = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(curr_tab);

            // process items in current category
            line = next_line(subr_catalog);   // "num=" line
            int num = line.mid(line.indexOf('=')+1).toInt();
            for (int i = 0; i < num; i++) {
                // parse routine info
                line = next_line(subr_catalog);
                int eq_pos = line.indexOf('=');
                int dash_pos = line.indexOf('-');
                QString ilk = line.left(eq_pos).simplified();
                QString name
                    = line.mid(eq_pos + 1, dash_pos - eq_pos - 1).simplified();
                QString description
                    = line.right(line.size() - dash_pos - 1).simplified();

                // make sure routine actually exists
                QString fname = ":/tapes/" + name + ".txt";
                if (not QFileInfo::exists(fname)) {
                    catalog_error("Tape " + name + ".txt does not exist");
                }

                // add routine button to category tab
                QPushButton *button = new QPushButton(tape_icon, " " + name);
                button->setToolTip(name + " - " + description);
                layout->addWidget(button);
                // add menu item for routine to category submenu
                QAction *action = menu->addAction(name + " - " + description);

                // set appropriate actions for routine buttons & menu items
                if (ilk[0] == 's') {    // subr for insertion
                    // don't enable subr insertion
                    // until editor window available
                    action->setEnabled(false);
                    button->setEnabled(false);
                    library_actions.push_back(action);
                    button_actions.push_back(button);
                    std::function<void()> to_do =
                        [name,fname]() {
                            if (QMessageBox::question(nullptr, "Confirmation",
                                "Insert " + name + "?") == QMessageBox::Yes)
                            {
                                Edit_Window::current_window()->
                                    insert_file(fname);
                            }
                        };
                    connect(action, &QAction::triggered, to_do);
                    connect(button, &QPushButton::clicked, to_do);
                } else {                // file for editor window
                    action->setEnabled(true);
                    button->setEnabled(true);
                    std::function<void()> to_do =
                        [fname]() { Edit_Window::open_file(fname); };
                    connect(action, &QAction::triggered, to_do);
                    connect(button, &QPushButton::clicked, to_do);
                }
            }
            layout->addStretch();
            tabs->addTab(curr_tab, *p);

            // set default tab as specified in catalog.ini
            if (*p == default_tab) { tabs->setCurrentWidget(curr_tab); }
        }
    }

    // put everything together
    outer->addLayout(left);
    outer->addLayout(right);
    window->setLayout(outer);
    setCentralWidget(window);

    // menu/toolbar can't be resized
    adjustSize();
    setFixedSize(QSize(width(), height()));
    setFixedHeight(height());   // also needed -- dunno why

    QSettings settings;

    // initialize Open Recent menu
    QStringList recent;
    recent = settings.value("Recent/Files", recent).toStringList();
    QList<QVariant> geom;
    geom = settings.value("Recent/Geometry", geom).toList();
    for (int i = 0, j = 0; i < recent.size(); i++) {
        if (QFileInfo::exists(recent[i])) {
            recent_files[j]->setText(recent[i]);
            recent_files[j]->setVisible(true);
            Edit_Window::set_geometry(recent[i], geom[j].toRect());
            j++;
        }
    }

    // place menu/toolbar window in its most recent position
    move(settings.value("WindowPlacement/MenuPos", QPoint(0,0)).toPoint());

    // show (already constructed) Display window
    // and add it to the "Window" menu
    display->show();
    add_window(display, "Edsac");
}

// enable (or disable) library routine insertion
void Menu::enable_library(bool flag)
{
    for (auto p = library_actions.begin(); p != library_actions.end(); p++) {
        (*p)->setEnabled(flag);
    }
    for (auto p = button_actions.begin(); p != button_actions.end(); p++) {
        (*p)->setEnabled(flag);
    }
}

// place given filename at the head of the "Open Recent" list
// (ignore built-in files -- i.e., resources)
void Menu::adjust_recent(const QString& filename)
{
    if (filename[0] == ':') { return; }
    int file_index = -1;        // assume not already in list
    // find filename in list or first empty position
    for (int i = 0; i < recent_files.size(); i++) {
        if (not recent_files[i]->isVisible()) {    // first empty position
            file_index = i;
            recent_files[i]->setText(filename);
            recent_files[i]->setVisible(true);
            break;
        } else if (recent_files[i]->text() == filename) {
            file_index = i;
            break;
        }
    }
    if (file_index < 0) {   // not in list & no empty positions
        // put filename in last position
        file_index = recent_files.size() - 1;
        recent_files[file_index]->setText(filename);
    }

    // move filename to the head of the list
    recent_files.move(file_index, 0);
    // and adjust the "Open Recent" menu accordingly
    open_recent->clear();
    for (int i = 0; i < recent_files.size(); i++) {
        open_recent->addAction(recent_files[i]);
    }
}

// close down the program
void Menu::closeEvent(QCloseEvent *e)
{
    QSettings settings;

    // close editor windows unless changed window close canceled
    if (not Edit_Window::close_all()) {
        e->ignore();
        return;
    }

    // kill running program (if any)
    edsac->kill();

    // save recent file list
    QStringList recent;
    QList<QVariant> geom;
    for (int i = 0; i < recent_files.size(); i++) {
        if (recent_files[i]->isVisible()) {
            recent.push_back(recent_files[i]->text());
            geom.push_back(Edit_Window::get_geometry(recent_files[i]->text()));
        }
    }
    settings.setValue("Recent/Files", recent);
    settings.setValue("Recent/Geometry", geom);

    // save menu window placement
    settings.setValue("WindowPlacement/MenuPos", menu->pos());

    settings.sync();   // probably not necessary, but some folks recommend

    // close remaining windows (?)
    display->force_close();
    //qApp->closeAllWindows();  // seems to be unnecessary
}

// open most recent file (if necessary), but only on first invocation
void Menu::showEvent(QShowEvent *e)
{
    static bool initial_invoke = true;
    if (initial_invoke) {
        if (Settings::open_recent() and recent_files.front()->isVisible()) {
            Edit_Window::open_file(recent_files.front()->text());
        }
        initial_invoke = false;
    }
}

// synchronize options bar checkboxes with currently selected options
void Menu::sync_options_bar()
{
    initial->setCurrentIndex(Settings::initial_orders() - 1);
    bell->setChecked(Settings::stop_bell());
    time->setChecked(Settings::real_time());
    sound->setChecked(Settings::sound());
    hints->setChecked(Settings::hints());
    tanks->setChecked(Settings::short_tanks());
}

// report an error in processing the catalog.ini resource
void Menu::catalog_error(const QString& msg)
{   if (Error::internal_error("catalog.ini error:\n" + msg)
        == QMessageBox::Abort)
    {
        close();
    }
}
