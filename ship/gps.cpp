#include "gps.h"
#include "pins.h"
#include "logging.h"

#include <Arduino.h> 

GpsEvent::GpsEvent(){
    reset();
}

void GpsEvent::reset(){
    m_latitude = 0;
    m_longitude = 0;
}
    
bool GpsEvent::isValid(){
    return m_latitude > 0.1;
}

GpsReader::GpsReader()
    :m_lastFix(0),
    m_serial(GPS_TX_PIN, GPS_RX_PIN)
{
    resetBuffer();
}

void GpsReader::init(){
    m_serial.begin(9600);
}

void GpsReader::resetBuffer(){
    m_bufferIndLast = 0;
}

bool GpsReader::isEqual(char * c, size_t size){
    for(size_t i = 0; i < size; ++i){
        if(c[i] == m_buffer[m_tempInd]){
            ++m_tempInd;
        } else {
            //INFO(c[i] << " " << m_buffer[m_tempInd] << " " << i);
            return false;
        }
    }
    return true;
}

bool GpsReader::readChar(char c){
    if(c == '$'){
        //INFO("readChar");
        resetBuffer();
    } else if(c == '\n'){
        //INFO("readChar");
        parseBuffer();
        return true;
    } else {
        this->m_buffer[m_bufferIndLast] = c;
        m_bufferIndLast++;
    }
    return false;
}

void GpsReader::error(){
    Serial.print("error");
}

void GpsReader::debug(){
    Serial.print(m_tempInd);
    Serial.print(" ");
    Serial.print(m_buffer[m_tempInd-1]);
    Serial.print(m_buffer[m_tempInd]);
    Serial.print(m_buffer[m_tempInd+1]);
    Serial.print("\n");
}

void GpsReader::readUntilCommat(){
    while(m_tempInd < m_bufferIndLast){
        if(m_buffer[m_tempInd] == ','){
            ++m_tempInd;
            return;
        }
        ++m_tempInd;
    }
    error();
}

int GpsReader::getOneInt(){
    char c = m_buffer[m_tempInd];
    m_tempInd++;  
    if(c =='0'){
         return 0;
    } else if(c =='1'){
         return 1;
    } else if(c =='2'){
         return 2;
    } else if(c =='3'){
         return 3;
    } else if(c =='4'){
         return 4;
    } else if(c =='5'){
         return 5;
    } else if(c =='6'){
         return 6;
    } else if(c =='7'){
         return 7;
    } else if(c =='8'){
         return 8;
    } else if(c =='9'){
         return 9;
    } else {
         error();
    }
    return 0;
}

double GpsReader::readDouble(){
    double res = 0;
    double virgule_part = 1;
    bool virgule = false;
    while(m_tempInd < m_bufferIndLast){
        char c = m_buffer[m_tempInd];
        int number = 0;
        if(c == ','){
            ++m_tempInd;
            return res;
        } else if(c =='0'){
            number = 0;
        } else if(c =='1'){
            number = 1;
        } else if(c =='2'){
            number = 2;
        } else if(c =='3'){
            number = 3;
        } else if(c =='4'){
            number = 4;
        } else if(c =='5'){
            number = 5;
        } else if(c =='6'){
            number = 6;
        } else if(c =='7'){
            number = 7;
        } else if(c =='8'){
            number = 8;
        } else if(c =='9'){
            number = 9;
        } else if(c =='.'){
            virgule = true;
        }
        if(!virgule){
            res = res * 10 + number;
        } else {

            res = res + number * virgule_part;
            virgule_part = virgule_part * 0.1;
        }
        ++m_tempInd;
    }
    error();
}

//GPRMC,210136.00,A,4902.71610,N,00324.04158,E,0.150,,180516,
//$GPGGA,114608.00,4905.46094,N,00332.09303,E,2,07,1.46,87.8,M,46.3,M,,0000*6B
void GpsReader::parseBuffer(){
    //INFO("parse buffer ");
    m_tempInd = 0;
    if(!isEqual("GPRMC,", 6)){
        return;
    }
    readUntilCommat();
    readUntilCommat();
    //debug();
    m_lastLatitude = readDouble();
    readUntilCommat();
    //debug();

    m_lastLongitude = readDouble();
    //m_lastFix = getOneInt();
    
    m_lastLatitude = convertToDecDeg(m_lastLatitude);
    m_lastLongitude = convertToDecDeg(m_lastLongitude);
    if(m_lastLatitude == 0){
        INFO(m_buffer);
    }
}

bool GpsReader::readNextFrame(GpsEvent & gpsEvent){
    gpsEvent.reset();
    while ( m_serial.available()){
        char c = m_serial.read();
        //INFO(c);
        if(this->readChar(c)){
            gpsEvent.m_latitude = m_lastLatitude;
            gpsEvent.m_longitude = m_lastLongitude;
            return true;
        }
    }
    return false;
}
    

double GpsReader::convertToDecDeg (double degMin)
{
  /*// Serial.println(degMin);
  int h = degMin/100;
  int minu = (degMin-(h*100));
  float sec =(degMin-((h*100)+minu))*100.0;
  float minum = minu +(sec/60.0);
  float decDeg = h + (minum/60.0) ;
  // Serial.println(decDeg,6);
  return decDeg;*/

  int h = degMin/100;
  double minu = (degMin-(h*100));
  return h + minu/60.0;
}

/*
#include "utils.h"
#include "logging.h"
#include "gps.h"
GpsReader gpsReader;
GpsEvent gpsEvent;

void setup(){
  Serial.begin(9600);
  
  Serial.println("Listening...");
  gpsReader.init();
  while(!gpsEvent.isValid()){
    gpsReader.readNextFrame(gpsEvent);
  }
  
}

void loopTelecommande(){
  
}

void loop(){
  gpsReader.readNextFrame(gpsEvent);
  if(gpsEvent.isValid()){
    INFO(gpsEvent.m_latitude << " " << gpsEvent.m_longitude);
  }
}
*/
