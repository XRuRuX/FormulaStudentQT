#include "telemetryPage.h"

TelemetryPage::TelemetryPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

TelemetryPage::TelemetryPage(QWidget* widget, QCustomPlot* customPlot,
                            QPushButton* serialConnectDisconnectButton, QWidget* centralContainer, MapPage* mapPage, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    this->mapPage = mapPage;

    this->customPlots.append(customPlot);
    this->widget = widget;
    this->serialConnectDisconnectButton = serialConnectDisconnectButton;
    this->centralContainer = centralContainer;

    // Initialize graph settings
    this->initializeGraph(customPlot);

    // Prepare the Timer to refresh graph
    timerGraphRefresh = new QTimer(this);
    connect(timerGraphRefresh, &QTimer::timeout, this, &TelemetryPage::refreshGraph);
    timerGraphRefresh->setInterval(1000);

    this->graphNames = {"Seconds ECU has been on(s)", "Main pulsewidth bank 1(ms)", "Main pulsewidth bank 2(ms)", "RPM", "AFR Target 1", "AFR Target 2", "Manifold air pressure(kPa)",
                            "Manifold air temperature(deg C)", "Coolant Temperature(deg C)", "Throttle Position(%)", "Battery voltage(V)", "Air density correction(%)",
                            "Warmup correction(%)", "TPS-based acceleration(%)", "TPS-based fuel cut(%)", "Total fuel correction(%)", "VE value table/bank 1(%)",
                            "VE value table/bank 2(%)", "Cold advance(deg)", "Rate of change of TPS(%/s)", "Rate of change of RPM(RPM/s)", "Sync-loss counter",
                            "Sync-loss reason code", "Average fuel flow(cc/min)", "BSPD(mV)", "Brake Pressure(mV)", "Steering Angle(mV)",
                            "GPS Latitude", "GPS Longitude", "GPS Speed(Km/h)", "Damper 1(mV)", "Damper 2(mV)", "Damper 3(mV)", "Damper 4(mV)", "Roll(deg)", "Pitch(deg)", "Yaw(deg)"};

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setVisible(false);

    setupMqttClient();
}

TelemetryPage::~TelemetryPage()
{
    delete progressBar;
    for(int i = 0; i < customPlots.size(); i++)
    {
        customPlots[i]->legend->clear();
        customPlots.remove(i);
    }
    if(currentVerticalLine)
    {
        delete currentVerticalLine;
    }
    if(textLabelGraphValues)
    {
        delete textLabelGraphValues;
    }
}

void TelemetryPage::on_serialConnectDisconnectButton_clicked()
{
    if (!isConnected) {
        mqttClient->connectToHost();
        serialConnectDisconnectButton->setText("Disconnect");
        // Start timer to refresh graph
        timerGraphRefresh->start();
    } else {
        mqttClient->disconnectFromHost();
        serialConnectDisconnectButton->setText("Connect");
        // Stop timer to refresh graph
        timerGraphRefresh->stop();
    }
}

void TelemetryPage::on_loadButton_clicked()
{
    CANData.clearData();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open data file"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);

            ProgressDialog progressDialog(this);
            progressDialog.show();

            int totalLines = 0;
            QString line;
            while (!in.atEnd()) {
                line = in.readLine();
                // Just to count total lines first
                totalLines++;
            }

            // Reset to start of file
            file.seek(0);
            in.setDevice(&file);

            int currentLine = 0;
            while (!in.atEnd()) {
                line = in.readLine();
                // Process the line
                CANData.extractDataFromString(line, mapPage);

                if (currentLine % 100 == 0) {
                    // Allow UI updates and user interaction with the dialog
                    QCoreApplication::processEvents();
                    progressDialog.setProgress(static_cast<int>((static_cast<double>(currentLine) / totalLines) * 100));
                }

                if (progressDialog.wasCanceled()) {
                    // Stop loading if the user cancels
                    break;
                }

                currentLine++;
            }

            // Ensure it's filled at the end if not canceled
            progressDialog.setProgress(100);
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Can't open the file"));
        }
    }
    refreshGraph();
}

void TelemetryPage::on_addGraphButton_clicked()
{
    QCustomPlot *newPlot = new QCustomPlot();
    customPlots.append(newPlot); // Adds the new graph to the list of graphs
    initializeGraph(newPlot); // Initializes the new chart
    centralContainer->layout()->addWidget(newPlot); // Adds the chart to the user interface

    if (!customPlots.isEmpty()) {
        // Get the ranges on the x and y axes from the first graph
        QCPRange xRange = customPlots.first()->xAxis->range();
        QCPRange yRange = customPlots.first()->yAxis->range();

        // Set these ranges for the new graph
        newPlot->xAxis->setRange(xRange);
        newPlot->yAxis->setRange(yRange);
    }

    emit addNewGraphDetected();
}

void TelemetryPage::on_removeGraphButton_clicked()
{
    // Check if the the selected graph is not null
    if (selectedCustomPlot != nullptr)
    {
        selectedCustomPlot->legend->clear();

        int index = customPlots.indexOf(selectedCustomPlot);
        if (index != -1) // -1 indicates that the element was not found
        {
            // Remove the element from the found index
            customPlots.remove(index);
            plotStates.remove(index);
            colorStates.remove(index);
        }

        delete selectedCustomPlot;
        selectedCustomPlot = nullptr;
    }

    emit deletedGraphDetected();
}

void TelemetryPage::setupMqttClient()
{
    mqttClient = new QMqttClient(this);
    mqttClient->setHostname("test.mosquitto.org");
    mqttClient->setPort(1883);
    isConnected = false;

    connect(mqttClient, &QMqttClient::connected, this, [this]() {
        isConnected = true;
        QMqttTopicFilter filter("pulaprajita");
        auto subscription = mqttClient->subscribe(filter);
        if (subscription) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("MQTT Connection");
            msgBox.setText("Connected successfully to MQTT broker.");
            msgBox.setStyleSheet("QLabel{color: black;}");
            msgBox.exec();
        } else {
            QMessageBox::critical(this, "MQTT Subscription Error", "Failed to subscribe to topic.", QMessageBox::Ok);
        }
    });
    connect(mqttClient, &QMqttClient::disconnected, this, [this]() {
            isConnected = false;
    });

    connect(mqttClient, &QMqttClient::errorChanged, this, [this](QMqttClient::ClientError error) {
        if (error != QMqttClient::NoError) {
            QMessageBox::critical(this, "MQTT Connection Error", "Connection error occurred.", QMessageBox::Ok);
        }
    });

    connect(mqttClient, &QMqttClient::messageReceived, this, &TelemetryPage::onMqttMessageReceived);
}



void TelemetryPage::onMqttMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    QString data = QString::fromUtf8(message);
    qDebug() << message;
    CANData.extractDataFromString(data, mapPage);
    refreshGraph();
}

void TelemetryPage::refreshGraph(void)
{
    for(int i = 0; i < customPlots.size(); i++)
    {
        // Selects which element will be displayed on the graph
        if (plotStates.at(i).plotStates[SECONDSECUON_PLOT])
            customPlots.at(i)->graph(SECONDSECUON_PLOT)->setData(CANData.ID05f0T, CANData.SecondsECUOn);
        if (plotStates.at(i).plotStates[MAINPULSEB1_PLOT])
            customPlots.at(i)->graph(MAINPULSEB1_PLOT)->setData(CANData.ID05f0T, CANData.MainPulseB1);
        if (plotStates.at(i).plotStates[MAINPULSEB2_PLOT])
            customPlots.at(i)->graph(MAINPULSEB2_PLOT)->setData(CANData.ID05f0T, CANData.MainPulseB2);
        if (plotStates.at(i).plotStates[RPM_PLOT])
            customPlots.at(i)->graph(RPM_PLOT)->setData(CANData.ID05f0T, CANData.RPM);
        if (plotStates.at(i).plotStates[AFRTARGET1_PLOT])
            customPlots.at(i)->graph(AFRTARGET1_PLOT)->setData(CANData.ID05f1T, CANData.AFRTarget1);
        if (plotStates.at(i).plotStates[AFRTARGET2_PLOT])
            customPlots.at(i)->graph(AFRTARGET2_PLOT)->setData(CANData.ID05f1T, CANData.AFRTarget2);
        if (plotStates.at(i).plotStates[MANIFOLDAIRP_PLOT])
            customPlots.at(i)->graph(MANIFOLDAIRP_PLOT)->setData(CANData.ID05f2T, CANData.ManifoldAirP);
        if (plotStates.at(i).plotStates[MANIFOLDAIRTEMP_PLOT])
            customPlots.at(i)->graph(MANIFOLDAIRTEMP_PLOT)->setData(CANData.ID05f2T, CANData.ManifoldAirTemp);
        if (plotStates.at(i).plotStates[COOLANTTEMP_PLOT])
            customPlots.at(i)->graph(COOLANTTEMP_PLOT)->setData(CANData.ID05f2T, CANData.CoolantTemp);
        if (plotStates.at(i).plotStates[THROTTLEPOS_PLOT])
            customPlots.at(i)->graph(THROTTLEPOS_PLOT)->setData(CANData.ID05f3T, CANData.ThrottlePos);
        if (plotStates.at(i).plotStates[BATTERYV_PLOT])
            customPlots.at(i)->graph(BATTERYV_PLOT)->setData(CANData.ID05f3T, CANData.BatteryV);
        if (plotStates.at(i).plotStates[AIRDCORR_PLOT])
            customPlots.at(i)->graph(AIRDCORR_PLOT)->setData(CANData.ID05f4T, CANData.AirDCorr);
        if (plotStates.at(i).plotStates[WARMUPCORR_PLOT])
            customPlots.at(i)->graph(WARMUPCORR_PLOT)->setData(CANData.ID05f5T, CANData.WarmupCorr);
        if (plotStates.at(i).plotStates[TPSBASEDACC_PLOT])
            customPlots.at(i)->graph(TPSBASEDACC_PLOT)->setData(CANData.ID05f5T, CANData.TPSBasedAcc);
        if (plotStates.at(i).plotStates[TPSBASEDFUEL_PLOT])
            customPlots.at(i)->graph(TPSBASEDFUEL_PLOT)->setData(CANData.ID05f5T, CANData.TPSBasedFuelCut);
        if (plotStates.at(i).plotStates[TOTALFUELCORR_PLOT])
            customPlots.at(i)->graph(TOTALFUELCORR_PLOT)->setData(CANData.ID05f6T, CANData.TotalFuelCorr);
        if (plotStates.at(i).plotStates[VEVALUETB1_PLOT])
            customPlots.at(i)->graph(VEVALUETB1_PLOT)->setData(CANData.ID05f6T, CANData.VEValueTB1);
        if (plotStates.at(i).plotStates[VEVALUETB2_PLOT])
            customPlots.at(i)->graph(VEVALUETB2_PLOT)->setData(CANData.ID05f6T, CANData.VEValueTB2);
        if (plotStates.at(i).plotStates[COLDADVANCE_PLOT])
            customPlots.at(i)->graph(COLDADVANCE_PLOT)->setData(CANData.ID05f7T, CANData.ColdAdvance);
        if (plotStates.at(i).plotStates[RATEOFCHANGETPS_PLOT])
            customPlots.at(i)->graph(RATEOFCHANGETPS_PLOT)->setData(CANData.ID05f7T, CANData.RateOfChangeTPS);
        if (plotStates.at(i).plotStates[RATEOFCHANGERPM_PLOT])
            customPlots.at(i)->graph(RATEOFCHANGERPM_PLOT)->setData(CANData.ID05f7T, CANData.RateOfChangeRPM);
        if (plotStates.at(i).plotStates[SYNCLOSSCOUNTER_PLOT])
            customPlots.at(i)->graph(SYNCLOSSCOUNTER_PLOT)->setData(CANData.ID061bT, CANData.SyncLossCounter);
        if (plotStates.at(i).plotStates[SYNCLOSSREASONCODE_PLOT])
            customPlots.at(i)->graph(SYNCLOSSREASONCODE_PLOT)->setData(CANData.ID061bT, CANData.SyncLossReasonCode);
        if (plotStates.at(i).plotStates[AVERAFUELF_PLOT])
            customPlots.at(i)->graph(AVERAFUELF_PLOT)->setData(CANData.ID0624T, CANData.AverageFuelF);
        if (plotStates.at(i).plotStates[BSPD_PLOT])
            customPlots.at(i)->graph(BSPD_PLOT)->setData(CANData.ID0115T, CANData.BSPD);
        if (plotStates.at(i).plotStates[BRAKEPRESSURE_PLOT])
            customPlots.at(i)->graph(BRAKEPRESSURE_PLOT)->setData(CANData.ID0115T, CANData.BrakePressure);
        if (plotStates.at(i).plotStates[STEERINGANGLE_PLOT])
            customPlots.at(i)->graph(STEERINGANGLE_PLOT)->setData(CANData.ID0115T, CANData.SteeringAngle);
        if (plotStates.at(i).plotStates[GPSLAT_PLOT])
            customPlots.at(i)->graph(GPSLAT_PLOT)->setData(CANData.ID0116T, CANData.GPSLat);
        if (plotStates.at(i).plotStates[GPSLONG_PLOT])
            customPlots.at(i)->graph(GPSLONG_PLOT)->setData(CANData.ID0116T, CANData.GPSLong);
        if (plotStates.at(i).plotStates[GPSSPEED_PLOT])
            customPlots.at(i)->graph(GPSSPEED_PLOT)->setData(CANData.ID0116T, CANData.GPSSpeed);
        if (plotStates.at(i).plotStates[DAMPER1_PLOT])
            customPlots.at(i)->graph(DAMPER1_PLOT)->setData(CANData.ID0112T, CANData.Damper1);
        if (plotStates.at(i).plotStates[DAMPER2_PLOT])
            customPlots.at(i)->graph(DAMPER2_PLOT)->setData(CANData.ID0112T, CANData.Damper2);
        if (plotStates.at(i).plotStates[DAMPER3_PLOT])
            customPlots.at(i)->graph(DAMPER3_PLOT)->setData(CANData.ID0112T, CANData.Damper3);
        if (plotStates.at(i).plotStates[DAMPER4_PLOT])
            customPlots.at(i)->graph(DAMPER4_PLOT)->setData(CANData.ID0112T, CANData.Damper4);
        if (plotStates.at(i).plotStates[ROLL_PLOT])
            customPlots.at(i)->graph(ROLL_PLOT)->setData(CANData.ID0113T, CANData.Roll);
        if (plotStates.at(i).plotStates[PITCH_PLOT])
            customPlots.at(i)->graph(PITCH_PLOT)->setData(CANData.ID0113T, CANData.Pitch);
        if (plotStates.at(i).plotStates[YAW_PLOT])
            customPlots.at(i)->graph(YAW_PLOT)->setData(CANData.ID0113T, CANData.Yaw);

        customPlots.at(i)->replot();
    }
}


void TelemetryPage::selectGraph(QCustomPlot *graphToSelect) {
    // Deselect all graphics
    for (auto &plot : customPlots) {
        plot->setBackground(QColor(24, 24, 24)); // Reset background color
    }

    // Select the desired graph
    if (graphToSelect) {
        graphToSelect->setBackground(QColor(28, 28, 28)); // Background color for the selection
        selectedCustomPlot = graphToSelect;
    } else {
        selectedCustomPlot->setBackground(QColor(24, 24, 24));
        selectedCustomPlot = nullptr;
    }

    // Refresh UI to reflect changes
    for (auto &plot : customPlots) {
        plot->replot();
    }
}

void TelemetryPage::initializeGraph(QCustomPlot* graph)
{
    // Create graphs
    for(int i = 0; i < NO_GRAPHS; i++)
    {
        graph->addGraph();
        graph->graph(i)->setPen(QColor(245, 245, 245));
    }

    // Make left and bottom axes transfer their ranges to right and top axes
    connect(graph->xAxis, SIGNAL(rangeChanged(QCPRange)), graph->xAxis2, SLOT(setRange(QCPRange)));
    connect(graph->yAxis, SIGNAL(rangeChanged(QCPRange)), graph->yAxis2, SLOT(setRange(QCPRange)));

    initializeLegend(graph);

    // Change background color
    graph->setBackground(QColor(24, 24, 24));

    // Set the pen for axes
    QList<QCPAxis*> allAxes;
    allAxes << graph->xAxis << graph->yAxis << graph->xAxis2 << graph->yAxis2;
    foreach (QCPAxis *axis, allAxes) {
        axis->setBasePen(QPen(QColor(50, 50, 50)));
        axis->setTickPen(QPen(QColor(50, 50, 50)));
        axis->setSubTickPen(QPen(QColor(50, 50, 50)));
        axis->setTickLabelColor(QColor(230, 230, 230));
        axis->setLabelColor(QColor(230, 230, 230));
        axis->grid()->setPen(QPen(QColor(50, 50, 50)));
        axis->setTickLabels(true);
    }

    // Allow user interactions
    graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Set the total number of points displayed
    maxNumberOfPoints = 100;

    // Set axes ranges
    graph->xAxis->setRange(0, maxNumberOfPoints);
    graph->yAxis->setRange(0, 1000);

    // Calculate margins automatically
    graph->axisRect()->setAutoMargins(QCP::msAll);

    // Connect the new graph to the sync slots
    connect(graph->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(syncXAxis(QCPRange)));

    // Initialize the plot states and color states for the new graph
    PlotStates state;
    ColorStates cState;
    for (int j = 0; j < NO_GRAPHS; ++j) {
        state.plotStates[j] = false;
        cState.colorStates[j] = QColor(Qt::white);
    }
    plotStates.append(state);
    colorStates.append(cState);

    graph->installEventFilter(this);

    graph->setNoAntialiasingOnDrag(true);

}


void TelemetryPage::initializeLegend(QCustomPlot* graph)
{
    // Delete all legend elements
    for(int i = 0; i < NO_GRAPHS; i++)
    {
        graph->legend->removeItem(graph->graph(i)->addToLegend());
    }

    // Create legend
    graph->legend->setVisible(true);

    QCPTextElement *legendTitle = new QCPTextElement(graph);
    // Place text element on same layer as legend, or it ends up below legend
    legendTitle->setLayer(graph->legend->layer());
    legendTitle->setText("Legend");
    legendTitle->setFont(QFont("Inter", 14, QFont::Bold));
    legendTitle->setTextColor(QColor(235, 235, 235));
    graph->legend->setBorderPen(Qt::NoPen);

    // Then we add it to the QCPLegend (which is a subclass of QCPLayoutGrid):
    if (graph->legend->hasElement(0, 0)) // If top cell isn't empty, insert an empty row at top
    {
        graph->legend->insertRow(0);
    }
    // Place the text element into the empty cell
    graph->legend->addElement(0, 0, legendTitle);

    // Change legend background color to transparent
    graph->legend->setBrush(QBrush(QColor(0, 0, 0, 0)));
}

// Method that changes what will be plotted on the graph based on the settings checkboxes
void TelemetryPage::changeValueDisplayed(int valueName, int graphNumber)
{
    if(plotStates.at(graphNumber).plotStates[valueName])
    {
        customPlots.at(graphNumber)->graph(valueName)->data()->clear();
        customPlots.at(graphNumber)->legend->removeItem(customPlots.at(graphNumber)->graph(valueName)->addToLegend());
    }

    plotStates[graphNumber].plotStates[valueName] = !plotStates[graphNumber].plotStates[valueName];
    changeLegendValues();

    // Refresh graph
    refreshGraph();
}

void TelemetryPage::changeGraphColor(int graphName, int graphNumber, QColor colorValue)
{
    customPlots.at(graphNumber)->graph(graphName)->setPen(QPen(colorValue));
    colorStates[graphNumber].colorStates[graphName] = colorValue;
}

void TelemetryPage::changeLegendValues()
{   
    for(int j = 0; j < customPlots.size(); j++)
    {
        // Dynamic legend
        // Iterate through the state vector of the plots and update the legend based on these states
        for (int i = 0; i < NO_GRAPHS; i++) {
            if (plotStates.at(j).plotStates[i]) {
                // If plotStates[i] is true, add the appropriate element to the legend
                customPlots.at(j)->graph(i)->addToLegend();
                // Update the chart name in the legend
                customPlots.at(j)->graph(i)->setName(graphNames[i]);
            } else {
                // If plotStates[i] is false, remove the corresponding element from the legend
                customPlots.at(j)->graph(i)->removeFromLegend();
            }
        }

        // Go through each item in the legend and set the text color
        for (int i = 0; i < customPlots.at(j)->legend->itemCount(); i++) {
            // Access the legend element at the specified index
            QCPAbstractLegendItem *item = customPlots.at(j)->legend->item(i);
            if (item) {
                // Set the text color for the legend element
                item->setTextColor(QColor(235, 235, 235));
            }
        }
    }
}

void TelemetryPage::drawRedVerticalLine()
{
    for(int i = 0; i < customPlots.size(); i++)
    {
        QCPItemLine *line = new QCPItemLine(customPlots.at(i));

        line->start->setCoords(CANData.ID0116T.last(), -20000);
        line->end->setCoords(CANData.ID0116T.last(), 20000);

        line->setPen(QPen(Qt::red));
    }
}

void TelemetryPage::syncXAxis(const QCPRange &range) {
    for (QCustomPlot *plot : customPlots) {
        plot->xAxis->setRange(range);
        plot->replot();
    }
}

bool TelemetryPage::eventFilter(QObject *watched, QEvent *event)
{
    // Event for resizing graph based on user input
    // Check if the event is a wheel event and if the watched object is a QCustomPlot
    if (event->type() == QEvent::Wheel && watched->inherits("QCustomPlot")) {
        // Cast the event to QWheelEvent and the watched object to QCustomPlot
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        QCustomPlot *customPlot = static_cast<QCustomPlot *>(watched);

        // Calculate the scaling factor based on the wheel angle delta
        double factor = qPow(0.8, wheelEvent->angleDelta().y() / 240.0);

        // If Ctrl key is pressed, zoom along the X-axis
        if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
            customPlot->xAxis->scaleRange(factor, customPlot->xAxis->pixelToCoord(wheelEvent->position().x()));
        }
        // If Shift key is pressed, zoom along the Y-axis
        else if (QApplication::keyboardModifiers() == Qt::ShiftModifier) {
            customPlot->yAxis->scaleRange(factor, customPlot->yAxis->pixelToCoord(wheelEvent->position().y()));
        }
        // If neither Ctrl nor Shift is pressed, zoom along both axes
        else {
            customPlot->xAxis->scaleRange(factor, customPlot->xAxis->pixelToCoord(wheelEvent->position().x()));
            customPlot->yAxis->scaleRange(factor, customPlot->yAxis->pixelToCoord(wheelEvent->position().y()));
        }

        // Redraw the plot
        customPlot->replot();
        // Indicate that the event has been handled
        return true;
    }

    // Event to select graph using right click
    if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

            // Check if there was a right click
            if (mouseEvent->button() == Qt::RightButton) {
                // Check if the object under observation is a QCustomPlot
                if (watched->inherits("QCustomPlot")) {
                    QCustomPlot *customPlot = static_cast<QCustomPlot *>(watched);

                    // If the clicked graph is already selected, we deselect it
                    if (customPlot == selectedCustomPlot) {
                        selectGraph(nullptr);
                    } else {
                        // Otherwise, select the chart that was clicked
                        selectGraph(customPlot);
                    }
                }
                // Indicate that the event has been handled
                return true;
            }
        }

    // Event to see values in a certain point using left click + ctrl
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (watched->inherits("QCustomPlot")) {
            QCustomPlot *customPlot = static_cast<QCustomPlot *>(watched);

            if (mouseEvent->button() == Qt::LeftButton && QApplication::keyboardModifiers() == Qt::ControlModifier) {
                double x = customPlot->xAxis->pixelToCoord(mouseEvent->pos().x());

                if(showGraphInfo(customPlot, x))
                {
                    // Indicate that the event has been handled
                    return true;
                }
            }
        }
    }

    // For all other events and objects, use the standard implementation
    return QWidget::eventFilter(watched, event);
}

// Show graph values in a text label at a certain point shown by a vertical line
bool TelemetryPage::showGraphInfo(QCustomPlot* customPlot, double mousePosX)
{
    // Checks if the vertical line exists
    if (currentVerticalLine != nullptr) {
        // Checks if it exists on every graph
        for(int i = 0; i < customPlots.size(); i++)
        {
            if((customPlots[i]->hasItem(currentVerticalLine)) && (customPlots[i]->hasItem(textLabelGraphValues)))
            {
                customPlots[i]->removeItem(currentVerticalLine);
                customPlots[i]->removeItem(textLabelGraphValues);
                customPlots[i]->replot();
            }
        }
        currentVerticalLine = nullptr;
    }

    // Alghoritm to find the closest point if you click between two point on the graph
    QString infoText;
    bool dataFound = false;
    double closestX = mousePosX;  // Start with the initial x position of the click
    double smallestDelta = std::numeric_limits<double>::max();

    for (int i = 0; i < customPlot->graphCount(); ++i) {
        QCPGraph *graph = customPlot->graph(i);
        auto it = graph->data()->findBegin(mousePosX);  // Find the point right of the click
        if (it != graph->data()->begin()) {
            auto itLeft = it - 1; // The point left of the click
            // Determine which point is closer to x
            double deltaRight = std::abs(it->key - mousePosX);
            double deltaLeft = std::abs(itLeft->key - mousePosX);
            if (deltaRight < deltaLeft) {
                if (deltaRight < smallestDelta) {
                    closestX = it->key;
                    smallestDelta = deltaRight;
                    infoText += QString("%1: %2\n").arg(graph->name()).arg(it->value);
                    dataFound = true;
                }
            } else {
                if (deltaLeft < smallestDelta) {
                    closestX = itLeft->key;
                    smallestDelta = deltaLeft;
                    infoText += QString("%1: %2\n").arg(graph->name()).arg(itLeft->value);
                    dataFound = true;
                }
            }
        }
    }

    // If data is found display a vertical line and a text label
    if (dataFound) {
            // Create a new vertical line at the closest x position
            currentVerticalLine = new QCPItemLine(customPlot);
            currentVerticalLine->start->setCoords(closestX, -100000);
            currentVerticalLine->end->setCoords(closestX, 100000);
            currentVerticalLine->setPen(QColor(246, 255, 181));

            // Display a text label at the closest x position
            textLabelGraphValues = new QCPItemText(customPlot);
            textLabelGraphValues->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
            textLabelGraphValues->position->setType(QCPItemPosition::ptAxisRectRatio);
            textLabelGraphValues->position->setCoords(1.0, 1.0); // Position at the bottom right of the axis rect
            textLabelGraphValues->setText(infoText);
            textLabelGraphValues->setFont(QFont("Segoe UI", 10)); // Using Segoe UI font
            textLabelGraphValues->setColor(QColor(235, 235, 235)); // Set text color to light gray
            textLabelGraphValues->setPadding(QMargins(8, 5, 8, 5)); // Adjust padding for aesthetics
            textLabelGraphValues->setBrush(QBrush(Qt::transparent)); // Completely transparent background
            textLabelGraphValues->setPen(Qt::NoPen); // No border around the text
        }

    customPlot->replot();
    return true;
}
