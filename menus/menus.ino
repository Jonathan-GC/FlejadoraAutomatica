#include <EEPROM.h>

/***************************************
    MACROS, CONSTANTES, ENUMERADORES, ESTRUCTURAS Y VARIABLES GLOBALES
****************************************/
    
#define COUNT(x) sizeof(x)/sizeof(*x) // Macro para contar el numero de elementos de un array


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
String MenuFlejes[] = {"Fleje10x10", "Fleje 10x15", "Fleje 18x15", "Fleje 8x20", "Fleje 30x30", "Fleje 8x8"};

enum Teclado {ENTER, LEFT, UP, DOWN, RIGHT, UNKNOWN};

struct PantallaPrincipal
{
    String* _txt = NULL;    //Puntero al menu asignado
    byte weight = 0;        //Peso o valor del objeto  
    

    PantallaPrincipal(String* txt, byte peso){
        /*  Se construye el objeto con su array y peso, cuestion de poder implementar 
            cientos de panntallas principales y sea dinamico, preferiblemente crear el 
            objeto con apuntadores
        */

        _txt = txt;
         weight=peso; 

    }

    ~PantallaPrincipal(){}; //Destructor

    void show(){
        /*  Funcion para mostrar el contenido en el menú
        */

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

struct PantallaSecundaria
{
    byte figurado[6][2];
    byte weight = 0;        //Peso o valor del objeto  
    String* _txt = NULL;    //Puntero al menu asignado

    PantallaSecundaria(String* txt, byte fleje[6][2], byte peso){
        for(byte row = 0; row < 6; row++){
           for(byte col = 0; col < 2; col++){
               figurado[row][col]=fleje[row][col];
           }
        }

        weight = peso;
        _txt = txt;

    }

    ~PantallaSecundaria(){}; //Destructor

    void show(){
        // Lo vuelvo mayuscula para que se vea el cambio en la interfaz
        _txt->toUpperCase();

        //ELimino los espacios para que se vea  responsive
        if(_txt->length()>10){
            _txt->remove(5,1);
        }
    
        //First line
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print(*_txt);

        //Second line
        lcd.setCursor(0, 1);
        lcd.print("<");
        lcd.print("P1");

        lcd.setCursor(5, 1);
        lcd.print(figurado[1][0]);
        lcd.print(" cm");
        
        lcd.setCursor(11, 1);
        lcd.print(figurado[0][1]);
        lcd.print("'");

        lcd.setCursor(LCD_colums - 1, 1);
        lcd.print(">");


    }
};


void setup()
{
    Serial.begin(9600);
    lcd.begin(LCD_colums, LCD_rows);
    lcd.clear();

    byte fleje[6][2]={
        {5, 45},
        {10,90},
        {10,90},
        {10,90},
        {10,90},
        {5 ,45}
        
    };

    /*

    //OBJETO FUNCIONAL menos uso de memoria
    PantallaSecundaria* displaySecond = NULL;
    for(byte i = 0; i < 6; i++){
        displaySecond = new PantallaSecundaria(&MenuFlejes[i], fleje, i);
        displaySecond->show();
        delay(3000);
    }
    
    delete[] displaySecond;
    displaySecond = NULL;
    

    
    

    OBJETO FUNCIONAL menos uso de memoria
    PantallaPrincipal* displayMain = NULL;
    -------------------------------------------------------------------------------

    for(byte i = 0; i < 6; i++){
        displayMain = new PantallaPrincipal(&MenuFlejes[i], i);
        displayMain->show();
        delay(3000);
    }

    delete [] displayMain;
    displayMain = NULL;


    -------------------------------------------------------------------------------
    PantallaPrincipal P1(&MenuFlejes[0], 0);
    P1.show();
    delay(5000);
    
    PantallaPrincipal P2(&MenuFlejes[1], 1);
    P2.show();
    delay(5000);
    //delete P2;
    PantallaPrincipal P3(&MenuFlejes[2], 2);
    P3.show();
    delay(5000);
    //delete P3;
    */
}

void loop()
{
    
}