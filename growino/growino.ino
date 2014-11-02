#include <TimedAction.h>

#include <DallasTemperature.h>

#include <OneWire.h>

#include <Wire.h>

#include <Time.h>

#include <DS1307RTC.h>

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include <SPI.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
//Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
 Adafruit_PCD8544 display = Adafruit_PCD8544(13,12,11, 10, 9);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!
#define LCDWIDTH = 84;
#define LCDWIDTH = 48;
tmElements_t tm;
OneWire ds(A0);
DallasTemperature sensors(&ds);

int currentscreen;

void cycle() {
   if(currentscreen < 4)
      currentscreen++;
     else 
      currentscreen = 0;
}

TimedAction screenCycle = TimedAction(5000,cycle);

double s1temp;
double s2temp;

void refreshtemp() {
   sensors.requestTemperatures();     
   s1temp = sensors.getTempCByIndex(0); 
   s2temp = sensors.getTempCByIndex(1);
}

TimedAction refreshTempAction = TimedAction(1000,refreshtemp);



static const unsigned char PROGMEM logo[] = {
0x7,0xff,0xff,0xff,0xe0,
0x1f,0xff,0xff,0xff,0xf8,
0x3f,0xff,0xff,0xff,0xfc,
0x7f,0xff,0xff,0xff,0xfe,
0x7f,0xff,0xff,0xff,0xfe,
0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xf8,0x1f,0xff,
0xff,0xff,0xe0,0x7,0xff,
0xff,0xff,0xc0,0x7,0xff,
0xe0,0xff,0x3,0x87,0xff,
0xc0,0x3e,0xf,0xf,0xff,
0xe0,0xe,0x1e,0xf,0xff,
0xe1,0xc0,0x3c,0x1f,0xff,
0xf0,0x60,0x78,0x3f,0xff,
0xf8,0x0,0xe0,0x7f,0xff,
0xfc,0x0,0x1,0xff,0xff,
0xff,0x0,0x3,0xff,0xff,
0xff,0xf0,0xf,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0xff,0xf1,0xff,0xff,0xff,
0x7f,0xf1,0xff,0xff,0xfe,
0x7f,0xe1,0xff,0xff,0xfe,
0x3f,0xe1,0xff,0xff,0xfc,
0x1f,0xe1,0xff,0xff,0xf8,
0x7,0xe1,0xff,0xff,0xe0
};



const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
};

void setup()   {
  display.begin();
  display.setContrast(55);
  display.clearDisplay();
  display.setRotation(90);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  display.drawBitmap(22,4, logo, 40,40, BLACK);
  display.display(); // show splashscreen


  Serial.begin(9600);
    if(timeStatus()!= timeSet) {
            Serial.println("Algo paso");

    if (getDate(__DATE__) && getTime(__TIME__)) {
      Serial.println("Intentando configurar RTC");
      if(RTC.write(tm)) {
         Serial.println("Todo OK"); 
           setSyncProvider(RTC.get);   // the function to get the time from the RTC
      }else{
        Serial.println("Ocurrio un error"); 
        

      };
    }
    }
    
    
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  
  sensors.begin();
    delay(1000); 

  display.clearDisplay();   // clears the screen and buffer
  display.setTextWrap(false);
   currentscreen = 0;


}



void printtemp(double val) {
  
  int IntegerPart = (int)(val);
  int DecimalPart = 100 * (val - IntegerPart); //10000 b/c my float values always have exactly 4 decimal places

display.setTextSize(2);
display.print(IntegerPart);
display.setTextSize(1);
if(DecimalPart<10) {
  display.print(0);
}
display.print(DecimalPart);
display.setTextSize(1);
}

void loop() {
  screenCycle.check();refreshTempAction.check();
  display.clearDisplay();   // clears the screen and buffer
  
    display.setTextSize(1);
    headerDisplay();
    digitalClockDisplay();
  
    display.setCursor(0,10);
    
    switch(currentscreen) {
       case 0:
       case 1:
          display.setCursor(9,10);
          display.println("TEMPERATURA"); 
          display.setCursor(0, 21);
          printtemp(s1temp);
          display.setCursor(0, 37);
          display.println("INT"); 
          display.setCursor(48, 21);
          printtemp(s2temp);
          display.setCursor(67, 37);
          display.println("EXT"); 
      break;
         
       case 2:
    display.setTextSize(1);
    display.println("Luz Principal"); 
    display.setTextSize(2);
    display.print("Apagada");
       break;
       case 3:
    display.setTextSize(1);
    display.println("Riego"); 
    display.setTextSize(2);
    display.print("Apagado");
       break;
       case 4:
    display.setTextSize(1);
    display.println("Ventilador"); 
    display.setTextSize(2);
    display.print("Apagado");
       break;
    }
    
    display.display();
}

void headerDisplay() {
  display.setCursor(0,0);
  display.print(day());
    display.print("/");
    display.print(month());
  /*  
    display.print("Growino");
  */
}

void digitalClockDisplay(){
  int time = hour();
  
  if(time>10) {
  display.setCursor(59,0);
  } else {
    display.setCursor(65,0);
  }
  display.print(hour());
  printDigits(minute());
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if( (second() % 2) == 0) {
      digitalWrite(6, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(7, HIGH);
  display.drawPixel(71,3, BLACK);
      display.drawPixel(71,5, BLACK);
  } else {
          digitalWrite(6, LOW);
          digitalWrite(5, LOW);
          digitalWrite(8, LOW);
      digitalWrite(7, LOW);
  }
display.setCursor(73,0);
  if(digits < 10)
    display.print('0');
  display.print(digits);
}


bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

