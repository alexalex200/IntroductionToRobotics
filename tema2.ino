#include "pitches.h" 

const int butoaneNivel[3] = {2, 4, 6} , ledNivel[3] = {8, 10, 12} , ledActivitate = 13 , buzzer = 7;

byte butoaneState[3] = {0, 0, 0}, reading[3] = {0, 0, 0}, lastReading[3] = {0, 0, 0}, ledActivitateState = 1;

int nivelCurent = 0  , destinatie = -1 , directie, fr = 1800, debounceDelay = 50 ,coada[10];

unsigned long lastDebounceTime[3] = {0, 0, 0}, prevTimeLedActivitate = 0, CurentTimeLedActivitate = 0, prevTimeLedNivel = 0, CurentTimeLedNivel = 0,t=0,timeclick=0;

int melody[] = {
  REST, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST, REST, NOTE_DS4,
  
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_G4, NOTE_B4, NOTE_E5,
  NOTE_DS5,   
  NOTE_D5, REST, REST, NOTE_DS4, 
  NOTE_E4, REST, NOTE_FS4, NOTE_G4, REST, NOTE_DS4,
  NOTE_E4, NOTE_FS4,  NOTE_G4, NOTE_C5, NOTE_B4, NOTE_E4, NOTE_G4, NOTE_B4,   
  
  NOTE_AS4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, 
  NOTE_E4, REST,
  REST, NOTE_E5, NOTE_D5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4,
  NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4, NOTE_AS4, NOTE_A4,   
  NOTE_G4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4
};

int durations[] = {
  2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  2, 16, 16, 16, 16, 
  2, 4, 8, 4,
  
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,
  1,   
  2, 4, 8, 8, 
  4, 8, 8, 4, 8, 8,
  8, 8,  8, 8, 8, 8, 8, 8,   
  
  2, 16, 16, 16, 16, 
  4, 4,
  4, 8, 8, 8, 8, 8, 8,
  16, 8, 16, 8, 16, 8, 16, 8,   
  16, 16, 16, 16, 16, 2
};

int size = sizeof(durations) / sizeof(int),note=size;

void setup() {

  pinMode(butoaneNivel[0], INPUT_PULLUP);
  pinMode(butoaneNivel[1], INPUT_PULLUP);
  pinMode(butoaneNivel[2], INPUT_PULLUP);

  pinMode(ledNivel[0], OUTPUT);
  pinMode(ledNivel[1], OUTPUT);
  pinMode(ledNivel[2], OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(ledActivitate, OUTPUT);

  Serial.begin(9600);
}

void inchisdeschis(int d)
{
  int cfr = fr - (d * 200), t1 = millis(), t2 = millis();
  while (cfr != fr)
  {
    t2 = millis();
    tone(buzzer, cfr);
    if (t2 - t1 > 5) {
      cfr += d;
      t1 = t2;
    }
  }
  noTone(buzzer);
}
  
void loop() {
  
  if(note<size)
  {
    int duration = 1000 / durations[note];
    tone(buzzer, melody[note], duration);
    int pauseBetweenNotes = duration * 1.30;
    if(millis()-t>pauseBetweenNotes){
      note++;
      t=millis();
    }
  }

  reading[0] = !digitalRead(butoaneNivel[0]);
  if (reading[0] != lastReading[0]) {
    lastDebounceTime[0] = millis();
  }
  if ((millis() - lastDebounceTime[0]) > debounceDelay) {
    if (reading[0] != butoaneState[0]) {
      butoaneState[0] = reading[0];
    }
  }
  lastReading[0] = reading[0];


  reading[1] = !digitalRead(butoaneNivel[1]);
  if (reading[1] != lastReading[1]) {
    lastDebounceTime[1] = millis();
  }
  if ((millis() - lastDebounceTime[1]) > debounceDelay) {
    if (reading[1] != butoaneState[1]) {
      butoaneState[1] = reading[1];
    }
  }
  lastReading[1] = reading[1];


  reading[2] = !digitalRead(butoaneNivel[2]);
  if (reading[2] != lastReading[2]) {
    lastDebounceTime[2] = millis();
  }
  if ((millis() - lastDebounceTime[2]) > debounceDelay) {
    if (reading[2] != butoaneState[2]) {
      butoaneState[2] = reading[2];
    }
  }
  lastReading[2] = reading[2];

  for (int i = 0; i < 3; i++)
  {
    if (butoaneState[i] && i != nivelCurent) {
      if(destinatie==-1){
        destinatie = i;
        directie = (nivelCurent < destinatie) ? 1 : -1;
        prevTimeLedNivel = millis();
        inchisdeschis(-1);
        note=0;
      }
    }
  }

  digitalWrite(ledNivel[nivelCurent], HIGH);
  digitalWrite(ledActivitate, ledActivitateState);
  if (destinatie != -1)
  {
    CurentTimeLedActivitate = millis();
    if (CurentTimeLedActivitate - prevTimeLedActivitate > 500) {
      ledActivitateState = !ledActivitateState;
      prevTimeLedActivitate = CurentTimeLedActivitate;
    }

    CurentTimeLedNivel = millis();

    if (CurentTimeLedNivel - prevTimeLedNivel > 5000)
    {
      prevTimeLedNivel = CurentTimeLedNivel;
      digitalWrite(ledNivel[nivelCurent], LOW);

      nivelCurent = nivelCurent + directie;

      digitalWrite(ledNivel[nivelCurent], HIGH);
    }
    if (nivelCurent == destinatie) {
      destinatie = -1;
      ledActivitateState=1;
      note=size;
      timeclick=millis();
      while(millis()-timeclick<1000)
      {          
        tone(buzzer, 2000);
      }
      noTone(buzzer);
    } 
  }
}
