#ifndef MAPPAGE_H
#define MAPPAGE_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QtWebEngineWidgets>
#include <QTime>
#include "GeoDistance.h"

class MapPage: public QWidget
{
    Q_OBJECT

private:
    static double GPSLatStart;
    static double GPSLongStart;
    static double MaxDistanceForNewLapThreshold;    // Maximum distance from start point to be considered a new lap

private:
    QWidget* widget;
    QVBoxLayout* centralContainer;
    QLabel* currentLap;
    QLabel* lastLap;
    QWebEngineView* webView;

private:
    QTimer* timer;
    bool timerStarted = false;
    bool sameLapCheck; // Checks if the car is on the same lap or if it had passed the start
    QElapsedTimer lapTimer;
    QString timeText;

public:
    MapPage(QWidget *parent = nullptr);
    MapPage(QVBoxLayout* w1, QLabel* currentLap, QLabel* lastLap, QWidget *parent = nullptr);
    ~MapPage();

private:
    void checkIfNewLap(double GPSLong, double GPSLat);
    void updateLapTime();

public:
    void addPointToMap(double latitude, double longitude);
    static void loadSettings(double _GPSLatStart, double _GPSLongStart, double _MaxDistanceForNewLapThreshold);

public slots:
    void removeAllPoints();

signals:
    void newLapDetected();
};

#endif // MAPPAGE_H
