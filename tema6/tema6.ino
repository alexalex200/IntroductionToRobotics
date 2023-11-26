#include "LedControl.h"                                 // need the library
const byte dinPin = 12;                                 // pin 12 is connected to the MAX7219 pin 1
const byte clockPin = 11;                               // pin 11 is connected to the CLK pin 13
const byte loadPin = 10;                                // pin 10 is connected to LOAD pin 12
const byte matrixSize = 8;                              // 1 as we are only using 1 MAX7219
LedControl lc = LedControl(dinPin, clockPin, loadPin);  //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;

byte matrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

byte matrix2[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

const uint64_t BOAT_IMAGES[] = {
  0x0000010000000000,
  0x0001030001000000,
  0x0103070003010000,
  0x03070f0007030100,
  0x070f1f010f070301,
  0x0f1f3f021e0e0602,
  0x1e3f7f043c1c0c04,
  0x3c7eff0878381808
};

const int BOAT_IMAGES_LEN = sizeof(BOAT_IMAGES) / 8;

const uint64_t WIN_IMAGES[] = {
  0x7e00000000000000,
  0xbd7e000000000000,
  0x7ebd7e0000000000,
  0x3c7ebd7e00000000,
  0x183c7ebd7e000000,
  0x18183c7ebd7e0000,
  0x3c18183c7ebd7e00
};
const int WIN_IMAGES_LEN = sizeof(WIN_IMAGES)/8;


byte semAnimation1 = 1,semAnimation2=0;

int indexAnimation = 0;

const byte boatL[3] = { 1, 2, 3 };
byte nrBoat[3] = { 4, 3, 2 };
byte boatsDestroyed=0;
int rotation = 0, indexL = 2, indexNr = nrBoat[indexL];

byte cursorX = 0;
byte cursorY = 0;
unsigned long lastMove = 0;
int moveDelay = 500;

const int pinX = A0;  // A0 - analog pin connected to X output
const int pinY = A1;  // A1 - analog pin connected to Y output
const int pinSW = 2;  // digital pin connected to switch output
const int pinB = 4;
unsigned long lastSwState = 0, lastBState = 0;

byte swState = LOW, bState = LOW;

int xValue = 0;
int yValue = 0;

int minThreshold = 400;
int maxThreshold = 600;

byte blinkPlayer = false, blinkBoat = false;
unsigned long blinkPlayerTime = 0, blinkBoatTime = 0, animationTime = 0;
const int playerBlinkTime = 100, bombBlinkTime = 200, boatBlinkTime = 300, buttonDelay = 1000, animationDelay = 200;

byte semGenRandomMatrix = 1;

byte mode = 0;

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}

byte boatDestroyed(int x, int y) {
  int poz = 0;
  while (matrix2[x + poz][y] != 2 && matrix2[x + poz][y] != 4 && x + poz < matrixSize) {
    if (matrix2[x + poz][y] == 1)
      return 0;
    poz++;
  }
  poz = 0;
  while (matrix2[x - poz][y] != 2 && matrix2[x - poz][y] != 4 && x - poz >= 0) {
    if (matrix2[x - poz][y] == 1)
      return 0;
    poz++;
  }
  poz = 0;
  while (matrix2[x][y + poz] != 2 && matrix2[x][y + poz] != 4 && y + poz < matrixSize) {
    if (matrix2[x][y + poz] == 1)
      return 0;
    poz++;
  }
  poz = 0;
  while (matrix2[x][y - poz] != 2 && matrix2[x][y - poz] != 4 && y - poz >= 0) {
    if (matrix2[x][y - poz] == 1)
      return 0;
    poz++;
  }
  return 1;
}
byte possiblePoz(int x, int y) {
  if (x < 0 || x >= matrixSize || y < 0 || y >= matrixSize)
    return 0;
  for (int i = 0; i < boatL[indexL]; i++) {
    if ((!rotation && x + i >= matrixSize) || (rotation && y + i >= matrixSize))
      return 0;
  }
  return 1;
}

byte drawCursor(int x, int y) {
  if ((abs(x - cursorX) + abs(y - cursorY)) != 1)
    return 0;
  return 1;
}

byte drawBoat(int cursorX, int cursorY, int x, int y) {
  if ((rotation && y >= cursorY && y < cursorY + boatL[indexL] && x == cursorX) || (!rotation && x >= cursorX && x < cursorX + boatL[indexL] && y == cursorY)) {
    return 1;
  }
  return 0;
}

byte setBoat(byte matrix[matrixSize][matrixSize], int x, int y) {
  for (int i = 0; i < boatL[indexL]; i++)
    if ((!rotation && matrix[x + i][y]) || (rotation && matrix[x][y + i]) || x + i >= matrixSize || y + i >= matrixSize)
      return 0;
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++)
      if (drawBoat(x, y, i, j))
        matrix[i][j] = 1;
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      if (!matrix[i][j]) {
        for (int k = max(0, i - 1); k <= min(i + 1, matrixSize - 1); k++)
          for (int l = max(0, j - 1); l <= min(j + 1, matrixSize - 1); l++)
            if (matrix[k][l] == 1)
              matrix[i][j] = 2;
      }
    }
  return 1;
}

void randomMatrix(byte matrix[matrixSize][matrixSize]) {
  indexL = 2;
  indexNr = nrBoat[indexL];

  byte cursorRandomX;
  byte cursorRandomY;

  byte sem = 1;
  while (sem) {
    Serial.println(indexL);
    cursorRandomX = random(0, matrixSize);
    cursorRandomY = random(0, matrixSize);
    rotation = random(0, 2);

    if (setBoat(matrix, cursorRandomX, cursorRandomY)) {
      for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++)
          Serial.print(matrix[i][j]);
        Serial.println();
      }
      Serial.println();
      indexNr--;
      if (indexL == 0 && indexNr == 0)
        sem = 0;
      if (indexNr == 0 && indexL != 0) {
        indexL--;
        indexNr = nrBoat[indexL];
      }
    }
  }
}
void setup() {
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // clear screen
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  randomSeed(analogRead(5));
  Serial.begin(9600);
}

void loop() {

  if (semAnimation1) {
    displayImage(BOAT_IMAGES[indexAnimation]);
    if (millis() - animationTime > animationDelay) {
      indexAnimation++;
      if (indexAnimation >= BOAT_IMAGES_LEN)
        indexAnimation=0;
      
      if (bState == true && millis() - lastBState > buttonDelay) {
        semAnimation1=0;
        indexAnimation=0;
        mode=2;
        lastBState = millis();
      }
      animationTime = millis();
    }
  }

  if (semAnimation2) {
    displayImage(WIN_IMAGES[indexAnimation]);
    if (millis() - animationTime > animationDelay) {
      indexAnimation++;
      if (indexAnimation >= WIN_IMAGES_LEN)
        indexAnimation=0;
      
      if (bState == true && millis() - lastBState > buttonDelay) {
        semAnimation1=1;
        semAnimation2=0;
        mode=0;
        lastBState = millis();
      }
      animationTime = millis();
    }
  }

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  swState = !digitalRead(pinSW);
  bState = !digitalRead(pinB);

  if (xValue < minThreshold && millis() - lastMove > moveDelay) {
    if (possiblePoz(cursorX, cursorY - 1)) {
      cursorY = cursorY - 1;
      lastMove = millis();
    }
  }
  if (xValue > maxThreshold && millis() - lastMove > moveDelay) {
    if (possiblePoz(cursorX, cursorY + 1)) {
      cursorY = cursorY + 1;
      lastMove = millis();
    }
  }
  if (yValue < minThreshold && millis() - lastMove > moveDelay) {
    if (possiblePoz(cursorX - 1, cursorY)) {
      cursorX = cursorX - 1;
      lastMove = millis();
    }
  }
  if (yValue > maxThreshold && millis() - lastMove > moveDelay) {
    if (possiblePoz(cursorX + 1, cursorY)) {
      cursorX = cursorX + 1;
      lastMove = millis();
    }
  }

  if (mode == 1) {

    if (bState == true && millis() - lastBState > buttonDelay) {
      if (setBoat(matrix, cursorX, cursorY)) {
        cursorX = 0;
        cursorY = 0;

        indexNr--;
        if (indexNr == 0 && indexL != 0) {
          indexL--;
          indexNr = nrBoat[indexL];
        }

        if (indexL == 0 && indexNr == 0) {
          mode = 2;
          for (int row = 0; row < matrixSize; row++)
            for (int col = 0; col < matrixSize; col++)
              lc.setLed(0, row, col, false);
        }
      }


      lastBState = millis();
    }

    if (swState == true && millis() - lastSwState > buttonDelay) {
      rotation = !rotation;
      if (!possiblePoz(cursorX, cursorY))
        rotation = !rotation;
      lastSwState = millis();
    }
  }

  if (mode == 2) {
    if (semGenRandomMatrix) {
      semGenRandomMatrix = 0;
      cursorX = 0;
      cursorY = 0;
      randomMatrix(matrix2);
    }

    if (bState == true && millis() - lastBState > buttonDelay) {
      if (matrix2[cursorX][cursorY] == 1) {
        matrix2[cursorX][cursorY] = 3;
        if (boatDestroyed(cursorX, cursorY)) {
          boatsDestroyed++;
          for (int i = 0; i < matrixSize; i++)
            for (int j = 0; j < matrixSize; j++) {
              if (matrix2[i][j] == 2) {
                for (int k = max(0, i - 1); k <= min(i + 1, matrixSize - 1); k++)
                  for (int l = max(0, j - 1); l <= min(j + 1, matrixSize - 1); l++)
                    if (matrix2[k][l] == 3)
                      matrix2[i][j] = 4;
              }
            }
        }
      } else
        matrix2[cursorX][cursorY] = 4;
      if(boatsDestroyed==9){
        boatsDestroyed=0;
        semAnimation2=1;
        mode=0;
        for (int row=0;row<matrixSize;row++)
            for(int col=0;col<matrixSize;col++)
                matrix2[row][col]=0;
        cursorX=0;
        cursorY=0;
      }
      lastBState = millis();
    }
  }

  for (int row = 0; row < matrixSize; row++)
    for (int col = 0; col < matrixSize; col++) {

      if (mode == 1) {
        if (matrix[row][col] == 1)
          lc.setLed(0, row, col, true);
        else
          lc.setLed(0, row, col, false);

        if (drawBoat(cursorX, cursorY, row, col)) {
          if (blinkBoat)
            lc.setLed(0, row, col, true);
          else
            lc.setLed(0, row, col, false);
        }
      }

      if (mode == 2) {
        if (row == cursorX && col == cursorY) {
          if (blinkPlayer)
            lc.setLed(0, row, col, true);
          else
            lc.setLed(0, row, col, false);
        } else {
          lc.setLed(0, row, col, false);
          if (matrix2[row][col] == 3) {
            if (blinkBoat)
              lc.setLed(0, row, col, true);
            else
              lc.setLed(0, row, col, false);
          }
          if (matrix2[row][col] == 4)
            lc.setLed(0, row, col, true);
        }
      }
    }
  if (millis() - blinkPlayerTime > playerBlinkTime) {
    blinkPlayer = !blinkPlayer;
    blinkPlayerTime = millis();
  }

  if (millis() - blinkBoatTime > boatBlinkTime) {
    blinkBoat = !blinkBoat;
    blinkBoatTime = millis();
  }
}