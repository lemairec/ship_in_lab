#ifndef LOGGING_H
#define LOGGING_H

#include <SoftwareSerial.h>
#include <Arduino.h> 

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 

#define INFO(x) Serial << x << "\n"

#endif
