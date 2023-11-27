#include "homePage.h"

HomePage::HomePage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

// Constructor that starts the timer for updating the date and time
 HomePage::HomePage(QWidget* w1, QLabel* l1, QLabel* l2, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    widget = w1;
    timeLabel = l1;
    dateLabel = l2;

    timerCurrentTime = new QTimer(this);
    timerCurrentTime->setInterval(1000);
    connect(timerCurrentTime, SIGNAL(timeout()), this, SLOT(updateDateAndTime()));
    timerCurrentTime->start(1000);
}

void HomePage::updateDateAndTime()
{
    timeLabel->setText(QDateTime::currentDateTime().time().toString("hh:mm:ss"));
    dateLabel->setText(QDateTime::currentDateTime().date().toString("dd-MMMM-yyyy"));
}
