#ifndef UTILS_H
#define UTILS_H

inline int readChar(char * data, size_t i){
  char c = data[i];
  if(c == '1'){
    return 1;
  } else if(c == '2'){
    return 2;
  } else if(c == '3'){
    return 3;
  } else if(c == '4'){
    return 4;
  } else if(c == '5'){
    return 5;
  } else if(c == '6'){
    return 6;
  } else if(c == '7'){
    return 7;
  } else if(c == '8'){
    return 8;
  } else if(c == '9'){
    return 9;
  } else {
    return 0;
  }
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

void write_uint4(char * data, int value, size_t i){
  data[i] = get_char((value/1000)%10);
  data[i+1] = get_char((value/100)%10);
  data[i+2] = get_char((value/10)%10);
  data[i+3] = get_char((value)%10);
}

void write_int5(char * data, int value, size_t i){
  if(value>0){
    data[i] = '+';
    write_uint4(data, value, i+1);
  } else {
    data[i] = '-';
    write_uint4(data, -value, i+1);
  }
}

void empty_data(char * data, size_t l){
  for(size_t i = 0; i < l; ++i){
    data[i] = ' ';
  }
}

#endif //MOTOR_DIRECTION_H
