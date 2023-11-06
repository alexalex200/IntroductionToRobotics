const int InPin1=A1;
const int InPin2=A2;
const int InPin3=A3;

unsigned int InVal1;
unsigned int InVal2;
unsigned int InVal3;

const int OutPin1=11;
const int OutPin2=10;
const int OutPin3=9;

unsigned int OutVal1;
unsigned int OutVal2;
unsigned int OutVal3;

const int MinInVal=0;
const int MaxInVal=1023;
const int MinOutVal=0;
const int MaxOutVal=255;

void setup() {
  pinMode(OutPin1,OUTPUT);
  pinMode(OutPin2,OUTPUT);
  pinMode(OutPin3,OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  
  InVal1=analogRead(InPin1);
  InVal2=analogRead(InPin2);
  InVal3=analogRead(InPin3);

  OutVal1=map(InVal1,MinInVal,MaxInVal,MinOutVal,MaxOutVal);
  OutVal2=map(InVal2,MinInVal,MaxInVal,MinOutVal,MaxOutVal);
  OutVal3=map(InVal3,MinInVal,MaxInVal,MinOutVal,MaxOutVal);
  
  analogWrite(OutPin1,OutVal1);
  analogWrite(OutPin2,OutVal2);
  analogWrite(OutPin3,OutVal3);
}
