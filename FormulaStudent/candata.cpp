#include "CANData.h"

// Static variabiles that can be changed on the Settings tab
int CANData::GPSLatInt = 0;
int CANData::GPSLongInt = 0;


// Constructor that resests the initial timestamp
CANData::CANData()
{
    initialTimestamp = 0;
}

// Extracts data from the string and populates the class members according to the id of the data
void CANData::extractDataFromString(const QString& data, MapPage* map)
{
    // We use regular expression to capture variable groups of hexadecimal values
    QRegularExpression regex("Timestamp:\\s+(\\d+\\.\\d+)\\s+ID:\\s+(\\w+)\\s+.*?DL:\\s+\\d+\\s+((\\w{2}\\s*)+)");
    QRegularExpressionMatch match = regex.match(data);

    if (match.hasMatch()) {
        double timestamp = match.captured(1).toDouble();
        QString dataID = match.captured(2);
        QString hexValuesStr = match.captured(3).trimmed();
        QStringList hexValues = hexValuesStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts); // Values ​​are extracted in pairs of one bit each

        // Initialization of the timestamp at the start of data transmission
        if(initialTimestamp == 0)
        {
            initialTimestamp = timestamp;
        }

        // According to each ID, the member is populated according to documentation
        if(dataID == "05f0")
        {
            QString hexGroup;
            int value;

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup = hexValues[6] + hexValues[7];
            hexToInt(&hexGroup, &value, 1);

            RPMT.append(timestamp-initialTimestamp);
            RPM.append(value);
        }
        if(dataID == "05f2")
        {
            QString hexGroup;
            int value;

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup = hexValues[6] + hexValues[7];
            hexToInt(&hexGroup, &value, 1);

            // Divide by 10 according to documentation
            double tempValue = value / 10.0;

            // Convert to Celsius
            double finalValue = fahrenheitToCelsius(tempValue);

            CoolantT.append(timestamp-initialTimestamp);
            CoolantTemp.append(finalValue);
        }

        if(dataID == "05f3")
        {
            QString hexGroup[2];
            int values[2];

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[0] + hexValues[1];
            hexGroup[1] = hexValues[4] + hexValues[5];
            hexToInt(hexGroup, values, 2);

            // Divide by 10 according to documentation
            values[0] = values[0] / 10;
            values[1] = values[1] / 10;

            AFRT.append(timestamp-initialTimestamp);
            ThrottlePos.append(values[0]);
            AFR.append(values[1]);
        }

        if((dataID == "0113") || (dataID == "0114"))
        {
            QString hexGroup[3];
            int values[3];

            // Concatenate 1-bit hex values into 2-bits
            hexGroup[0] = hexValues[0] + hexValues[1];
            hexGroup[1] = hexValues[2] + hexValues[3];
            hexGroup[2] = hexValues[4] + hexValues[5];

            // Convert from hex to int
            hexToIntNegative(hexGroup, values, 3);

            if(dataID == "0113")
            {
                AT.append(timestamp-initialTimestamp);
                Ax.append(values[0]);
                Ay.append(values[1]);
                Az.append(values[2]);
            }
            else
            {
                GT.append(timestamp-initialTimestamp);
                Gx.append(values[0]);
                Gy.append(values[1]);
                Gz.append(values[2]);
            }
        }
        if((dataID == "0115") || (dataID == "0112"))
        {
            int values[4];
            int tempValues[8];
            QString hexTempGroup[8];

            // Extracts every bit and converts it to int
            for(int i = 0; i < 8; i++)
            {
                hexTempGroup[i] = hexValues[i];
            }
            hexToInt(hexTempGroup, tempValues, 8);

            // Apply formula according to documentation FB * 100 + LB
            for(int i = 0; i < 4; i++)
            {
                values[i] = tempValues[2*i] * 100 + tempValues[2*i+1];
            }

            if(dataID == "0115")
            {
                AnalogT.append(timestamp-initialTimestamp);
                BSPD.append(values[0]);
                BrakePressure.append(values[1]);
                SteeringAngle.append(values[2]);
                OilPressure.append(values[3]);
            }
            else
            {
                DamperT.append(timestamp-initialTimestamp);
                Damper1.append(values[0]);
                Damper2.append(values[1]);
                Damper3.append(values[2]);
                Damper4.append(values[3]);
            }
        }
        if(dataID == "0116")
        {
            QString hexGroup[3];
            int values[3];

            // Concatenate 1-bit hex values to 3-bit hex values and convert it to int
            hexGroup[0] = hexValues[0] + hexValues[1] + hexValues[2];
            hexGroup[1] = hexValues[3] + hexValues[4] + hexValues[5];
            hexGroup[2] = hexValues[6];
            hexToInt(hexGroup, values, 3);

            // Apply formula according to documentation
            double finalValues[2];
            finalValues[0] = (values[0] / 1000000.0) + GPSLatInt;
            finalValues[1] = (values[1] / 1000000.0) + GPSLongInt;

            GPST.append(timestamp-initialTimestamp);
            GPSLat.append(finalValues[0]);
            GPSLong.append(finalValues[1]);
            GPSSpeed.append(values[2]);

            // Add point to map
            map->addPointToMap(finalValues[0], finalValues[1]);
        }
    }
}

void CANData::setGPSCoordinates(int GPSLat, int GPSLong)
{
    GPSLatInt = GPSLat;
    GPSLongInt = GPSLong;
}

void CANData::clearData()
{
    CANData.initialTimestamp = 0;
    RPMT.clear();
    RPM.clear();
    CoolantT.clear();
    CoolantTemp.clear();
    AFRT.clear();
    ThrottlePos.clear();
    AFR.clear();
    AnalogT.clear();
    BSPD.clear();
    BrakePressure.clear();
    SteeringAngle.clear();
    OilPressure.clear();
    GPST.clear();
    GPSLat.clear();
    GPSLong.clear();
    GPSSpeed.clear();
    DamperT.clear();
    Damper1.clear();
    Damper2.clear();
    Damper3.clear();
    Damper4.clear();
    GT.clear();
    Gx.clear();
    Gy.clear();
    Gz.clear();
    AT.clear();
    Ax.clear();
    Ay.clear();
    Az.clear();
}

// Converts an array of hexadecimal strings to their corresponding integer values, taking into account the posibility of negative values
void hexToIntNegative(QString hexGroup[], int values[], int noValues)
{
    bool ok;
    for(int i = 0; i < noValues; i++)
    {
        values[i] = hexGroup[i].toInt(&ok, 16);
        if(ok)
        {
            if(values[i] > 32767)
            {
                values[i] -= 65536;
            }
        }
    }
}

// Converts an array of hexadecimal strings to their corresponding integer values without negative values
void hexToInt(QString hexGroup[], int values[], int noValues)
{
    bool ok;
    for(int i = 0; i < noValues; i++)
    {
         values[i] = hexGroup[i].toInt(&ok, 16);
    }
}

// Converts Fahrenheit to Celsius
double fahrenheitToCelsius(double fahrenheit)
{
    double temp;

    temp = (fahrenheit - 32) * 5.0 / 9.0;

    return temp;
}

