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
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  Serial.begin(9600);
}

void loop() {
  mostrarPantalla();
  
  

  
  /*
  
  if(Botones2 () == Abajo){
      Contador++;
      delay(200);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      MovFlecha();
  }
  if(LecturaBotones == 100 or LecturaBotones == 101 ){
      Contador--;
      Serial.println(Contador);
      delay(200);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
      MovFlecha();
      
  }
  if(Flag == 0){
     if(Contador == 0){
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(0, 0);
        lcd.write(byte(0));
     }
    if(Contador == 0 || Contador == 1){
      lcd.setCursor(1, 0);
      lcd.print("Inicio");
      lcd.setCursor(1, 1);
      lcd.print("Fleje1");
    }
    if(Contador == 2 || Contador == 3){
      lcd.setCursor(1, 0);
      lcd.print("Fleje2");
      lcd.setCursor(1, 1);
      lcd.print("Fleje3");
    }
    if(Contador == 4 || Contador == 5){
      lcd.setCursor(1, 0);
      lcd.print("Fleje4");
      lcd.setCursor(1, 1);
      lcd.print("Fleje5");
    }
  }
  if(Contador == 0 && LecturaBotones == 641){
    Flag = 1;
    lcd.clear();
    delay(200);
    lcd.setCursor(0, 0);
    lcd.write(byte(0));
    lcd.setCursor(1, 0);
    lcd.print("Elegir Fleje");
    MovFlecha();
    if(Contador == 0 || Contador == 1){
      lcd.setCursor(1, 0);
      lcd.print("Elegir Fleje");
      lcd.setCursor(2, 1);
      String jhjhjh = "Fleje1";
      lcd.print(jhjhjh);
    }
    if(Contador == 2 || Contador == 3){
      lcd.setCursor(2, 0);
      lcd.print("Fleje2");
      lcd.setCursor(2, 1);
      lcd.print("Fleje3");
    }
    if(Contador == 4 || Contador == 5){
      lcd.setCursor(2, 0);
      lcd.print("Fleje4");
      lcd.setCursor(2, 1);
      lcd.print("Fleje5");
    }
  }
  if(Flag == 1 && LecturaBotones == 410){
    lcd.clear();
    Flag = 0;
  }
  */
}

/**
   MUESTRA EL SUBMENU EN EL LCD.
   @param index    ID del menu principal para usarlo como titulo del submenu
   @param screen    Segun el tipo, se representara el submenu de una forma u otra.
   @param variable     Puntero a la variable que almacena el dato, y que se modificara.
   @param min  Valor minimo que puede tener la variable.
   @param max  Valor maximo que puede tener la variable.
*/
void openMenu(byte index, Screen tipoMenu, int* variable, int min, int max){
  boolean exitSubMenu = false;
  boolean forcePrint  = true;

  lcd.clear();

   while ( !exitSubMenu )
  {
    lecturaBotones = leerBotones();

    if ( lecturaBotones == Selec)
    {
      exitSubMenu = true;
    }
    else if ( lecturaBotones == Izq && (*variable) - 1 >= min )
    {
      (*variable)--;
    }
    else if ( lecturaBotones == Der && (*variable) + 1 <= max)
    {
      (*variable)++;
    }



    if ( !exitSubMenu && (forcePrint || lecturaBotones != Unknow) )
    {
       forcePrint = false;
       lcd.setCursor(0, 0);
       lcd.print(txMainMenu[index]);
       
       lcd.setCursor(0, 1);
       lcd.print("<");
       lcd.setCursor(columnsLCD - 1, 1);
       lcd.print(">");

       if ( tipoMenu == MainMenu )
       {
          lcd.setCursor(1, 1);
          lcd.print(txMenuPuntos[*variable]);
       }

       
    }

  }
  lcd.clear();
}

void Inicio (){
  lcd.setCursor(4, 0);
  lcd.print("Tecnobot");
  lcd.setCursor(3, 1);
  lcd.print("Tecniclosh");
  delay(3000);
  lcd.clear();
}
Botones leerBotones (){
   LecturaBotones = analogRead(PinBotones);
  //Serial.println(LecturaBotones);
  
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
void MovFlecha (){
  if(Contador % 2 == 1){
     lcd.setCursor(0, 0);
     lcd.print(" ");
     lcd.setCursor(0, 1);
     lcd.write(byte(0));
   }
  if(Contador % 2 == 0){
     lcd.setCursor(0, 1);
     lcd.print(" ");
     lcd.setCursor(0, 0);
     lcd.write(byte(0));
  }
}
void openMenu(){
  byte idxMenu = 0;
  boolean exitMenu = false;
  boolean forcePrint = true;

  lcd.clear();

  while(!exitMenu){
    lecturaBotones = leerBotones ();

    //if (lecturaBotones == Abajo){
    if (lecturaBotones == Abajo && idxMenu - 1 >= 0){  
      idxMenu--;
      //lcd.clear();
      //lcd.print(idxMenu);
    }
    //else if(lecturaBotones == Arriba){
    else if(lecturaBotones == Arriba && idxMenu + 1  < tamanioMenu){
      idxMenu++;
      //lcd.clear();
      //lcd.print(idxMenu);
    }
    
    else if(lecturaBotones == Selec){
      
      switch(idxMenu){
        case 0:  lcd.setCursor(0, 1); lcd.print(idxMenu); delay(1000);exitMenu = true; break;
        case 1:  lcd.setCursor(0, 1); lcd.print(idxMenu); delay(1000);exitMenu = true; break;
        case 2:  lcd.setCursor(0, 1); lcd.print(idxMenu); delay(1000);exitMenu = true; break;
        case 3:  lcd.setCursor(0, 1); lcd.print(idxMenu); delay(1000);exitMenu = true; break;
        case 4:  lcd.setCursor(0, 1); lcd.print(idxMenu); delay(1000);exitMenu = true; break;
        case 5:  lcd.setCursor(0, 1); lcd.print(idxMenu); delay(1000);exitMenu = true; break;
        //case 1: openSubMenu( idxMenu, Screen::Flag, &startProduccion, 0, 1); exitMenu = true; break;
      }

      forcePrint=true;
     
    }
    
  }
}

void mostrarPantalla(){
  static unsigned long tNow = 0;
  static unsigned long tPrevious = 0;

  tNow = millis();
  lecturaBotones = leerBotones ();

  if (lecturaBotones == Selec){
    openMenu();
  }

  if(tNow - tPrevious > 1000){
    tPrevious = tNow;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("TecnoBot");

    //Aqui esta la pantalla principal
  }
  

  
}
