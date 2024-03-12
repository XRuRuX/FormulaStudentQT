#include "settingsPage.h"

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

SettingsPage::SettingsPage(QWidget* w1, QSpinBox* s1, QSpinBox* s2, QDoubleSpinBox* ds1, QDoubleSpinBox* ds2, QDoubleSpinBox* ds3, QWidget* parent)
    : QWidget(parent), widget(nullptr)
{
    this->widget = w1;
    this->gpsLatSelector = s1;
    this->gpsLongSelector = s2;
    this->gpsLatSelectorStart = ds1;
    this->gpsLongSelectorStart = ds2;
    this->maxDistanceSelector = ds3;

    CANData::setGPSCoordinates(gpsLatSelector->value(), gpsLongSelector->value());
    MapPage::loadSettings(gpsLatSelectorStart->value(), gpsLongSelectorStart->value(), maxDistanceSelector->value());
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::setGPSSettings(int GPSLat, int GPSLong, double GPSLatStart, double GPSLongStart, double MaxDistanceForNewLapThreshold)
{
    this->gpsLatSelector->setValue(GPSLat);
    this->gpsLongSelector->setValue(GPSLong);
    this->gpsLatSelectorStart->setValue(GPSLatStart);
    this->gpsLongSelectorStart->setValue(GPSLongStart);
    this->maxDistanceSelector->setValue(MaxDistanceForNewLapThreshold);
}

void SettingsPage::on_gpsLatSelector_valueChanged()
{
    CANData::setGPSCoordinates(gpsLatSelector->value(), gpsLongSelector->value());
}

void SettingsPage::on_mapPage_loadSettings()
{
    MapPage::loadSettings(gpsLatSelectorStart->value(), gpsLongSelectorStart->value(), maxDistanceSelector->value());
}
