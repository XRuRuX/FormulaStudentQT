#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMouseEvent>
#include "homePage.h"
#include "telemetryPage.h"
#include "settingsPage.h"
#include "databaseManager.h"
#include "mapPage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    bool isWindowMaximized;

private:
    int initialWindowWidth;
    int initialWindowHeight;

private:
    HomePage *homePage;
    TelemetryPage* telemetryPage;
    SettingsPage* settingsPage;
    MapPage* mapPage;

private:
    bool mouseDragging = false;
    QPoint mouseDragPosition;

private:
    DatabaseManager* database;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void connectDatabase();

// Methods for window movement
private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

// Slots for title bar buttons
private slots:
    void on_quitButton_clicked();
    void on_maximizeButton_clicked();
    void on_minimizeButton_clicked();

// Slots for menu buttons
private slots:
    void on_homeButton_clicked();
    void on_telemetryButton_clicked();
    void on_settingsButton_clicked();
    void on_lapTimeButton_clicked();
    void on_mapButton_clicked();

// Slots for telemetry page
private slots:
    void on_serialConnectDisconnectButton_clicked();
    void on_loadButton_clicked();
    void on_addGraphButton_clicked();

// Slots for settings page
private slots:
    void on_gpsLatSelector_valueChanged(int arg1);
    void on_gpsLongSelector_valueChanged(int arg1);
    void on_graphSelectorComboBox_currentIndexChanged(int index);
    void on_rpmCheckBox_clicked();
    void on_coolantTempCheckBox_clicked();
    void on_afrCheckBox_clicked();
    void on_oilPressureCheckBox_clicked();
    void on_throttlePositionCheckBox_clicked();
    void on_bspdCheckBox_clicked();
    void on_brakePressureCheckBox_clicked();
    void on_steeringAngleCheckBox_clicked();
    void on_gpsLatitudeCheckBox_clicked();
    void on_gpsLongitudeCheckBox_clicked();
    void on_maxDistanceSelector_valueChanged(double arg1);
    void on_gpsLatSelectorStart_valueChanged(double arg1);
    void on_gpsLongSelectorStart_valueChanged(double arg1);
    void on_speedCheckBox_clicked();
    void on_damper1CheckBox_clicked();
    void on_damper2CheckBox_clicked();
    void on_damper3CheckBox_clicked();
    void on_damper4CheckBox_clicked();
    void on_accelerationXCheckBox_clicked();
    void on_accelerationYCheckBox_clicked();
    void on_accelerationZCheckBox_clicked();
    void on_angleXCheckBox_clicked();
    void on_angleYCheckBox_clicked();
    void on_angleZCheckBox_clicked();
    void on_rpmColorPicker_clicked();
    void on_coolantTempColorPicker_clicked();
    void on_afrColorPicker_clicked();
    void on_oilPressureColorPicker_clicked();
    void on_throttlePositionColorPicker_clicked();
    void on_bspdColorPicker_clicked();
    void on_brakePressureColorPicker_clicked();
    void on_steeringAngleColorPicker_clicked();
    void on_gpsLatitudeColorPicker_clicked();
    void on_gpsLongitudeColorPicker_clicked();
    void on_speedColorPicker_clicked();
    void on_damper1ColorPicker_clicked();
    void on_damper2ColorPicker_clicked();
    void on_damper3ColorPicker_clicked();
    void on_damper4ColorPicker_clicked();
    void on_accelerationXColorPicker_clicked();
    void on_accelerationYColorPicker_clicked();
    void on_accelerationZColorPicker_clicked();
    void on_angleXColorPicker_clicked();
    void on_angleYColorPicker_clicked();
    void on_angleZColorPicker_clicked();

// Slots for map page
private slots:
    void on_mapResetButton_clicked();
    void on_removeGraphButton_clicked();
};

#endif // MAINWINDOW_H
