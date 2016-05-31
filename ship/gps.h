#ifndef GPS_H
#define GPS_H
#include "logging.h"

class GpsEvent {
  public:
    GpsEvent();
    double m_latitude;
    double m_longitude;

    void reset();
    bool isValid();
};


class GpsReader {
    int m_bufferIndLast;
    char m_buffer[200];
    
    int m_tempInd;
    void parseBuffer();
    bool isEqual(char * c, size_t size);
    void readUntilCommat();
    void error();
    void debug();
    double readDouble();
    int getOneInt();
    
  

    SoftwareSerial m_serial;
public:
    double m_lastLatitude;
    double m_lastLongitude;
    int m_lastFix;
    
    GpsReader();
    void init();

    void resetBuffer();
    bool readChar(char c);
    bool readNextFrame(GpsEvent & event);
    
    double convertToDecDeg (double degMin);

};


#endif //MOTOR_DIRECTION_H
