
#include <TimerOne.h>

// Monitor
const boolean mon_en = true;
const boolean data_en = true;

// Pin int
const byte interruptPin = 2; // This can only be pin 2 or 3
volatile byte cnt = 0;

// Timed counter
volatile unsigned long ticks = 0;

const int timeArrSize = 22;
unsigned long timeArr[timeArrSize];


const int charArrSize = timeArrSize-1;
char charArr[charArrSize];

//#define BEO_ZERO    (3125 / TICK)
//#define BEO_SAME    (6250 / TICK)
//#define BEO_ONE     (9375 / TICK)
//#define BEO_STOP    (12500 / TICK)
//#define BEO_START   (15625 / TICK)

int err  = 0;
int link = 0;
int addr = 0;
int data = 0;


void setup() {

  // Serial port
  Serial.begin(9600);
  if (mon_en) {
    Serial.println("Welcome to Joachim's B&O IR codes receiver v2");
    Serial.println("");
  }
  
  // Setup pin interrupts
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), detectPin, RISING);

  // Setup timer and interrupt
  Timer1.initialize(625);
  Timer1.attachInterrupt(incTick);
}

void loop() {

  // Test if message received
  int cnt2 = cnt;
  if (cnt2 == 22) {
    noInterrupts();
    cnt = 0;

    // Print time differences
    if (mon_en) {
      Serial.print("Message: ");
      for (int i = 0; i < charArrSize; i++) {
        Serial.print(timeArr[i+1] - timeArr[i]);
        Serial.print(" ");
      }
      Serial.println("");
    }

    // Convert time difference to chars
    char lastChar = 'N';
    for (int i = 0; i < charArrSize; i++) {
      charArr[i] = findBit(timeArr[i+1] - timeArr[i], lastChar);
      lastChar = charArr[i];
    }

    // Print chars
    if (mon_en) {
      for (int i = 0; i < 21; i++) {
        Serial.print(charArr[i]);
      }
      Serial.println("");
    }
    
    // Search start of data 
    // If it has not found start of message in the first three chars 
    // the string can't have data
    int i = 0;
    boolean error = true;
    while (error) {
      if (charArr[i] == 'S')
        break;
      if (i > 3) {
        if (mon_en) {
          Serial.println("ERROR: Could not find start of message");
        }
        error = false;
        break;
      }
      i = i + 1;
    }
    i = i + 1;
    while (error) {
      if (mon_en) {
        Serial.print("Messeage starts at pos:");
        Serial.println(i);
      }
      // Check that message is pure '1' and '0'
      int res = 0;
      for (int j = i; j < i+17; j++) {
        if (charArr[j] == '1') {
          res = res << 1;
          res |= 1;
        } else if (charArr[j] == '0') {
          res = res << 1;
        } else {
          if (mon_en) {
            Serial.print("ERROR: Found not a number at pos: ");
            Serial.println(j);
          }
          error = false;
          break;
        }
      }
      //Serial.println(res);

      // Convert split data
      err = 0;
      link = res >> 9 & 0x01;
      addr = res >> 8 & 0xff;
      data = res & 0xff;
      if (mon_en || data_en) {
        Serial.print("link: ");
        Serial.print(link,HEX);
        Serial.print(", addr: ");
        Serial.print(addr,HEX);
        Serial.print(", data: ");
        Serial.print(data,HEX);
        Serial.println("");
        //Serial.println("");
      }

      // Do something with the newly required data
      // ......
      
      break;
    }
    if (error) {
      err = 1;
    }
    cnt = 0;
    interrupts();
  }
}

// Converting time difference to char
char findBit(unsigned long d, char lastChar) {
  if ( d >= 4 and d <= 6)   // ZERO
    return '0';
  if ( d >= 9 and d <= 11)  // SAME
    return lastChar;
  if ( d >= 14 and d <= 16) // ONE
    return '1';
  if ( d >= 19 and d <= 21) // STOP
    return 'E';
  if ( d >= 24 and d <= 26) // START
    return 'S';
  return 'F';               // FATAL
}

void detectPin() {
  noInterrupts();
  //timeArr[cnt] = millis();
  timeArr[cnt] = ticks;
  cnt = cnt + 1;
  interrupts();
}

void incTick(void) {
  noInterrupts();
  ticks = ticks + 1;
  interrupts();
}

