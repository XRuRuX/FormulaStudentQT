#ifndef GEODISTANCE_H
#define GEODISTANCE_H

#define EARTH_RADIUS_KM 6371.0

#include <QtMath>

// Class to calculate geographical distances between coordinates.
class GeoDistance
{
public:
    // Converts degrees to radians.
    static double toRadians(double degrees);

    // Calculates the distance in meters between two geographic coordinates.
    static double distanceBetweenCoordinates(double latitudeStart, double longitudeStart, double latitudeStop, double longitudeStop);
};

#endif // GEODISTANCE_H
