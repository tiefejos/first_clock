#include <Time.h>

time_t number;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  number=now();
 Serial.print(number);
 delay(2000);

 
}
