#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "qspinbox.h"
#include <QWidget>
#include <QColorDialog>
#include <QColor>
#include <QPalette>
#include "candata.h"
#include "mapPage.h"
#include "telemetryPage.h"

class SettingsPage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QSpinBox* gpsLatSelector;
    QSpinBox* gpsLongSelector;
    QDoubleSpinBox* gpsLatSelectorStart;
    QDoubleSpinBox* gpsLongSelectorStart;
    QDoubleSpinBox* maxDistanceSelector;

public:
    SettingsPage(QWidget *parent = nullptr);
    SettingsPage(QWidget* w1, QSpinBox* s1, QSpinBox* s2, QDoubleSpinBox* ds1, QDoubleSpinBox* ds2, QDoubleSpinBox* ds3, QWidget* parent = nullptr);
    ~SettingsPage();

public:
    void setColorForButtonGraphSettings(QPushButton *button, int graphName, TelemetryPage* telemetryPage);

public:
    void setGPSSettings(int GPSLat, int GPSLong, double GPSLatStart, double GPSLongStart, double MaxDistanceForNewLapThreshold);

public slots:
    void on_gpsLatSelector_valueChanged();
    void on_mapPage_loadSettings();
};

#endif // SETTINGSPAGE_H
