#include "mapPage.h"

// Static variabiles that can be changed on the Settings tab
double MapPage::GPSLatStart = 0;
double MapPage::GPSLongStart = 0;
bool MapPage::sameLapCheck = true;

// Inițializare a membrului static
QWebEngineView* MapPage::webView = nullptr;

MapPage::MapPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

MapPage::MapPage(QVBoxLayout* w1, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    centralContainer = w1;

    // Load the web page
    webView = new QWebEngineView;
    QUrl url = QUrl("qrc:/API/API/googlemaps.html");

    // Display it in the main container
    webView->page()->load(url);
    centralContainer->addWidget(webView);
}

MapPage::~MapPage()
{
    delete webView;
}

void MapPage::addPointToMap(double latitude, double longitude)
{
    QString latString = QString::number(latitude);
    QString lngString = QString::number(longitude);

    QString jsCode = QString("addCircle(%1, %2);").arg(latString, lngString);

    webView->page()->runJavaScript(jsCode);

    checkIfNewLap(longitude, latitude);
}

void MapPage::removeAllPoints()
{
    QString jsCode = QString("removeAllCircles();");

    webView->page()->runJavaScript(jsCode);
}

void MapPage::checkIfNewLap(double GPSLong, double GPSLat)
{

    double distance = distanceBetweenCoordinates(GPSLatStart, GPSLongStart, GPSLat, GPSLong);

    // Check if the distance to the starting point is smaller than the distance in the settings
    if(distance < 20)
    {
        sameLapCheck = false;
    }

    if(sameLapCheck == false)
    {
        QString jsCode = QString("setAllCirclesOpacity(0.1);");
        webView->page()->runJavaScript(jsCode);
        sameLapCheck = true;
    }
}

void MapPage::setStartGPSCoordinates(double _GPSLatStart, double _GPSLongStart)
{
    GPSLatStart = _GPSLatStart;
    GPSLongStart = _GPSLongStart;
}

double toRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

double distanceBetweenCoordinates(double latitudeStart, double longitudeStart, double latitudeStop, double longitudeStop)
{
    double distance;

    // Radian conversion
    double lat1 = toRadians(latitudeStart);
    double lon1 = toRadians(longitudeStart);
    double lat2 = toRadians(latitudeStop);
    double lon2 = toRadians(longitudeStop);

    // Difference between longitude and latitude
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    // Haversine formula
    double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    // Distance in meters
    distance = EARTH_RADIUS_KM * c * 1000;

    qDebug() << distance;

    return distance;
}
