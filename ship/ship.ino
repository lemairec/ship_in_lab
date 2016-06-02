//server
/**
 * An Mirf example which copies back the data it recives.
 *
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 8
 * CSN -> 7
 *
 */

#include "utils.h"
#include "pins.h"
#include "logging.h"
#include "gps.h"
#include "compass.h"
#include "moving_waypoint.h"
#include <SPI.h>
#include <Mirf.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

char dataToSend[32];
char dataReceive[32];

GpsReader gpsReader;
GpsEvent gpsEvent;

int angle_servo;
Servo servo;

MovingWaypoint movingWayPoint;
MovingWaypointEvent movingWaypointEvent;

class MovingEvent{
public:
  int m_angle;
  int m_vitesse;
};
MovingEvent movingEvent;

class TelecommandEvent {
public:
    int m_xref;
    int m_yref;
    bool m_active;
    void setActive(){
      m_active = m_xref < 400 || m_xref > 600 || m_yref < 400 || m_yref > 600;
    }

    void init(){
      m_xref = 500;
      m_yref = 500;
      m_active = false;
    }
};
TelecommandEvent telecommandEvent;

void parse_dataReceive(){
  telecommandEvent.m_xref = readChar(dataReceive, 2) *1000 + readChar(dataReceive, 3) *100 + readChar(dataReceive, 4) *10 + readChar(dataReceive, 5);
  telecommandEvent.m_yref = readChar(dataReceive, 7) *1000 + readChar(dataReceive, 8) *100 + readChar(dataReceive, 9) *10 + readChar(dataReceive, 10);
}

bool receiveAndSendMessage(){
  if(!Mirf.isSending() && Mirf.dataReady()){
    INFO("Got packet");
    Mirf.getData((byte *)dataReceive);
    Mirf.setTADDR((byte *)"clie1");
    parse_dataReceive();
    telecommandEvent.setActive();
    if(telecommandEvent.m_active){
      empty_data(dataToSend, 32);
      write_uint4(dataToSend, telecommandEvent.m_xref, 0);
      write_uint4(dataToSend, telecommandEvent.m_yref, 5);
      INFO("telecommande " << telecommandEvent.m_xref);
      loopTelecommande();
      
      INFO(dataToSend);
    }
    Mirf.send((byte *)dataToSend);
    Serial.println("Reply sent.");
  }
}

bool receiveAndSendMessage(String s){
  INFO(s);
  empty_data(dataToSend, 32);
  size_t l = s.length();
  for(size_t i = 0; i < min(l,32); ++i){
    dataToSend[i] = s[i]; 
  }
  receiveAndSendMessage();
}

void setup(){
  Serial.begin(9600);
  Mirf.cePin = 10; 
  Mirf.csnPin = 9; 
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();   
  Mirf.setRADDR((byte *)"serv1");   
  Mirf.payload = 32;
  Mirf.config();

  servo.attach(7);
  
  receiveAndSendMessage("init ok");
  telecommandEvent.init();
  INFO("telecommande " << telecommandEvent.m_xref);

  Serial.println("Listening...");

  receiveAndSendMessage("gps init");
  gpsReader.init();

  compass_x_offset = -210.88;
  compass_y_offset = 126.20;
  compass_z_offset = 361.92;
  compass_x_gainError = 1.12;
  compass_y_gainError = 1.13;
  compass_z_gainError = 1.03;
  receiveAndSendMessage("compass init");
  compass_init(2);
  compass_debug = 1;

  receiveAndSendMessage("compass calibration");
  //compass_offset_calibration(3);
  
  while(!gpsEvent.isValid()){
    receiveAndSendMessage("wait gps");
    if(!telecommandEvent.m_active){
      gpsReader.readNextFrame(gpsEvent);
      movingEvent.m_angle = 0;
      movingEvent.m_vitesse = 0;
      move_();
    }
  }
  receiveAndSendMessage("gps ok");
}

void loopTelecommande(){
  movingEvent.m_angle = map(telecommandEvent.m_xref, 0, 1024, -180, 180);
  movingEvent.m_vitesse = map(telecommandEvent.m_yref, 0, 1024, -100, 100);
  move_();
}

void move_(){
  if(movingEvent.m_angle < -90){
    angle_servo = -90;
  } else if(movingEvent.m_angle > 90){
    angle_servo = 90;
  } else {
    angle_servo = movingEvent.m_angle;
  }
  angle_servo += 90;
  servo.write(angle_servo);
}

void loop(){
  
  if(!telecommandEvent.m_active){
    empty_data(dataToSend, 32);
    gpsReader.readNextFrame(gpsEvent);
    if(gpsEvent.isValid()){
      /*char s1[40];
      dtostrf(gpsEvent.m_latitude, 4, 8, s1);
      char s2[40];
      dtostrf(gpsEvent.m_longitude, 4, 8, s2);
      for(size_t i = 0; i < 12; ++i){
        dataToSend[i] = s1[i];
        dataToSend[i + 16] = s2[i];
      }*/

      compass_scalled_reading();
      compass_heading();

      movingWayPoint.getMovingEvent(gpsEvent, movingWaypointEvent);

      movingEvent.m_angle = bearing - movingEvent.m_angle;
      if(movingEvent.m_angle > 180){
        movingEvent.m_angle -= 360;
      }
      if(movingEvent.m_angle < -180){
        movingEvent.m_angle += 360;
      }

      
      

      dataToSend[0] = 'w';
      dataToSend[1] = 'p';
      write_uint4(dataToSend, movingWaypointEvent.m_waypoint_no, 4);

      dataToSend[16] = 'd';
      write_uint4(dataToSend, movingWaypointEvent.m_distance, 18);

      dataToSend[24] = 'a';
      write_int5(dataToSend, movingEvent.m_angle, 26);

      INFO("d " << movingWaypointEvent.m_distance << " a " << movingEvent.m_angle);
      receiveAndSendMessage();

      move_();
      //servo.write(angle_servo);
    }
  } else {
      receiveAndSendMessage();
    }
    
  //delay(3000);
}

