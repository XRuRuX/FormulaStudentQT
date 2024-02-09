#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   // Remove the current titlebar to implement a custom one
    isWindowMaximized = false;
    ui->mainBodyContainer->setCurrentIndex(0);              // Set first page as Home Page

    homePage = new HomePage(ui->homePage, ui->timeLabel, ui->dateLabel, this);
    homePage->updateDateAndTime();

    telemetryPage = new TelemetryPage(ui->telemetryPage, ui->customPlot, ui->comPortSelector, ui->valueGraphSelector,
                                    ui->serialConnectDisconnectButton, ui->autoScaleSelectorCheckBox, this);

    settingsPage = new SettingsPage(ui->settingsPage, ui->gpsLatSelector, ui->gpsLongSelector, this);

    database = new DatabaseManager();
    this->connectDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete homePage;
    delete telemetryPage;
    delete settingsPage;
}

// Quit button
void MainWindow::on_quitButton_clicked()
{
    QCoreApplication::quit();
}

// Maximize button
void MainWindow::on_maximizeButton_clicked()
{
    if(isWindowMaximized)
    {
        showNormal();
        isWindowMaximized = false;
    }
    else
    {
        showMaximized();
        isWindowMaximized = true;
    }
}

// Minimize button
void MainWindow::on_minimizeButton_clicked()
{
    showMinimized();
}

// Shows home page
void MainWindow::on_homeButton_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(0);
}

// Shows telemetry page
void MainWindow::on_telemetryButton_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(1);
}

// Shows lap timer page
void MainWindow::on_lapTimeButton_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(2);
}

// Shows settings page
void MainWindow::on_settingsButton_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(3);
}

// Connect or Disconnect from Serial Data
void MainWindow::on_serialConnectDisconnectButton_clicked()
{
    telemetryPage->on_serialConnectDisconnectButton_clicked();
}


void MainWindow::on_autoScaleSelectorCheckBox_stateChanged(int arg1)
{
    telemetryPage->on_autoScaleSelectorCheckBox_stateChanged();
}


void MainWindow::on_gpsLatSelector_valueChanged(int arg1)
{
    settingsPage->on_gpsLatSelector_valueChanged();
    database->updateSetting("GPSLat", ui->gpsLatSelector->value());
}


void MainWindow::on_gpsLongSelector_valueChanged(int arg1)
{
    settingsPage->on_gpsLatSelector_valueChanged();
    database->updateSetting("GPSLong", ui->gpsLongSelector->value());
}

// Connect to the local database for loading the settings
void MainWindow::connectDatabase()
{
    QString relativePath = QDir::current().filePath("../FormulaStudent/Database/database.db");

    if(database->connect(relativePath))
    {
        qDebug() << "Database connected";
    }

    QSqlQuery query = database->queryData();
    while(query.next())
    {
        int GPSLat = query.value(0).toInt();
        int GPSLong = query.value(1).toInt();

        settingsPage->setGPSSettings(GPSLat, GPSLong);
    }
}
