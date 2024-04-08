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
    // 05f0
    QList<double> ID05f0T;
    QList<double> SecondsECUOn;
    QList<double> MainPulseB1;
    QList<double> MainPulseB2;
    QList<double> RPM;
    // 05f1
    QList<double> ID05f1T;
    QList<double> AFRTarget1;
    QList<double> AFRTarget2;
    // 05f2
    QList<double> ID05f2T;
    QList<double> ManifoldAirP;
    QList<double> ManifoldAirTemp;
    QList<double> CoolantTemp;
    // 05f3
    QList<double> ID05f3T;
    QList<double> ThrottlePos;
    QList<double> BatteryV;
    // 05f4
    QList<double> ID05f4T;
    QList<double> AirDCorr;
    // 05f5
    QList<double> ID05f5T;
    QList<double> WarmupCorr;
    QList<double> TPSBasedAcc;
    QList<double> TPSBasedFuelCut;
    // 05f6
    QList<double> ID05f6T;
    QList<double> TotalFuelCorr;
    QList<double> VEValueTB1;
    QList<double> VEValueTB2;
    // 05f7
    QList<double> ID05f7T;
    QList<double> ColdAdvance;
    QList<double> RateOfChangeTPS;
    QList<double> RateOfChangeRPM;
    // 061b
    QList<double> ID061bT;
    QList<double> SyncLossCounter;
    QList<double> SyncLossReasonCode;
    // 0624
    QList<double> ID0624T;
    QList<double> AverageFuelF;
    // 0115
    QList<double> ID0115T;
    QList<double> BSPD;
    QList<double> BrakePressure;
    QList<double> SteeringAngle;
    // 0116
    QList<double> ID0116T;
    QList<double> GPSLat;
    QList<double> GPSLong;
    QList<double> GPSSpeed;
    // 0112
    QList<double> ID0112T;
    QList<double> Damper1;
    QList<double> Damper2;
    QList<double> Damper3;
    QList<double> Damper4;
    // 0113
    QList<double> ID0113T;
    QList<double> Roll;
    QList<double> Pitch;
    QList<double> Yaw;

public:
    CANData();

public:
    void extractDataFromString(const QString& data, MapPage* mapPage);

public:
    void clearData();

public:
    static void setGPSCoordinates(int GPSLat, int GPSLong);
};

void hexToInt(QString hexGroup[], int values[], int noValues);

double fahrenheitToCelsius(double fahrenheit);

#endif // CANDATA_H
