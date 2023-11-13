// Pin assignments for the shift register
const int latchPin = 11;  // Pin connected to STCP of the shift register (Latch control)
const int clockPin = 12;  // Pin connected to SHCP of the shift register (Clock)
const int dataPin = 10;   // Pin connected to DS of the shift register (Data input)
// Pin assignments for controlling the common cathode/anode pins of the 7-segment digits
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

const int butonStartStopPin = 9;
const int butonResetPin = 2;
const int butonLapPin = 3;

byte butonLapState = 0, butonStartStopState = 0;

volatile unsigned long lastInterruptTime = 0;
// Debounce time in milliseconds
const unsigned long debounceDelay = 200;

unsigned long time = 0, timeCopy = 0, displayLapTimeStart = 0;
unsigned long timeClickStartStop = 0;
unsigned long lastPressTime = 0;
const int nrOfSimbols = 14, nrOfSegments = 8, displayLapTime = 1000;

const int nrMaxLaps = 4;
unsigned long lapTimes[nrMaxLaps];
int nrLapsSaved = 0, lapDisplayed = -1;
byte lapDisplayState = 0;
byte encodingArray[nrOfSimbols][nrOfSegments] = {
  // A B C D E F G DP
  { 1, 1, 1, 1, 1, 1, 0, 0 },  // 0
  { 0, 1, 1, 0, 0, 0, 0, 0 },  // 1
  { 1, 1, 0, 1, 1, 0, 1, 0 },  // 2
  { 1, 1, 1, 1, 0, 0, 1, 0 },  // 3
  { 0, 1, 1, 0, 0, 1, 1, 0 },  // 4
  { 1, 0, 1, 1, 0, 1, 1, 0 },  // 5
  { 1, 0, 1, 1, 1, 1, 1, 0 },  // 6
  { 1, 1, 1, 0, 0, 0, 0, 0 },  // 7
  { 1, 1, 1, 1, 1, 1, 1, 0 },  // 8
  { 1, 1, 1, 1, 0, 1, 1, 0 },  // 9
  { 1, 1, 1, 0, 1, 1, 1, 0 },  // A
  { 0, 0, 0, 1, 1, 1, 0, 0 },  // L
  { 1, 1, 0, 0, 1, 1, 1, 0 },  // P
  { 0, 0, 0, 0, 0, 0, 0, 0 },  // NULL
};
// Size of the register in bits
const byte regSize = 8;
// Array to keep track of the digit control pins
int displayDigits[] = {
  segD1, segD2, segD3, segD4
};
const int displayCount = 4;  // Number of digits in the display
// Array representing the state of each bit in the shift register
byte registers[regSize];
void setup() {
  // Initialize the digital pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(butonStartStopPin, INPUT_PULLUP);
  pinMode(butonResetPin, INPUT_PULLUP);
  pinMode(butonLapPin, INPUT_PULLUP);

  // Initialize the digit control pins as outputs and turn them off
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  // Decomment any of the following lines to turn off the specific digit.
  digitalWrite(displayDigits[0], HIGH);
  digitalWrite(displayDigits[1], HIGH);
  digitalWrite(displayDigits[2], HIGH);
  digitalWrite(displayDigits[3], HIGH);

  attachInterrupt(digitalPinToInterrupt(butonResetPin), handleInterruptReset, FALLING);
  attachInterrupt(digitalPinToInterrupt(butonLapPin), handleInterruptLap, FALLING);

  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  int reading = !digitalRead(butonStartStopPin);

  if (reading == HIGH && millis() - lastPressTime > debounceDelay && !lapDisplayState) {
    butonStartStopState = !butonStartStopState;
    if (butonStartStopState)
      timeClickStartStop = (millis() / 100) - time;
    lastPressTime = millis();
  }

  if (butonStartStopState) {
    time = (millis() / 100) - timeClickStartStop;
    timeCopy = time;
    lapDisplayState = 0;
  }
  if (butonLapState) {
    if (butonStartStopState) {
      for (int i = nrMaxLaps - 1; i > 0; i--)
        lapTimes[i] = lapTimes[i - 1];
      lapTimes[0] = time;
      nrLapsSaved++;
      if (nrLapsSaved > nrMaxLaps)
        nrLapsSaved--;
      lapDisplayed = nrLapsSaved-1;
    } else{
      lapDisplayed--;
      if (lapDisplayed < 0)
        lapDisplayed = nrLapsSaved - 1;
    }
    butonLapState = 0;
  }

  if (!lapDisplayState) {
    WriteTime(time, 0);
  } else {
    WriteTime(lapTimes[lapDisplayed], nrLapsSaved - lapDisplayed);
  }
}

void handleInterruptLap() {
  static unsigned long interruptTime = 0;  // Timestamp for the current interrupt, retains its value between ISR calls
  interruptTime = micros();                // Capture the current time in microseconds
  // Debounce logic: If interrupts come in faster succession than the debounce delay, they're ignored
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {  // Convert debounceDelay to microseconds for comparison
    butonLapState = 1;
    displayLapTimeStart = millis();
  }
  // Update the last interrupt timestamp
  lastInterruptTime = interruptTime;
}

void handleInterruptReset() {
  static unsigned long interruptTime = 0;  // Timestamp for the current interrupt, retains its value between ISR calls
  interruptTime = micros();                // Capture the current time in microseconds
  // Debounce logic: If interrupts come in faster succession than the debounce delay, they're ignored
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {  // Convert debounceDelay to microseconds for comparison
    if (butonStartStopState == 0){
      time = 0;
      if(lapDisplayState==0&&nrLapsSaved){
        lapDisplayState = 1;
        displayLapTimeStart = millis();
      }
      else
      {
        for (int i = 0; i < nrMaxLaps; i++)
        lapTimes[i] = 0;
        nrLapsSaved = 0;
        lapDisplayState = 0;
      }
    }
  }
  // Update the last interrupt timestamp
  lastInterruptTime = interruptTime;
}

void WriteTime(unsigned long time, int lap) {
  int timeDigits[displayCount];

  if (lap && millis() - displayLapTimeStart < displayLapTime) {
    timeDigits[3] = lap;
    timeDigits[2] = 12;
    timeDigits[1] = 10;
    timeDigits[0] = 11;
  } else {
    timeDigits[3] = time % 10;
    timeDigits[2] = (time / 10) % 60 % 10;
    timeDigits[1] = (time / 10) % 60 / 10;
    timeDigits[0] = ((time / 10) / 60) % 10;
  }

  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], LOW);
    WriteSimbol(timeDigits[i], (i == 2 || i == 0) ? 1 : 0);
    delay(5);
    WriteSimbol(13, 0);
    digitalWrite(displayDigits[i], HIGH);
  }
}
void WriteSimbol(int simbol, byte dp) {

  digitalWrite(latchPin, LOW);                                             // Begin sending data to the shift register
  for (int i = 0; i < 8; i++) {                                            // Loop over each bit
    digitalWrite(clockPin, LOW);                                           // Prepare to send the bit
    digitalWrite(dataPin, (i == 7 && dp) ? 1 : encodingArray[simbol][i]);  // Send the bit
    digitalWrite(clockPin, HIGH);                                          // Clock in the bit
  }
  digitalWrite(latchPin, HIGH);  // Latch the data, updating the output of the shift register
}
