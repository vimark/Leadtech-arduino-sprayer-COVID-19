#include "hardware.h"

//#define VERBOSE

#define POT_DELAY_MULTIPLIER 30

long duration;
long distance;
long distance_sensor1;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 2000;
unsigned long lastDelayTime = 0;
unsigned long currentTimeForDelay;

bool isActiveK1;
bool isActiveK2;
bool delayChanged;
bool proximityChanged;
bool sensor1State;
bool last_sensor1State;

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
  pinMode(sw1, INPUT);

  setK1(LOW);
  setK2(LOW);

  isActiveK1 = false;
  isActiveK2 = false;

  readAnalog();

  delayChanged = true;
  proximityChanged = true;

  last_sensor1State = false;

  distance_sensor1 = readSensor1();
  if(distance_sensor1 < proximity) sensor1State = true;
  else sensor1State = false;

  currentTimeForDelay = millis();
}

void loop() {

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
  
  if(distance_sensor1 < proximity) sensor1State = true;
  else sensor1State = false;
  
  if(sensor1State != last_sensor1State) lastDebounceTime = millis();

  if((millis() - lastDebounceTime) > debounceDelay){
    
    if(distance_sensor1 < proximity) turn_on_k1();
    //else turn_off_k1();
    else if(isActiveK1==false) turn_off_k1();
  }
  
  distance_sensor1 = readSensor1();
  if(distance_sensor1 < proximity) last_sensor1State = true;
  else last_sensor1State = false;

  if(isActiveK1){

    unsigned long Time = millis();
    Serial.print("\nDelay Time: "); Serial.print(Time - lastDelayTime); 
    if((Time - lastDelayTime) > (pot1_val * POT_DELAY_MULTIPLIER)){
      turn_off_k1();
    }
  }

#ifdef VERBOSE
  Serial.print("\nDistance: "); Serial.print(distance); Serial.print(" | Pot 1 = ");
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

void setK1(bool val){
  digitalWrite(k1, val);
}
void setK2(bool val){
  digitalWrite(k2, val);
  isActiveK2 = true;
}

void setDelayK1(short val){
  
}

void setDelayK2(short val){
  
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
