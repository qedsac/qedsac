/*
 * menu.h -- declarations for Menu class
 *           (main menu/toolbar window)
 */
#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include <QActionGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QMenu>
#include <QPushButton>
#include <QStatusBar>

class About;
class Options;

// conditional menu items
enum Conditional {
    // "File" ("Open Recent" is QMenu, not QAction)
    NEW, OPEN, INSERT, SAVE, SAVE_AS, CLOSE, PRINT,
    // "Edit"
    UNDO, REDO, CUT, COPY, PASTE, DELETE, SELECT_ALL,
    // "Edsac"
    PRINT_OUTPUT, SAVE_OUTPUT, DISCARD_OUTPUT,
    // toolbar items
    SAVE_TOOL, PRINT_TOOL, DISCARD_OUTPUT_TOOL, UNDO_TOOL, REDO_TOOL,
    // --
    NUM_ITEMS
};

class Menu : public QMainWindow
{
public:
    Menu(QWidget *parent = nullptr);

    void add_window(QWidget *window, const QString& label)
    {   window_list[window].name = label;
        window_list[window].action
            = window_group->addAction(window_menu->addAction(label));
        window_list[window].action->setCheckable(true);
        connect(window_list[window].action, &QAction::triggered,
            [window]() { window->activateWindow(); window->raise(); });
    }
    void remove_window(QWidget *window)
    {   window_group->removeAction(window_list[window].action);
        delete window_list[window].action;
    }
    void adjust_window(QWidget *window, const QString& label)
        { remove_window(window); add_window(window, label); }
    void select_window_item(QWidget *window)
        { window_list[window].action->setChecked(true); }

    void enable(Conditional which, bool flag) {items[which]->setEnabled(flag); }
    void enable_library(bool flag = true);
    void adjust_recent(const QString& filename);
    void show_hint(const QString& message) { status->showMessage(message); }
    void clear_hint() { status->clearMessage(); }
protected:
    void closeEvent(QCloseEvent *e);
    void showEvent(QShowEvent *e);
private:
    void sync_options_bar();    // set options bar properly

    // error reporting
    void catalog_error(const QString& msg);

    About *about_dialog;         // "About Edsac" dialog
    Options *options_dialog;     // "Edsac Options" dialog

    // conditional menu items
    QAction *items[Conditional::NUM_ITEMS];
        // stuff for "Open Recent" (under "File" menu)
    QMenu *open_recent;
    static const int MAX_RECENT = 8;
    QList<QAction*> recent_files;
        // stuff for "Window" menu
    QMenu *window_menu;
    QActionGroup *window_group;
    struct Window_Action { QString name; QAction *action; };
    QHash<QWidget*, Window_Action> window_list;

    // stuff for subroutine library
    QList<QAction*> library_actions;
    QList<QPushButton*> button_actions;

    // option bar widgets
    QComboBox *initial;         // "initial orders" option
    QCheckBox *bell;            // "stop bell" option
    QCheckBox *time;            // "real time" option
    QCheckBox *sound;           // "sound" option
    QCheckBox *hints;           // "hints" option
    QCheckBox *tanks;           // "short tanks" option

    QStatusBar *status;         // status bar
};

extern Menu *menu;      // single menu/toolbar window for simulator

#endif
