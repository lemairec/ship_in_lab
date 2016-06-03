#ifndef MOVE_H
#define MOVE_H

class MovingEvent{
public:
  int m_angle;
  int m_vitesse;

  void stop();
};

void moveInit();
void moveRobot(MovingEvent & event);


#endif
