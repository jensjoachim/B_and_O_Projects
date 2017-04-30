
#include <TimerOne.h>

// Timing
int MLCPeriod = 3124;
int MLCPeriodDiv4 = MLCPeriod/4;

// Digital ports
int dataPin = 2;

// Streams
const int seq1Size = 9;   // Amp OFF
unsigned seq1[] = {0x00f8,0x1002,0x0080,0x00b8,0x0080,0x1034,0x00c1,0x1007,0x00c1};

const int seq2Size = 25;  // Amp ON
unsigned seq2[] = {0x00f4,0x1002,0x00f8,0x00f8,0x00f8,0x00f8,0x0080,0x103d,0x00b7,0x1007,0x00b7,0x103e,0x00fa,0x1002,0x00f8,0x00f8,0x00f8,0x00f8,0x0098,0x103d,0x00f8,0x1002,0x00a0,0x00b8,0x00a0};


// Interrupt variable
volatile int tick = 0;

// Sound sense
int senseR = A0;
int senseL = A1;
int valR = 0;
int valL = 0;

int sleepTime = 60*1;// in s
int sleepTimeCnt = 0;// in s
long cntIn = 0;

int waitTime = 5; // in ms
int forCycles = 200;

int ampStateOn = 0;

boolean enMon = false;
//boolean enMon = true;

void setup() {
  
  // General
  // Serial port
  if (enMon == true) {
	  Serial.begin(9600);
	  Serial.println("Welcome to Joachim's automaitc standby sound dection on TAPE 2 interface on the TV/AUX port");
  }

  // B&O I/O's
  pinMode(dataPin,INPUT_PULLUP);

  // Setup timer and interrupt
  Timer1.initialize(MLCPeriodDiv4);
  Timer1.attachInterrupt(toggleTick);
}

void loop() {

  cntIn = 0;
    
  for(int i = 0; i < forCycles; i++) {
    // Read sensors
    valR = analogRead(senseR);
    delay(waitTime);
    valL = analogRead(senseL);
    cntIn = cntIn + valR + valL;
  }
    
  if (cntIn < 500 ) { //|| cntIn > 1500) {
    if (sleepTimeCnt == 0) {
      //digitalWrite(relayPin, relayOff);
      if (ampStateOn == 1) {
        sendCmd(seq1,seq1Size);
        delay(100);
        sendCmd(seq1,seq1Size);
        if (enMon == true) {
          Serial.println("OFF!");
        }
        ampStateOn = 0;
      }
      
    } else {
      sleepTimeCnt = sleepTimeCnt - 1;
    }
  } else {
    //digitalWrite(relayPin, relayOn);
    if (ampStateOn == 0) {
      sendCmd(seq2,seq2Size);
      delay(100);
      sendCmd(seq2,seq2Size);
      if (enMon == true) {
        Serial.println("ON!");
      }
      ampStateOn = 1;
    }
    
    sleepTimeCnt = sleepTime;
  } 
  
  if (enMon == true) {
    Serial.print(cntIn);
    Serial.print("    ");
    Serial.print(sleepTimeCnt);
    Serial.println("");
  }
    
  /*
  sendCmd(seq2,seq2Size);
  delay(100);
  sendCmd(seq2,seq2Size);
  
  delay(10000);

  sendCmd(seq1,seq1Size);
  delay(1000);
  sendCmd(seq1,seq1Size);
  
  delay(10000);
  */
}

void sendCmd(unsigned arr[],int arrSize) {
  // Make the pin output
  pinMode(dataPin,OUTPUT);    
  digitalWrite(dataPin,1);

  int cmd = 0;
  // Read commands one after one
  for (int i = 0; i < arrSize; i++) {
    cmd = arr[i];
    if (cmd > 0x0fff) { // Idle command
      cmd = cmd & 0x0fff;
      while (cmd) {
        waitTicks(5);
        digitalWrite(dataPin,1);
        cmd = cmd - 1;
      }
      waitTicks(5);
      digitalWrite(dataPin,1);
        
    } else {            // Data commands    
      for (int j = 0; j < 8; j++) {
        waitTicks(5);
        if (cmd & 0x0080) {
          digitalWrite(dataPin,0);
        } else {
          digitalWrite(dataPin,1);
        }
        cmd = cmd << 1;
      }   
    }
  }
  // Stop
  waitTicks(5);
  digitalWrite(dataPin,1);
  // Use pullup so that the other can also send commands
  pinMode(dataPin,INPUT_PULLUP);
}

void waitTicks(int n) {
  int tick_copy;
  noInterrupts();
  tick_copy = tick;
  interrupts();
  int next_tick;
  next_tick = tick_copy;
  for (int j = 1; j < n; j++) {
    while (next_tick == tick_copy) {
      noInterrupts();
      tick_copy = tick;
      interrupts();
    }
    next_tick = next_tick ^ 1;
  }
}

void toggleTick(void) {
  tick = tick ^ 1;
}

