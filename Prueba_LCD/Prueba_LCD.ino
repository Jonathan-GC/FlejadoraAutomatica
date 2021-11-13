
// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned long timeCounter;
const int intPin = 3;
volatile int ISRCounter = 0;
int counter = 0;
const int emuPin = 13;

void setup() {
  // set up the LCD's number of columns and rows:
  Serial.begin(115200);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("    TecnoBot   ");
  
  lcd.setCursor(0, 1);
  lcd.print("  Tecni Closch ");

   pinMode(emuPin, OUTPUT);
   
   pinMode(intPin, INPUT);
   
   
   //attachInterrupt(digitalPinToInterrupt(intPin), interruptCount, FALLING);

}

void loop()
{
   ISRCounter = digitalRead(intPin);
//   if(ISRCounter==0){
//    Serial.println(1); 
//   }

    Serial.println(ISRCounter);
   
}



void interruptCount()
{
   //ISRCounter++;
   //timeCounter = millis();
   Serial.println("ISRCounter");
}
