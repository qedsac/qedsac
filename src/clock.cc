/*
 * clock.cc -- definitions for Clock class
 *             (clock in display window)
 */
#ifdef _MSC_VER
    #define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <QPaintEvent>
#include <QPainter>

#include "clock.h"

#include "attributes.h"
#include "layout.h"
#include "settings.h"

static const QString DIGITAL_CLOCK_FONT = DEFAULT_MONOSPACED_FONT;

void Clock::paintEvent(QPaintEvent *e)
{
    QPainter qp(this);
    qp.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::white, 2, Qt::SolidLine);
    qp.setPen(pen);
    // tic marks every 30 degrees
    const int x = width()/2, y = height()/2,
        radius = Layout::SIZE.value(Settings::scale_factor()).outer_radius;
    for (int theta = 0; theta < 360; theta += 30) {
        int dx, dy; double t;
        t = theta * M_PI / 180.0;
        dx = round(radius * cos(t)); dy = round(radius * sin(t));
        qp.drawPoint(x+dx, y+dy);
    }
    // larger tics at 12, 3, 6, and 9 o'clock
    qp.drawLine(x+radius, y, x+radius+3, y);
    qp.drawLine(x-radius, y, x-radius-3, y);
    qp.drawLine(x, y+radius, x, y+radius+3);
    qp.drawLine(x, y-radius, x, y-radius-3);

    // paint clock face black
    const int inner_radius
        = Layout::SIZE.value(Settings::scale_factor()).inner_radius;
    qp.setPen(Qt::black);
    qp.setBrush(QBrush(Qt::black));
    qp.drawEllipse(x-inner_radius, y-inner_radius,
                   2*inner_radius, 2*inner_radius);
    
    // paint minute-hand white
    const int mh_radius
        = Layout::SIZE.value(Settings::scale_factor()).minute_hand;
    qp.setPen(QPen(Qt::white, 2));
    int mins = curr_time / TICKS_PER_SECOND / 60 % 60;
    double t = ((mins*6 - 90) % 360) * M_PI / 180.0;
    int dx = round(mh_radius*cos(t)),
        dy = round(mh_radius*sin(t));
    qp.drawLine(x, y, x+dx+1, y+dy);

    // paint second-hand red
    const int sh_radius
        = Layout::SIZE.value(Settings::scale_factor()).second_hand;
    qp.setPen(QPen(Qt::red, 1));
    int secs = curr_time / TICKS_PER_SECOND % 60;
    t = ((secs*6 - 90) % 360) * M_PI / 180.0;
    dx = round(sh_radius*cos(t)); dy = round(sh_radius*sin(t));
    qp.drawLine(x, y, x+dx, y+dy);

    // [original has hours mod 60 instead of 100]
    int hours = curr_time / TICKS_PER_SECOND / 60 / 60 % 100;

    // digital clock (if selected or hours >= 1)
    if (hours >= 1 or Settings::digital_clock()) {
        QFont f(DIGITAL_CLOCK_FONT);
        f.setPixelSize(Layout::SIZE.value(Settings::scale_factor()).font);
        qp.setFont(f);
        qp.setPen(Qt::white);

        QString time = "%1:%2:%3";
        qp.drawText(Layout::DIMEN.value(Settings::scale_factor()).digital_clock,
                    Qt::AlignHCenter | Qt::AlignTop,
                    time.arg(hours,2,10,QChar('0'))
                        .arg(mins,2,10,QChar('0'))
                        .arg(secs,2,10,QChar('0')));
    }
}

// advance clock (10000 ticks/sec)
// should we update only if a second has passed?
// --> yes, for now
void Clock::advance(TICKS increment)
{
    bool needs_repaint = (curr_time + increment) / TICKS_PER_SECOND
                            > curr_time / TICKS_PER_SECOND;
    curr_time += increment;
    if (needs_repaint) { repaint(); }
}
