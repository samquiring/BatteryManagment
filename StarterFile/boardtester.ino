// IMPORTANT: ELEGOO_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Elegoo_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com
/*
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> // touchscreen 

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define XMAX 240
#define YUPDATE 290
#define LNSPACE 20
#define DATAOFFSET 140


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


#define BOXSIZE 40
#define BOXWIDTH 80
#define TEXTOFFSET 10

//local holders of current state alarms to check if we should update their values on the screen
int HVILStateL = -1;
int OvercurrentStateL = -1;
int HVOutOfRangeStateL = -1;

//local holders of current value of measure to check if we should update their values on the screen
int SOCL = -1;
float tempL = -1;
float HVVoltL = -1;
float HVCurL = -1;
bool HVILL = true;   //Change this according to startup condition of HVIL

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Elegoo_TFTLCD tft;



void displaySetup() {
  //opens up the serial at address 9600
  Serial.begin(9600);
  //prints out the function below in the serial
  Serial.println(F("TFT LCD test"));

//if you are using the elegoo shield print out this
#ifdef USE_Elegoo_SHIELD_PINOUT       // This is defined in Elegoo_TFTLCD.h
  Serial.println(F("Using Elegoo 2.4\" TFT Arduino Shield Pinout"));
#else //else print this out
  Serial.println(F("Using Elegoo 2.4\" TFT Breakout Board Pinout"));
#endif
  //prints out the size of a tft and its height
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

//tries to find given drivers to opperate the lcd screen
   uint16_t identifier = tft.readID();
   if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }
  else if(identifier==0x1111)
  {     
      identifier=0x9328;
       Serial.println(F("Found 0x9328 LCD driver"));
  }
  else {  //tells us the drivers arent found and makes you point to them
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9328;
  
  }
  tft.begin(identifier);
  //begins the display after all drivers and propperly running
  tft.fillScreen(WHITE);
  tft.fillRect(0, 300, BOXSIZE*2, BOXSIZE, RED);
  tft.fillRect(BOXWIDTH, 300, BOXWIDTH, BOXSIZE, GREEN);
  tft.fillRect(BOXWIDTH * 2, 300, BOXWIDTH, BOXSIZE, MAGENTA);
  tft.drawRect(BOXWIDTH *2, 300, BOXWIDTH, BOXSIZE, WHITE);

 // ****************Call the battery display function**************
  
  
  pinMode(13, OUTPUT);


  tft.setCursor(TEXTOFFSET, 310);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("ALARM");

  tft.setCursor(TEXTOFFSET + BOXWIDTH, 310);
  tft.print("MEASUREMENT");
  

  tft.setCursor(TEXTOFFSET + BOXWIDTH * 2, 310);
  tft.print("BATTERY");
}

void clicker(int* displayState, bool* state, bool* batteryOn){
   digitalWrite(13, HIGH);
   TSPoint p = ts.getPoint();
   digitalWrite(13, LOW);
   pinMode(XM, OUTPUT);
   pinMode(YP, OUTPUT);
   if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {


    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    //p.x = tft.width()-map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     //p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

    if (p.y < BOXSIZE) {

       if (p.x < BOXWIDTH) { 
            *displayState = 1;
            *state = true;
         
       }  else if (p.x < BOXWIDTH*2) {
            *displayState = 0;
            *state = true;
       
       } else if (p.x < BOXWIDTH*3) {
            *displayState = 2;
            *state = true;
       
       }
    }
    if (*displayState == 1) {
      if ((p.y > 170) && (p.y < 170 + BOXSIZE)) {
        if ((p.x > 50) && (p.x < 50 + BOXWIDTH)) {
          *batteryOn = false;
        } else if ((p.x > 50 + BOXWIDTH) && (p.x < 50 + BOXWIDTH * 2)){
         *batteryOn = true;
        }
      }     
    }
  }
}

void batteryScreen(bool nText, bool nScreen, bool* batteryOn){
  if (nScreen) {   
    tft.fillRect(0,0,XMAX,YUPDATE,WHITE);
    tft.setCursor(0, 0);
    tft.setTextColor(BLACK); tft.setTextSize(2);
    tft.print("Battery Screen");
    tft.setCursor(50 + TEXTOFFSET, 100 + TEXTOFFSET);
    tft.fillRect(50, 100, BOXWIDTH, BOXSIZE, BLUE);
    tft.print("ON");
    tft.setCursor(50 + BOXWIDTH + TEXTOFFSET, 100 + TEXTOFFSET);
    tft.fillRect(50 + BOXWIDTH, 100, BOXWIDTH, BOXSIZE, YELLOW);
    tft.print("OFF");       
  }
       if (*batteryOn) {
        tft.fillRect(50, 200, BOXSIZE, BOXSIZE, WHITE);
      } else {
        tft.fillRect(50, 200, BOXSIZE, BOXSIZE, MAGENTA);
      }
}

void measurementScreen(int SOC, float temp,float HVVolt, float HVCur, bool HVIL, bool nText, bool nScreen){
  //fills the touch screen black
  if(nScreen){
    tft.fillRect(0,0,XMAX,YUPDATE,WHITE);
    unsigned long start = micros();
    //sets the cursor on the screen to the point 0,0
    tft.setCursor(0, 0);
    tft.setTextColor(BLACK); tft.setTextSize(2);
    tft.print("Measurement Screen");
    tft.setCursor(0,LNSPACE*2);
    tft.print("SOC: ");
    tft.setTextSize(2);
    tft.setCursor(0,LNSPACE*3);
    tft.print("Temperature: ");
    tft.setCursor(0,LNSPACE*4);
    tft.print("HV Current: ");
    tft.setCursor(0,LNSPACE*5);
    tft.print("HV Voltage: ");
    tft.setCursor(0,LNSPACE*6);
    tft.print("HVIL: ");
   //makes sure HVIL is displayed if new cycle
    HVILL = !HVIL;
  }
  if(SOCL != SOC){
    SOCL = SOC;
    tft.fillRect(DATAOFFSET,LNSPACE*2,XMAX-DATAOFFSET,LNSPACE-5,WHITE);
    tft.setCursor(DATAOFFSET,LNSPACE*2);
    tft.print(SOC);
  }
  if(temp != tempL){
    tempL = temp;
    tft.fillRect(DATAOFFSET,LNSPACE*3,XMAX-DATAOFFSET,LNSPACE-5,WHITE);
    tft.setCursor(DATAOFFSET,LNSPACE*3);
    tft.print(temp);
  }
  if(HVCur != HVCurL){
    HVCurL = HVCur;
    tft.fillRect(DATAOFFSET,LNSPACE*4,XMAX-DATAOFFSET,LNSPACE-5,WHITE);
    tft.setCursor(DATAOFFSET,LNSPACE*4);
    tft.print(HVCur);
  }
  if(HVVolt != HVVoltL){
    HVVoltL = HVVolt;
    tft.fillRect(DATAOFFSET,LNSPACE*5,XMAX-DATAOFFSET,LNSPACE-5,WHITE);
    tft.setCursor(DATAOFFSET,LNSPACE*5);
    tft.print(HVVolt);
  }
  //work on HVIL when I work on the pins
  if(HVIL != HVILL){
    HVILL = HVIL;
    tft.fillRect(DATAOFFSET,LNSPACE*6,XMAX-DATAOFFSET,LNSPACE-5,WHITE);
    tft.setCursor(DATAOFFSET,LNSPACE*6);
    if(HVIL){
      tft.print("CLOSED");
    } else {
      tft.print("OPEN");
    }
  }
}

void AlarmScreen(int HVILState, int OvercurrentState, int HVOutOfRangeState, bool nText, bool nScreen){
  //fills the touch screen black
  if(nScreen){
    tft.fillRect(0,0,XMAX,YUPDATE,WHITE);
    unsigned long start = micros();
    //sets the cursor on the screen to the point 0,0
    tft.setCursor(0, 0);
    tft.setTextColor(BLACK); tft.setTextSize(2);
    tft.print("Alarm Screen");
    tft.setCursor(0,LNSPACE*2);
    tft.print("HVIL: ");
    tft.setCursor(0,LNSPACE*4);
    tft.print("Overcurrent: ");
    tft.setCursor(0,LNSPACE*6);
    tft.print("High Voltage Out of Range: ");
  }
  enum state{NOT_ACTIVE = 0, ACTIVE_NA = 1, ACTIVE_A = 2};
  if(HVILState != HVILStateL){
    HVILStateL = HVILState;
    tft.setTextSize(1);
    tft.setCursor(0,LNSPACE*3);
    tft.fillRect(0,60,XMAX,10,WHITE);
    if(HVILState == NOT_ACTIVE){
        tft.print("NOT ACTIVE");
    } else if(HVILState == 1){
        tft.print("ACTIVE, NOT ACKNOWLEDGED");
    } else {
        tft.print("ACTIVE, ACKNOWLEDGED");
    }
  }
  if(OvercurrentState != OvercurrentStateL){
    OvercurrentStateL = OvercurrentState;
    tft.setCursor(0,LNSPACE*5);
    tft.fillRect(0,100,XMAX,10,WHITE);
    if(OvercurrentState == NOT_ACTIVE){
        tft.print("NOT ACTIVE");
    } else if(OvercurrentState == 1){
        tft.print("ACTIVE, NOT ACKNOWLEDGED");
    } else {
        tft.print("ACTIVE, ACKNOWLEDGED");
    }
  }
  if(HVOutOfRangeState != HVOutOfRangeStateL){
    HVOutOfRangeStateL = HVOutOfRangeState;
    tft.setCursor(0,LNSPACE*8);
    tft.fillRect(0,160,XMAX,10,WHITE);
    if(HVOutOfRangeState == NOT_ACTIVE){
        tft.print("NOT ACTIVE");
    } else if(HVOutOfRangeState == 1){
        tft.print("ACTIVE, NOT ACKNOWLEDGED");
    } else {
        tft.print("ACTIVE, ACKNOWLEDGED");
    }
  }
}

*/
