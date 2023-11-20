#include <EEPROM.h>

byte MenuCurrent=0,submenuCurrent=0;
byte semAfis=0,semAfisSubmenu=1;
byte samplingRate=1;

int input=0;
byte inputComplete=false;

const byte photocellPin=0;
int photocellValue;

const byte trigPin=9,echoPin=10;

long duration=0;
int distance=0;

const byte ledRedPin=5,ledGreenPin=6,ledBluePin=3;
int redVal=0,greenVal=0,blueVal=0;

int minUltrasonicThreshold=0,maxUltrasonicThreshold=400;
int minLDRThreshold=0,maxLDRThreshold=1024;
byte semMinMax=1;

byte index=0;
const byte intSize=2,UltrasonicStartPoz=1,LDRStartPoz=30;

unsigned long lastUpdate=0,lastWrite=0;

byte semToggleLed=0;

byte indexWrite=0;
void MainMenu()
{
  if(!semAfis){
    Serial.print("Main Menu:\n  1. Sensor Settings\n  2. Reset Logger Data\n  3. System Status\n  4. RGB LED Control\n");
    semAfis=1;
  }
  else {
    if (Serial.available() > 0) {
      input= Serial.parseInt();
      
      if(input>=1&&input<=4){
        MenuCurrent=input;
        semAfis=0;
      }
      else
        Serial.println("Invalid Input");
    }
  }
}

void SensorSettings()
{
  if(!semAfis){
    Serial.print("Sensor Settings:\n  1. Sensors Sampling Interval\n  2. Ultrasonic Alert Threshold\n  3. LDR Alert Threshold\n  4. Back\n");
    semAfis=1;
  }
  else {

    if(!submenuCurrent)
    {
      if (Serial.available() > 0) {
        input= Serial.parseInt(); 
        if(input>=1&&input<=4)
          submenuCurrent=input;
        else
          Serial.println("Invalid Input");
      }
    }
    else {
      switch (submenuCurrent) {
        case 1:
          if(semAfisSubmenu)
          {
            Serial.print("Provide a value between 1 and 10 seconds(the value will be used as a sampling rate for the sensors):\n");
            semAfisSubmenu=0;
          }
          else {
            if (Serial.available() > 0) {
              input= Serial.parseInt(); 
              if(input>=1&&input<=10){
                samplingRate=input;
                Serial.print("sampling rate set to ");
                Serial.println(input);
                submenuCurrent=0;
                semAfis=0;
                semAfisSubmenu=1;
              }
              else
                Serial.println("Invalid Input");
            }
          }
          break;
        case 2:
          if(semAfisSubmenu)
          {
            Serial.print("Provide the treshold values for the ultrasonic sensor(0-400)\nMin: Max:");
            semAfisSubmenu=0;
          }
          else {
            if (Serial.available() > 0) {
              input= Serial.parseInt();

              if(input>=0&&input<=400){
                if(semMinMax){
                  minUltrasonicThreshold=input;
                  semMinMax=0;  
                }            
                else {
                  maxUltrasonicThreshold=input;
                  semMinMax=1;
                  submenuCurrent=0;
                  semAfis=0;
                  semAfisSubmenu=1;
                }
              }
              else {
                semMinMax=1;
                Serial.println("Invalid Input");
              }
            } 
          }
          break;
        case 3:
          if(semAfisSubmenu)
          {
            Serial.print("Provide the treshold values for the LDR sensor(0-1024)\nMin: Max:");
            semAfisSubmenu=0;
          }
          else {
            if (Serial.available() > 0) {
              input= Serial.parseInt();

              if(input>=0&&input<=1024){
                if(semMinMax){
                  minLDRThreshold=input;
                  semMinMax=0;  
                }            
                else {
                  maxLDRThreshold=input;
                  semMinMax=1;
                  submenuCurrent=0;
                  semAfis=0;
                  semAfisSubmenu=1;
                }
              }
              else {
                semMinMax=1;
                Serial.println("Invalid Input");
              }
            } 
          }
          break;
        case 4:
          MenuCurrent=0;
          semAfis=0;
          submenuCurrent=0;
          break;
      }
    }
  }
}

void ResetLoggerData()
{
  if(!semAfis){
    Serial.print("Reset Logger Data:\nAre you sure?\n  1 YES\n  2 NO\n");
    semAfis=1;
  }
  else {
    if (Serial.available() > 0) {
        input= Serial.parseInt(); 
        switch (input) {
          case 1:
            samplingRate=1;
            redVal=0;
            greenVal=0;
            blueVal=0;
            minUltrasonicThreshold=0;
            maxUltrasonicThreshold=400;
            minLDRThreshold=0;
            maxLDRThreshold=1024;
            semToggleLed=0;
            MenuCurrent=0;
            semAfis=0;
            break;
          case 2:
            MenuCurrent=0;
            semAfis=0;
            break;
          default:
            Serial.println("Invalid Input");
            break;
        }
    }
  }
}

void SystemStatus()
{
  if(!semAfis){
    Serial.print("System Status:\n  1. Current Sensor Readings\n  2. Current Sensor Settings\n  3. Display Logged Data\n  4. Back\n");
    semAfis=1;
  }
  else {
    if(!submenuCurrent)
    {
      if (Serial.available() > 0) {
        input= Serial.parseInt(); 
        if(input>=1&&input<=4)
          submenuCurrent=input;
        else
          Serial.println("Invalid Input");
      }
    }
    else {
      switch (submenuCurrent) {
        case 1:
          if(millis()-lastWrite>samplingRate*1000){
            Serial.print("Ultrasonic sensor reading: ");
            Serial.println(distance);
            Serial.print("LDR sensor reading: ");
            Serial.println(photocellValue);
            Serial.println("Type any character to exit");

            lastWrite=millis();
            if(Serial.available()>0)
            {
              char character=Serial.read();
              submenuCurrent=0;
              semAfis=0; 
            }
          }
          break;
        case 2:
          Serial.print("Sampling rate: ");
          Serial.print(samplingRate);
          Serial.println("s");

          Serial.print("Ultrasonic treshold values (min-max): ");
          Serial.print(minUltrasonicThreshold);
          Serial.print("-");
          Serial.println(maxUltrasonicThreshold);

          Serial.print("LDR treshold values (min-max): ");
          Serial.print(minLDRThreshold);
          Serial.print("-");
          Serial.println(maxLDRThreshold);
          
          submenuCurrent=0;
          semAfis=0;

          break;
        case 3:

          Serial.println("The last 10 readings for the ultrasonic sensor:");
          int val;
          for (int i=0;i<10;i++)
          {
              EEPROM.get(UltrasonicStartPoz+i*intSize,val);
              Serial.print(val);
              Serial.print(" ");
          }
          Serial.println();
          Serial.println("The last 10 readings for the LDR sensor:");
          for (int i=0;i<10;i++)
          {
              EEPROM.get(LDRStartPoz+i*intSize,val);
              Serial.print(val);
              Serial.print(" ");
          }
          Serial.println();  
          submenuCurrent=0;
          semAfis=0;
          
          break;
        case 4:
          MenuCurrent=0;
          semAfis=0;
          submenuCurrent=0;
          break;
      }
    }
  }
}

void RGBLEDControl()
{
  if(!semAfis){
    Serial.print("RGB LED Control:\n  1. Manual Color Control\n  2. LED: Toggle Automatic ON/OFF\n  3. Back\n");
    semAfis=1;
  }
  else {
    if(!submenuCurrent)
    {
      if (Serial.available() > 0) {
        input= Serial.parseInt(); 
        if(input>=1&&input<=3)
          submenuCurrent=input;
        else
          Serial.println("Invalid Input");
      }
    }
    else {
      switch (submenuCurrent) {
        case 1:
          if(semAfisSubmenu){
            if(indexWrite==0)
              Serial.print("Red value(0-254):");
            if(indexWrite==1)
              Serial.print("Green value(0-254):");
            if(indexWrite==2)
              Serial.print("Blue value(0-254):");
            semAfisSubmenu=0;
          }
          else
          { 
            if (Serial.available() > 0) {
              input= Serial.parseInt(); 
              if(input>=0&&input<=254){
                if(indexWrite==0){
                  redVal=input;
                  Serial.println(input);
                }
                if(indexWrite==1){
                  greenVal=input;
                  Serial.println(input);
                }
                if(indexWrite==2){
                  blueVal=input;
                  Serial.println(input);
                  submenuCurrent=0;
                  semAfis=0;
                }
                indexWrite++;
                semAfisSubmenu=1;
                if(indexWrite==3)
                  indexWrite=0;
              }
              else
                Serial.println("Invalid Input");
            }
          }
          break;
        case 2:
            if(semAfisSubmenu){
              Serial.print("LED toggle automatic:\n1.ON\n2.YES\n");
              semAfisSubmenu=0;
            }
            else {
              if (Serial.available() > 0) {
                  input= Serial.parseInt(); 
                  switch (input) {
                    case 1:
                      semToggleLed=1;
                      submenuCurrent=0;
                      semAfisSubmenu=1;
                      semAfis=0;
                      break;
                    case 2:
                      semToggleLed=0;
                      submenuCurrent=0;
                      semAfisSubmenu=1;
                      semAfis=0;
                      break;
                    default:
                      Serial.println("Invalid Input");
                      break;
                  }
              }
            }
          break;
        case 3:
          MenuCurrent=0;
          semAfis=0;
          submenuCurrent=0;
          break; 
      }
    }
  }
}


void (*myPointer[])() = {
  MainMenu,
  SensorSettings,
  ResetLoggerData,
  SystemStatus,
  RGBLEDControl
};

void setup() {
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  myPointer[MenuCurrent]();

  if(millis()-lastUpdate>samplingRate*1000)
  {
    photocellValue=analogRead(photocellPin);

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);

    distance=duration*0.034/2;

    // EEPROM.put(UltrasonicStartPoz+index*intSize,distance);
    // EEPROM.put(LDRStartPoz+index*intSize,photocellValue);

    // index+=1;
    // if(index>=10)
    //   index=0;

    lastUpdate=millis();
  }


  if(semToggleLed)
  {
    if(photocellValue<minLDRThreshold ||photocellValue>maxLDRThreshold ){
      analogWrite(ledGreenPin, 0);
      analogWrite(ledRedPin, 250);
    }
    else{
      if(distance<minUltrasonicThreshold||distance>maxUltrasonicThreshold){
        analogWrite(ledGreenPin, 0);
        analogWrite(ledRedPin, 250);
      }
      else {
        analogWrite(ledRedPin, 0);
        analogWrite(ledGreenPin, 254);
      }
    }
  }
  else 
  {
    analogWrite(ledRedPin, redVal);
    analogWrite(ledGreenPin, greenVal);
    analogWrite(ledBluePin, blueVal);
  }
}
