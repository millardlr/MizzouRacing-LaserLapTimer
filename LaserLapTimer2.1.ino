#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Wire.h>

#define laserInput               13
#define button1                  2
#define button2                  3
#define button3                  4
#define button4                  5
#define button5                  6
#define button6                  7
#define button7                  8
#define button8                  9
#define TIMEOUT_US               10000

Adafruit_7segment matrix = Adafruit_7segment();

int timeCentiseconds = 0;
unsigned long presentTime = 0;
//int baseTime = 0;
int button1State = 0;
int button2State = 0;
int button3State = 0;
int button4State = 0;
int button5State = 0;
int button6State = 0;
int button7State = 0;
int button8State = 0;
int delayState = 0;
int driverState = 0;
int displayState = 0;
int displayCounter = 1;
int lastDisplay = 0;
bool TIMER_STATE = 0;
bool lastLaserState = 1;
bool lastButton2State = 0;
bool lastButton3State = 0;
bool lastButton4State = 0;
bool lastButton5State = 0;
bool lastButton6State = 0;
bool lastButton7State = 0;

//char driverNames[8][] = {"AARON","BEN","JUSTIN","JACK","HAYDEN","ALEX","DROD","TEAM"};
char driver0[] = "AARON";
char driver1[] = "BEN";
char driver2[] = "JUSTIN";
char driver3[] = "JACK";
char driver4[] = "HAYDEN";
char driver5[] = "___1";
char driver6[] = "___2";
char driver7[] = "___3";
char driverName[] = "DRIVER NAME";


unsigned long lapTimesDriverPrime[32] = {0};
// unsigned long lapTimesDriver[8][32];
unsigned long lapTimesDriver0[32] = {0};
unsigned long lapTimesDriver1[32] = {0};
unsigned long lapTimesDriver2[32] = {0};
unsigned long lapTimesDriver3[32] = {0};
unsigned long lapTimesDriver4[32] = {0};
unsigned long lapTimesDriver5[32] = {0};
unsigned long lapTimesDriver6[32] = {0};
unsigned long lapTimesDriver7[32] = {0};
unsigned long lastCurrentTime[32] = {0};
int lapCounter[8] = {0};
//int lapCounter = 0;

void runTimer();
void timer();
void newLap();
void lapTimerReset();
void completeTimerReset();
void allDriverTimerReset();
void freezeTimer();
void lapDisplay();
bool timerState();
bool laserState();
int setDelay();
int setDisplay();
int setDriver();
void getButtonStates();

void runTimer(){
  //laserState();
  //Serial.println(lapTimes[lapCounter[driverState]]);
  //Serial.println(laserState());
  if(lapTimesDriverPrime[lapCounter[driverState]] == 0 && laserState() != 0){                                       //At startup waits for first laser break
    matrix.print("0000");
    //lastDisplay = 0000;
    matrix.writeDisplay();
  }
  else if(laserState() == 0 && lastLaserState == 1){                                        //At laser break, new lap
    lastLaserState = 0;
    newLap();
    //Serial.println("New lap called");
    timer();
    //Serial.println("Timer called");
    //Serial.println(lapTimesDriverPrime[lapCounter[driverState]]);
  }
  else if(laserState() != 0){                                                               //After laser break, set lastLaserState to seeing each other and continue timer
    if (lapTimesDriverPrime[lapCounter[driverState]] < 100){                                             //delay for both wheels to get through. If undertray will never break the lasers, could change laser states to acount for both wheels
      matrix.print(lapTimesDriverPrime[lapCounter[driverState]-1]);
      matrix.writeDisplay();
      delay(1000);                                                                          //Does create a permanent 1s delay
    }
    //lastLaserState = 1;
    timer();
  }
  else {                                                                                    //If no conditions are met, continue timer
    timer();
  }
  delay(10);
}

void timer(){
  presentTime = millis();
  timeCentiseconds = presentTime/10;
  lapTimesDriverPrime[lapCounter[driverState]] = timeCentiseconds - lastCurrentTime[lapCounter[driverState] - 1];

  if (lapTimesDriverPrime[lapCounter[driverState]] < (delayState * 100) && lapCounter[driverState] > 1){
    matrix.print(lapTimesDriverPrime[lapCounter[driverState]-1]);
    matrix.writeDisplay();
    //Serial.println(lapTimesDriverPrime[lapCounter[driverState]-1]);
  }
  else{
    matrix.print(lapTimesDriverPrime[lapCounter[driverState]]);
    matrix.writeDisplay();
    //Serial.println(lapTimesDriverPrime[lapCounter[driverState]]);
  }

  //Serial.println(timeCentiseconds);
  //Serial.println(lapTimesDriverPrime[lapCounter[driverState] - 1]);
  //Serial.println(lapTimesDriverPrime[lapCounter[driverState]]);
  //Serial.println(lapCounter[driverState]);

  delay(10);
}

void newLap(){
  Serial.print("New lap, driver ");
  Serial.println(driverState);
  presentTime = millis();
  timeCentiseconds = presentTime/10;
  lastCurrentTime[lapCounter[driverState]] = timeCentiseconds;
  //Serial.println(lastCurrentTime[lapCounter[driverState]]);
  Serial.print("Lap  ");
  Serial.print(lapCounter[driverState]);
  Serial.print(": ");
  Serial.println(lapTimesDriverPrime[lapCounter[driverState]]);
  if (lapTimesDriverPrime[lapCounter[driverState]] == 0 && lapCounter[driverState] > 0){                             //Timer calls lastCurrentTime[lapCounter - 1]
    lapCounter[driverState] -= 1;
    lastCurrentTime[lapCounter[driverState]] = timeCentiseconds;
  }
  lapCounter[driverState] += 1;
  Serial.print("lapCounter: ");
  Serial.println(lapCounter[driverState]);
  //Serial.println(lastCurrentTime[lapCounter[driverState]-1]);
  delay(10);                                                                    //delay to give the timer a chance to get above 0
}

void lapTimerReset(){                                                           //just resets the current lap time
  if (lastButton2State == 0){                                                   //currently only works when timer running
    lapTimesDriverPrime[lapCounter[driverState]] = 0;
    //lapCounter[driverState] -= 1;
    lastButton2State = 1;
    lastLaserState = 1;
    TIMER_STATE = 1;
    Serial.println("Lap reset");
    //Serial.println(lapCounter[driverState]);
    //Serial.println(lapTimesDriverPrime[lapCounter[driverState]]);
  }
}

void completeTimerReset(){                                                      //complete timer reset, as if the arduino just started up
  for (int i = 0; i < 32; i+=1){
    lapTimesDriverPrime[i] = {0};
  }
  lapCounter[driverState] = 0;
  displayCounter = 1;
  displayState = 0;
  lastLaserState = 1;
  TIMER_STATE = 1;
}

void allDriverTimerReset(){
  for (int i = 0; i < 32; i+=1){
    lapTimesDriverPrime[i] = {0};
    lapTimesDriver0[i] = {0};
    lapTimesDriver1[i] = {0};
    lapTimesDriver2[i] = {0};
    lapTimesDriver3[i] = {0};
    lapTimesDriver4[i] = {0};
    lapTimesDriver5[i] = {0};
    lapTimesDriver6[i] = {0};
    lapTimesDriver7[i] = {0};
  }
  for(int j = 0; j < 8; ++j){
    lapCounter[j] = {0};
  }
  driverState = 0;
  displayCounter = 1;
  displayState = 0;
  lastLaserState = 1;
  TIMER_STATE = 1;
  Serial.println(lapCounter[0]);
}

void freezeTimer(){
  TIMER_STATE = 0;
  matrix.print(lapTimesDriverPrime[lapCounter[driverState]]);
  matrix.writeDisplay();
  lastDisplay = lapTimesDriverPrime[lapCounter[driverState]];
}

void lapDisplay(){
  TIMER_STATE = 0;
  matrix.print(lapTimesDriverPrime[lapCounter[driverState]]);
  matrix.writeDisplay();
  lastDisplay = lapTimesDriverPrime[lapCounter[driverState]];
  if (lastButton4State == 0){                                         //Go to the next display state each time the button is pressed
    if (displayCounter > lapCounter[driverState]){
      displayCounter = 1;
    }
    if (displayCounter <= lapCounter[driverState]){
      matrix.print(displayCounter);
      matrix.writeDisplay();
      Serial.print("Lap ");
      Serial.print(displayCounter);
      Serial.print(": ");
      Serial.println(lapTimesDriverPrime[displayCounter]);
      delay(750);
      matrix.print(lapTimesDriverPrime[displayCounter]);
      matrix.writeDisplay();
      lastDisplay = lapTimesDriverPrime[displayCounter];
      if (displayState == 2){
        delay(1500);
      }
      ++displayCounter;
      if (displayCounter == 0){
        displayCounter = 1;
      }
      //Serial.println(displayCounter);
    }
    lastButton4State = 1;
  }
}

bool timerState(){                                                   //Timer freeze or run
  if (lastButton5State == 0){
    if (TIMER_STATE == 0){                                           //Not starting new lap right
      TIMER_STATE = 1;
      lapTimesDriverPrime[lapCounter[driverState]] = 0;
    }
    else{
      displayState = 0;
      freezeTimer();
    }
    Serial.println(lapCounter[driverState]);
    Serial.println(lapTimesDriverPrime[lapCounter[driverState]]);
    Serial.println(lastLaserState);
    lastButton5State = 1;
  }
  return;
}

bool laserState(){
  long duration = pulseIn(laserInput, LOW, TIMEOUT_US);
  if (duration != 0){
    lastLaserState = 1;
  }
  return duration;
}

int setDisplay(){
  if (lastButton6State == 0){                                         //Choose between displaying loop of laps or just last lap
    switch (displayState){
      case 0:
        displayState = 1;
        break;
      case 1:
        displayState = 2;
        break;
      default:
        displayState = 0;
        displayCounter = 1;
    }
    lastButton6State = 1;
    Serial.println(displayState);
  }
}

int setDelay(){
  //lapTimesDriverPrime[lapCounter[driverState]] = 0;
  if (lastButton3State == 0){                                         //Go to the next display state each time the button is pressed
    switch (delayState){
      case 0:
        delayState = 1;
        break;
      case 1:
        delayState = 2;
        break;
      case 2:
        delayState = 3;
        break;
      case 3:
        delayState = 5;
        break;
      default:
        delayState = 0;
    }
    lastButton3State = 1;
    Serial.println(delayState);
    matrix.print(delayState);
    matrix.writeDisplay();
    delay(750);
  }
  //Serial.println(lapCounter[driverState]);
}

int setDriver(){
  if (lastButton7State == 0){                                         //Go to the next display state each time the button is pressed
    matrix.println();
    switch (driverState){
      case 0:
        driverState = 1;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver0[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver1[i];
        }
        Serial.println(driver1);
        matrix.println(driver1);                                      //Displays driver name
        matrix.writeDisplay();
        //delay(750);
        break;
      case 1:
        driverState = 2;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver1[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver2[i];
        }
        Serial.println(driver2);
        matrix.println(driver2);
        matrix.writeDisplay();
        //delay(750);
        break;
      case 2:
        driverState = 3;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver2[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver3[i];
        }
        Serial.println(driver3);
        matrix.println(driver3);
        matrix.writeDisplay();
        break;
      case 3:
        driverState = 4;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver3[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver4[i];
        }
        Serial.println(driver4);
        matrix.println(driver4);
        matrix.writeDisplay();
        break;
      case 4:
        driverState = 5;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver4[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver5[i];
        }
        Serial.println(driver5);
        matrix.println(driver5);
        matrix.writeDisplay();
        //delay(750);
        break;
      case 5:
        driverState = 6;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver5[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver6[i];
        }
        Serial.println(driver6);
        matrix.println(driver6);
        matrix.writeDisplay();
        break;
      case 6:
        driverState = 7;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver6[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver7[i];
        }
        Serial.println(driver7);
        matrix.println(driver7);
        matrix.writeDisplay();
        break;
      default:
        driverState = 0;
        for (int i = 0; i < 32; i+=1){
          lapTimesDriver7[i] = lapTimesDriverPrime[i];
        }
        for (int i = 0; i < 32; i+=1){
          lapTimesDriverPrime[i] = lapTimesDriver0[i];
        }
        Serial.println(driver0);
        matrix.println(driver0);
        matrix.writeDisplay();
    }
    lastButton3State = 1;
    Serial.print("Driver: ");
    Serial.println(driverState);
    delay(1000);
    matrix.println();
    matrix.writeDisplay();
  }
  //Serial.println(lapCounter[driverState]);
}

void getButtonStates(){
  button1State = digitalRead(button1);
  button2State = digitalRead(button2);
  button3State = digitalRead(button3);
  button4State = digitalRead(button4);
  button5State = digitalRead(button5);
  button6State = digitalRead(button6);
  button7State = digitalRead(button7);
  button8State = digitalRead(button8);
  while (button1State == HIGH) {                                    //Complete Reset
    completeTimerReset();
    //Serial.println("Button 1");
    button1State = digitalRead(button1);
  }
  while (button2State == HIGH) {                                    //Current lap reset
    lapTimerReset();
    //Serial.println("Button 2");
    button2State = digitalRead(button2);
  }
  if (button2State == LOW){                                         //Setting delay
    lastButton2State = 0;
  }
  while (button3State == HIGH) {
    setDelay();
    //Serial.println("Button 3");
    button3State = digitalRead(button3);
  }
  if (button3State == LOW){
    lastButton3State = 0;
  }
  while (button4State == HIGH && TIMER_STATE == 0) {                //display laps
    lapDisplay();
    //Serial.println("Button 4");
    button4State = digitalRead(button4);
  }
  if (button4State == LOW){
    lastButton4State = 0;
  }
  while (button5State == HIGH) {                                    //Timer On/Off
    timerState();
    //Serial.println("Button 5");
    button5State = digitalRead(button5);
  }
  if (button5State == LOW){                                         //Freeze Timer
    lastButton5State = 0;
  }
  while (button6State == HIGH && TIMER_STATE == 0) {
    setDisplay();
    //Serial.println("Button 6");
    button6State = digitalRead(button6);
  }
  if (button6State == LOW){                                         //Freeze Timer
    lastButton6State = 0;
  }
  while (button7State == HIGH && TIMER_STATE == 0) {
    setDriver();
    //Serial.println("Button 7");
    button7State = digitalRead(button7);
  }
  if (button7State == LOW){
    lastButton7State = 0;
  }
  while (button8State == HIGH) {                                    //Complete Reset
    allDriverTimerReset();
    Serial.println("Button 8");
    button8State = digitalRead(button8);
  }
  delay(10);
}

void setup() {                                     //Initialize serial monitor, laser, buttons, and display
  Serial.begin(9600);
  pinMode(laserInput, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
  pinMode(button6, INPUT);
  pinMode(button7, INPUT);
  matrix.begin(0x70);
  //matrix.setBrightness(15);
  delay(2000);                                      //Delay 2 seconds because it likes to be weird when it starts up
}

void loop() {
  getButtonStates();
  if (lapCounter[driverState] < 32 && TIMER_STATE == 1){
    runTimer();
  }
  else if (TIMER_STATE == 0){                                     //took out (&& lapTimesDriverPrime[lapCounter[driverState]] > 0)
    if (displayState == 0){                                       //Displays last lap time
      matrix.print(lastDisplay);
      matrix.writeDisplay();
    }
    else if (displayState == 1){
      if (laserState() == 0){
        matrix.println(0);
        matrix.writeDisplay();
        //Serial.println(laserState());
      }
      else{
        matrix.println(1);                                 //Displays 0 if laser beam is broken, 1 if not
        matrix.writeDisplay();
        //Serial.println(laserState());
      }
    }
    else {
      lapDisplay();
    }
  }
}
