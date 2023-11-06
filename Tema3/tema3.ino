// Declare all the joystick pins
const int pinSW = 2; // Digital pin connected to switch output
const int pinX = A0; // A0 - Analog pin connected to X output
const int pinY = A1; // A1 - Analog pin connected to Y output
// Declare all the segments pins
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;

long timeBlink=0,lastMove=0,butonReadingTime=0;
int blinkDelay=300;

int index = 7;

bool commonAnode = false; // Modify if you have common anode
const int nrOfSegments = 8;
const int nrMiscari = 5;

byte sem=LOW;
byte state = HIGH;
byte dpState = LOW;

int xValue = 0;
int yValue = 0;

int digit = 0;
int minThreshold = 400;
int maxThreshold = 600;

volatile unsigned long lastInterruptTime = 0;

const unsigned long debounceDelay = 200,mutariDelay=400,longPressDelay=1000;

byte lastSwState = LOW;  
byte swState;     
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

int segments[] = {
pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
byte digitMatrix[nrOfSegments][nrMiscari] = {
// up down left right state
  { 0,   6,   5,    1,    0}, // a 0
  { 0,   6,   5,    1,    0}, // b 1
  { 6,   3,   4,    7,    0}, // c 2
  { 6,   3,   4,    2,    0}, // d 3
  { 6,   3,   4,    2,    0}, // e 4
  { 0,   6,   5,    1,    0}, // f 5
  { 0,   3,   6,    6,    0}, // g 6
  { 7,   7,   2,    7,    0}, // dp 7
};
void setup() {
  pinMode(pinSW, INPUT_PULLUP);  
  
  for (int i = 0; i < nrOfSegments; i++) {
    pinMode(segments[i], OUTPUT);
  }

  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, RISING);
  Serial.begin(9600);
}
void loop() {

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  swState = digitalRead(pinSW);

  if(lastSwState == HIGH && swState == LOW)
    pressedTime = millis();
  else if(lastSwState == LOW && swState == HIGH) { 
    releasedTime = millis();

    if( releasedTime - pressedTime > mutariDelay )
    {
       index=7;
       for(int i=0;i<nrOfSegments;i++)
       {
          digitMatrix[i][4]=0;
       } 
    }
  }
  lastSwState = swState;

  
  digitalWrite(segments[index],state);
  for(int i=0;i<nrOfSegments;i++)
  {
      if(i!=index)
        digitalWrite(segments[i],digitMatrix[i][4]);
  }
  
  if(millis()-timeBlink>blinkDelay)
  {
    timeBlink=millis();
    state=!state;  
  }
     
  if(xValue>maxThreshold&&millis()-lastMove>mutariDelay){
     digitalWrite(segments[index],LOW);
     index=digitMatrix[index][1];
     lastMove=millis();
  }
  if(xValue<minThreshold&&millis()-lastMove>mutariDelay){
     digitalWrite(segments[index],LOW);
     index=digitMatrix[index][0];
     lastMove=millis();
  }
  if(yValue>maxThreshold&&millis()-lastMove>mutariDelay){
     digitalWrite(segments[index],LOW);
     index=digitMatrix[index][3];
     lastMove=millis();
  }
  if(yValue<minThreshold&&millis()-lastMove>mutariDelay){
     digitalWrite(segments[index],LOW);
     index=digitMatrix[index][2];
     lastMove=millis();
  }
}
void handleInterrupt() {
  static unsigned long interruptTime = 0;
  interruptTime = micros();
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {
    digitMatrix[index][4]=1-digitMatrix[index][4];
  }
  lastInterruptTime = interruptTime;
}
