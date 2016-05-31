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
#include "logging.h"
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

char dataToSend[32];
char dataReceive[32];

class TelecommandEvent {
public:
    int m_xref;
    int m_yref;
    bool m_active;
    void setActive(){
      m_active = m_xref < 400 || m_xref > 600 || m_yref < 400 || m_yref > 600;
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
      for(size_t i = 0; i < 32; ++i){
        dataToSend[i] = '-';
      }
      dataToSend[0] = 't'; dataToSend[1] = 'e'; dataToSend[2] = 'l'; dataToSend[3] = 'e'; dataToSend[4] = 'c'; dataToSend[5] = 'o'; dataToSend[6] = 'm';
      dataToSend[16] = 'x';
      write_int4(dataToSend, telecommandEvent.m_xref, 17);
      write_int4(dataToSend, telecommandEvent.m_yref, 22);
      loopTelecommande();
      INFO(dataToSend);
    }
    Mirf.send((byte *)dataToSend);
    Serial.println("Reply sent.");
  }
}

bool receiveAndSendMessage(String s){
  INFO(s);
  size_t l = s.length();
  for(size_t i = 0; i < min(l,32); ++i){
    dataToSend[i] = s[i]; 
  }
  for(size_t i = l; i < 32; ++i){
    dataToSend[i] = ' '; 
  }
  receiveAndSendMessage();
}

void setup(){
  Serial.begin(9600);
  Mirf.cePin = 9; 
  Mirf.csnPin = 8; 
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();   
  Mirf.setRADDR((byte *)"serv1");   
  Mirf.payload = 32;
  Mirf.config();

  receiveAndSendMessage("init ok");
  
  Serial.println("Listening...");

void loopTelecommande(){
  
}

void loop(){
  receiveAndSendMessage("loop");
}

