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
  // put your main code here, to run repeatedly:
  zTotalUp = 0;
  zTotalDown = 0;
  Serial.println("beginning running");
  for(int i = 0; i < 100; i++){
    zVal = analogRead(A12);
    zTotalUp += zVal;
    delay(100);
  }
  int incomingByte = -1;
  Serial.println("please flip device over and enter 1 when flipped");
  while(incomingByte != 49){
    incomingByte = Serial.read();
  }
  for(int j = 0; j < 100; j++){
    zVal = analogRead(A12);
    zTotalDown += zVal;
    delay(100);
  }
  gravity = ((zTotalUp/100) - (zTotalDown/100));
  Serial.print("gravity: ");
  Serial.println(gravity);
  delay(1000);
}
