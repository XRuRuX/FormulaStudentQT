#include "telemetryPage.h"

TelemetryPage::TelemetryPage(QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
}

TelemetryPage::TelemetryPage(QWidget* widget, QCustomPlot* customPlot, QComboBox* comPortSelector, QPushButton* serialConnectDisconnectButton, QWidget *parent)
    : QWidget(parent), widget(nullptr)
{
    this->customPlot = customPlot;
    this->widget = widget;
    this->comPortSelector = comPortSelector;
    this->serialConnectDisconnectButton = serialConnectDisconnectButton;

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

        bool conversionOk = false;
        double data = message.toDouble(&conversionOk);

        if (conversionOk)
        {
            xAxisDataCount.append(totalLinesReadSerial);
            yAxisData.append(data);
            totalLinesReadSerial++;
            qDebug() << totalLinesReadSerial << data;
        }
        else
        {
            qWarning() << "Failed to convert data to double:" << message;
        }
    }
}

void TelemetryPage::refreshGraph(void)
{
    if (isSerialComConnected)
    {
        customPlot->graph()->setData(xAxisDataCount, yAxisData);
        // Check if the graph is being dragged; if not, update the x-axis range
        if (!isGraphDragged)
        {
            // If the total number of points is reached then start shifting the graph
            if (totalLinesReadSerial > maxNumberOfPoints)
            {
                customPlot->xAxis->setRange(totalLinesReadSerial, maxNumberOfPoints, Qt::AlignRight);
            }
        }
        customPlot->replot();
    }
}

void TelemetryPage::onGraphMousePress(QMouseEvent *event)
{
    // Set the flag to true when the mouse button is pressed on the graph
    if (customPlot->plottableAt(event->pos()))
    {
        isGraphDragged = true;
    }
}

void TelemetryPage::onGraphMouseRelease(QMouseEvent *event)
{
    // Set the flag to false when the mouse button is released
    isGraphDragged = false;
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

    // Connect mouse actions to methods
    connect(customPlot, &QCustomPlot::mousePress, this, &TelemetryPage::onGraphMousePress);
    connect(customPlot, &QCustomPlot::mouseRelease, this, &TelemetryPage::onGraphMouseRelease);

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select the graph by clicking
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Graph title
    QCPTextElement *title = new QCPTextElement(customPlot);
    title->setText("Real-Time Plot");
    title->setFont(QFont("sans", 12, QFont::Bold));

    // Insert an empty row above the axis
    customPlot->plotLayout()->insertRow(0);
    // Place the title in the empty cell we've just created
    customPlot->plotLayout()->addElement(0, 0, title);

    maxNumberOfPoints = 500;
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
