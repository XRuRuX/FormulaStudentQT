#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "homePage.h"
#include "telemetryPage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    bool isWindowMaximized;

private:
    HomePage *homePage;
    TelemetryPage* telemetryPage;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_quitButton_clicked();
    void on_maximizeButton_clicked();
    void on_minimizeButton_clicked();

    void on_homeButton_clicked();
    void on_telemetryButton_clicked();

    void on_lapTimeButton_clicked();

private slots:
    void on_serialConnectDisconnectButton_clicked();
};

#endif // MAINWINDOW_H
