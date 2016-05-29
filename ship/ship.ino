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

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

void setup(){
  Serial.begin(9600);
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();   
  Mirf.setRADDR((byte *)"serv1");   
  Mirf.payload = sizeof(unsigned long);
  Mirf.config();
  Serial.println("Listening..."); 
}

void loop(){
  byte data[Mirf.payload];
  if(!Mirf.isSending() && Mirf.dataReady()){
    Serial.println("Got packet");
    Mirf.getData(data);
    Mirf.setTADDR((byte *)"clie1");
    Mirf.send(data);
    Serial.println("Reply sent.");
  }
}

