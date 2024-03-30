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
    this->setWindowIcon(QIcon(":/Pictures/Images/Pictures/Logo_Taskbar.png"));

    // Initial width and height for resizing window
    QSize windowSize = this->size();
    initialWindowWidth = windowSize.width();
    initialWindowHeight = windowSize.height();

    homePage = new HomePage(ui->homePage, ui->timeLabel, ui->dateLabel, this);

    mapPage = new MapPage(ui->mapCentralContainer, ui->mapCurrentLap, ui->mapLastLap, this);

    telemetryPage = new TelemetryPage(ui->telemetryPage, ui->customPlot, ui->comPortSelector,
                                    ui->serialConnectDisconnectButton, ui->centralContainer, mapPage, this);

    settingsPage = new SettingsPage(ui->settingsPage, ui->gpsLatSelector, ui->gpsLongSelector, ui->gpsLatSelectorStart, ui->gpsLongSelectorStart, ui->maxDistanceSelector, ui->graphSelectorComboBox, this);

    // Connect signals between pages
    QObject::connect(mapPage, &MapPage::newLapDetected, telemetryPage, &TelemetryPage::drawRedVerticalLine);
    QObject::connect(telemetryPage, &TelemetryPage::addNewGraphDetected, settingsPage, &SettingsPage::newGraphAdded);

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

// Press with the mouse on the titlebar
void MainWindow::mousePressEvent(QMouseEvent *event) {
    // Check if the click is inside the titleBar frame
        if (ui->titleBar->rect().contains(event->pos())) {
            mouseDragging = true;
            mouseDragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }

// Move the titlebar
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
        if (mouseDragging) {
            move(event->globalPosition().toPoint() - mouseDragPosition);
            event->accept();
        }
    }

// Release the titlebar
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
        mouseDragging = false;
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
        this->resize(initialWindowWidth, initialWindowHeight);
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

// Load data from file to plot on telemetry page
void MainWindow::on_loadButton_clicked()
{
    telemetryPage->on_loadButton_clicked();
}

// Add new graph to telemetry page
void MainWindow::on_addGraphButton_clicked()
{
    telemetryPage->on_addGraphButton_clicked();
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

void MainWindow::on_rpmCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(RPM_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_coolantTempCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(COOLANTTEMP_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_afrCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AFR_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_oilPressureCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(OILPRESSURE_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_throttlePositionCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(THROTTLEPOS_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_bspdCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(BSPD_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_brakePressureCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(BRAKEPRESSURE_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_steeringAngleCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(STEERINGANGLE_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_gpsLatitudeCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(GPSLAT_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_gpsLongitudeCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(GPSLONG_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_speedCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(GPSSPEED_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_damper1CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(DAMPER1_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_damper2CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(DAMPER2_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_damper3CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(DAMPER3_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_damper4CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(DAMPER4_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_accelerationXCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AX_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_accelerationYCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AY_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_accelerationZCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AZ_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_angleXCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(GX_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_angleYCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(GY_PLOT, ui->graphSelectorComboBox->currentIndex());
}


void MainWindow::on_angleZCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(GZ_PLOT, ui->graphSelectorComboBox->currentIndex());
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

// Changes the state of the check boxes depending on which graph is selected
void MainWindow::on_graphSelectorComboBox_currentIndexChanged(int index)
{
    PlotStates plotStates = telemetryPage->plotStates.at(index);

    // Mapping each checkbox to its respective index in plotStates array
    QCheckBox *checkBoxes[] = {
        ui->rpmCheckBox,             // Index 0
        ui->coolantTempCheckBox,     // Index 1
        ui->afrCheckBox,             // Index 2
        ui->oilPressureCheckBox,     // Index 3
        ui->throttlePositionCheckBox,// Index 4
        ui->bspdCheckBox,            // Index 5
        ui->brakePressureCheckBox,   // Index 6
        ui->steeringAngleCheckBox,   // Index 7
        ui->gpsLatitudeCheckBox,     // Index 8
        ui->gpsLongitudeCheckBox,    // Index 9
        ui->speedCheckBox,           // Index 10
        ui->damper1CheckBox,         // Index 11
        ui->damper2CheckBox,         // Index 12
        ui->damper3CheckBox,         // Index 13
        ui->damper4CheckBox,         // Index 14
        ui->accelerationXCheckBox,   // Index 15
        ui->accelerationYCheckBox,   // Index 16
        ui->accelerationZCheckBox,   // Index 17
        ui->angleXCheckBox,          // Index 18
        ui->angleYCheckBox,          // Index 19
        ui->angleZCheckBox           // Index 20
    };

    int size = sizeof(checkBoxes) / sizeof(checkBoxes[0]);

    settingsPage->on_graphSelectorComboBox_currentIndexChanged(plotStates, size, checkBoxes);
}

void MainWindow::on_mapResetButton_clicked()
{
    mapPage->removeAllPoints();
}


