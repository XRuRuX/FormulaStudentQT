#include "settingsPage.h"

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

SettingsPage::SettingsPage(QWidget* w1, QSpinBox* s1, QSpinBox* s2, QDoubleSpinBox* ds1, QDoubleSpinBox* ds2, QDoubleSpinBox* ds3, QComboBox* c3, QWidget* parent)
    : QWidget(parent), widget(nullptr)
{
    this->widget = w1;
    this->gpsLatSelector = s1;
    this->gpsLongSelector = s2;
    this->gpsLatSelectorStart = ds1;
    this->gpsLongSelectorStart = ds2;
    this->maxDistanceSelector = ds3;
    this->graphSelectorComboBox = c3;

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

// Method to change graph color
void SettingsPage::setColorForButtonGraphSettings(QPushButton *button, int graphName, TelemetryPage* telemetryPage)
{
    // Extracts color
    QColor colorValue = QColorDialog::getColor(Qt::white, this, tr("Select color"));

    // Converts it to string
    QString colorString = QString("rgb(%1, %2, %3)").arg(colorValue.red()).arg(colorValue.green()).arg(colorValue.blue());

    // Set button color based on the string
    button->setStyleSheet(QString("background-color: %1;").arg(colorString));

    telemetryPage->changeGraphColor(graphName, colorValue);
}

void SettingsPage::newGraphAdded()
{
    int noElements = graphSelectorComboBox->count();
    graphSelectorComboBox->addItem(QString("Plot %1").arg(noElements + 1));
}

void SettingsPage::deletedGraph()
{
    int noElements = graphSelectorComboBox->count();
    graphSelectorComboBox->removeItem(noElements - 1);
}

void SettingsPage::on_graphSelectorComboBox_currentIndexChanged(PlotStates plotStates, int noCheckBoxes, QCheckBox* checkBoxes[])
{
    // Update each checkbox based on the plotStates
    for (int i = 0; i < noCheckBoxes; ++i)
    {
        checkBoxes[i]->setChecked(plotStates.plotStates[i]);
    }
}
