/*
 * about.h -- declarations for About class
 *            ("About" window dialog)
 */
#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QWidget>

class About : public QDialog
{
public:
    About(QWidget *parent = nullptr);
};

#endif
