#include <QApplication>
#include <QClipboard>
#include <QSplashScreen>
#include <QString>
#include <QTimer>

#include "menu.h"

static const QString STYLE = 
    #if defined(_WIN32) or defined(_WIN64)
        "windows";
    #elif defined(__APPLE__)
        "macintosh";
    #else
        "fusion";
    #endif

extern QClipboard *clipboard;   // defined in editor.cc, but initialized below

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  

    app.setWindowIcon(QIcon(":/logo.png"));     // set app-wide icon
    app.setStyle(STYLE);    // keeps qgnomeplatform from modifying widgets

    QCoreApplication::setOrganizationName("Edsac Simulator");
    QCoreApplication::setApplicationName("qedsac");

    clipboard = QGuiApplication::clipboard();

    // show splash screen
    QPixmap pixmap(":/splash.png");
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    splash.show();

    // original shows splash screen for 1.5 seconds; so we do likewise
    QTimer::singleShot(1500, &splash, SLOT(close()));

    // Menu constructor builds everything else
    // ("menu" is global; declared in menu.h)
    menu = new Menu();
    QTimer::singleShot(1500, menu, SLOT(show()));

    return app.exec();
}
