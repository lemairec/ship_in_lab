#include "move.h"
#include "pins.h"
#include "logging.h"
#include <Servo.h>
#include <Arduino.h>

Servo servo;

void MovingEvent::stop(){
  m_angle = 0;
  m_vitesse = 0;

  pinMode(MOTOR_LEFT_0_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_1_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_PWM_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_0_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_1_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM_PIN, OUTPUT);
}

void moveInit(){
  servo.attach(7);
}

void motor_direction(int left, int right){
  int l;
  int r;
  if(left>0){
    digitalWrite(MOTOR_LEFT_0_PIN, HIGH); 
    digitalWrite(MOTOR_LEFT_1_PIN, LOW); 
    l = left;
  } else {
    digitalWrite(MOTOR_LEFT_0_PIN, LOW); 
    digitalWrite(MOTOR_LEFT_1_PIN, HIGH);
    l = -left;
  }
  if(right>0){
    digitalWrite(MOTOR_RIGHT_0_PIN, HIGH); 
    digitalWrite(MOTOR_RIGHT_1_PIN, LOW);
    r = right;
  } else {
    digitalWrite(MOTOR_RIGHT_0_PIN, LOW); 
    digitalWrite(MOTOR_RIGHT_1_PIN, HIGH);
    r = -right;
  }
  
  analogWrite(MOTOR_LEFT_PWM_PIN, l*255/100);
  analogWrite(MOTOR_RIGHT_PWM_PIN, r*255/100);
}

void moveLeftRight(MovingEvent & movingEvent){
  int left = 0;
  int right = 0;
  if(movingEvent.m_vitesse > 0){
    if(movingEvent.m_angle < 0){
      left = 60;
      int angle = min(-movingEvent.m_angle, 40);
      right = 60 - angle;      
    } else {
      right = 60;
      int angle = min(movingEvent.m_angle, 40);
      left = 60 - angle;
    }
    left = left * movingEvent.m_vitesse / 100;
    right = right * movingEvent.m_vitesse / 100;
  } else {
    if(movingEvent.m_angle < 0){
      left = -60;
      int angle = min(-movingEvent.m_angle, 40);
      right = -60 + angle;      
    } else {
      right = -60;
      int angle = min(movingEvent.m_angle, 40);
      left = -60 + angle;
    }
    left = left * -movingEvent.m_vitesse/ 100;
    right = right * -movingEvent.m_vitesse/ 100;
  }
  //INFO("totot " << left << " " << right);
  motor_direction(left, right);
}


void moveRobot(MovingEvent & movingEvent){
  int angle_servo;
  if(movingEvent.m_angle < -90){
    angle_servo = -90;
  } else if(movingEvent.m_angle > 90){
    angle_servo = 90;
  } else {
    angle_servo = movingEvent.m_angle;
  }
  
  angle_servo += 90;
  servo.write(angle_servo);

  moveLeftRight(movingEvent);
  
}



