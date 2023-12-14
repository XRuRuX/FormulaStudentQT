#ifndef TELEMETRYPAGE_H
#define TELEMETRYPAGE_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "qcustomplot.h"
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
    QCheckBox* autoScaleSelectorCheckBox;
    QTimer* timer;

private:
    bool isSerialComConnected = false;
    QByteArray serialDataBuffer;
    int totalLinesReadSerial = 0;
    int maxNumberOfPoints;
    QList<double> xAxisDataCount;
    QList<double> yAxisData;

private:
    bool isAutoScale = true;

public:
    TelemetryPage(QWidget *parent = nullptr);
    TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector,
                  QPushButton* serialConnectDisconnectButton, QCheckBox* autoScaleSelectorCheckBox, QWidget *parent = nullptr);
    ~TelemetryPage();

private:
    void initializeGraph();
    void initializeSerialPort();

public slots:
    void on_serialConnectDisconnectButton_clicked();
    void on_autoScaleSelectorCheckBox_stateChanged();

private slots:
    void readData();

private:
    void refreshGraph();
};

#endif // TELEMETRYPAGE_H
