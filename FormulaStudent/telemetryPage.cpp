#include "telemetryPage.h"

TelemetryPage::TelemetryPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

TelemetryPage::TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector,
                            QPushButton* serialConnectDisconnectButton, QCheckBox* autoScaleSelectorCheckBox, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    this->customPlot = customPlot;
    this->widget = widget;
    this->comPortSelector = comPortSelector;
    this->serialConnectDisconnectButton = serialConnectDisconnectButton;
    this->autoScaleSelectorCheckBox = autoScaleSelectorCheckBox;

    // Initialize graph settings
    this->initializeGraph();

    // Get information from all serial ports available
    QList<QSerialPortInfo> serial_port_infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port_info : serial_port_infos )
    {
        // Add these found com ports to the combo box
        comPortSelector->addItem(port_info.portName());
    }

    // Start Timer to refresh graph
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TelemetryPage::refreshGraph );
    timer->start(1000);
}

TelemetryPage::~TelemetryPage()
{
    delete timer;
}

void TelemetryPage::on_serialConnectDisconnectButton_clicked( void )
{
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

            // Set axes ranges to see data
            customPlot->xAxis->setRange(0, maxNumberOfPoints);
            customPlot->yAxis->setRange(0, 1000);
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

        // Reset initial Timestamp
        CANData.initialTimestamp = 0;
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
            CANData.extractDataFromString(data);
        }
        else
        {
            qWarning() << "Failed to convert data: " << message;
        }
    }
}

void TelemetryPage::refreshGraph(void)
{
    if (isSerialComConnected) {
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

        // Check if the graph is on AutoScale
        if (isAutoScale)
        {
            // If the total number of points is reached then start shifting the graph
            if (totalLinesReadSerial > maxNumberOfPoints)
            {
                customPlot->xAxis->setRange(totalLinesReadSerial, maxNumberOfPoints, Qt::AlignRight);

                // Manual scaling of the Y axis
                customPlot->yAxis->setRange(0, 1000);
            }
        }
        customPlot->replot();
    }
}

void TelemetryPage::on_autoScaleSelectorCheckBox_stateChanged()
{
    if(isAutoScale == false)
    {
        isAutoScale = true;
        // Disallow user to drag axis ranges with mouse, zoom with mouse wheel and select the graph by clicking
        customPlot->setInteractions(QFlags<QCP::Interaction>(0));
    }
    else
    {
        isAutoScale = false;
        // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select the graph by clicking
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    }
}

void TelemetryPage::initializeGraph()
{
    // Create graphs
    for(int i = 0; i < NO_GRAPHS; i++)
    {
        customPlot->addGraph();
    }

    // Axis label
    customPlot->yAxis->setLabel("Sensor value: ");

    // Make left and bottom axes transfer their ranges to right and top axes
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // Graph title
    QCPTextElement *title = new QCPTextElement(customPlot);
    title->setText("Real-Time Plot");
    title->setFont(QFont("sans", 12, QFont::Bold));

    // Insert an empty row above the axis
    customPlot->plotLayout()->insertRow(0);
    // Place the title in the empty cell we've just created
    customPlot->plotLayout()->addElement(0, 0, title);

    // The total number of points displayed at any time
    maxNumberOfPoints = 100;
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
    }
    plotStates[valueName] = !plotStates[valueName];
}

void TelemetryPage::changeGraphColor(int graphName, QColor colorValue)
{
    customPlot->graph(graphName)->setPen(QPen(colorValue));
}
