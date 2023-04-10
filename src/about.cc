/*
 * about.cc -- declarations for About class
 *             ("About" window dialog)
 */
#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>

#include "config.h"

#include "about.h"

// helper class for webpage links
class Link_Label : public QLabel
{
public:
    Link_Label(QString link, QWidget *parent = nullptr, QString text = "")
        : QLabel(parent)
    {
        if (text.isEmpty()) { text = link; }
        setCursor(Qt::PointingHandCursor);
        QFont f(font());
        f.setPixelSize(13);
        setFont(f);
        setTextInteractionFlags(Qt::TextBrowserInteraction);
        setOpenExternalLinks(true);
        setText("<a href=\"http://" + link + "\">" + text + "<a>");
    }
};

About::About(QWidget *parent)
    : QDialog(parent)
{
    // match layout of original implementation
    setFixedSize(307, 301);
    setWindowTitle("About Edsac (Qt)");

    QPushButton *ok = new QPushButton("OK", this);
    ok->setGeometry(223, 264, 75, 25);
    connect(ok, &QPushButton::clicked, this, &About::accept);
    
    // picture
    QLabel *pic = new QLabel(this);
    pic->setGeometry(8, 8, 290, 190);
    pic->setPixmap(QPixmap(":/about-pic.png"));

    // version/copyright info
    QLabel *memo = new QLabel(this);
    memo->setGeometry(8, 208, 290, 48);
    QPalette pal = memo->palette();
    pal.setColor(QPalette::Window, Qt::white);
    memo->setAutoFillBackground(true);
    memo->setPalette(pal);
    QFont f(memo->font());
    f.setPixelSize(11);
    memo->setFont(f);
    memo->setText("The Edsac Simulator (Qt)\n"
                  "Version " PACKAGE_VERSION "\n"
                  "© 2023 Martin Campbell-Kelly"
                  " and Lee Wittenberg"
                  );

    // links to relevant webpages
    Link_Label *link1 = new Link_Label("www.edsac.org", this);
    link1->setGeometry(8, 280, 200, 17);

    Link_Label *link2 = new Link_Label("www.dcs.warwick.ac.uk/~edsac", this);
    link2->setGeometry(8, 264, 200, 17);
}
