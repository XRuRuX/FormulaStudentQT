#include "mapPage.h"

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
}
