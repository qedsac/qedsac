/*
 * light.cc -- definitions for Light class
 *             (stop light in display window)
 */
#include <QPainter>

#include "light.h"

// paint the light in the appropriate color
void Light::paintEvent(QPaintEvent *e)
{
    QColor color = on ? Qt::red : Qt::darkRed;
    QPainter qp(this);
    qp.setRenderHint(QPainter::Antialiasing);
    QPen pen(color);
    qp.setPen(pen);
    qp.setBrush(QBrush(color));
    qp.drawEllipse(0,0,height(),width());
}
