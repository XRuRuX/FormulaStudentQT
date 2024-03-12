#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "qlabel.h"
#include <QWidget>
#include <QDateTime>
#include "qtimer.h"
#include <QDebug>

class HomePage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QLabel* timeLabel;
    QLabel* dateLabel;
    QTimer* timerCurrentTime;

public:
    HomePage(QWidget *parent = nullptr);
    HomePage(QWidget* w1, QLabel* l1, QLabel* l2, QWidget *parent = nullptr);
    ~HomePage();

public slots:
    void updateDateAndTime();
};

#endif // HOMEPAGE_H
