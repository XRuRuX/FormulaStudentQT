#include "geodistance.h"

// Converts degrees to radians.
double GeoDistance::toRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

// Computes the distance between two points (latitude and longitude) using the Haversine formula
double GeoDistance::distanceBetweenCoordinates(double latitudeStart, double longitudeStart, double latitudeStop, double longitudeStop)
{
    double distance;

    // Converts degrees to radians
    double lat1 = toRadians(latitudeStart);
    double lon1 = toRadians(longitudeStart);
    double lat2 = toRadians(latitudeStop);
    double lon2 = toRadians(longitudeStop);

    // Difference between longitude and latitude
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    // Haversine formula
    double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Distance in meters
    distance = EARTH_RADIUS_KM * c * 1000;

    return distance;
}
