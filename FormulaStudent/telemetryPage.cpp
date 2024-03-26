#include "telemetryPage.h"

TelemetryPage::TelemetryPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

TelemetryPage::TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector,
                            QPushButton* serialConnectDisconnectButton, MapPage* mapPage, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    this->mapPage = mapPage;

    this->customPlot = customPlot;
    this->widget = widget;
    this->comPortSelector = comPortSelector;
    this->serialConnectDisconnectButton = serialConnectDisconnectButton;

    // Initialize graph settings
    this->initializeGraph();

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
        // Selects which element will be displayed on the graph
        if (plotStates[RPM_PLOT])
            customPlot->graph(RPM_PLOT)->setData(CANData.RPMT, CANData.RPM);
        if (plotStates[COOLANTTEMP_PLOT])
            customPlot->graph(COOLANTTEMP_PLOT)->setData(CANData.CoolantT, CANData.CoolantTemp);
        if (plotStates[AFR_PLOT])
            customPlot->graph(AFR_PLOT)->setData(CANData.AFRT, CANData.AFR);
        if (plotStates[OILPRESSURE_PLOT])
            customPlot->graph(OILPRESSURE_PLOT)->setData(CANData.AnalogT, CANData.OilPressure);
        if (plotStates[THROTTLEPOS_PLOT])
            customPlot->graph(THROTTLEPOS_PLOT)->setData(CANData.AFRT, CANData.ThrottlePos);
        if (plotStates[BSPD_PLOT])
            customPlot->graph(BSPD_PLOT)->setData(CANData.AnalogT, CANData.BSPD);
        if (plotStates[BRAKEPRESSURE_PLOT])
            customPlot->graph(BRAKEPRESSURE_PLOT)->setData(CANData.AnalogT, CANData.BrakePressure);
        if (plotStates[STEERINGANGLE_PLOT])
            customPlot->graph(STEERINGANGLE_PLOT)->setData(CANData.AnalogT, CANData.SteeringAngle);
        if (plotStates[GPSLAT_PLOT])
            customPlot->graph(GPSLAT_PLOT)->setData(CANData.GPST, CANData.GPSLat);
        if (plotStates[GPSLONG_PLOT])
            customPlot->graph(GPSLONG_PLOT)->setData(CANData.GPST, CANData.GPSLong);
        if (plotStates[GPSSPEED_PLOT])
            customPlot->graph(GPSSPEED_PLOT)->setData(CANData.GPST, CANData.GPSSpeed);
        if (plotStates[DAMPER1_PLOT])
            customPlot->graph(DAMPER1_PLOT)->setData(CANData.DamperT, CANData.Damper1);
        if (plotStates[DAMPER2_PLOT])
            customPlot->graph(DAMPER2_PLOT)->setData(CANData.DamperT, CANData.Damper2);
        if (plotStates[DAMPER3_PLOT])
            customPlot->graph(DAMPER3_PLOT)->setData(CANData.DamperT, CANData.Damper3);
        if (plotStates[DAMPER4_PLOT])
            customPlot->graph(DAMPER4_PLOT)->setData(CANData.DamperT, CANData.Damper4);
        if (plotStates[AX_PLOT])
            customPlot->graph(AX_PLOT)->setData(CANData.AT, CANData.Ax);
        if (plotStates[AY_PLOT])
            customPlot->graph(AY_PLOT)->setData(CANData.AT, CANData.Ay);
        if (plotStates[AZ_PLOT])
            customPlot->graph(AZ_PLOT)->setData(CANData.AT, CANData.Az);
        if (plotStates[GX_PLOT])
            customPlot->graph(GX_PLOT)->setData(CANData.GT, CANData.Gx);
        if (plotStates[GY_PLOT])
            customPlot->graph(GY_PLOT)->setData(CANData.GT, CANData.Gy);
        if (plotStates[GZ_PLOT])
            customPlot->graph(GZ_PLOT)->setData(CANData.GT, CANData.Gz);

        customPlot->replot();
        loadButtonPressed = false;
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

void TelemetryPage::initializeGraph()
{
    // Create graphs
    for(int i = 0; i < NO_GRAPHS; i++)
    {
        customPlot->addGraph();
        customPlot->graph(i)->setPen(QColor(245, 245, 245));
    }

    // Make left and bottom axes transfer their ranges to right and top axes
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

/*
    // Graph title
    QCPTextElement *title = new QCPTextElement(customPlot);
    title->setText("Sensors Visualization Graph");
    title->setFont(QFont("Inter", 16, QFont::Bold));
    title->setTextColor(QColor(240, 240, 240));

    // Insert an empty row above the axis
    customPlot->plotLayout()->insertRow(0);
    // Place the title in the empty cell we've just created
    customPlot->plotLayout()->addElement(0, 0, title);
*/
    initializeLegend();

    // Change background color
    customPlot->setBackground(QColor(24, 24, 24));
    // Change color of X-Axis
    customPlot->xAxis->setBasePen(QPen(QColor(50, 50, 50)));
    customPlot->xAxis->setTickPen(QPen(QColor(50, 50, 50)));
    customPlot->xAxis->setSubTickPen(QPen(QColor(50, 50, 50)));
    // Change color of Y-Axis
    customPlot->yAxis->setBasePen(QPen(QColor(50, 50, 50)));
    customPlot->yAxis->setTickPen(QPen(QColor(50, 50, 50)));
    customPlot->yAxis->setSubTickPen(QPen(QColor(50, 50, 50)));
    // Change color of grid
    customPlot->xAxis->grid()->setPen(QPen(QColor(50, 50, 50)));
    customPlot->yAxis->grid()->setPen(QPen(QColor(50, 50, 50)));
    // Change color of X-Axis label
    customPlot->xAxis->setLabelColor(QColor(230, 230, 230));
    // Change color of Y-Axis label
    customPlot->yAxis->setLabelColor(QColor(230, 230, 230));
    // Change color of X-Axis numbers
    customPlot->xAxis->setTickLabelColor(QColor(230, 230, 230));
    // Change color of Y-Axis numbers
    customPlot->yAxis->setTickLabelColor(QColor(230, 230, 230));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select the graph by clicking
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // The total number of points displayed at any time
    maxNumberOfPoints = 100;

    // Set axes ranges to see data
    customPlot->xAxis->setRange(0, maxNumberOfPoints);
    customPlot->yAxis->setRange(0, 1000);
}

void TelemetryPage::initializeLegend()
{
    // Delete all legend elements
    for(int i = 0; i < NO_GRAPHS; i++)
    {
        customPlot->legend->removeItem(customPlot->graph(i)->addToLegend());
    }

    // Create legend
    customPlot->legend->setVisible(true);

    QCPTextElement *legendTitle = new QCPTextElement(customPlot);
    // Place text element on same layer as legend, or it ends up below legend
    legendTitle->setLayer(customPlot->legend->layer());
    legendTitle->setText("Legend");
    legendTitle->setFont(QFont("Inter", 14, QFont::Bold));
    legendTitle->setTextColor(QColor(235, 235, 235));
    customPlot->legend->setBorderPen(Qt::NoPen);

    // Then we add it to the QCPLegend (which is a subclass of QCPLayoutGrid):
    if (customPlot->legend->hasElement(0, 0)) // If top cell isn't empty, insert an empty row at top
    {
        customPlot->legend->insertRow(0);
    }
    // Place the text element into the empty cell
    customPlot->legend->addElement(0, 0, legendTitle);

    // Change legend background color
    customPlot->legend->setBrush(QColor(24, 24, 24));
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
void TelemetryPage::changeValueDisplayed(int valueName)
{
    if(plotStates[valueName])
    {
        customPlot->graph(valueName)->data()->clear();
        customPlot->legend->removeItem(customPlot->graph(valueName)->addToLegend());
    }
    plotStates[valueName] = !plotStates[valueName];
    changeLegendValues();

    // Make load button pressed true to refresh the graph
    loadButtonPressed = true;
}

void TelemetryPage::changeGraphColor(int graphName, QColor colorValue)
{
    customPlot->graph(graphName)->setPen(QPen(colorValue));
}

void TelemetryPage::changeLegendValues()
{
    // Dynamic legend
    // Iterate through the state vector of the plots and update the legend based on these states
    for (int i = 0; i < NO_GRAPHS; i++) {
        if (plotStates[i]) {
            // If plotStates[i] is true, add the appropriate element to the legend
            customPlot->graph(i)->addToLegend();
            // Update the chart name in the legend
            customPlot->graph(i)->setName(graphNames[i]);
        } else {
            // If plotStates[i] is false, remove the corresponding element from the legend
            customPlot->graph(i)->removeFromLegend();
        }
    }

    // Go through each item in the legend and set the text color
    for (int i = 0; i < customPlot->legend->itemCount(); i++) {
        // Access the legend element at the specified index
        QCPAbstractLegendItem *item = customPlot->legend->item(i);
        if (item) {
            // Set the text color for the legend element
            item->setTextColor(QColor(235, 235, 235));
        }
    }
}

void TelemetryPage::drawRedVerticalLine()
{
    QCPItemLine *line = new QCPItemLine(customPlot);

    line->start->setCoords(CANData.GPST.last(), -20000);
    line->end->setCoords(CANData.GPST.last(), 20000);

    line->setPen(QPen(Qt::red));
}
