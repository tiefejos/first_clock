/*
 * TimeSerial.pde
 * example code illustrating Time library set through serial port messages.
 *
 * Messages consist of the letter T followed by ten digit time (as seconds since Jan 1 1970)
 * you can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2013
 T1357041600
 *
 * A Processing example sketch to automatically send the messages is included in the download
 * On Linux, you can use "date +T%s\n > /dev/ttyACM0" (UTC time zone)
 */

#include <Time.h>
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
//int j;
// void makePattern(int j, int leds[], int pattern[][j], int num);
//int ledPins[] = {2, 3, 4, 5, 6, 7};
//int ledPins[] = {2, 3, 4};
const int leds8 = 8;
int dataS = 2;// seconds
int dataM = 9; //minutes
int dataH = 9;// hour
int clock = 3;
int latchS = 4;
int latchM = 10;
int latchH = 10; //hour
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

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  pinMode(dataM, OUTPUT);
  pinMode(clock, OUTPUT);
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

  delay(1000);
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  //Serial.print(month());
  //Serial.print(" ");
  //Serial.print(year());
  Serial.println();
}

void digitalClockLED() {

  int s = second();
  int h = hour();
  int m = minute();

  switch (m)
  {
    case (0):
      fullHour();
      break;
    case (15):
      firstQuarter();
      break;
    case 30:
      halfHour();
      break;
    case 45:
      thirdQuater();
      break;
    default:
      makePattern(m, dataM, secondpattern, latchS);

  }

  makePattern(s, dataS, secondpattern, latchS);
  makePattern(h, dataH, secondpattern, latchH);

}


void makePattern(int j, int leds, int secondpattern[][8], int latch)
{
  int delayTime = 200;
  digitalWrite(latch, LOW);
  for (int i = 0; i < leds8; i++)
  {
    //digitalWrite(leds[i], secondpattern[j][i]);
    digitalWrite(leds, secondpattern[j][i]);
    digitalWrite(clock, HIGH);
    delay(1);
    digitalWrite(clock, LOW);
  }
  digitalWrite(latch, HIGH);
  // delay(2000);
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

void firstQuarter() {
  int j = 15;
  tone(13, 415, 1000);
 makePattern(j, dataS, secondpattern, latchS);
  delay(1000);

  j = 16;
  tone(13, 740, 1000);
 makePattern(j, dataS, secondpattern, latchS);
  delay(1000);

  j = 17;
  tone(13, 330, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);

  j = 18;
  tone(13, 377, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 19;
  makePattern(j, dataS, secondpattern, latchS);
  //delay(1000);
}
void halfHour() {
  int j = 30;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 31;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 32;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 33;
  tone(13, 377, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 34;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 35;
  tone(13, 330, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 36;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 37;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 38;
  tone(13, 370, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 39;
  makePattern(j, dataS, secondpattern, latchS);
}

void thirdQuater() {
  int j = 45;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 46;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 47;
  tone(13, 330, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 48;
  tone(13, 377, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 49;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);

  j = 50;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 51;
  tone(13, 330, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 52;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 53;
  tone(13, 377, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 54;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);

  j = 55;
  tone(13, 377, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 56;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 57;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 58;
  tone(13, 330, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 59;
  makePattern(j, dataS, secondpattern, latchS);
}

void fullHour() {
  int j = 0;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 1;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 2;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 3;
  tone(13, 377, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 4;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  //////////////////
  j = 5;
  tone(13, 330, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 6;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 7;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 8;
  tone(13, 370, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 9;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  //////////////////////////////////
  j = 10;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 11;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 12;
  tone(13, 330, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 13;
  tone(13, 377, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 14;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  //////////////////////////////////////
  j = 15;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 16;
  tone(13, 330, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 17;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 18;
  tone(13, 377, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 19;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  ///////////////////////////////
  j = 20;
  tone(13, 377, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 21;
  tone(13, 740, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 22;
  tone(13, 415, 1000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 23;
  tone(13, 330, 2000);
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  j = 24;
  makePattern(j, dataS, secondpattern, latchS);
  delay(1000);
  ////////////////////////
  bigBen();
}

void bigBen()
{
  int h = hour();
  int s = 1;
  int t = 0;
  for (int i = 1; i <= h; i++)
  {
    makePattern(s, dataH, secondpattern,latchH);
    tone(13, 165, 1000);
    //makePattern(s, ledPins, secondpattern, 6);
    delay(1000);
    makePattern(t, dataH, secondpattern,latchH);
    delay(1000);
  }
}


