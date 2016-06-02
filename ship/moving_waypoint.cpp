#include "moving_waypoint.h"
#include "logging.h"
#include <math.h>


#define WAYPOINT_DIST_TOLERANE  2   // tolerance in meters to waypoint; once within this tolerance, will advance to the next waypoint
#define NUMBER_WAYPOINTS 5          // enter the numebr of way points here (will run from 0 to (n-1))
int waypointNumber = 0;            // current waypoint number; will run from 0 to (NUMBER_WAYPOINTS -1); start at -1 and gets initialized during setup()
Waypoint waypointList[NUMBER_WAYPOINTS] = {
    Waypoint(3.398085, 49.03748) 
    , Waypoint(3.398278, 49.037558)
    , Waypoint(3.4082915783, 49.0403060913)
    , Waypoint(3.4079160690, 49.0402221680)
    , Waypoint(3.4080543518, 49.0402641296)
};


Waypoint::Waypoint(double longitude, double latitude)
    :m_longitude(longitude)
    ,m_latitude(latitude)
{
}


void MovingWaypoint::getMovingEvent(GpsEvent & event, MovingWaypointEvent & movingEvent){
    if(waypointNumber > NUMBER_WAYPOINTS){
        movingEvent.m_distance = 0;
        movingEvent.m_angle = 0;
        return;
    }
    Waypoint & targetWaypoint = waypointList[waypointNumber];

    double currentLong = event.m_longitude;
    double currentLat = event.m_latitude;
    double targetLong = targetWaypoint.m_longitude;
    double targetLat = targetWaypoint.m_latitude;

    double delta = radians(currentLong - targetLong);
    double sdlong = sin(delta);
    double cdlong = cos(delta);
    double lat1 = radians(currentLat);
    double lat2 = radians(targetLat);
    double slat1 = sin(lat1);
    double clat1 = cos(lat1);
    double slat2 = sin(lat2);
    double clat2 = cos(lat2);
    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong); 
    delta = sq(delta); 
    delta += sq(clat2 * sdlong); 
    delta = sqrt(delta); 
    double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong); 
    delta = atan2(delta, denom); 
    double distanceToTarget =  delta * 6372795; 

    if (distanceToTarget <= WAYPOINT_DIST_TOLERANE){
        waypointNumber++;
        getMovingEvent(event, movingEvent);
        //INFO("next waypoint " << waypointNumber);
    }

    double dlon = radians(targetLong-currentLong);
    double cLat = radians(currentLat);
    double tLat = radians(targetLat);
    double a1 = sin(dlon) * cos(tLat);
    double a2 = sin(cLat) * cos(tLat) * cos(dlon);
    a2 = cos(cLat) * sin(tLat) - a2;
    a2 = atan2(a1, a2);
    if (a2 < 0.0)
    {
      a2 += TWO_PI;
    }
    double targetHeading = degrees(a2);


    movingEvent.m_distance = distanceToTarget;
    movingEvent.m_angle = targetHeading;
    movingEvent.m_waypoint_no = waypointNumber;
}

// returns distance in meters between two positions, both specified 
// as signed decimal-degrees latitude and longitude. Uses great-circle 
// distance computation for hypothetical sphere of radius 6372795 meters.
// Because Earth is no exact sphere, rounding errors may be up to 0.5%.
// copied from TinyGPS library
/*int distanceToWaypoint() 
{

  float delta = radians(currentLong - targetLong);
  float sdlong = sin(delta);
  float cdlong = cos(delta);
  float lat1 = radians(currentLat);
  float lat2 = radians(targetLat);
  float slat1 = sin(lat1);
  float clat1 = cos(lat1);
  float slat2 = sin(lat2);
  float clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong); 
  delta = sq(delta); 
  delta += sq(clat2 * sdlong); 
  delta = sqrt(delta); 
  float denom = (slat1 * slat2) + (clat1 * clat2 * cdlong); 
  delta = atan2(delta, denom); 
  distanceToTarget =  delta * 6372795; 

  // check to see if we have reached the current waypoint
  if (distanceToTarget <= WAYPOINT_DIST_TOLERANE){
    nextWaypoint();}

  return distanceToTarget;
}  // distanceToWaypoint()




// returns course in degrees (North=0, West=270) from position 1 to position 2,
// both specified as signed decimal-degrees latitude and longitude.
// Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
// copied from TinyGPS library
int courseToWaypoint() 
{
  float dlon = radians(targetLong-currentLong);
  float cLat = radians(currentLat);
  float tLat = radians(targetLat);
  float a1 = sin(dlon) * cos(tLat);
  float a2 = sin(cLat) * cos(tLat) * cos(dlon);
  a2 = cos(cLat) * sin(tLat) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0)
  {
    a2 += TWO_PI;
  }
  targetHeading = degrees(a2);
  return targetHeading;
}   // courseToWaypoint()*/



