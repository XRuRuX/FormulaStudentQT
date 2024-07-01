#include "mapPage.h"

// Static variabiles that can be changed on the Settings tab
double MapPage::GPSLatStart = 0;
double MapPage::GPSLongStart = 0;
double MapPage::MaxDistanceForNewLapThreshold = 0;

MapPage::MapPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

MapPage::MapPage(QVBoxLayout* w1, QLabel* currentLap, QLabel* lastLap, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    this->centralContainer = w1;
    this->currentLap = currentLap;
    this->lastLap = lastLap;

    sameLapCheck = true;

    // Start timer to update lap time label
    timer = new QTimer(this);
    timer->setInterval(50);
    connect(timer, &QTimer::timeout, this, &MapPage::updateLapTime);

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
    if (!timerStarted) {
            lapTimer.start();
            timer->start();
            timerStarted = true;
        }

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

    double distance = GeoDistance::distanceBetweenCoordinates(GPSLatStart, GPSLongStart, GPSLat, GPSLong);

    // Check if the distance to the starting point is smaller than the distance in the settings
    if(distance < MaxDistanceForNewLapThreshold)
    {
        sameLapCheck = false;
    }

    if(sameLapCheck == false)
    {
        // Reset the timer for the new lap
        lapTimer.restart();

        // Stops updating the time to the next point
        timerStarted = false;
        QString jsCode = QString("setAllCirclesOpacity(0.1);");
        webView->page()->runJavaScript(jsCode);
        sameLapCheck = true;
        lastLap->setText(timeText);

        // Send signal to telemetry page
        emit newLapDetected();
    }
}

void MapPage::loadSettings(double _GPSLatStart, double _GPSLongStart, double _MaxDistanceForNewLapThreshold)
{
    GPSLatStart = _GPSLatStart;
    GPSLongStart = _GPSLongStart;
    MaxDistanceForNewLapThreshold = _MaxDistanceForNewLapThreshold;
}

void MapPage::updateLapTime()
{
    // If the timer hasn't been started, don't update the time
    if (!timerStarted) return;

    // Time in milliseconds since the timer was started
    qint64 time = lapTimer.elapsed();
    int minutes = time / 60000;
    int seconds = (time % 60000) / 1000;
    int milliseconds = (time % 1000);
    timeText = QString::asprintf("Current lap: %d:%02d:%03d", minutes, seconds, milliseconds);

    currentLap->setText(timeText);
}
