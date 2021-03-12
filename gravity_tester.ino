int xVal;
int yVal;
int zVal;
long zTotalUp;
int ZcountUp;
long zTotalDown;
long gravity;
#include <Arduino.h>
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  Serial.println(analogRead(A11));
  delay(100);
}
