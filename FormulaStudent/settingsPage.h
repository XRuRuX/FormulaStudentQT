#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "qspinbox.h"
#include <QWidget>
#include "candata.h"

class SettingsPage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QSpinBox* gpsLatSelector;
    QSpinBox* gpsLongSelector;

public:
    SettingsPage(QWidget *parent = nullptr);
    SettingsPage(QWidget* w1, QSpinBox* s1, QSpinBox* s2, QWidget* parent = nullptr);
    ~SettingsPage();

public:
    void setGPSSettings(int GPSLat, int GPSLong);

public slots:
    void on_gpsLatSelector_valueChanged();
};

#endif // SETTINGSPAGE_H
