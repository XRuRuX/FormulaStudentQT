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
            QString hexGroup[4];
            int values[4];

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[0] + hexValues[1];
            hexGroup[1] = hexValues[2] + hexValues[3];
            hexGroup[2] = hexValues[4] + hexValues[5];
            hexGroup[3] = hexValues[6] + hexValues[7];
            hexToInt(hexGroup, values, 4);

            // Divide by 1000 according to documentation
            values[1] = values[1] / 10;
            values[2] = values[2] / 10;

            ID05f0T.append(timestamp-initialTimestamp);
            SecondsECUOn.append(values[0]);
            MainPulseB1.append(values[1]);
            MainPulseB2.append(values[2]);
            RPM.append(values[3]);
        }
        if(dataID == "05f1")
        {
            QString hexGroup[2];
            int values[2];

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[4];
            hexGroup[1] = hexValues[5];
            hexToInt(hexGroup, values, 2);

            // Divide by 10 according to documentation
            values[0] = values[0] / 10;
            values[1] = values[1] / 10;

            ID05f1T.append(timestamp-initialTimestamp);
            AFRTarget1.append(values[0]);
            AFRTarget2.append(values[1]);
        }
        if(dataID == "05f2")
        {
            QString hexGroup[3];
            int values[3];

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[2] + hexValues[3];
            hexGroup[1] = hexValues[4] + hexValues[5];
            hexGroup[2] = hexValues[6] + hexValues[7];
            hexToInt(hexGroup, values, 3);

            // Divide by 10 according to documentation
            values[0] = values[0] / 10;
            double tempValueM = values[1] / 10.0;
            double tempValueC = values[2] / 10.0;

            // Convert to Celsius
            double finalValueC = fahrenheitToCelsius(tempValueC);
            double finalValueM = fahrenheitToCelsius(tempValueM);

            ID05f2T.append(timestamp-initialTimestamp);
            CoolantTemp.append(finalValueC);
            ManifoldAirP.append(values[0]);
            ManifoldAirTemp.append(tempValueM);
        }

        if(dataID == "05f3")
        {
            QString hexGroup[2];
            int values[2];

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[0] + hexValues[1];
            hexGroup[1] = hexValues[2] + hexValues[3];
            hexToInt(hexGroup, values, 2);

            // Divide by 10 according to documentation
            values[0] = values[0] / 10;
            values[1] = values[1] / 10;

            ID05f3T.append(timestamp-initialTimestamp);
            ThrottlePos.append(values[0]);
            BatteryV.append(values[1]);
        }

        if(dataID == "05f4")
        {
            QString hexGroup[1];
            int value;

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[6] + hexValues[7];
            hexToInt(hexGroup, &value, 1);

            // Divide by 10 according to documentation
            value = value / 10;

            ID05f4T.append(timestamp-initialTimestamp);
            AirDCorr.append(value);
        }

        if((dataID == "05f5") || (dataID == "05f6") || (dataID == "05f7"))
        {
            QString hexGroup[3];
            int values[3];

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[0] + hexValues[1];
            hexGroup[1] = hexValues[2] + hexValues[3];
            hexGroup[2] = hexValues[4] + hexValues[5];
            hexToInt(hexGroup, values, 3);

            // Divide by 10 according to documentation
            values[0] = values[0] / 10;
            values[1] = values[1] / 10;
            values[2] = values[2] / 10;

            if(dataID == "05f5")
            {
                ID05f5T.append(timestamp-initialTimestamp);
                WarmupCorr.append(values[0]);
                TPSBasedAcc.append(values[1]);
                TPSBasedFuelCut.append(values[2]);
            }
            else if(dataID == "05f6")
            {
                ID05f6T.append(timestamp-initialTimestamp);
                TotalFuelCorr.append(values[0]);
                VEValueTB1.append(values[1]);
                VEValueTB2.append(values[2]);
            }
            else
            {
                ID05f7T.append(timestamp-initialTimestamp);
                ColdAdvance.append(values[0]);
                RateOfChangeTPS.append(values[1]);
                RateOfChangeRPM.append(values[2]*100);
            }
        }

        if(dataID == "061b")
        {
            QString hexGroup[2];
            int values[2];

            hexGroup[0] = hexValues[0];
            hexGroup[1] = hexValues[1];
            hexToInt(hexGroup, values, 2);

            ID061bT.append(timestamp-initialTimestamp);
            SyncLossCounter.append(values[0]);
            SyncLossReasonCode.append(values[1]);
        }

        if(dataID == "0624")
        {
            QString hexGroup[1];
            int value;

            // Concatenate 1-bit hex values into 2-bits and converts it to int
            hexGroup[0] = hexValues[4] + hexValues[5];
            hexToInt(hexGroup, &value, 1);

            ID0624T.append(timestamp-initialTimestamp);
            AverageFuelF.append(value);
        }

        if(dataID == "0113")
        {
            QString hexGroup[4];
            int values[4];

            // Concatenate 1-bit hex values into 2-bits
            hexGroup[0] = hexValues[0] + hexValues[1];
            hexGroup[1] = hexValues[2] + hexValues[3];
            hexGroup[2] = hexValues[4] + hexValues[5];
            hexGroup[3] = hexValues[6];

            // Convert from hex to int
            hexToInt(hexGroup, values, 4);

            // Interpret the sign for each orientation value according to the sign value
            if (values[3] == 4) {
                values[2] *= -1;
            }
            if (values[3] == 2) {
                values[1] *= -1;
            }
            if (values[3] == 1) {
                values[0] *= -1;
            }

            ID0113T.append(timestamp-initialTimestamp);
            Roll.append(values[0]);
            Yaw.append(values[1]);
            Pitch.append(values[2]);
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
                ID0115T.append(timestamp-initialTimestamp);
                BSPD.append(values[0]);
                BrakePressure.append(values[1]);
                SteeringAngle.append(values[2]);
            }
            else
            {
                ID0112T.append(timestamp-initialTimestamp);
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

            ID0116T.append(timestamp-initialTimestamp);
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
    initialTimestamp = 0;

    // 05f0
    ID05f0T.clear();
    SecondsECUOn.clear();
    MainPulseB1.clear();
    MainPulseB2.clear();
    RPM.clear();

    // 05f1
    ID05f1T.clear();
    AFRTarget1.clear();
    AFRTarget2.clear();

    // 05f2
    ID05f2T.clear();
    ManifoldAirP.clear();
    ManifoldAirTemp.clear();
    CoolantTemp.clear();

    // 05f3
    ID05f3T.clear();
    ThrottlePos.clear();
    BatteryV.clear();

    // 05f4
    ID05f4T.clear();
    AirDCorr.clear();

    // 05f5
    ID05f5T.clear();
    WarmupCorr.clear();
    TPSBasedAcc.clear();
    TPSBasedFuelCut.clear();

    // 05f6
    ID05f6T.clear();
    TotalFuelCorr.clear();
    VEValueTB1.clear();
    VEValueTB2.clear();

    // 05f7
    ID05f7T.clear();
    ColdAdvance.clear();
    RateOfChangeTPS.clear();
    RateOfChangeRPM.clear();

    // 061b
    ID061bT.clear();
    SyncLossCounter.clear();
    SyncLossReasonCode.clear();

    // 0624
    ID0624T.clear();
    AverageFuelF.clear();

    // 0115
    ID0115T.clear();
    BSPD.clear();
    BrakePressure.clear();
    SteeringAngle.clear();

    // 0116
    ID0116T.clear();
    GPSLat.clear();
    GPSLong.clear();
    GPSSpeed.clear();

    // 0112
    ID0112T.clear();
    Damper1.clear();
    Damper2.clear();
    Damper3.clear();
    Damper4.clear();

    // 0113
    ID0113T.clear();
    Roll.clear();
    Pitch.clear();
    Yaw.clear();
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

