int xVal;
int yVal;
int zVal;
long zTotalUp;
int ZcountUp;
long zTotalDown;
long gravity;
#include <Arduino.h>
#define bufferSize 100
float xVel = 0;
float yVel = 0;
float zVel = 0;
float totalDistance = 0;
float xTotal = 0;
float yTotal = 0;
float zTotal = 0;
float xA = 0;
float yA = 0;
float zA = 0;


float xBuf[bufferSize] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float yBuf[bufferSize] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float zBuf[bufferSize] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int p = 0;

int counter = 0;
#define zeroPointX 348
#define zeroPointY 322
#define zeroPointZ 320
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
#define zeroPointX 348
#define zeroPointY 322
#define zeroPointZ 320

//memset(xBuf,0,bufferSize);
//memset(yBuf,0,bufferSize);
//memset(zBuf,0,bufferSize);

float xDisplacement = 0;
float yDisplacement = 0;
float zDisplacement = 0;

float xVel = 0;
float yVel = 0;
float zVel = 0;

//pinMode(22, OUTPUT);
//digitalWrite(22, HIGH);

float totaldistance = 0;


int i = 0;
float xTot = 0;
float yTot = 0;
float zTot = 0;
delay(1000);

while(i < 100) {
  xTot = xTot + analogRead(A13);
  yTot = yTot + analogRead(A14);
  zTot = zTot + analogRead(A15);
  i = i + 1;
  delay(100);
}

xA = xTot / 100.0;
yA = yTot / 100.0;
zA = zTot / 100.0;

Serial.println(zA);
}

 float xAcc = 0;
 float yAcc = 0;
 float zAcc = 0;


void loop() {
  // put your main code here, to run repeatedly:
  float timeBase = 0.1;
  /*
  zTotalUp = 0;
  zTotalDown = 0;
  Serial.println("beginning running");
  for(int i = 0; i < 100; i++){
    zVal = analogRead(A13);
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
  */ 


  int xRaw = analogRead(A13);
  int yRaw = analogRead(A14);
  int zRaw = analogRead(A15);

  float xGAcc = ((xRaw - xA) / 164.0);
  float yGAcc = ((yRaw - yA) / 164.0);
  float zGAcc = ((zRaw - zA + 164) / 164.0);

  float xAcc2 = xGAcc * 980.0;
  float yAcc2 = yGAcc * 980.0;
  float zAcc2 = zGAcc * 980.0;

   xAcc += (xAcc2 - xBuf[p])/bufferSize;
   yAcc += (yAcc2 - yBuf[p])/bufferSize;
   zAcc += (zAcc2 - zBuf[p])/bufferSize;
   xBuf[p] = xAcc2;
   yBuf[p] = yAcc2;
   zBuf[p] = zAcc2;
   if(p != bufferSize-1){
    p += 1;
   } else {
    p = 0;
   }


  xTotal = xTotal + xAcc;
  yTotal = yTotal + yAcc;
  zTotal = zTotal + zAcc;

  counter = counter + 1;
  float xAve = xTotal / counter;
  float yAve = yTotal / counter;
  float zAve = zTotal / counter;

  xVel = xVel + xAve * timeBase;
  yVel = yVel + yAve * timeBase;
  zVel = zVel + zAve * timeBase;


  float xChange = (xVel * timeBase) + 0.5 * xAcc * (timeBase * timeBase);
  float yChange = (yVel * timeBase) + 0.5 * yAcc * (timeBase * timeBase);
  float zChange = (zVel * timeBase) + 0.5 * (zAcc - 980) * (timeBase * timeBase);

  float distanceChange = sqrt((xChange * xChange) + (yChange * yChange));
  totalDistance = totalDistance + distanceChange;





/* 
  Serial.print("X: ");
  Serial.print(xRaw);
  Serial.print("    ");
  Serial.print("Y: ");
  Serial.print(yRaw);
  Serial.print("    ");
  Serial.print("Z: ");
  Serial.println(zRaw);
*/
  Serial.println("Averages");
  Serial.print("X: ");
  Serial.print(xAcc);
  Serial.print("  ");
  Serial.print(xAcc2);
  Serial.print("    ");
  Serial.print("Y: ");
  Serial.print(yAcc);
   Serial.print("  ");
  Serial.print(yAcc2);
  Serial.print("    ");
  Serial.print("Z: ");
  Serial.println(zAcc);
   Serial.print("  ");
  Serial.println(zAcc2);
  Serial.println(totalDistance);
  Serial.println("");

  
 

  
  delay(100);
  
}
