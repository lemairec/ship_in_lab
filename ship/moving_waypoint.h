#ifndef MOVING_WAYPOINT_H
#define MOVING_WAYPOINT_H

#include "gps.h"

class  Waypoint {
public:
  Waypoint(double longitude, double latitude);
      
  double m_longitude;
  double m_latitude;
};

class MovingEvent {
public:
  int m_waypoint_no;
  double m_distance;
  double m_angle;
};

class MovingWaypoint {
public:
  void getMovingEvent(GpsEvent & event, MovingEvent & movingEvent);
};


#endif
