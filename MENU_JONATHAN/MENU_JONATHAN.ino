// include the library code:
#include <LiquidCrystal.h>
//Pines de la LCD
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int PinBotones = A0;
int LecturaBotones = 0;
int Contador = 0;
boolean Flag = 0;

//Crear la flechita para seleccionar
byte arrow[] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B00110,
  B00100,
  B00000,
  B00000
};

/**
    MACROS, CONSTANTES, ENUMERADORES, ESTRUCTURAS Y VARIABLES GLOBALES
*/
#define COUNT(x) sizeof(x)/sizeof(*x)  
const byte rowsLCD    = 2;                              // Filas del LCD
const byte columnsLCD = 16;                             // Columnas del LCD
const byte iARROW     = 0;                              // ID icono flecha

enum Screen {MainMenu, MenuPuntos, MenuMedidas, BIT, Number};

enum Botones {Izq, Der, Arriba, Abajo, Selec, Unknow} lecturaBotones; // para leer mas adelante los botones



const char *txMainMenu[]={
  "1.Inicio        ",
  "2.Fleje1        ",
  "3.Fleje2        ",
  "4.Fleje3        ",
  "5.Fleje4        ",
  "6.Fleje5        ",
  "7.Fleje6        "
};

const char *txMenuPuntos[]={
  "1.Punto 1       ",
  "2.Punto 2       ",
  "3.Punto 3       ",
  "4.Punto 4       ",
  "5.Punto 5       "
};

const char *txMenuMedidas[]={
  "1.Medida        ",
  "2.Angulo        "
};

const byte tamanioMenu = COUNT(txMainMenu);                       // Numero de items/opciones del menu principal


void setup() {
  pinMode(PinBotones, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, arrow);
  //Inicio();
  lcd.setCursor(4, 0);lcd.print("TecnoBot");
  lcd.setCursor(2, 1);lcd.print("Tecni-Closch");
  
  //lcd.write(byte(0));
  Serial.begin(9600);

  
}

Botones leerBotones (){
   LecturaBotones = analogRead(PinBotones);
   Serial.println(LecturaBotones);
  
  if(LecturaBotones == 410){
    delay(200);
    return Izq;
  }
  else if(LecturaBotones == 0){
    delay(200);
    return Der;
  }
  else if(LecturaBotones == 100){
    delay(200);
    return Arriba;
  }
  else if(LecturaBotones == 257){
    delay(200);
    return Abajo;
  }
  else if(LecturaBotones == 641){
    delay(200);
    return Selec;
  }
  else{
    return Unknow;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
