#ifndef TELEMETRYPAGE_H
#define TELEMETRYPAGE_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "qcustomplot.h"
#include "qtimer.h"
#include <QRegularExpression>
#include <QDateTime>

class TelemetryPage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QSerialPort serialPort;
    QCustomPlot* customPlot;
    QComboBox* comPortSelector;
    QPushButton* serialConnectDisconnectButton;
    QTimer* timer;

private:
    bool isSerialComConnected = false;
    QByteArray serialDataBuffer;
    int totalLinesReadSerial = 0;
    int maxNumberOfPoints;
    QList<double> xAxisDataCount;
    QList<double> yAxisData;

private:
    bool isGraphDragged = false;

public:
    TelemetryPage(QWidget *parent = nullptr);
    TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector, QPushButton* serialConnectDisconnectButton, QWidget *parent = nullptr);
    ~TelemetryPage();

private:
    void initializeGraph();
    void initializeSerialPort();

public slots:
    void on_serialConnectDisconnectButton_clicked();

private slots:
    void readData();

private:
    void refreshGraph();

private slots:
    void onGraphMousePress(QMouseEvent *event);
    void onGraphMouseRelease(QMouseEvent *event);
};

#endif // TELEMETRYPAGE_H
