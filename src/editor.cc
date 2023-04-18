/*
 * editor.cc -- definitions for Edit_Window class
 *              (stuff for opening/editing/saving files)
 *           -- adapted from code editor example in Qt docs
 */
#include <QApplication>
#include <QGuiApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPainter>
#include <QRegularExpression>
#include <QScreen>
#include <QSettings>
#include <QTextStream>

#include "editor.h"

#include "display.h"
#include "error.h"
#include "menu.h"
#include "settings.h"

QClipboard *clipboard;      // initialized in main()

// display bracketed comments in red
// (adapted from QSyntaxHighlighter web page)
void Edit_Window::Comment_Highlighter::highlightBlock(const QString& text)
{   // should we count brackets to allow nesting?
    QTextCharFormat comment_format;
    comment_format.setForeground(Qt::red);

    QRegularExpression start_comment("\\[");
    QRegularExpression end_comment("\\]");

    setCurrentBlockState(0);

    int start = 0;
    if (previousBlockState() != 1) {
        start = text.indexOf(start_comment);
    }

    while (start >= 0) {
        QRegularExpressionMatch match;
        int end = text.indexOf(end_comment, start, &match);
        int len;
        if (end < 0) {
            setCurrentBlockState(1);
            len = text.length() - start;
        } else {
            len = end - start + match.capturedLength();
        }
        setFormat(start, len, comment_format);
        start = text.indexOf(start_comment, start + len);
    }
}

// for differentiating untitled ("new") windows 
int Edit_Window::untitled_count = 1;

// for keeping track of available editor windows (and their geometries)
QVector<Edit_Window *> Edit_Window::window_stack;
QHash<QString, QRect> Edit_Window::window_geometry;

// construct edit window from file
Edit_Window::Edit_Window(const QString& fname, QWidget *parent)
    : QWidget(parent)
{
    // need to deal with nonexistent file! --> dealt with in caller
    window_stack.push_back(this);

    // prevent memory leak when window closed
    setAttribute(Qt::WA_DeleteOnClose);
    full_path = fname;

    // actual editing area
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(5,0,1,1);
    edit_area = new Edit_Area(this);
    edit_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_area->setFont(QFont(Settings::edit_font_name(),
                       Settings::edit_font_size()));
    edit_area->setLineWrapMode(QPlainTextEdit::NoWrap);
    layout->addWidget(edit_area);
    setLayout(layout);

    // read the file (or construct new, empty window)
    // (readonly files will have readonly windows)
    QString title = fname;
    if (fname.isEmpty()) {
        title = "Untitled " + QString::number(untitled_count++);
        readonly = false;
    } else {
        QFileInfo finfo(fname);
        title = finfo.completeBaseName();
        readonly = not finfo.isWritable();
        edit_area->setReadOnly(readonly);
        QFile f(fname);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            edit_area->setPlainText(in.readAll());
        } else {    // error
            Error::error("Unable to open " + fname);
        }
    }

    // set window title bar text
    // (based on filename, w/readonly caveat, if needed)
    setWindowTitle(title + (readonly ? " [readonly]" : ""));

    // let main menu/toolbar, know about this window
    menu->add_window(this, title);

    // set window geometry to default or last known geometry
    if (full_path.isEmpty() or not window_geometry.contains(full_path)) {
        const int DEFAULT_WIDTH = 320;
        const int DEFAULT_HEIGHT = 355;
        const int MARGIN = 20;
        // menu window geometry
        QRect mg = menu->frameGeometry();
        // available screen space
        QRect ag = QGuiApplication::primaryScreen()->availableGeometry(); 
        int new_y = mg.y() + mg.height() + MARGIN
                        + MARGIN * window_stack.size();
        if (new_y + DEFAULT_HEIGHT > ag.height()) {
            new_y -= mg.height()
                + 2 * (mg.height() + MARGIN + MARGIN * window_stack.size());
            new_y = std::max(new_y, ag.y() +
                QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight));
        }
        window_geometry.insert(full_path,
                               QRect(mg.x() + MARGIN
                                        + MARGIN * window_stack.size(),
                                     new_y, DEFAULT_WIDTH, DEFAULT_HEIGHT));
    }
    setGeometry(window_geometry[full_path]);
}

// open specified file in new window
void Edit_Window::open_file(const QString& filename)
{
    if (not filename.isEmpty()) {   // empty file name => canceled dialog
        Edit_Window *window = Edit_Window::existing_window(filename);
        if (window != nullptr) {
            // file in existing window --> make it active window
            window->activateWindow();
            window->show();
        } else {
            // file not in existing window --> open new window for it
            window = new Edit_Window(filename);
            window->show();
        }

        // add file to Open Recent menu
        menu->adjust_recent(filename);
    }
}

// "save" or "save as" current window contents to a file
void Edit_Window::save_file(bool save_as)
{
    // find out where to save file
    QSettings settings;
    QString save_loc = settings.value("FileDialog/SaveLocation",
                                      SAVE_DIRECTORY).toString();

    // use full pathname as default filename
    QString filename = full_path;
    if (filename.isEmpty()) {
        // untitled file --> add ".txt" extension to window
        // title to get default filename
        filename = save_loc + "/" + windowTitle() + ".txt";
    }

    if (save_as) {   // Save As...
        // use dialog to pick actual filename for saving
        QFileInfo finfo(filename);
        filename = save_loc + "/" + finfo.fileName();
        QFileDialog dialog(menu, "Save File As",
                filename, "Text Files (*.txt);;All Files (*)");
        dialog.setOption(QFileDialog::DontUseNativeDialog);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setSidebarUrls(dialog.sidebarUrls()
                    << QUrl::fromLocalFile(SAVE_DIRECTORY));
        if (dialog.exec() == QDialog::Accepted) {
            // remember (possibly new) path for window's file
            full_path = dialog.selectedFiles().front();
            window_geometry.insert(full_path, geometry());
        } else { return; }  // no file selected

        {   // extra braces to keep finfo local
            QFileInfo finfo(full_path);
            // remember save location for next time
            settings.setValue("FileDialog/SaveLocation", finfo.path());

            // adjust window info for saved filename
            // note that saved file is no longer readonly
            QString title = finfo.completeBaseName();
            readonly = false;
            edit_area->setReadOnly(false);
            setWindowTitle(title);
            menu->adjust_window(this, title);
        }
        // inform main menu/toolbar about saved pathname
        menu->adjust_recent(full_path);
    }

    // go ahead and save the window contents
    QFile f(full_path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate
                                    | QIODevice::Text))
    {
        QTextStream out(&f);
        QString output_text = edit_area->toPlainText();
        out << output_text;
        if (not output_text.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            QChar last_char = output_text.back();
#else
            QChar last_char = output_text[output_text.size()-1];
#endif
            if (last_char != '\n') {    // ensure EOL on last line
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                out << Qt::endl;
#else
                out << endl;
#endif
            }
        }
        if (out.status() == QTextStream::WriteFailed) {
            Error::error("Write to " + full_path + " failed");
        } else {
            edit_area->document()->setModified(false);
        }
    } else {
        Error::error("Unable to save " + full_path);
    }
    menu->enable(Conditional::SAVE, false);
    menu->enable(Conditional::SAVE_TOOL, false);
}

// insert contents of specified file at current cursor position
void Edit_Window::insert_file(const QString& filename)
{
    if (not filename.isEmpty()) {   // read file and insert contents
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            QString new_text = in.readAll();
            Edit_Window::current_window()->contents()->
                            textCursor().insertText(new_text);
        } else {
            Error::error("Unable to open " + filename);
        }
    }
}

// close all currently open editor windows, fail (return false) if one
// won't close (because of user cancelling "Save?" dialog)
bool Edit_Window::close_all()
{
    auto close_list = window_stack;
    for (auto p = close_list.begin(); p != close_list.end(); p++) {
        if (not (*p)->close()) { return false; }
    }
    return true;
}


// change all editor windows to use specified font
void Edit_Window::reset_font(const QFont& f)
{
    for (auto p = window_stack.begin(); p != window_stack.end(); p++) {
        (*p)->edit_area->setFont(f);
    }
}

// close the current window
void Edit_Window::closeEvent(QCloseEvent *e)
{
    // ask user to save file if window contents have been modified
    if (edit_area->document()->isModified()) {
        QMessageBox::StandardButton answer
            = QMessageBox::question(this, "File Modified",
                    "Save changes to " + windowTitle() + "?",
                    QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
                    QMessageBox::Yes);
        if (answer == QMessageBox::Cancel) { e->ignore(); return; }
        if (answer == QMessageBox::Yes) {
            save_file(full_path.isEmpty());
        }
    }

    // note that window no longer exists
    menu->remove_window(this);
    window_stack.removeOne(this);

    if (window_stack.isEmpty()) {
        // no more edit windows --> disable relevant menu items & buttons
        menu->enable(Conditional::SAVE, false);
        menu->enable(Conditional::SAVE_TOOL, false);
        menu->enable(Conditional::SAVE_AS, false);
        menu->enable(Conditional::CLOSE, false);
        menu->enable(Conditional::PRINT, false);
        menu->enable(Conditional::PRINT_TOOL, false);
        menu->enable(Conditional::UNDO, false);
        menu->enable(Conditional::UNDO_TOOL, false);
        menu->enable(Conditional::REDO, false);
        menu->enable(Conditional::REDO_TOOL, false);
        menu->enable(Conditional::CUT, false);
        menu->enable(Conditional::COPY, false);
        menu->enable(Conditional::PASTE, false);
        menu->enable(Conditional::DELETE, false);
        menu->enable(Conditional::SELECT_ALL, false);
        menu->enable(Conditional::INSERT, false);
        menu->enable_library(false);
        display->set_program_name("No Program");
    } else {
        // make next edit window the currently active one
        window_stack.back()->activateWindow();
        window_stack.back()->raise();
        display->set_program_name(window_stack.back()->windowTitle());
    }

    // free pointers to prevent memory leaks
    delete edit_area;
    delete layout;
}

// adjust menu items & tools appropriately when a window becomes active
void Edit_Window::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::ActivationChange and isActiveWindow()) {
        menu->select_window_item(this);
        window_stack.move(window_stack.indexOf(this), window_stack.size() - 1);
        display->set_program_name(window_stack.back()->windowTitle());
        menu->enable(Conditional::SAVE, edit_area->document()->isModified()
            and not full_path.isEmpty() and not readonly);
        menu->enable(Conditional::SAVE_TOOL, edit_area->document()->isModified()
            and not full_path.isEmpty() and not readonly);
        menu->enable(Conditional::SAVE_AS, true);
        menu->enable(Conditional::CLOSE, true);
        menu->enable(Conditional::PRINT, true);
        menu->enable(Conditional::PRINT_TOOL, true);
        menu->enable(Conditional::UNDO, can_undo);
        menu->enable(Conditional::UNDO_TOOL, can_undo);
        menu->enable(Conditional::REDO, can_redo);
        menu->enable(Conditional::REDO_TOOL, can_redo);
        menu->enable(Conditional::SELECT_ALL, true);
        menu->enable(Conditional::INSERT, not readonly);
        menu->enable_library(not readonly);
        bool has_selection =
                    not edit_area->textCursor().selectedText().isEmpty();
        menu->enable(Conditional::COPY, has_selection);
        menu->enable(Conditional::CUT, has_selection and not readonly);
        menu->enable(Conditional::DELETE, has_selection and not readonly);
        menu->enable(Conditional::PASTE,
            not clipboard->text().isEmpty() and not readonly);
        menu->adjust_recent(full_path);
    }
}

// keep track of new geometry when window is resized
void Edit_Window::resizeEvent(QResizeEvent *e)
{
    window_geometry.insert(full_path, geometry());
}

// keep track of new geometry when window is moved
void Edit_Window::moveEvent(QMoveEvent *e)
{
    window_geometry.insert(full_path, geometry());
}

// --------------- Edit_Area functions --------------- 
//    (adapted from code editor example in Qt docs)

// constructor sets up connections to adjust menu items and tools
// appropriately as various events occur
Edit_Window::Edit_Area::Edit_Area(QWidget *parent)
    : QPlainTextEdit(parent)
{
    line_number_area = new Line_Number_Area(this);
    highlighter = new Comment_Highlighter(document());

    // is there a better way to do this?
    connect(this, &Edit_Area::modificationChanged,
        [this](bool changed) {
            menu->enable(Conditional::SAVE,
                            changed and this->parent()->writeable()
                                and not this->parent()->save_name().isEmpty());
            menu->enable(Conditional::SAVE_TOOL,
                            changed and this->parent()->writeable()
                                and not this->parent()->save_name().isEmpty());
        });
    connect(this, &Edit_Area::blockCountChanged, this,
        &Edit_Area::update_line_number_digits);
    connect(this, &Edit_Area::updateRequest, this,
        &Edit_Area::update_line_numbers);

    connect(this, &Edit_Area::copyAvailable,
        [this](bool yes) {
            menu->enable(Conditional::COPY, yes);
            menu->enable(Conditional::CUT, not isReadOnly() and yes);
            menu->enable(Conditional::DELETE, not isReadOnly() and yes);
        });
    connect(this, &Edit_Area::undoAvailable,
        [this](bool yes) {
            menu->enable(Conditional::UNDO, yes);
            menu->enable(Conditional::UNDO_TOOL, yes);
            this->parent()->can_undo = yes;
        });
    connect(this, &Edit_Area::redoAvailable,
        [this](bool yes) {
            menu->enable(Conditional::REDO, yes);
            menu->enable(Conditional::REDO_TOOL, yes);
            this->parent()->can_redo = yes;
        });

    update_line_number_digits(0);
}

Edit_Window::Edit_Area::~Edit_Area()
{
    delete line_number_area;  // probably taken care of by Qt
    //delete highlighter;     // causes crash when window closed!
}

// display the numbers corresponding to each visible line
void Edit_Window::Edit_Area::paint_line_number_area(QPaintEvent *e)
{
    QPainter qp(line_number_area);
    qp.fillRect(e->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blocknum = block.blockNumber();
    int top =
        qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() and top <= e->rect().bottom()) {
        if (block.isVisible() and bottom >= e->rect().top()) {
            QString number = QString::number(blocknum + 1);
            qp.setPen(Qt::black);
            qp.drawText(0, top, line_number_area->width(),
                             fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blocknum;
    }
}

// report the space needed for the largest line number
// (for determining the width of line number area)
int Edit_Window::Edit_Area::line_number_digits() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
#else   // not as good, but workable
    int space = 3 +
            fontMetrics().boundingRect(QLatin1Char('9')).width() * digits;
#endif

    return space;
}

// magic used when edit area changes
void Edit_Window::Edit_Area::update_line_number_digits(int)
{
    setViewportMargins(line_number_digits()+9, 0, 0, 0);
}

// magic used when edit area changes
void Edit_Window::Edit_Area::update_line_numbers(const QRect &rect, int dy)
{
    if (dy != 0)
        line_number_area->scroll(0, dy);
    else
        line_number_area->update(0, rect.y(),
                                 line_number_area->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        update_line_number_digits(0);
}

// adjust line numbers whenever window is resized
void Edit_Window::Edit_Area::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    line_number_area->setGeometry(QRect(cr.left(), cr.top(),
                                  line_number_digits(), cr.height()));
}

// determine the Edit_Window that owns this Edit_Area
Edit_Window *Edit_Window::Edit_Area::parent() const
{
    return dynamic_cast<Edit_Window *>(parentWidget());
}
