#include <EEPROM.h>

/***************************************
    MACROS, CONSTANTES, ENUMERADORES, ESTRUCTURAS Y VARIABLES GLOBALES
****************************************/
    
#define COUNT(x) sizeof(x)/sizeof(*x) // Macro para contar el numero de elementos de un array


unsigned long currentTimeMillis = 0;
unsigned long previosTimeMillis = 0;




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
/*
    FIGURADO FLEJE CONVENCIONAL
              2____3
              |    |
              |//__|
              1 5  4
*/
# define valueVerticesXFleje  5

String MenuFlejes[] = {"Fleje10x10", "Fleje 10x15", "Fleje 18x15", "Fleje 8x20", "Fleje 30x30", "Fleje 8x8"};

enum Teclado {ENTER, LEFT, UP, DOWN, RIGHT, UNKNOWN};

/***************************************
Funciones
****************************************/
Teclado readButtons(){
  short keyVal = analogRead(pinTeclado);
  
  if(keyVal >= 720 && keyVal <= 725) return ENTER;
  else if(keyVal >= 480 && keyVal <= 485 ) return LEFT;
  else if(keyVal >= 304 && keyVal <= 319) return DOWN;
  else if(keyVal >= 129 && keyVal <= 135) return UP;
  else if(keyVal == 0 && keyVal <= 5) return RIGHT;
  else return UNKNOWN;

}


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
    byte index = 0;         //Indice para buscar en el figurado
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
        lcd.print("P");
        lcd.print(index);

        lcd.setCursor(5, 1);
        lcd.print(figurado[index][0]);
        lcd.print(" cm");
        
        lcd.setCursor(11, 1);
        lcd.print(figurado[index][1]);
        lcd.print("'");

        lcd.setCursor(LCD_colums - 1, 1);
        lcd.print(">");


    }

    void selectPuntoX(){
        //Funcion para poner a parpadear la pantalla cuando quiera cambiar el punto el usuario
        bool flag = 1;
        bool stateBlink = 1;
        byte valueToEdit = 0; 
        boolean angleToEdit = false;
        
        while(flag){
            currentTimeMillis = millis();
            if(currentTimeMillis - previosTimeMillis > 250 ){
                
                Teclado but = readButtons();

                if (but == ENTER){
                    flag=0;
                    stateBlink = 1; //La igualo a 1 para forzar a que se vea antes de salir y evitar que desaparezca
                }
                else if (but == DOWN)
                {
                    if(valueToEdit == 1){
                        figurado[index][0]--;
                        figurado[index][0] %= 32;
                    }
                    else if(valueToEdit == 2){
                        angleToEdit?figurado[index][1]=45:figurado[index][1]=90;
                    }
                    else{
                        index++;
                        index%=6;
                        show(); // refrescar pantalla
                    }
                    
                    
                }
                else if (but == UP)
                {   
                    if(valueToEdit == 1){
                        figurado[index][0]++;
                        figurado[index][0] %= 32;
                    }
                    else if(valueToEdit == 2){
                        angleToEdit?figurado[index][1]=90:figurado[index][1]=45;
                    }
                    else{
                        index--;
                        index==255?index=5:index;
                        index%=6;
                        show(); // refrescar pantalla
                    }
                }

                else if (but == RIGHT)
                {
                    valueToEdit++;
                    valueToEdit%=3;
                    show(); // refrescar pantalla
                }
                else if (but == LEFT)
                {
                    valueToEdit--;
                    valueToEdit==255?valueToEdit=2:valueToEdit;
                    valueToEdit%=3;
                    show(); // refrescar pantalla
                }
                
                Serial.println(angleToEdit);

                if(stateBlink){
                    //Desaparece
                    switch (valueToEdit)
                    {
                        case 0:
                            lcd.setCursor(1, 1);
                            break;
                        case 1:
                            lcd.setCursor(5, 1);
                        break;
                        case 2:
                            lcd.setCursor(11, 1);
                        break;
                    }
                    
                    lcd.print("  ");
                    stateBlink = 0;

                }else{
                    
                    //Aparece
                    show();
                    stateBlink = 1;
                }
                previosTimeMillis = currentTimeMillis;
            }
            

            
        }
    
    }
};

struct PantallaTerciaria{
    String* _txt = NULL;    //Puntero al menu asignado
    /*Pantalla para aceptar el proceso e iniciar*/
    
    PantallaTerciaria(String* txt, byte fleje[6][2]){
        _txt = txt;
    }

    

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
        lcd.setCursor(0,1);
        lcd.print("<");
        lcd.setCursor(3,1);
        lcd.print("Enter(2s)");

    }

    boolean ordenArrancar(){
        
        //se le agrega un while para que entre en un bucle
        bool flag = 1;
        while(flag){
            previosTimeMillis = millis();

            if(readButtons() == LEFT){
                flag = 0;
            }

            while(readButtons() == ENTER && flag == 1){
                currentTimeMillis = millis();
                if(currentTimeMillis - previosTimeMillis > 2000){
                    lcd.clear();
                    lcd.setCursor(3,0);
                    lcd.print("En proceso");
                    flag = 0;
                }
            }
        }
        if(flag == 0){
            return true;
        }
        else{
            return false;
        }
    }

};

/***************************************
Punteros
****************************************/
    PantallaPrincipal** displayMain = NULL;


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

//    PantallaTerciaria *displayTerciario = NULL;

//    displayTerciario = new PantallaTerciaria(&MenuFlejes[1], fleje);
//    displayTerciario->show();
//    displayTerciario->ordenArrancar();

    /*
    //OBJETO FUNCIONAL menos uso de memoria
    PantallaSecundaria* displaySecond = NULL;
    for(byte i = 0; i < 6; i++){
        displaySecond = new PantallaSecundaria(&MenuFlejes[i], fleje, i);
        displaySecond->show();
        delay(500);
    }
    
    displaySecond->selectPuntoX();


    delete[] displaySecond;
    displaySecond = NULL;
    

    
    */
    
    //OBJETO FUNCIONAL menos uso de memoria
 //   PantallaPrincipal* displayMain = NULL;
    //-------------------------------------------------------------------------------
/*
    for(byte i = 0; i < 6; i++){
        displayMain = new PantallaPrincipal(&MenuFlejes[i], i);
        displayMain->show();
        delay(3000);
    }

    delete [] displayMain;
    displayMain = NULL;
*/
/*
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

    displayMain = new PantallaPrincipal*[6];

    for(byte i = 0; i < 6; i++){
        displayMain[i] = new PantallaPrincipal(&MenuFlejes[i], i);
        displayMain[i]->show();
        delay(2000);
    }
    
}

void loop()
{
    
    Teclado Button = readButtons();

    if(Button == RIGHT){
        delay(200);
        displayMain[0]->show();
        delay(5000);
    }
    delay(100);
    
}

