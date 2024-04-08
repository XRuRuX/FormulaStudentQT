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

#define NO_GRAPHS 37

#define SECONDSECUON_PLOT 0
#define MAINPULSEB1_PLOT 1
#define MAINPULSEB2_PLOT 2
#define RPM_PLOT 3
#define AFRTARGET1_PLOT 4
#define AFRTARGET2_PLOT 5
#define MANIFOLDAIRP_PLOT 6
#define MANIFOLDAIRTEMP_PLOT 7
#define COOLANTTEMP_PLOT 8
#define THROTTLEPOS_PLOT 9
#define BATTERYV_PLOT 10
#define AIRDCORR_PLOT 11
#define WARMUPCORR_PLOT 12
#define TPSBASEDACC_PLOT 13
#define TPSBASEDFUEL_PLOT 14
#define TOTALFUELCORR_PLOT 15
#define VEVALUETB1_PLOT 16
#define VEVALUETB2_PLOT 17
#define COLDADVANCE_PLOT 18
#define RATEOFCHANGETPS_PLOT 19
#define RATEOFCHANGERPM_PLOT 20
#define SYNCLOSSCOUNTER_PLOT 21
#define SYNCLOSSREASONCODE_PLOT 22
#define AVERAFUELF_PLOT 23
#define BSPD_PLOT 24
#define BRAKEPRESSURE_PLOT 25
#define STEERINGANGLE_PLOT 26
#define GPSLAT_PLOT 27
#define GPSLONG_PLOT 28
#define GPSSPEED_PLOT 29
#define DAMPER1_PLOT 30
#define DAMPER2_PLOT 31
#define DAMPER3_PLOT 32
#define DAMPER4_PLOT 33
#define ROLL_PLOT 34
#define PITCH_PLOT 35
#define YAW_PLOT 36

struct PlotStates
{
    bool plotStates[NO_GRAPHS];
};

struct ColorStates
{
    QColor colorStates[NO_GRAPHS];
};

class TelemetryPage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QWidget* centralContainer;
    QSerialPort serialPort;
    QComboBox* comPortSelector;
    QPushButton* serialConnectDisconnectButton;
    QVector<QCustomPlot*> customPlots;
    QCustomPlot* selectedCustomPlot;
    QTimer* timerGraphRefresh;
    QTimer* timerCheckComPorts;

private:
    QProgressBar* progressBar;

private:
    MapPage* mapPage;

private:
    CANData CANData;

public:
    QVector<PlotStates> plotStates;     // Vector for storing variable states to determine what to plot
    QVector<ColorStates> colorStates;   // Vector for storing variabile states to determine the colors to plot
    QStringList graphNames;             // String list with every graph name

private:
    bool isSerialComConnected = false;
    bool loadButtonPressed = false;
    QByteArray serialDataBuffer;
    int totalLinesReadSerial = 0;
    int maxNumberOfPoints;

public:
    TelemetryPage(QWidget *parent = nullptr);
    TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector,
                  QPushButton* serialConnectDisconnectButton, QWidget* centralContainer, MapPage* mapPage, QWidget *parent = nullptr);
    ~TelemetryPage();

private:
    void initializeGraph(QCustomPlot* graph);
    void initializeLegend(QCustomPlot* graph);
    void initializeSerialPort();

public slots:
    void on_serialConnectDisconnectButton_clicked();
    void on_loadButton_clicked();
    void on_addGraphButton_clicked();
    void on_removeGraphButton_clicked();
    void drawRedVerticalLine();

private slots:
    void readData();

private:
    void refreshGraph();
    void selectGraph(QCustomPlot *graphToSelect);

private slots:
    void syncXAxis(const QCPRange &range);

private:
    void checkComPorts();
    bool eventFilter(QObject *watched, QEvent *event);

public:
    void changeValueDisplayed(int valueName, int graphNumber);
    void changeGraphColor(int graphName, int graphNumber, QColor colorValue);
    void changeLegendValues();

signals:
    void addNewGraphDetected();
    void deletedGraphDetected();
};

#endif // TELEMETRYPAGE_H
