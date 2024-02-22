#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "homePage.h"
#include "telemetryPage.h"
#include "settingsPage.h"
#include "databaseManager.h"

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
    HomePage *homePage;
    TelemetryPage* telemetryPage;
    SettingsPage* settingsPage;

private:
    DatabaseManager* database;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void connectDatabase();

private slots:
    void on_quitButton_clicked();
    void on_maximizeButton_clicked();
    void on_minimizeButton_clicked();

    void on_homeButton_clicked();
    void on_telemetryButton_clicked();
    void on_settingsButton_clicked();
    void on_lapTimeButton_clicked();

private slots:
    void on_serialConnectDisconnectButton_clicked();
    void on_autoScaleSelectorCheckBox_stateChanged(int arg1);

private slots:
    void on_gpsLatSelector_valueChanged(int arg1);
    void on_gpsLongSelector_valueChanged(int arg1);
    void on_rpmCheckBox_stateChanged(int arg1);
    void on_coolantTempCheckBox_stateChanged(int arg1);
    void on_afrCheckBox_stateChanged(int arg1);
    void on_oilPressureCheckBox_stateChanged(int arg1);
    void on_throttlePositionCheckBox_stateChanged(int arg1);
    void on_bspdCheckBox_stateChanged(int arg1);
    void on_brakePressureCheckBox_stateChanged(int arg1);
    void on_steeringAngleCheckBox_stateChanged(int arg1);
    void on_gpsLatitudeCheckBox_stateChanged(int arg1);
    void on_gpsLongitudeCheckBox_stateChanged(int arg1);
    void on_speedCheckBox_stateChanged(int arg1);
    void on_damper1CheckBox_stateChanged(int arg1);
    void on_damper2CheckBox_stateChanged(int arg1);
    void on_damper3CheckBox_stateChanged(int arg1);
    void on_damper4CheckBox_stateChanged(int arg1);
    void on_accelerationXCheckBox_stateChanged(int arg1);
    void on_accelerationYCheckBox_stateChanged(int arg1);
    void on_accelerationZCheckBox_stateChanged(int arg1);
    void on_angleXCheckBox_stateChanged(int arg1);
    void on_angleYCheckBox_stateChanged(int arg1);
    void on_angleZCheckBox_stateChanged(int arg1);
};

#endif // MAINWINDOW_H
