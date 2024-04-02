#include "telemetryPage.h"

TelemetryPage::TelemetryPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

TelemetryPage::TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector,
                            QPushButton* serialConnectDisconnectButton, QWidget* centralContainer, MapPage* mapPage, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    this->mapPage = mapPage;

    this->customPlots.append(customPlot);
    this->widget = widget;
    this->comPortSelector = comPortSelector;
    this->serialConnectDisconnectButton = serialConnectDisconnectButton;
    this->centralContainer = centralContainer;

    // Initialize graph settings
    this->initializeGraph(customPlot);

    // Display avabile COM Ports
    checkComPorts();

    // Start Timer to refresh graph
    QTimer *timerGraphRefresh = new QTimer(this);
    connect(timerGraphRefresh, &QTimer::timeout, this, &TelemetryPage::refreshGraph);
    timerGraphRefresh->start(1000);

    // Start Timer to check if new COM Ports appear
    QTimer *timerCheckComPorts = new QTimer(this);
    connect(timerCheckComPorts, &QTimer::timeout, this, &TelemetryPage::checkComPorts);
    timerCheckComPorts->start(5000);

    this->graphNames = {"RPM", "Coolant Temperature", "AFR", "Oil Pressure", "Throttle Position", "BSPD",
                              "Brake Pressure", "Steering Angle", "GPS Latitude", "GPS Longitude", "GPS Speed",
                              "Damper 1", "Damper 2", "Damper 3", "Damper 4", "Ax", "Ay", "Az", "Gx", "Gy", "Gz"};

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setVisible(false);
}

TelemetryPage::~TelemetryPage()
{
    delete timerGraphRefresh;
}

void TelemetryPage::on_serialConnectDisconnectButton_clicked( void )
{
    // Reset all data
    CANData.clearData();

    if(isSerialComConnected == false )
    {
        qDebug() << "Connecting...";
        this->initializeSerialPort();
        if( serialPort.open( QIODevice::ReadWrite ) )
        {
            qDebug() << "Serial Port Opened Successfully";
            isSerialComConnected = true;
            serialConnectDisconnectButton->setText("Disconnect");

            // Disable the combo box
            comPortSelector->setEnabled(false);

            // Connect Signal and Slots
            connect(&serialPort, SIGNAL( readyRead() ), this, SLOT( readData() ) );
        }
        else
        {
            qDebug() << "Unable to open the Selected Serial Port" << serialPort.error();
        }
    }
    else
    {
        qInfo() << "Disconnecting...";

        // Close the serial port
        serialPort.close();
        isSerialComConnected = false;
        serialConnectDisconnectButton->setText("Connect");

        // Enable the combo box
        comPortSelector->setEnabled(true);
    }
}

void TelemetryPage::on_loadButton_clicked()
{
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
        }

        delete selectedCustomPlot;
        selectedCustomPlot = nullptr;
    }

    emit deletedGraphDetected();
}

void TelemetryPage::readData()
{
    QByteArray newData = serialPort.readAll();
    serialDataBuffer += newData;

    while (serialDataBuffer.contains("\r\n"))
    {
        QByteArray message = serialDataBuffer.left(serialDataBuffer.indexOf("\r\n"));
        serialDataBuffer.remove(0, message.length() + 2);   // +2 for "\r\n"

        bool conversionOk = true;
        QString data = QString::fromUtf8(message);

        if (conversionOk)
        {
            CANData.extractDataFromString(data, mapPage);
        }
        else
        {
            qWarning() << "Failed to convert data: " << message;
        }
    }
}

void TelemetryPage::refreshGraph(void)
{
    if (isSerialComConnected || loadButtonPressed) {
        for(int i = 0; i < customPlots.size(); i++)
        {
            // Selects which element will be displayed on the graph
            if (plotStates.at(i).plotStates[RPM_PLOT])
                customPlots.at(i)->graph(RPM_PLOT)->setData(CANData.RPMT, CANData.RPM);
            if (plotStates.at(i).plotStates[COOLANTTEMP_PLOT])
                customPlots.at(i)->graph(COOLANTTEMP_PLOT)->setData(CANData.CoolantT, CANData.CoolantTemp);
            if (plotStates.at(i).plotStates[AFR_PLOT])
                customPlots.at(i)->graph(AFR_PLOT)->setData(CANData.AFRT, CANData.AFR);
            if (plotStates.at(i).plotStates[OILPRESSURE_PLOT])
                customPlots.at(i)->graph(OILPRESSURE_PLOT)->setData(CANData.AnalogT, CANData.OilPressure);
            if (plotStates.at(i).plotStates[THROTTLEPOS_PLOT])
                customPlots.at(i)->graph(THROTTLEPOS_PLOT)->setData(CANData.AFRT, CANData.ThrottlePos);
            if (plotStates.at(i).plotStates[BSPD_PLOT])
                customPlots.at(i)->graph(BSPD_PLOT)->setData(CANData.AnalogT, CANData.BSPD);
            if (plotStates.at(i).plotStates[BRAKEPRESSURE_PLOT])
                customPlots.at(i)->graph(BRAKEPRESSURE_PLOT)->setData(CANData.AnalogT, CANData.BrakePressure);
            if (plotStates.at(i).plotStates[STEERINGANGLE_PLOT])
                customPlots.at(i)->graph(STEERINGANGLE_PLOT)->setData(CANData.AnalogT, CANData.SteeringAngle);
            if (plotStates.at(i).plotStates[GPSLAT_PLOT])
                customPlots.at(i)->graph(GPSLAT_PLOT)->setData(CANData.GPST, CANData.GPSLat);
            if (plotStates.at(i).plotStates[GPSLONG_PLOT])
                customPlots.at(i)->graph(GPSLONG_PLOT)->setData(CANData.GPST, CANData.GPSLong);
            if (plotStates.at(i).plotStates[GPSSPEED_PLOT])
                customPlots.at(i)->graph(GPSSPEED_PLOT)->setData(CANData.GPST, CANData.GPSSpeed);
            if (plotStates.at(i).plotStates[DAMPER1_PLOT])
                customPlots.at(i)->graph(DAMPER1_PLOT)->setData(CANData.DamperT, CANData.Damper1);
            if (plotStates.at(i).plotStates[DAMPER2_PLOT])
                customPlots.at(i)->graph(DAMPER2_PLOT)->setData(CANData.DamperT, CANData.Damper2);
            if (plotStates.at(i).plotStates[DAMPER3_PLOT])
                customPlots.at(i)->graph(DAMPER3_PLOT)->setData(CANData.DamperT, CANData.Damper3);
            if (plotStates.at(i).plotStates[DAMPER4_PLOT])
                customPlots.at(i)->graph(DAMPER4_PLOT)->setData(CANData.DamperT, CANData.Damper4);
            if (plotStates.at(i).plotStates[AX_PLOT])
                customPlots.at(i)->graph(AX_PLOT)->setData(CANData.AT, CANData.Ax);
            if (plotStates.at(i).plotStates[AY_PLOT])
                customPlots.at(i)->graph(AY_PLOT)->setData(CANData.AT, CANData.Ay);
            if (plotStates.at(i).plotStates[AZ_PLOT])
                customPlots.at(i)->graph(AZ_PLOT)->setData(CANData.AT, CANData.Az);
            if (plotStates.at(i).plotStates[GX_PLOT])
                customPlots.at(i)->graph(GX_PLOT)->setData(CANData.GT, CANData.Gx);
            if (plotStates.at(i).plotStates[GY_PLOT])
                customPlots.at(i)->graph(GY_PLOT)->setData(CANData.GT, CANData.Gy);
            if (plotStates.at(i).plotStates[GZ_PLOT])
                customPlots.at(i)->graph(GZ_PLOT)->setData(CANData.GT, CANData.Gz);

            customPlots.at(i)->replot();
            loadButtonPressed = false;
        }
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

// Check all COM Ports avabile and add them to the combo box
void TelemetryPage::checkComPorts()
{
    // Used to find the index of the previous selected option
    int currentIndex = -1;

    // Save current selected COM Port
    QString currentSelectedPort = comPortSelector->currentText();

    // Delete all existing COM Ports from combo box
    comPortSelector->clear();

    // Get information from all serial ports available
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    for (int i = 0; i < serialPortInfos.size(); ++i)
    {
        const QSerialPortInfo &portInfo = serialPortInfos.at(i);

        // Add these found com ports to the combo box
        comPortSelector->addItem(portInfo.portName());

        // Checks if the current port was the one previously selected and updates currentIndex
        if (portInfo.portName() == currentSelectedPort) {
            currentIndex = i;
        }
    }

    // Sets the selected COM port to the saved value if it exists
    if (currentIndex != -1) {
        comPortSelector->setCurrentIndex(currentIndex);
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
    }

    // Allow user interactions
    graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Set the total number of points displayed
    maxNumberOfPoints = 100;

    // Set axes ranges
    graph->xAxis->setRange(0, maxNumberOfPoints);
    graph->yAxis->setRange(0, 1000);

    // Adjust margins
    graph->axisRect()->setMinimumMargins(QMargins(5, 5, 5, 5));
    graph->axisRect()->setMargins(QMargins(5, 5, 5, 5));
    graph->axisRect()->setAutoMargins(QCP::msNone); // Disable automatic margin calculation

    // Connect the new graph to the sync slots
    connect(graph->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(syncXAxis(QCPRange)));

    // Initialize the plot states for the new graph
    PlotStates state;
    for (int j = 0; j < NO_GRAPHS; ++j) {
        state.plotStates[j] = false;
    }
    plotStates.append(state);

    graph->installEventFilter(this);
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

void TelemetryPage::initializeSerialPort()
{
    serialPort.setBaudRate( QSerialPort::Baud9600 );
    serialPort.setDataBits( QSerialPort::Data8 );
    serialPort.setParity( QSerialPort::NoParity );
    serialPort.setStopBits( QSerialPort::OneStop );
    serialPort.setFlowControl( QSerialPort::NoFlowControl );

    // Select the COM Port from Combo Box
    serialPort.setPortName( comPortSelector->currentText() );
}

// Method that changes what will be plotted on the graph based on the settings checkboxes
void TelemetryPage::changeValueDisplayed(int valueName, int graphNumber)
{
    qDebug() << graphNumber;
    if(plotStates.at(graphNumber).plotStates[valueName])
    {
        customPlots.at(graphNumber)->graph(valueName)->data()->clear();
        customPlots.at(graphNumber)->legend->removeItem(customPlots.at(graphNumber)->graph(valueName)->addToLegend());
    }

    plotStates[graphNumber].plotStates[valueName] = !plotStates[graphNumber].plotStates[valueName];
    changeLegendValues();

    // Make load button pressed true to refresh the graph
    loadButtonPressed = true;
}

void TelemetryPage::changeGraphColor(int graphName, QColor colorValue)
{
    for(int i = 0; i < customPlots.size(); i++)
    {
        customPlots.at(i)->graph(graphName)->setPen(QPen(colorValue));
    }
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

        line->start->setCoords(CANData.GPST.last(), -20000);
        line->end->setCoords(CANData.GPST.last(), 20000);

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


    // For all other events and objects, use the standard implementation
    return QWidget::eventFilter(watched, event);
}

