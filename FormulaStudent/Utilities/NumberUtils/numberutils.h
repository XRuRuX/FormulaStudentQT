#ifndef NUMBERUTILS_H
#define NUMBERUTILS_H

#include <QString>

// Utility class for number operations
class NumberUtils
{
public:
    // Combines an integer part and a fractional part into a double precision number
    double static mergeIntegerAndFraction(double integer, double fraction);

    // Converts an array of hexadecimal strings to their corresponding integer values.
    void static hexStringToIntArray(QString hexGroup[], int values[], int noValues);
};

#endif // NUMBERUTILS_H
