#ifndef MAPPAGE_H
#define MAPPAGE_H

#define EARTH_RADIUS_KM 6371.0

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QtWebEngineWidgets>
#include <QtLocation>

class MapPage: public QWidget
{
    Q_OBJECT

private:
    static double GPSLatStart;
    static double GPSLongStart;
    static bool sameLapCheck; // Checks if the car is on the same lap or if it had passed the start

private:
    QWidget* widget;
    QVBoxLayout* centralContainer;
    static QWebEngineView* webView;

public:
    MapPage(QWidget *parent = nullptr);
    MapPage(QVBoxLayout* w1, QWidget *parent = nullptr);
    ~MapPage();

private:
    static void checkIfNewLap(double GPSLong, double GPSLat);

public:
    static void addPointToMap(double latitude, double longitude);
    static void setStartGPSCoordinates(double _GPSLatStart, double _GPSLongStart);

public slots:
    void removeAllPoints();
};

#endif // MAPPAGE_H

double toRadians(double degrees);
double distanceBetweenCoordinates(double latitudeStart, double longitudeStart, double latitudeStop, double longitudeStop);
