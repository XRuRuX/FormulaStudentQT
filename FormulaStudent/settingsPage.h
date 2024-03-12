#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "qspinbox.h"
#include <QWidget>
#include "candata.h"
#include "mapPage.h"

class SettingsPage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QSpinBox* gpsLatSelector;
    QSpinBox* gpsLongSelector;
    QDoubleSpinBox* gpsLatSelectorStart;
    QDoubleSpinBox* gpsLongSelectorStart;

public:
    SettingsPage(QWidget *parent = nullptr);
    SettingsPage(QWidget* w1, QSpinBox* s1, QSpinBox* s2, QDoubleSpinBox* ds1, QDoubleSpinBox* ds2, QWidget* parent = nullptr);
    ~SettingsPage();

public:
    void setGPSSettings(int GPSLat, int GPSLong, double GPSLatStart, double GPSLongStart);

public slots:
    void on_gpsLatSelector_valueChanged();
    void on_gpsLatSelectorStart_valueChanged();
};

#endif // SETTINGSPAGE_H
