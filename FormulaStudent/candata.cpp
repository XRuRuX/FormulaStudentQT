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
void CANData::extractDataFromString(const QString& data)
{
    // We use regular expression to capture variable groups of hexadecimal values
    QRegularExpression regex("Timestamp:\\s+(\\d+\\.\\d+)\\s+ID:\\s+(\\w+)\\s+.*?DL:\\s+\\d+\\s+((\\w{2}\\s*)+)");
    QRegularExpressionMatch match = regex.match(data);

    if (match.hasMatch()) {
        double timestamp = match.captured(1).toDouble();
        QString dataID = match.captured(2);
        QString hexValuesStr = match.captured(3).trimmed();
        QStringList hexValues = hexValuesStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        // Concatenate 2-bit hex values into 4-bits
        QString hexGroup[4];
        if (hexValues.length() >= 2) {
            hexGroup[0] = hexValues[0] + hexValues[1];
        }
        if (hexValues.length() >= 4) {
            hexGroup[1] = hexValues[2] + hexValues[3];
        }
        if (hexValues.length() >= 6) {
            hexGroup[2] = hexValues[4] + hexValues[5];
        }
        if (hexValues.length() >= 8) {
            hexGroup[3] = hexValues[6] + hexValues[7];
        }

        // Initialization of the timestamp at the start of data transmission
        if(initialTimestamp == 0)
        {
            initialTimestamp = timestamp;
        }

        // According to each ID, the member is populated according to documentation
        if(dataID == "0113")
        {
            int values[3];
            hexToIntNegative(hexGroup, values, 3);

            AT.append(timestamp-initialTimestamp);
            Ax.append(values[0]);
            Ay.append(values[1]);
            Az.append(values[2]);
        }
        if(dataID == "0114")
        {
            int values[3];
            hexToIntNegative(hexGroup, values, 3);

            GT.append(timestamp-initialTimestamp);
            Gx.append(values[0]);
            Gy.append(values[1]);
            Gz.append(values[2]);
        }
        if(dataID == "0116")
        {
            int values[3];
            hexGroup[0] = hexValues[0] + hexValues[1] + hexValues[2];
            hexGroup[1] = hexValues[3] + hexValues[4] + hexValues[5];
            hexGroup[2] = hexValues[6];
            hexToInt(hexGroup, values, 3);

            double finalValues[2];
            finalValues[0] = (values[0] / 1000000.0) + GPSLatInt;
            finalValues[1] = (values[1] / 1000000.0) + GPSLongInt;

            GPST.append(timestamp-initialTimestamp);
            GPSLat.append(finalValues[0]);
            GPSLong.append(finalValues[1]);
            GPSSpeed.append(values[2]);
        }
    }
}

void CANData::setGPSCoordinates(int GPSLat, int GPSLong)
{
    GPSLatInt = GPSLat;
    GPSLongInt = GPSLong;
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
