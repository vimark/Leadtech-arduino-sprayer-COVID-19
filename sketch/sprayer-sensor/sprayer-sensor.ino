#include "hardware.h"

//#define VERBOSE

#define POT_DELAY_MULTIPLIER 30

long duration;
long distance;
long distance_sensor1;
long distance_sensor2;

unsigned long lastDebounceTime = 0;
unsigned long lastDebounceTimeSensor2 = 0;
unsigned long debounceDelay = 2000;
unsigned long lastDelayTime = 0;
unsigned long lastDelayTimeSensor2 = 0;

bool isActiveK1;
bool isActiveK2;
bool delayChanged;
bool proximityChanged;
bool sensor1State;
bool sensor2State;
bool last_sensor1State;
bool last_sensor2State;
bool halt;

int pot1_val;
int pot2_val;

int delayVal;
int proximity;

void setup() {
  Serial.begin(9600);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(k1, OUTPUT);
  pinMode(k2, OUTPUT);
  pinMode(eStop, INPUT);

  setK1(LOW);
  setK2(LOW);

  isActiveK1 = false;
  isActiveK2 = false;

  readAnalog();

  delayChanged = true;
  proximityChanged = true;

  last_sensor1State = false;
  last_sensor2State = false;

  distance_sensor1 = readSensor1();
  if(distance_sensor1 < proximity) sensor1State = true;
  else sensor1State = false;

  distance_sensor2 = readSensor2();
  if(distance_sensor2 < proximity) sensor2State = true;
  else sensor2State = false;

  halt = false;
}

void loop() {

  if(digitalRead(eStop)==LOW){
    turn_off_k1();
    turn_off_k2();
    halt=true;
  }

  readAnalog();

  if(delayChanged){
    delayVal = pot1_val;
    delayChanged = false;
  }

  if(proximityChanged){
    proximity = pot2_val;
    proximityChanged = false;
  }

  
  distance_sensor1 = readSensor1();
  distance_sensor2 = readSensor2();

  if(halt==false){
    
    if(distance_sensor1 < proximity) sensor1State = true;
    else sensor1State = false;
  
    if(distance_sensor2 < proximity) sensor2State = true;
    else sensor2State = false;
    
    if(sensor1State != last_sensor1State) lastDebounceTime = millis();
    if(sensor2State != last_sensor2State) lastDebounceTimeSensor2 = millis();
  
    if((millis() - lastDebounceTime) > debounceDelay){
      
      if(distance_sensor1 < proximity) turn_on_k1();
      else if(isActiveK1==false) turn_off_k1();
    }
  
    if((millis() - lastDebounceTimeSensor2) > debounceDelay){
      
      if(distance_sensor2 < proximity) turn_on_k2();
      else if(isActiveK2==false) turn_off_k2();
    }
    
    distance_sensor1 = readSensor1();
    if(distance_sensor1 < proximity) last_sensor1State = true;
    else last_sensor1State = false;
  
    distance_sensor2 = readSensor2();
    if(distance_sensor2 < proximity) last_sensor2State = true;
    else last_sensor2State = false;
    
    if(isActiveK1){
  
      unsigned long Time1 = millis();
      //Serial.print("\nDelay Time: "); Serial.print(Time - lastDelayTime);
      if((Time1 - lastDelayTime) > (pot1_val * POT_DELAY_MULTIPLIER)){
        turn_off_k1();
      }
    }
  
    if(isActiveK2){
  
      unsigned long Time2 = millis();
      //Serial.print("\nDelay Time: "); Serial.print(Time - lastDelayTime);
      if((Time2 - lastDelayTimeSensor2) > (pot1_val * POT_DELAY_MULTIPLIER)){
        turn_off_k2();
      }
    }

  }
#ifdef VERBOSE
  Serial.print("\nDistance 1 = "); Serial.print(distance_sensor1); Serial.print(" | Distance 2 = "); Serial.print(distance_sensor2); Serial.print(" | Pot 1 = ");
  Serial.print(pot1_val); Serial.print(" | Pot 2 = ");
  Serial.print(pot2_val); 
#endif
  
  //Millisecond
  delay(50);
}

long readSensor1(){
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin1, LOW);

  duration = pulseIn(echoPin1, HIGH);

  //This gives us distance in cm
  distance = duration/58.2;

  //Serial.println(distance);

  return distance;
}

long readSensor2(){
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin2, LOW);

  duration = pulseIn(echoPin2, HIGH);

  //This gives us distance in cm
  distance = duration/58.2;

  //Serial.println(distance);

  return distance;
}

void setK1(bool val){
  digitalWrite(k1, val);
}
void setK2(bool val){
  digitalWrite(k2, val);
  isActiveK2 = true;
}

void readAnalog(){

  int val1;
  int val2;
  
  val1 = analogRead(pot1);
  val2 = analogRead(pot2);

  if(pot1_val != val1){
     pot1_val = val1;
     delayChanged = true;
  }
  if(pot2_val != val2){
     pot2_val = val2;
     proximityChanged = true;
  }
}

void turn_on_k1(){
  if(isActiveK1==false){
    setK1(HIGH);
    lastDelayTime = millis();
    isActiveK1 = true;
  }
}

void turn_off_k1(){
  setK1(LOW);
  isActiveK1 = false;
}

void turn_on_k2(){
  if(isActiveK2==false){
    setK2(HIGH);
    lastDelayTimeSensor2 = millis();
    isActiveK2 = true;
  }
}

void turn_off_k2(){
  setK2(LOW);
  isActiveK2 = false;
}
