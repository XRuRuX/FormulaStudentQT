#include "settingsPage.h"

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

SettingsPage::SettingsPage(QWidget* w1, QSpinBox* s1, QSpinBox* s2, QWidget* parent)
    : QWidget(parent), widget(nullptr)
{
    this->widget = w1;
    this->gpsLatSelector = s1;
    this->gpsLongSelector = s2;

    CANData::setGPSCoordinates(gpsLatSelector->value(), gpsLongSelector->value());
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::on_gpsLatSelector_valueChanged()
{
    CANData::setGPSCoordinates(gpsLatSelector->value(), gpsLongSelector->value());
}
