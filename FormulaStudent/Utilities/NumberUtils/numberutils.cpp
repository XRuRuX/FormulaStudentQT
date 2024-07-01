#include "numberutils.h"

// Merges an integer and fraction into a single double, ensuring precision is maintained
double NumberUtils::mergeIntegerAndFraction(double integer, double fraction)
{
    int factor = 1;
    int temp = fraction;
    double result;

    // Increase the factor by a power of 10 based on the fraction's length
    while(temp)
    {
        factor = factor * 10;
        temp = temp / 10;
    }

    result = integer * factor + fraction;
    result = result / factor;

    return result;
}

// Converts an array of hexadecimal strings to their corresponding integer values without negative values
void NumberUtils::hexStringToIntArray(QString hexGroup[], int values[], int noValues)
{
    bool ok;
    for(int i = 0; i < noValues; i++)
    {
         values[i] = hexGroup[i].toInt(&ok, 16); // Convert hex string to integer using base 16
    }
}
