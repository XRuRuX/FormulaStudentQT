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

    settingsPage = new SettingsPage(ui->settingsPage, ui->gpsLatSelector, ui->gpsLongSelector, ui->gpsLatSelectorStart, ui->gpsLongSelectorStart,
                                    ui->maxDistanceSelector, ui->graphSelectorComboBox, this);

    // Connect signals between pages
    QObject::connect(mapPage, &MapPage::newLapDetected, telemetryPage, &TelemetryPage::drawRedVerticalLine);
    QObject::connect(telemetryPage, &TelemetryPage::addNewGraphDetected, settingsPage, &SettingsPage::newGraphAdded);
    QObject::connect(telemetryPage, &TelemetryPage::deletedGraphDetected, settingsPage, &SettingsPage::deletedGraph);

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

// Delete graph on the telemetry page
void MainWindow::on_removeGraphButton_clicked()
{
    telemetryPage->on_removeGraphButton_clicked();
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

void MainWindow::on_secondsECUOnCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(SECONDSECUON_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_mainPulseB1CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(MAINPULSEB1_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_mainPulseB2CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(MAINPULSEB2_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_rpmCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(RPM_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_afrTarget1CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AFRTARGET1_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_afrTarget2CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AFRTARGET2_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_manifoldAirPCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(MANIFOLDAIRP_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_manifoldAirTempCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(MANIFOLDAIRTEMP_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_coolantTempCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(COOLANTTEMP_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_throttlePositionCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(THROTTLEPOS_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_batteryVCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(BATTERYV_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_airDCorrCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AIRDCORR_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_warmupCorrCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(WARMUPCORR_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_tPSBasedAccCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(TPSBASEDACC_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_tPSBasedFuelCutCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(TPSBASEDFUEL_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_totalFuelCorrCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(TOTALFUELCORR_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_vEValueTB1CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(VEVALUETB1_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_vEValueTB2CheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(VEVALUETB2_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_coldAdvanceCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(COLDADVANCE_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_rateOfChangeTPSCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(RATEOFCHANGETPS_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_rateOfChangeRPMCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(RATEOFCHANGERPM_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_syncLossCounterCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(SYNCLOSSCOUNTER_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_syncLossReasonCodeCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(SYNCLOSSREASONCODE_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_averageFuelFCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(AVERAFUELF_PLOT, ui->graphSelectorComboBox->currentIndex());
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

void MainWindow::on_rollCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(ROLL_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_pitchCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(PITCH_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_yawCheckBox_clicked()
{
    telemetryPage->changeValueDisplayed(YAW_PLOT, ui->graphSelectorComboBox->currentIndex());
}

void MainWindow::on_secondsECUOnColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->secondsECUOnColorPicker, SECONDSECUON_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_mainPulseB1ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->mainPulseB1ColorPicker, MAINPULSEB1_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_mainPulseB2ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->mainPulseB2ColorPicker, MAINPULSEB2_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_rpmColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->rpmColorPicker, RPM_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_afrTarget1ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->afrTarget1ColorPicker, AFRTARGET1_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_afrTarget2ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->afrTarget2ColorPicker, AFRTARGET2_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_manifoldAirPColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->manifoldAirPColorPicker, MANIFOLDAIRP_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_manifoldAirTempColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->manifoldAirTempColorPicker, MANIFOLDAIRTEMP_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_coolantTempColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->coolantTempColorPicker, COOLANTTEMP_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_throttlePositionColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->throttlePositionColorPicker, THROTTLEPOS_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_batteryVColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->batteryVColorPicker, BATTERYV_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_airDCorrColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->airDCorrColorPicker, AIRDCORR_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_warmupCorrColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->warmupCorrColorPicker, WARMUPCORR_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_tPSBasedAccColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->tPSBasedAccColorPicker, TPSBASEDACC_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_tPSBasedFuelCutColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->tPSBasedFuelCutColorPicker, TPSBASEDFUEL_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_totalFuelCorrColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->totalFuelCorrColorPicker, TOTALFUELCORR_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_vEValueTB1ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->vEValueTB1ColorPicker, VEVALUETB1_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_vEValueTB2ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->vEValueTB2ColorPicker, VEVALUETB2_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_coldAdvanceColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->coldAdvanceColorPicker, COLDADVANCE_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_rateOfChangeTPSColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->rateOfChangeTPSColorPicker, RATEOFCHANGETPS_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_rateOfChangeRPMColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->rateOfChangeRPMColorPicker, RATEOFCHANGERPM_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_syncLossCounterColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->syncLossCounterColorPicker, SYNCLOSSCOUNTER_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_syncLossReasonCodeColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->syncLossReasonCodeColorPicker, SYNCLOSSREASONCODE_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_averageFuelFColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->averageFuelFColorPicker, AVERAFUELF_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_bspdColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->bspdColorPicker, BSPD_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_brakePressureColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->brakePressureColorPicker, BRAKEPRESSURE_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_steeringAngleColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->steeringAngleColorPicker, STEERINGANGLE_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_gpsLatitudeColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->gpsLatitudeColorPicker, GPSLAT_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_gpsLongitudeColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->gpsLongitudeColorPicker, GPSLONG_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_speedColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->speedColorPicker, GPSSPEED_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_damper1ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper1ColorPicker, DAMPER1_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_damper2ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper2ColorPicker, DAMPER2_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_damper3ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper3ColorPicker, DAMPER3_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_damper4ColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->damper4ColorPicker, DAMPER4_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_rollColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->rollColorPicker, ROLL_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_pitchColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->pitchColorPicker, PITCH_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

void MainWindow::on_yawColorPicker_clicked()
{
    settingsPage->setColorForButtonGraphSettings(ui->yawColorPicker, YAW_PLOT, ui->graphSelectorComboBox->currentIndex(), telemetryPage);
}

// Changes the state of the check boxes depending on which graph is selected
void MainWindow::on_graphSelectorComboBox_currentIndexChanged(int index)
{
    PlotStates plotStates = telemetryPage->plotStates.at(index);
    ColorStates colorStates = telemetryPage->colorStates.at(index);

    // Mapping each checkbox to its respective index in plotStates array
    QCheckBox *checkBoxes[] = {
        ui->secondsECUOnCheckBox,           // Index 0
        ui->mainPulseB1CheckBox,            // Index 1
        ui->mainPulseB2CheckBox,            // Index 2
        ui->rpmCheckBox,                    // Index 3
        ui->afrTarget1CheckBox,             // Index 4
        ui->afrTarget2CheckBox,             // Index 5
        ui->manifoldAirPCheckBox,           // Index 6
        ui->manifoldAirTempCheckBox,        // Index 7
        ui->coolantTempCheckBox,            // Index 8
        ui->throttlePositionCheckBox,       // Index 9
        ui->batteryVCheckBox,               // Index 10
        ui->airDCorrCheckBox,               // Index 11
        ui->warmupCorrCheckBox,             // Index 12
        ui->tPSBasedAccCheckBox,            // Index 13
        ui->tPSBasedFuelCutCheckBox,        // Index 14
        ui->totalFuelCorrCheckBox,          // Index 15
        ui->vEValueTB1CheckBox,             // Index 16
        ui->vEValueTB2CheckBox,             // Index 17
        ui->coldAdvanceCheckBox,            // Index 18
        ui->rateOfChangeTPSCheckBox,        // Index 19
        ui->rateOfChangeRPMCheckBox,        // Index 20
        ui->syncLossCounterCheckBox,        // Index 21
        ui->syncLossReasonCodeCheckBox,     // Index 22
        ui->averageFuelFCheckBox,           // Index 23
        ui->bspdCheckBox,                   // Index 24
        ui->brakePressureCheckBox,          // Index 25
        ui->steeringAngleCheckBox,          // Index 26
        ui->gpsLatitudeCheckBox,            // Index 27
        ui->gpsLongitudeCheckBox,           // Index 28
        ui->speedCheckBox,                  // Index 29
        ui->damper1CheckBox,                // Index 30
        ui->damper2CheckBox,                // Index 31
        ui->damper3CheckBox,                // Index 32
        ui->damper4CheckBox,                // Index 33
        ui->rollCheckBox,                   // Index 34
        ui->pitchCheckBox,                  // Index 35
        ui->yawCheckBox                     // Index 36
    };

    // Mapping each color picker to its respective index in colorStates array
    QPushButton *pushButtons[] {
        ui->secondsECUOnColorPicker,           // Index 0
        ui->mainPulseB1ColorPicker,            // Index 1
        ui->mainPulseB2ColorPicker,            // Index 2
        ui->rpmColorPicker,                    // Index 3
        ui->afrTarget1ColorPicker,             // Index 4
        ui->afrTarget2ColorPicker,             // Index 5
        ui->manifoldAirPColorPicker,           // Index 6
        ui->manifoldAirTempColorPicker,        // Index 7
        ui->coolantTempColorPicker,            // Index 8
        ui->throttlePositionColorPicker,       // Index 9
        ui->batteryVColorPicker,               // Index 10
        ui->airDCorrColorPicker,               // Index 11
        ui->warmupCorrColorPicker,             // Index 12
        ui->tPSBasedAccColorPicker,            // Index 13
        ui->tPSBasedFuelCutColorPicker,        // Index 14
        ui->totalFuelCorrColorPicker,          // Index 15
        ui->vEValueTB1ColorPicker,             // Index 16
        ui->vEValueTB2ColorPicker,             // Index 17
        ui->coldAdvanceColorPicker,            // Index 18
        ui->rateOfChangeTPSColorPicker,        // Index 19
        ui->rateOfChangeRPMColorPicker,        // Index 20
        ui->syncLossCounterColorPicker,        // Index 21
        ui->syncLossReasonCodeColorPicker,     // Index 22
        ui->averageFuelFColorPicker,           // Index 23
        ui->bspdColorPicker,                   // Index 24
        ui->brakePressureColorPicker,          // Index 25
        ui->steeringAngleColorPicker,          // Index 26
        ui->gpsLatitudeColorPicker,            // Index 27
        ui->gpsLongitudeColorPicker,           // Index 28
        ui->speedColorPicker,                  // Index 29
        ui->damper1ColorPicker,                // Index 30
        ui->damper2ColorPicker,                // Index 31
        ui->damper3ColorPicker,                // Index 32
        ui->damper4ColorPicker,                // Index 33
        ui->rollColorPicker,                   // Index 34
        ui->pitchColorPicker,                  // Index 35
        ui->yawColorPicker                     // Index 36
    };

    int size = sizeof(checkBoxes) / sizeof(checkBoxes[0]);

    settingsPage->on_graphSelectorComboBox_currentIndexChanged(plotStates, colorStates, size, checkBoxes, pushButtons);
}

void MainWindow::on_mapResetButton_clicked()
{
    mapPage->removeAllPoints();
}
