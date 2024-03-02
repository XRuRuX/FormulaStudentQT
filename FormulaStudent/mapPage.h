#ifndef MAPPAGE_H
#define MAPPAGE_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QtWebEngineWidgets>
#include <QtLocation>

class MapPage: public QWidget
{
    Q_OBJECT

private:
    QWidget* widget;
    QVBoxLayout* centralContainer;
    static QWebEngineView* webView;

public:
    MapPage(QWidget *parent = nullptr);
    MapPage(QVBoxLayout* w1, QWidget *parent = nullptr);
    ~MapPage();

public:
    static void addPointToMap(double latitude, double longitude);
};

#endif // MAPPAGE_H
