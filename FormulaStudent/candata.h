#ifndef CANDATA_H
#define CANDATA_H

#include <QRegularExpression>
#include <QDebug>

class CANData
{
private:
    static int GPSLatInt;
    static int GPSLongInt;

public:
    double initialTimestamp;
    QList<double> GPST;
    QList<double> GPSLat;
    QList<double> GPSLong;
    QList<double> GPSSpeed;
    QList<double> GT;
    QList<double> Gx;
    QList<double> Gy;
    QList<double> Gz;
    QList<double> AT;
    QList<double> Ax;
    QList<double> Ay;
    QList<double> Az;

public:
    CANData();

public:
    void extractDataFromString(const QString& data);

public:
    static void setGPSCoordinates(int GPSLat, int GPSLong);
};

void hexToIntNegative(QString hexGroup[], int values[], int noValues);
void hexToInt(QString hexGroup[], int values[], int noValues);

#endif // CANDATA_H
