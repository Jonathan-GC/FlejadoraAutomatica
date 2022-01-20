#include <LiquidCrystal.h>

#define rs  8
#define en  9
#define d4  4
#define d5  5
#define d6  6
#define d7  7

#define LCD_colums 16
#define LCD_rows 2

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#define pinTeclado A0

/**
    MACROS, CONSTANTES, ENUMERADORES, ESTRUCTURAS Y VARIABLES GLOBALES
*/
#define COUNT(x) sizeof(x)/sizeof(*x)                   // Macro para contar el numero de elementos de un array


enum Teclado {ENTER, LEFT, UP, DOWN, RIGHT, UNKNOWN};

String Menu1[] = {"Fleje 10x10", "Fleje 10x15", "Fleje 18x15", "Fleje 8x20", "Fleje 30x30", "Fleje 8x8"};

//Pantallas
struct MainDisplay{
  //Atributs
  
  byte index = 0;
  byte _size;


  MainDisplay(String* txt1, byte size){
    //firstline
    lcd.setCursor(0, 0);
    lcd.print(" PROGRAM SELECT ");

    //Second line
    lcd.setCursor(0, 1);
    lcd.print("<");
    lcd.print(*txt1);
    lcd.setCursor(LCD_colums - 1, 1);
    lcd.print(">");

  }
  //Meths
  void changeMenu(){
    this->index++;
    if(this->index < this->_size-1 ){
      this->index = 0;
    }
  }

  byte getIndex(){
    return this->index;
  } 

} ;

MainDisplay Pantalla1(&Menu1[0], COUNT(Menu1));

void setup() {
  
  Serial.begin(9600);
  //set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //MainDisplay Pantalla1(&Menu1[0], COUNT(Menu1));


}

void loop() {

  byte x = readButtons();
  
  
  switch (x)
  {
  case RIGHT:
    Pantalla1.changeMenu();
    delay(200);
    break;
  }
}


Teclado readButtons(){
  short keyVal = analogRead(pinTeclado);
  
  if(keyVal >= 720 && keyVal <= 725) return ENTER;
  else if(keyVal >= 480 && keyVal <= 485 ) return LEFT;
  else if(keyVal >= 304 && keyVal <= 319) return DOWN;
  else if(keyVal >= 129 && keyVal <= 135) return UP;
  else if(keyVal == 0 && keyVal <= 5) return LEFT;
  else return UNKNOWN;

}
