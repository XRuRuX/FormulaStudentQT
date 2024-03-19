#ifndef CANDATA_H
#define CANDATA_H

#include <QRegularExpression>
#include <QDebug>
#include "MapPage.h"

class CANData
{
private:
    static int GPSLatInt;
    static int GPSLongInt;

public:
    double initialTimestamp;
    QList<double> RPMT;
    QList<double> RPM;
    QList<double> CoolantT;
    QList<double> CoolantTemp;
    QList<double> AFRT;
    QList<double> ThrottlePos;
    QList<double> AFR;
    QList<double> AnalogT;
    QList<double> BSPD;
    QList<double> BrakePressure;
    QList<double> SteeringAngle;
    QList<double> OilPressure;
    QList<double> GPST;
    QList<double> GPSLat;
    QList<double> GPSLong;
    QList<double> GPSSpeed;
    QList<double> DamperT;
    QList<double> Damper1;
    QList<double> Damper2;
    QList<double> Damper3;
    QList<double> Damper4;
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
    void extractDataFromString(const QString& data, MapPage* mapPage);

public:
    void clearData();

public:
    static void setGPSCoordinates(int GPSLat, int GPSLong);
};

void hexToIntNegative(QString hexGroup[], int values[], int noValues);
void hexToInt(QString hexGroup[], int values[], int noValues);

double fahrenheitToCelsius(double fahrenheit);

#endif // CANDATA_H
