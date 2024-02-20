#include "telemetryPage.h"

TelemetryPage::TelemetryPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

TelemetryPage::TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector, QComboBox* valueGraphSelector,
                            QPushButton* serialConnectDisconnectButton, QCheckBox* autoScaleSelectorCheckBox, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    this->customPlot = customPlot;
    this->widget = widget;
    this->comPortSelector = comPortSelector;
    this->valueGraphSelector = valueGraphSelector;
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
    if (isSerialComConnected)
    {
        // Selects which element is printed on the graph
        switch(valueGraphSelector->currentIndex())
        {
        case 0:
            customPlot->graph()->setData(CANData.GT, CANData.Gx);
            break;
        case 1:
            customPlot->graph()->setData(CANData.GT, CANData.Gy);
            break;
        case 2:
            customPlot->graph()->setData(CANData.GT, CANData.Gz);
            break;
        case 3:
            customPlot->graph()->setData(CANData.AT, CANData.Ax);
            break;
        case 4:
            customPlot->graph()->setData(CANData.AT, CANData.Ay);
            break;
        case 5:
            customPlot->graph()->setData(CANData.AT, CANData.Az);
            break;
        case 6:
            customPlot->graph()->setData(CANData.GPST, CANData.GPSLat);
            break;
        case 7:
            customPlot->graph()->setData(CANData.GPST, CANData.GPSLong);
            break;
        case 8:
            customPlot->graph()->setData(CANData.GPST, CANData.GPSSpeed);
            break;
        case 9:
            customPlot->graph()->setData(CANData.AnalogT, CANData.BSPD);
            break;
        case 10:
            customPlot->graph()->setData(CANData.AnalogT, CANData.BrakePressure);
            break;
        case 11:
            customPlot->graph()->setData(CANData.AnalogT, CANData.SteeringAngle);
            break;
        case 12:
            customPlot->graph()->setData(CANData.AnalogT, CANData.OilPressure);
            break;
        case 13:
            customPlot->graph()->setData(CANData.DamperT, CANData.Damper1);
            break;
        case 14:
            customPlot->graph()->setData(CANData.DamperT, CANData.Damper2);
            break;
        case 15:
            customPlot->graph()->setData(CANData.DamperT, CANData.Damper3);
            break;
        case 16:
            customPlot->graph()->setData(CANData.DamperT, CANData.Damper4);
            break;
        case 17:
            customPlot->graph()->setData(CANData.RPMT, CANData.RPM);
            break;
        case 18:
            customPlot->graph()->setData(CANData.CoolantT, CANData.CoolantTemp);
            break;
        case 19:
            customPlot->graph()->setData(CANData.AFRT, CANData.ThrottlePos);
            break;
        case 20:
            customPlot->graph()->setData(CANData.AFRT, CANData.AFR);
            break;

        default:
            customPlot->clearPlottables();
            customPlot->replot();
            break;
        }
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
    // Create graph
    customPlot->addGraph();

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
