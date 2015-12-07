/*
 *  CS 207 Bianry CLock
    Joey Tiefenbach
    December 7 
 *
 * Reference TimeSerial.pde
 * example code illustrating Time library set through serial port messages.
 *
 * Messages consist of the letter T followed by ten digit time (as seconds since Jan 1 1970)
 * you can send the text on the next line using Serial Monitor to set the clock in any number
 *
 *   T1448852160 2:56:00
 *   T1421638195 3:29:55
 *   T1421636395 2:59:55
 * A Processing example sketch to automatically send the messages is included in the download
 * On Linux, you can use "date +T%s\n > /dev/ttyACM0" (UTC time zone)
 */

#include <Time.h>
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
#define NOTE_E4  330
#define NOTE_GS4 415
#define NOTE_FS5 740
#define NOTE_B3  247  /* */

const int leds8 = 8;
const int dataS = 2;// seconds blue
const int latchS = 4;
const int dataM = 9; //minutes blue 9
const int latchM = 10; //green  10
const int clock = 3;
const int dataH = 7;// hour blue
const int latchH = 6; //hourr 11 yellow
const int tonePin = 13;

//four rows and two columns
int secondpattern[][8] = {{LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},  {HIGH, LOW, LOW, LOW, LOW, LOW, LOW, LOW},     {LOW, HIGH, LOW, LOW, LOW, LOW, LOW, LOW},   {HIGH, HIGH, LOW, LOW, LOW, LOW, LOW, LOW}, /* 0-3*/
  {LOW, LOW, HIGH, LOW, LOW, LOW, LOW, LOW}, {HIGH, LOW, HIGH, LOW, LOW, LOW, LOW, LOW},    {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW},  {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW}, /* 4-7*/
  {LOW, LOW, LOW, HIGH, LOW, LOW, LOW, LOW}, {HIGH, LOW, LOW, HIGH, LOW, LOW, LOW, LOW},    {LOW, HIGH, LOW, HIGH, LOW, LOW, LOW, LOW},  {HIGH, HIGH, LOW, HIGH, LOW, LOW, LOW, LOW}, /* 7-11*/
  {LOW, LOW, HIGH, HIGH, LOW, LOW, LOW, LOW}, {HIGH, LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},   {LOW, HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW},  {HIGH, HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW}, /* 12-15*/
  {LOW, LOW, LOW, LOW, HIGH, LOW, LOW, LOW}, {HIGH, LOW, LOW, LOW, HIGH, LOW, LOW, LOW},    {LOW, HIGH, LOW, LOW, HIGH, LOW, LOW, LOW},   {HIGH, HIGH, LOW, LOW, HIGH, LOW, LOW, LOW}, /* 16-19*/
  {LOW, LOW, HIGH, LOW, HIGH, LOW, LOW, LOW}, {HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW, LOW},   {LOW, HIGH, HIGH, LOW, HIGH, LOW, LOW, LOW},  {HIGH, HIGH, HIGH, LOW, HIGH, LOW, LOW, LOW}, /* 20-23*/
  {LOW, LOW, LOW, HIGH, HIGH, LOW, LOW, LOW}, {HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW, LOW},   {LOW, HIGH, LOW, HIGH, HIGH, LOW, LOW, LOW},  {HIGH, HIGH, LOW, HIGH, HIGH, LOW, LOW, LOW}, /* 24-27*/
  {LOW, LOW, HIGH, HIGH, HIGH, LOW, LOW, LOW}, {HIGH, LOW, HIGH, HIGH, HIGH, LOW, LOW, LOW}, {LOW, HIGH, HIGH, HIGH , HIGH, LOW, LOW, LOW}, {HIGH, HIGH, HIGH, HIGH, HIGH, LOW, LOW, LOW}, /* 28-31*/
  {LOW, LOW, LOW, LOW, LOW, HIGH, LOW, LOW},  {HIGH, LOW, LOW, LOW, LOW, HIGH, LOW, LOW},   {LOW, HIGH, LOW, LOW, LOW, HIGH, LOW, LOW},   {HIGH, HIGH, LOW, LOW, LOW, HIGH, LOW, LOW}, /* 32-35*/
  {LOW, LOW, HIGH, LOW, LOW, HIGH, LOW, LOW}, {HIGH, LOW, HIGH, LOW, LOW, HIGH, LOW, LOW},  {LOW, HIGH, HIGH, LOW, LOW, HIGH, LOW, LOW},  {HIGH, HIGH, HIGH, LOW, LOW, HIGH, LOW, LOW}, /* 36-39*/
  {LOW, LOW, LOW, HIGH, LOW, HIGH, LOW, LOW}, {HIGH, LOW, LOW, HIGH, LOW, HIGH, LOW, LOW},  {LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW},  {HIGH, HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW}, /* 40-43*/
  {LOW, LOW, HIGH, HIGH, LOW, HIGH, LOW, LOW}, {HIGH, LOW, HIGH, HIGH, LOW, HIGH, LOW, LOW}, {LOW, HIGH, HIGH, HIGH, LOW, HIGH, LOW, LOW}, {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, LOW, LOW}, /* 44-47*/
  {LOW, LOW, LOW, LOW, HIGH, HIGH, LOW, LOW}, {HIGH, LOW, LOW, LOW, HIGH, HIGH, LOW, LOW},  {LOW, HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW},  {HIGH, HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW}, /* 48-51*/
  {LOW, LOW, HIGH, LOW, HIGH, HIGH, LOW, LOW}, {HIGH, LOW, HIGH, LOW, HIGH, HIGH, LOW, LOW}, {LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW, LOW}, {HIGH, HIGH, HIGH, LOW, HIGH, HIGH, LOW, LOW}, /* 52-55*/
  {LOW, LOW, LOW, HIGH, HIGH, HIGH, LOW, LOW}, {HIGH, LOW, LOW, HIGH, HIGH, HIGH, LOW, LOW}, {LOW, HIGH, LOW, HIGH, HIGH, HIGH, LOW, LOW}, {HIGH, HIGH, LOW, HIGH, HIGH, HIGH, LOW, LOW}
}; /* 56-59*/

int hourpattern[][8] = {{LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}};
int hourpatternTwo[][8] = { {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, {HIGH, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW, HIGH, LOW, LOW, LOW, LOW, LOW, LOW}, {HIGH, HIGH, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW, LOW, HIGH, LOW, LOW, LOW, LOW, LOW},  {HIGH, LOW, HIGH, LOW, LOW, LOW, LOW, LOW},
  {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW}, {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, HIGH, LOW, LOW, LOW, LOW},  {HIGH, LOW, LOW, HIGH, LOW, LOW, LOW, LOW},
  {LOW, HIGH, LOW, HIGH, LOW, LOW, LOW, LOW},  {HIGH, HIGH, LOW, HIGH, LOW, LOW, LOW, LOW},
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}
};

int firstFreq[] = {415,  370, 330, 247}; //first quarter
int halfFreq[] = {330, 415,  370, 247, 330,   370, 415, 330};  //half hour
int thirdFreq[] = {415, 330,  370, 247, 247,  370, 415, 247, 415, 370, 330, 247}; //third quarter
int fullFreq[] = {330, 415,  370, 247, 330,  370, 415, 330, 415, 330,  370, 247, 247,  370, 415, 247};  //Full hour
int timeDur[] = {1000, 1000, 1000, 2000, 1000, 1000, 1000, 2000, 1000, 1000, 1000, 2000, 1000, 1000, 1000, 2000};

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only

  pinMode(clock, OUTPUT);
  pinMode(dataM, OUTPUT);
  pinMode(latchM, OUTPUT);
  pinMode(dataS, OUTPUT);
  pinMode(latchS, OUTPUT);
  pinMode(dataH, OUTPUT);
  pinMode(latchH, OUTPUT);

  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
}

void loop() {
  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus() != timeNotSet) {
    digitalClockDisplay();
    digitalClockLED();
  }
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  //Serial.print(" ");
  //Serial.print(day());
  //Serial.print(" ");
  //Serial.print(month());
  //Serial.print(" ");
  //Serial.print(year());
  Serial.println();
}

void digitalClockLED() {

  int s = second();
  int h = hourFormat12();
  int m = minute();

  makePattern(h, dataH, hourpatternTwo, latchH);
  makePattern(m, dataM, secondpattern, latchM);

  if (s == 0)
  {
    switch (m)
    {
      case (0):
        playWest(16, fullFreq, true);
        break;
      case (15):
        playWest(4, firstFreq, false);
        break;
      case 30:
        playWest(8, halfFreq, false);
        break;
      case 45:
        playWest(12, thirdFreq, false);
        break;
      default:
        makePattern(m, dataM, secondpattern, latchM);
        makePattern(s, dataS, secondpattern, latchS);
    }
  }
  else {
    //  makePattern(m, dataM, secondpattern, latchM);
    makePattern(s, dataS, secondpattern, latchS);
  }

}
/* Credits go to Alex Clark for this function */
void makePattern(int timeUnit, int data, int pattern[][8], int latch)
{
  digitalWrite(latch, LOW);
  for (int i = 0; i < leds8; i++)
  {
    digitalWrite(data, pattern[timeUnit][i]);
    digitalWrite(clock, HIGH);
    delay(1);
    digitalWrite(clock, LOW);
  }
  digitalWrite(latch, HIGH);
}


void playWest(int i, int freq[], bool isHour)
{
  int j;
  for (int k = 0 ; k < i ; k++)
  {
    tone(13, freq[k], timeDur[k]);
    j = second();
    makePattern(j, dataS, secondpattern, latchS);

    if (k == 3 || k == 7 || k == 11 || k == 15)
    {
      delay(1000);
      j = second();
      makePattern(j, dataS, secondpattern, latchS);
      delay(1000);
    }
    else
    {
      delay(1000);
    }
  }
  
  if (isHour)
  {
    bigBen();
  }
}
void bigBen()
{
  int h = hour();
  int s = second();
  for (int i = 1; i <= h; i++)
  {
    makePattern(1, dataH, hourpattern, latchH);
    makePattern(0, dataM, hourpattern, latchM);
    tone(13, 165, 1000);
    s = second();
    makePattern(s, dataS, secondpattern, latchS);
    delay(1000);
    s = second();
    makePattern(s, dataS, secondpattern, latchS);
    makePattern(0, dataH, hourpattern, latchH);
    makePattern(1, dataM, hourpattern, latchM);
    delay(1000);
  }
}
void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}
