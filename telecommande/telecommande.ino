//client
/**
 * A Mirf example to test the latency between two Ardunio.
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
 * Note: To see best case latency comment out all Serial.println
 * statements not displaying the result and load 
 * 'ping_server_interupt' on the server.
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <LiquidCrystal.h>

const int speed_knob = A2;
const int turn_knob = A3;


LiquidCrystal lcd(2,3, 4,5,6,7);

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("beginning");
  Mirf.cePin = 9; 
  Mirf.csnPin = 8; 
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setRADDR((byte *)"clie1");
  Mirf.payload = 32;
  Mirf.config();
  Serial.println("Beginning ... "); 

  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  digitalWrite(A0,LOW);
  digitalWrite(A1,HIGH);
}

char get_char(int i){
  if(i == 0){
    return '0';
  } else if(i == 1){
    return '1';
  } else if(i == 2){
    return '2';
  } else if(i == 3){
    return '3';
  } else if(i == 4){
    return '4';
  } else if(i == 5){
    return '5';
  } else if(i == 6){
    return '6';
  } else if(i == 7){
    return '7';
  } else if(i == 8){
    return '8';
  }  else if(i == 9){
    return '9';
  } else {
    return 'x';
  }
}

void write_int(char * data, int value, int i){
  data[i] = get_char((value/1000)%10);
  data[i+1] = get_char((value/100)%10);
  data[i+2] = get_char((value/10)%10);
  data[i+3] = get_char((value)%10);
}

void printLcdTimeOut(){
  Serial.println("Timeout on response from server!");
  String s = "Time out";
  size_t l = s.length();
  for(size_t i = 0; i < min(l,32); ++i){
    lcd.setCursor(i%16, i/16);
    lcd.print(s[i]);
  }
  for(size_t i = l; i < 32; ++i){
    lcd.setCursor(i%16, i/16);
    lcd.print(' ');
  }
}

void loop(){
  bool b = true;
  int turn_val= analogRead(turn_knob);
  int spd_val= analogRead(speed_knob);

  char data[32];
  for(int i = 0; i < 32; ++i){
    data[i] = ' ';
  }
  if(b){
    data[0] = '1';
  } else {
    data[0] = '0';
  }
  data[1] = ',';
  write_int(data, turn_val, 2);
  data[6] = ',';
  write_int(data, spd_val, 7);
  
  Mirf.setTADDR((byte *)"serv1");

  Serial.print("send ");
  Serial.println(data);
  Mirf.send((byte *)data);
  
  while(Mirf.isSending()){
  }
  Serial.println("Finished sending");
  delay(10);
  unsigned long time = millis();
  while(!Mirf.dataReady()){
    //Serial.println("Waiting");
    if ( ( millis() - time ) > 3000 ) {
      printLcdTimeOut();
      return;
    }
  }
  
  Mirf.getData((byte *) &data);
  
  Serial.print("receive: ");
  Serial.println(data);
  for(int i = 0; i < 32; ++i){
    lcd.setCursor(i%16, i/16);
    lcd.print(data[i]);
  }
  delay(1000);
} 
  
  
  

