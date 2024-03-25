#ifndef TELEMETRYPAGE_H
#define TELEMETRYPAGE_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFileDialog>
#include <QColor>
#include "qcustomplot.h"
#include <QRegularExpression>
#include <QDateTime>
#include "candata.h"
#include "progressDialog.h"

#define NO_GRAPHS 21

#define RPM_PLOT 0
#define COOLANTTEMP_PLOT 1
#define AFR_PLOT 2
#define OILPRESSURE_PLOT 3
#define THROTTLEPOS_PLOT 4
#define BSPD_PLOT 5
#define BRAKEPRESSURE_PLOT 6
#define STEERINGANGLE_PLOT 7
#define GPSLAT_PLOT 8
#define GPSLONG_PLOT 9
#define GPSSPEED_PLOT 10
#define DAMPER1_PLOT 11
#define DAMPER2_PLOT 12
#define DAMPER3_PLOT 13
#define DAMPER4_PLOT 14
#define AX_PLOT 15
#define AY_PLOT 16
#define AZ_PLOT 17
#define GX_PLOT 18
#define GY_PLOT 19
#define GZ_PLOT 20

class TelemetryPage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QSerialPort serialPort;
    QCustomPlot* customPlot;
    QComboBox* comPortSelector;
    QPushButton* serialConnectDisconnectButton;
    QTimer* timerGraphRefresh;
    QTimer* timerCheckComPorts;

private:
    QProgressBar* progressBar;

private:
    MapPage* mapPage;

private:
    CANData CANData;

private:
    bool plotStates[NO_GRAPHS] = {false}; // Vector for storing variable states to determine what to plot
    QStringList graphNames;     // String list with every graph name

private:
    bool isSerialComConnected = false;
    bool loadButtonPressed = false;
    QByteArray serialDataBuffer;
    int totalLinesReadSerial = 0;
    int maxNumberOfPoints;

public:
    TelemetryPage(QWidget *parent = nullptr);
    TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector,
                  QPushButton* serialConnectDisconnectButton, MapPage* mapPage, QWidget *parent = nullptr);
    ~TelemetryPage();

private:
    void initializeGraph();
    void initializeLegend();
    void initializeSerialPort();

public slots:
    void on_serialConnectDisconnectButton_clicked();
    void on_loadButton_clicked();

private slots:
    void readData();

private:
    void refreshGraph();

private:
    void checkComPorts();

public:
    void changeValueDisplayed(int valueName);
    void changeGraphColor(int graphName, QColor colorValue);
    void changeLegendValues();
};

#endif // TELEMETRYPAGE_H
