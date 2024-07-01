#include "temperature.h"

// Converts a temperature from Fahrenheit to Celsius
double Temperature::convertFahrenheitToCelsius(double fahrenheit)
{
    double temp;

    temp = (fahrenheit - 32) * 5.0 / 9.0;

    return temp;
}
