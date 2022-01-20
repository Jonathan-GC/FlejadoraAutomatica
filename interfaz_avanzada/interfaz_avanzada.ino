#include <LiquidCrystal.h>

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7= 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int lecturaPinA0 = 1023;
enum botones {Into, Left, Right, Up, Down, unknown} lecturaTeclado;
enum Pantalla{MainMenu, MenuPuntos, MenuMedidas, BIT, Number};

#define tamanioArray(x) sizeof(x)/sizeof(*x)
byte arrowUP[] = {
  B00000,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000,
  B00000,
  B00000
};

byte arrowDOWN[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B10001,
  B01010,
  B00100,
  B00000
};



botones readButtons(){
  lecturaPinA0 = analogRead(A0);
  if(lecturaPinA0 < 10){
      //Serial.println("Entro al right2");
      delay(200);
      return Right;
    }
    else if(lecturaPinA0 > 90 && lecturaPinA0 < 110){
      delay(200);
      return Up;
    }
    else if(lecturaPinA0 > 250 && lecturaPinA0 < 265 ){
      delay(200);
      return Down;
    }
    else if(lecturaPinA0 > 400 && lecturaPinA0 < 420){
      delay(200);
      return Left;
    }
    else if(lecturaPinA0 > 635 && lecturaPinA0 < 650){
      delay(200);
      return Into;
    }
    else{
      return unknown;
    }
}


//Creacion de menus y submenus
String mainMenu[6] =
{
  "1. Inicio      ",
  "2. Fleje_1     ",
  "3. Fleje_2     ",
  "4. Fleje_3     ",
  "5. Fleje_4     "
  
};

String subInicio[5] =
{
  "1. Fleje_1     ",
  "2. Fleje_2     ",
  "3. Fleje_3     ",
  "4. Fleje_4     "
  
};

String menuPuntos[6] =
{
  "1. punto      ",
  "2. punto      ",
  "3. punto      ",
  "4. punto      ",
  "5. punto      "
};


static unsigned long tNow = 0;
static unsigned long tPrevious = 0;

struct MENU {
  //Atributos
  int indexMenu = 0;
  enum menuType {LIST, NUMBER, BIT};
  int peso;
  int tamanio;
  String *contenMenu;
  menuType visualizacion;
  
  
  //Contructor
  MENU(String *content = NULL, int _tamanio = 0,  byte visualizationType=0, byte _peso=0){
    contenMenu = content;
    visualizacion = visualizationType;
    tamanio = _tamanio;
    peso = _peso;
  };

  //destructor
  ~MENU(){};

  //MEtodos
  void mostrarEnPantalla(int* idxSelection = 0, int Selection = 0){
    tNow = millis();
    
    if (tNow - tPrevious > 1000){
      
      //Puntero de menu
      //Igualo el puntero en la misma direccion del menu

      if(visualizacion == LIST){
        //Serial.print("LCD");Serial.println(*idxSelection);
        lcd.clear();
        //Selector
        lcd.setCursor(0, 0); lcd.print(">");
        //Rodillo giratorio
        lcd.setCursor(1, 0);
        lcd.print(contenMenu[*idxSelection]);
        
        lcd.setCursor(1, 1);
        lcd.print(contenMenu[*idxSelection+1]);
        tPrevious = tNow;
    
        
        
      }
      else if(visualizacion == NUMBER){
        //Serial.println("entro al number");
        
        lcd.clear();
        lcd.setCursor(1, 0); lcd.print(contenMenu[Selection]);
        lcd.setCursor(12, 1); lcd.print("Unds");

        //Selector
        lcd.setCursor(0, 1); lcd.write(byte(0));
        
        //Poner la variable a modificar
        lcd.setCursor(4,1); lcd.print(*idxSelection);

        lcd.setCursor(10, 1); lcd.write(byte(1));
        tPrevious = tNow;
      }
      else if(visualizacion == BIT){
        lcd.clear();
        lcd.setCursor(1, 0); lcd.print("    Arrancar?    ");
        
        //Selector
        lcd.setCursor(0, 1); lcd.write(byte(0));
        lcd.setCursor(4,1);
        //Poner la variable a modificar
        *idxSelection==0?lcd.print("NO"):lcd.print("SI");
        

        //Selector
        lcd.setCursor(10, 1); lcd.write(byte(1));
        tPrevious = tNow;
      }
          
    } 
      
  }
  
};


struct Senialador{
  enum menuType {LIST, NUMBER, BIT};
  boolean autorizationOutData = false;
  int leerTeclado(byte minValue = 0 , int maxValue = 2, byte iteractionType = 0, int*  OutValue = NULL, int* weight=0){
    //Los retornos son -1 var por defecto
    //-2 cancelar
    //-3 IndexMenu
    //-4 number
    //-5 bit
    lecturaTeclado = readButtons();
    
    if(lecturaTeclado == Down){
      autorizationOutData = false;
      (*OutValue) ++;
      //Serial.print("En teclado "); Serial.print(*OutValue);Serial.print("\t");
      if(*OutValue > maxValue-2){
        *OutValue = 0;
      }
      
    }
    else if(lecturaTeclado == Up){
      autorizationOutData = false;
      (*OutValue)--;
      //Serial.print("En teclado "); Serial.print(*OutValue); Serial.print("\t");
      if(*OutValue <= 0){ //al ser index menu la blindamos contra desbordamientos
        *OutValue = maxValue-2;
      }
      
    }
    else if(lecturaTeclado == Right || lecturaTeclado == Into){
      //Serial.println("Entro al right");
      if(iteractionType== LIST){
        autorizationOutData = true;
        return -3;
        //return *OutValue + *weight;
      }
      else if(iteractionType == NUMBER){
        return -4;
      }
      else if(iteractionType == BIT){
        return -5;
      }
      
    }
    else if(lecturaTeclado == Left){
      if(iteractionType == LIST){
        return -2;
      }
        
    }
    
    return -1;
    
    
  }
  
};


void setup(){
  
  lcd.begin(16, 2);
  pinMode(A0, INPUT);
  Serial.begin(9600);

  lcd.createChar(0, arrowUP);
  lcd.createChar(1, arrowDOWN);
}

void loop(){
   
//  do{
//    Home();
//  }while(readButtons()==unknown || readButtons()==Up || readButtons()==Down);
  MENU main_Menu(mainMenu,tamanioArray(mainMenu), 0, 0);
  MENU subMenuInicio(subInicio,tamanioArray(subInicio), 0, 10);
  MENU subMenuUnidades(subInicio,1000, 1, 20);
  MENU subMenuArrancar(subInicio,2, 0, 30);
  Senialador puntero;
  
  struct MENU *vis = &main_Menu; 
  int flag = 0;
  int sizeSelection = tamanioArray(mainMenu);
  int encabezado = 0;
  do{
    vis->mostrarEnPantalla(&vis->indexMenu, encabezado);
    flag = puntero.leerTeclado(0, sizeSelection, 0, &vis->indexMenu, &vis->peso);

    if(flag == -3){
      int *valueMenu, *valuePeso; 
      valueMenu = &vis->indexMenu;
      valuePeso = &vis->peso;
      //Se le coloca el respectivo peso para validar el menu
      //valueMenu += &vis->peso;
      
      switch(*valueMenu + *valuePeso){
        case 0:
          encabezado=0;
          sizeSelection = tamanioArray(subInicio);
          vis = &subMenuInicio;
          break;
        
        case 10:
          Serial.println("entro al 10");
          encabezado = 0;
          sizeSelection = 1000;
          vis = &subMenuUnidades;
        break;
      }  
    }
    else if(flag == -4){
      
    }
    
  }while(flag != -2);
  Serial.println("Salio");
  
  
  
}


void Home (){
  tNow=millis();
  if(tNow-tPrevious>1000){
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("TecnoBot");
    tPrevious=tNow;
  }
}
 
