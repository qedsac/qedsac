/*
 * editor.h -- declarations for Edit_Window class
 *             (stuff for opening/editing/saving files)
 */
#ifndef EDITOR_H
#define EDITOR_H

#include <QHash>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QString>
#include <QSyntaxHighlighter>
#include <QVector>
#include <QWidget>

// adapted from code editor example in Qt docs
class Edit_Window : public QWidget
{
private:    // nested classes
    class Line_Number_Area;
    friend class Edit_Area;

    // adapted from QSyntaxHighlighter web page
    class Comment_Highlighter : public QSyntaxHighlighter
    {
    public:
        Comment_Highlighter(QTextDocument *parent)
            : QSyntaxHighlighter(parent) {}
        void highlightBlock(const QString& text);
    };

    class Edit_Area : public QPlainTextEdit
    {
    private:
        friend class Edit_Window;
    public:
        Edit_Area(QWidget *parent = nullptr);
        ~Edit_Area();

        void paint_line_number_area(QPaintEvent *e);

        int line_number_digits() const;
    protected:
        void resizeEvent(QResizeEvent *e);
    private:
        void update_line_number_digits(int new_block_count);
        void update_line_numbers(const QRect &rect, int dy);
        void highlightCurrentLine();

        Edit_Window *parent() const;

        Line_Number_Area *line_number_area;
        Comment_Highlighter *highlighter;
    };

    class Line_Number_Area : public QWidget
    {
    public:
        Line_Number_Area(Edit_Area *editor)
            : QWidget(editor), edit_area(editor)
        {}

        QSize sizeHint() const
            { return QSize(edit_area->line_number_digits(), 0); }
    protected:
        void paintEvent(QPaintEvent *e)
            { edit_area->paint_line_number_area(e); }
    private:
        Edit_Area *edit_area;
    };
public:
    Edit_Window(const QString& = "", QWidget *parent = nullptr);
    static void set_geometry(const QString& window_name, const QRect& r)
        { window_geometry[window_name] = r; }

    static void open_file(const QString& filename);
    void save_file(bool exists);
    void insert_file(const QString& filename);
    static bool close_all();

    static void reset_font(const QFont& f);

    static Edit_Window *existing_window(const QString& filename)
    {   for (auto p = window_stack.begin(); p != window_stack.end(); p++) {
            if ((*p)->full_path == filename) { return *p; }
        }
        return nullptr;
    }
    static Edit_Window *current_window()
        { return (window_stack.isEmpty() ? nullptr : window_stack.back()); }

    bool writeable() const { return not readonly; }
    QString text() const { return edit_area->toPlainText(); }
    QString save_name() const { return full_path; }
    Edit_Area *contents() const { return edit_area; }

    static const QRect& get_geometry(const QString& window_name)
        { return window_geometry[window_name]; }
    bool find(const QString& exp, QTextDocument::FindFlags options =
            QTextDocument::FindFlags()) const
        { return contents()->find(exp, options); }
protected:
    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void moveEvent(QMoveEvent *e);
private:
    static int untitled_count;
    static QVector<Edit_Window *> window_stack;
    static QHash<QString, QRect> window_geometry;
    QHBoxLayout *layout;
    Edit_Area *edit_area;
    QString full_path;
    bool readonly;
    bool can_undo = false, can_redo = false;
};

extern QClipboard *clipboard;

#endif
