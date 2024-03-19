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

    mapPage = new MapPage(ui->mapCentralContainer, ui->mapCurrentLap, ui->mapLastLap, this);

    telemetryPage = new TelemetryPage(ui->telemetryPage, ui->customPlot, ui->comPortSelector,
                                    ui->serialConnectDisconnectButton, mapPage, this);

    settingsPage = new SettingsPage(ui->settingsPage, ui->gpsLatSelector, ui->gpsLongSelector, ui->gpsLatSelectorStart, ui->gpsLongSelectorStart, ui->maxDistanceSelector, this);

    database = new DatabaseManager();
    this->connectDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete homePage;
    delete telemetryPage;
    delete settingsPage;
    delete mapPage;
    delete database;
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
        double GPSLatStart = query.value(2).toDouble();
        double GPSLongStart =  query.value(3).toDouble();
        double MaxDistanceForNewLapThreshold = query.value(4).toDouble();

        settingsPage->setGPSSettings(GPSLat, GPSLong, GPSLatStart, GPSLongStart, MaxDistanceForNewLapThreshold);
    }
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

// Shows map page
void MainWindow::on_mapButton_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(3);
}

// Shows settings page
void MainWindow::on_settingsButton_clicked()
{
    ui->mainBodyContainer->setCurrentIndex(4);
}

// Connect or Disconnect from Serial Data
void MainWindow::on_serialConnectDisconnectButton_clicked()
{
    telemetryPage->on_serialConnectDisconnectButton_clicked();
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

void MainWindow::on_gpsLatSelectorStart_valueChanged(double arg1)
{
    settingsPage->on_mapPage_loadSettings();
    database->updateSetting("GPSLatStart", ui->gpsLatSelectorStart->value());
}

void MainWindow::on_gpsLongSelectorStart_valueChanged(double arg1)
{
    settingsPage->on_mapPage_loadSettings();
    database->updateSetting("GPSLongStart", ui->gpsLongSelectorStart->value());
}

void MainWindow::on_maxDistanceSelector_valueChanged(double arg1)
{
    settingsPage->on_mapPage_loadSettings();
    database->updateSetting("MaxDistanceForNewLapThreshold", ui->maxDistanceSelector->value());
}

void MainWindow::on_rpmCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(RPM_PLOT);
}


void MainWindow::on_coolantTempCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(COOLANTTEMP_PLOT);
}


void MainWindow::on_afrCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(AFR_PLOT);
}


void MainWindow::on_oilPressureCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(OILPRESSURE_PLOT);
}


void MainWindow::on_throttlePositionCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(THROTTLEPOS_PLOT);
}


void MainWindow::on_bspdCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(BSPD_PLOT);
}


void MainWindow::on_brakePressureCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(BRAKEPRESSURE_PLOT);
}


void MainWindow::on_steeringAngleCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(STEERINGANGLE_PLOT);
}


void MainWindow::on_gpsLatitudeCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(GPSLAT_PLOT);
}


void MainWindow::on_gpsLongitudeCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(GPSLONG_PLOT);
}


void MainWindow::on_speedCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(GPSSPEED_PLOT);
}


void MainWindow::on_damper1CheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(DAMPER1_PLOT);
}


void MainWindow::on_damper2CheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(DAMPER2_PLOT);
}


void MainWindow::on_damper3CheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(DAMPER3_PLOT);
}


void MainWindow::on_damper4CheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(DAMPER4_PLOT);
}


void MainWindow::on_accelerationXCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(AX_PLOT);
}


void MainWindow::on_accelerationYCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(AY_PLOT);
}


void MainWindow::on_accelerationZCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(AZ_PLOT);
}


void MainWindow::on_angleXCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(GX_PLOT);
}


void MainWindow::on_angleYCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(GY_PLOT);
}


void MainWindow::on_angleZCheckBox_stateChanged(int arg1)
{
    telemetryPage->changeValueDisplayed(GZ_PLOT);
}


void MainWindow::on_rpmColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->rpmColorPicker, RPM_PLOT, telemetryPage);
}


void MainWindow::on_coolantTempColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->coolantTempColorPicker, COOLANTTEMP_PLOT, telemetryPage);
}


void MainWindow::on_afrColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->afrColorPicker, AFR_PLOT, telemetryPage);
}


void MainWindow::on_oilPressureColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->oilPressureColorPicker, OILPRESSURE_PLOT, telemetryPage);
}


void MainWindow::on_throttlePositionColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->throttlePositionColorPicker, THROTTLEPOS_PLOT, telemetryPage);
}


void MainWindow::on_bspdColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->bspdColorPicker, BSPD_PLOT, telemetryPage);
}


void MainWindow::on_brakePressureColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->brakePressureColorPicker, BRAKEPRESSURE_PLOT, telemetryPage);
}


void MainWindow::on_steeringAngleColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->steeringAngleColorPicker, STEERINGANGLE_PLOT, telemetryPage);
}


void MainWindow::on_gpsLatitudeColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->gpsLatitudeColorPicker, GPSLAT_PLOT, telemetryPage);
}


void MainWindow::on_gpsLongitudeColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->gpsLongitudeColorPicker, GPSLONG_PLOT, telemetryPage);
}


void MainWindow::on_speedColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->speedColorPicker, GPSSPEED_PLOT, telemetryPage);
}


void MainWindow::on_damper1ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper1ColorPicker, DAMPER1_PLOT, telemetryPage);
}


void MainWindow::on_damper2ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper2ColorPicker, DAMPER2_PLOT, telemetryPage);
}


void MainWindow::on_damper3ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper3ColorPicker, DAMPER3_PLOT, telemetryPage);
}


void MainWindow::on_damper4ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper4ColorPicker, DAMPER4_PLOT, telemetryPage);
}


void MainWindow::on_accelerationXColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->accelerationXColorPicker, AX_PLOT, telemetryPage);
}


void MainWindow::on_accelerationYColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->accelerationYColorPicker, AY_PLOT, telemetryPage);
}


void MainWindow::on_accelerationZColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->accelerationZColorPicker, AZ_PLOT, telemetryPage);
}


void MainWindow::on_angleXColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->angleXColorPicker, GX_PLOT, telemetryPage);
}


void MainWindow::on_angleYColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->angleYColorPicker, GY_PLOT, telemetryPage);
}


void MainWindow::on_angleZColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->angleZColorPicker, GZ_PLOT, telemetryPage);
}


void MainWindow::on_mapResetButton_clicked()
{
    mapPage->removeAllPoints();
}

