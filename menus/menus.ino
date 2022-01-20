#include <EEPROM.h>

/***************************************
LCD elementos necesarios para display
****************************************/

#include <LiquidCrystal.h>
//Pines del LCD
#define rs  8
#define en  9
#define d4  4
#define d5  5
#define d6  6
#define d7  7

#define LCD_colums 16
#define LCD_rows 2

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/***************************************
Pines necesarios para el funcionamiento del hardware
****************************************/

#define pinPLCSignal A1 // Signal come PLC
#define pinAlimentar A5 // Pin I6 PLC para alimentar
#define pinDoblar_45 11 // Pin I7 PLC para giro 45°
#define pinDoblar_90 12 // Pin I11 PLC para giro retraer
#define pinTeclado   A0 // pin interfaz LCD

/***************************************
Variables de lista de flejes
****************************************/
String MenuFlejes[] = {"Fleje 10x10", "Fleje 10x15", "Fleje 18x15", "Fleje 8x20", "Fleje 30x30", "Fleje 8x8"};


struct PantallaPrincipal
{
    String* _txt = NULL;
    byte weight = 0; //Peso o valor del objeto  
    
    PantallaPrincipal(String* txt, byte peso){
        _txt = txt;
         weight=peso; 

            
        
    }

    void show(){
        //to erase first
        lcd.clear();
        //first line
        lcd.setCursor(1,0);
        lcd.print("PROGRAM FLEJE");

        //Second line
        lcd.setCursor(0, 1);
        lcd.print("<");
        lcd.print(*_txt);
        lcd.setCursor(LCD_colums - 1, 1);
        lcd.print(">");
    }


};


void setup()
{
    lcd.begin(LCD_colums, LCD_rows);
    PantallaPrincipal P1(&MenuFlejes[0], 0);
    P1.show();
    delay(5000);
    PantallaPrincipal P2(&MenuFlejes[1], 1);
    P2.show();
    delay(5000);
    PantallaPrincipal P3(&MenuFlejes[2], 2);
    P3.show();
    delay(5000);
}

void loop()
{
    
}