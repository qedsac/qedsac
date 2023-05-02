/*
 * light.h -- declarations for Light class
 *            (for stop light in display window)
 */
#ifndef LIGHT_H
#define LIGHT_H

#include <QWidget>

class Light : public QWidget
{
public:
    Light(QWidget *parent = nullptr) : QWidget(parent) {}
    void set_light(bool value) { on = value; repaint(); }
protected:
    void paintEvent(QPaintEvent *e);
private:
    bool on = false;
};

#endif
