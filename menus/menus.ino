#include <EEPROM.h>
#include <avr/wdt.h> // Incluir la librería que contiene el watchdog (wdt.h)

/***************************************
    MACROS, CONSTANTES, ENUMERADORES, ESTRUCTURAS Y VARIABLES GLOBALES
****************************************/
#define debug 1

#ifdef __arm__
    // should use uinstd.h to define sbrk but Due causes a conflict
    extern "C" char* sbrk(int incr);
#else  // __ARM__
    extern char *__brkval;
#endif  // __arm__

int freeMemory() {
    char top;
    #ifdef __arm__
        return &top - reinterpret_cast<char*>(sbrk(0));
    #elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
        return &top - __brkval;
    #else  // __arm__
        return __brkval ? &top - __brkval : &top - __malloc_heap_start;
    #endif  // __arm__
}
    
#define COUNT(x) sizeof(x)/sizeof(*x) // Macro para contar el numero de elementos de un array


#define startFirstTime 0  //SE usa para configurar la EEprom por primera vfez o cuando se desconfigura


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
#define pinDoblar_45 A4 // Pin I7 PLC para giro 45°
#define pinRetraer   12 // Pin I11 PLC para giro retraer
#define pinDoblar_90 11 // Pin I8 PLC para giro 90°
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

String MenuFlejes[] = {"", "Fleje 10x15", "Fleje10x10", "Fleje 18x15", "Fleje 8x20", "Fleje 30x30", "Fleje 8x8"};

enum Teclado {ENTER, LEFT, UP, DOWN, RIGHT, UNKNOWN};

byte fleje[6][2]={
        {5, 45},
        {10,90},
        {10,90},
        {10,90},
        {10,90},
        {5 ,45}      
    };


//Vars de proceso menu
int presionado = 0;
short desplazamiento = 1; 
boolean startProcess = 0;
byte index = 0;

/***************************************
Funciones
****************************************/
Teclado readButtons();
void crearPantallaPrincipal();
void Encoder ();
int deCmAPulsos (int Cm);
int dePulsosACm (int Pulsos);
boolean inOrden();
//void putEprom(byte fleje[6][2], int direction);
void readEEPROM(short *index);
void updateEEPROM(short *index);
/***************************************
Encoder
****************************************/

#define ConstanteDeConversion 10 // con velocida de 26 en Driver Variador
#define ConstanteDeConversionPulsos 0,0612612

#define channelPinA  2
#define channelPinB  3

unsigned char stateChannelA;
unsigned char stateChannelB;
unsigned char prevStateChannelA = 0;

int maxSteps = 9999;
int prevValue;
int value;

const int timeThreshold = 200; 
unsigned long currentTime;
unsigned long loopTime;

bool IsCW = true;

//*****************************************************************
struct PantallaPrincipal
{
    String *_txt = NULL;    //Puntero al menu asignado
    byte weight = 0;        //Peso o valor del objeto  
    

    PantallaPrincipal(String *txt, byte peso){
        /*  Se construye el objeto con su array y peso, cuestion de poder implementar 
            cientos de panntallas principales y sea dinamico, preferiblemente crear el 
            objeto con apuntadores
        */

        _txt = txt;
        weight=peso; 
        //Serial.print(*_txt); Serial.print("\tDireccion:\t"); Serial.println(long(_txt));
    }

    ~PantallaPrincipal(){
      //delete[] _txt;
    }; //Destructor

    void show(){
        /*  Funcion para mostrar el contenido en el menú
        */
        _txt->toLowerCase();

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
    bool setConfig = false; //Setear configuraciones
    
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

    bool isEligiendo(){
        if (setConfig)
            return true;
        else
            return false;
        
    }

    void selectPuntoX(){
        //Funcion para poner a parpadear la pantalla cuando quiera cambiar el punto el usuario
        bool flag = 1;
        bool stateBlink = 1;
        byte valueToEdit = 0; 
        boolean angleToEdit = false;
        setConfig = false; 

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
                    if(valueToEdit == 1){ //Lo fuerzo solamente a tener  valores hasta 30
                        figurado[index][0]++;
                        figurado[index][0] %= 32;
                    }
                    else if(valueToEdit == 2){ //Lo fuerzo solamente a tener dos valores
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
        //Lo muestro nuevamente para evitar que se quede en blanco despues de haber seleccionado
        show();
        setConfig = true;
        updateEprom(&desplazamiento, figurado);
        
    }
};

struct PantallaTerciaria
{
    String* _txt = NULL;    //Puntero al menu asignado
    /*Pantalla para aceptar el proceso e iniciar*/
    
    PantallaTerciaria(String* txt, byte fleje[6][2]){
        _txt = txt;
    }

    ~PantallaTerciaria(){}; //Destructor

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
        bool flag = 1;  //Salida del segundo while que permite el hold en el enter antes del arranque
        bool flag2 = 1; //Salida del primer while que permite cancelar con left antes del arranque y elegir otra cosa
        while(flag2){
            previosTimeMillis = millis();

            if(readButtons() == LEFT){
                flag2 = 0;
            }

            while(readButtons() == ENTER && flag == 1){
                currentTimeMillis = millis();
                if(currentTimeMillis - previosTimeMillis > 2000){
                    lcd.clear();
                    lcd.setCursor(3,0);
                    lcd.print("En proceso");
                    
                    flag = 0;
                    flag2 = 0;
                    
                }
            }
        }
        if(flag == 0){
            return true;
        }
        else if (flag2 == 0){
            return false;
        }
    }

};

struct Fleje {
    byte weight = 0;
    byte fleje[6][2]={
          {0, 45},
          {0, 90},
          {0, 90},
          {0, 90},
          {0, 90},
          {0 ,45}      
      };
};


/***************************************
Punteros
****************************************/
    PantallaPrincipal** displayMain = NULL;



void setup(){
    delay(2000);
    lcd.begin(LCD_colums, LCD_rows);
    lcd.clear();

    Serial.begin(115200);

    pinMode(channelPinA, INPUT_PULLUP);
    pinMode(channelPinB, INPUT_PULLUP);
    pinMode(pinPLCSignal, INPUT_PULLUP); // Signal come PLC
    pinMode(pinAlimentar, OUTPUT); // Pin I6 PLC para alimentar
    pinMode(pinDoblar_45, OUTPUT); // Pin I7 PLC para giro 45°
    pinMode(pinDoblar_90, OUTPUT); // Pin I8 PLC para giro 90°
    pinMode(pinRetraer, OUTPUT); // Pin I11 PLC para giro retraer

    digitalWrite(pinAlimentar, HIGH); //para apagarlos de inicio ya que es negado
    digitalWrite(pinDoblar_45, HIGH); //para apagarlos de inicio ya que es negado
    digitalWrite(pinDoblar_90, HIGH); //para apagarlos de inicio ya que es negado
    digitalWrite(pinRetraer, HIGH); //para apagarlos de inicio ya que es negado

    //Para configurar por primera vez cuando se desconfigura la EEPROM
    #if startFirstTime
      putEpromFirstTime();
    #endif

    lcd.print(freeMemory());
    crearPantallaPrincipal();
    lcd.print(" ");
    lcd.print(freeMemory());
    
    delay(1000);
    Serial.println(F("se reinicio."));
    lcd.setCursor(1, 1);
    lcd.print("reinicio");

    wdt_disable();// Desactivar el watchdog mientras se configura, para que no se resetee
   //wdt_enable(WDTO_4S);

}



void loop() {

    if(!startProcess){   
        Teclado Button = readButtons();

        if(Button == RIGHT){
            delay(250);
            desplazamiento++;
            desplazamiento = desplazamiento % 6;
            Serial.print("memoria: "); Serial.println(freeMemory());
            //Serial.print("Desplazamiento: ");Serial.println(desplazamiento+1);
            //Serial.print(*displayMain[desplazamiento+1]->_txt);
            //Serial.print("\tDireccion:\t"); Serial.println(long(displayMain[desplazamiento+1]->_txt));
        }  
    
        else if(Button == LEFT){
            delay(250);
            desplazamiento--; 
            if(desplazamiento<0){
                desplazamiento=5;
            }
            Serial.print("memoria: "); Serial.println(freeMemory());
            //Serial.print("Desplazamiento: ");Serial.println(desplazamiento+1);
            //Serial.print(*displayMain[desplazamiento+1]->_txt);
            //Serial.print("\tDireccion:\t"); Serial.println(long(displayMain[desplazamiento+1]->_txt));
        }
        else if(Button == ENTER){
            delay(250);
            //lee el dato de la eprom  para mostrar
            readEEPROM(&desplazamiento);
            index = displayMain[desplazamiento+1]->weight;

            ///***************
            Serial.println(*displayMain[0]->_txt);
            ///***************
            Serial.print("memoria: "); Serial.println(freeMemory());
            
            Serial.print("Selecciono: "); Serial.println(index);
            
            // 3. Crear el nuevo objeto pantalla secundaria
            PantallaSecundaria displaySecond(&MenuFlejes[index], fleje, index);
            //displaySecond = new PantallaSecundaria(&MenuFlejes[index], fleje, index);
            displaySecond.show();
            
            Serial.print("memoria: "); Serial.println(freeMemory());

            // 4. Elegir la configuracion
            if (!displaySecond.isEligiendo()){
                displaySecond.selectPuntoX();
                
                  
                ///***************
                //Serial.println(*displayMain[0]->_txt);
                ///***************  
            }

            // 5. Reconfirmar si realmente eligio o cambio configuracion
            if (displaySecond.isEligiendo()){
                PantallaTerciaria displayTerciario(&MenuFlejes[index], fleje);

                //displayTerciario = new PantallaTerciaria(&MenuFlejes[index], fleje);
                displayTerciario.show();

                ///***************
                //Serial.println(*displayMain[0]->_txt);
                ///***************  

                Serial.print("memoria: "); Serial.println(freeMemory());

                if (displayTerciario.ordenArrancar()){
                    Serial.println("Entro aca");
                    delay(5000);
                    startProcess = 1;
                    Serial.print("memoria: "); Serial.println(freeMemory());
                }
                else{
                    startProcess = 0;
                    
                    Serial.print("memoria: "); Serial.println(freeMemory());
                    //limpiar la memoria de la tercera pantalla

                    //limpiar la memoria de la segunda pantalla
                    Serial.print("memoria: "); Serial.println(freeMemory());
                    

                    Serial.print("memoria: "); Serial.println(freeMemory());
                }

            }

            
            //Serial.println("salio"); 
        
            
                
            ///***************
            //Serial.println(MenuFlejes[0]);
            ///***************  
        }
    
        displayMain[desplazamiento+1]->show();
        
        delay(100); 
    }
    else if (startProcess) {
    
        //limpiar memoria y prepararla para el encoder 
        for(byte i = 0; i < 7; i++) 
          delete[] displayMain[index];
        
        delete[] displayMain;

        //limpiar la memoria de la segunda pantalla
        //delete[] displaySecond;

        //limpiar la memoria de la tercera pantalla
        //delete[] displayTerciario;
        while(1){
            Serial.print("memoria: "); Serial.println(freeMemory());
            if(inOrden()){
                for(int i = 0; i < 5; i++){
                    Serial.print("Giro: ");Serial.println(i+1);
                    //Toma medidas
                    int Medida = 0;
                    Medida = fleje[i][0];
                    Serial.println(deCmAPulsos(Medida));
                    Medida = deCmAPulsos(Medida);
                    
                    // 1.-------------------------------------------
                    digitalWrite(pinAlimentar, LOW);
                    //Alimente mientras este lleno
                    while(value < Medida){
                        Encoder();
                    }
                    digitalWrite(pinAlimentar, HIGH);
                    value = 0;
                    
                    // 2.-------------------------------------------
                    //Eleccion de ángulo
                    if (fleje[i][1] == 45){
                        digitalWrite(pinDoblar_45, LOW);
                        delay(500);
                        digitalWrite(pinDoblar_45, HIGH);
                    }
                    else{
                        digitalWrite(pinDoblar_90, LOW);
                        delay(500);
                        digitalWrite(pinDoblar_90, HIGH);
                    }
                    
                    /*
                    if(i == 0 || i == 4 ){
                    digitalWrite(A4, LOW);
                    delay(500);
                    digitalWrite(A4, HIGH);
                    }
                    
                    else{
                    digitalWrite(11, LOW);
                    delay(500);
                    digitalWrite(11, HIGH);
                    }
                    */
                    while(!inOrden());
                    
                    //Esperar al que el plc de la orden de 
                    //volver a arrancar
                    
                }

                //Reset en el contador encoder0
                value = 0;
                Serial.println(F("Salio del for a corte"));
                //Toma medidas
                int Medida = 0;
                

                //Le quito la distancia entre la cizalla y el pivot (23) y lo traslado a pulsos para retraer
                Medida = 23 - fleje[5][0];
                Serial.println(deCmAPulsos(Medida));
                Medida = deCmAPulsos(Medida);
                
                digitalWrite(pinRetraer, LOW);
                //Alimente mientras este lleno


                while(value < Medida){
                    Encoder();
                }

                digitalWrite(pinRetraer, HIGH);
                value = 0;
                

                //Avanzar hasta el punto de inicio
                //Pausa para que no avance tan rapido  delay(1000);
                delay(1200);
                Medida=23;
                Medida = deCmAPulsos(Medida);
                    
                // 1.-------------------------------------------
                digitalWrite(pinAlimentar, LOW);
                
                //Alimente mientras este lleno
                while(value < Medida){
                    Encoder();
                }
                digitalWrite(pinAlimentar, HIGH);
                value = 0;
                
                while(1);
                
            }

        }
    
            

        
    }   

}

boolean inOrden(){
  if(digitalRead(pinPLCSignal) == 1){
    delay(300);
    return true;        
  }
  else{
    return false;
  }
  
  /*
   if(digitalRead(pinPLCSignal) == 1){
      presionado = 1; 
      delay(50);
   }
   if (digitalRead(pinPLCSignal) == 0 && presionado == 1){
      presionado = 0;
      return true;
   }
   else{
      return false;
   }
*/   
}
Teclado readButtons(){
  short keyVal = analogRead(pinTeclado);
  
  /*  
  if(keyVal >= 720 && keyVal <= 725) return ENTER;
  else if(keyVal >= 480 && keyVal <= 485 ) return LEFT;
  else if(keyVal >= 304 && keyVal <= 319) return DOWN;
  else if(keyVal >= 129 && keyVal <= 135) return UP;
  else if(keyVal >= 0 && keyVal <= 5) return RIGHT;
  else return UNKNOWN; 
  */

  if(keyVal >= 639 && keyVal <= 655){return ENTER;}
  else if(keyVal >= 400 && keyVal <= 420 ) {return LEFT;}
  else if(keyVal >= 250 && keyVal <= 300) {return DOWN;}
  else if(keyVal >= 97 && keyVal <= 110) {return UP;}
  else if(keyVal >= 0 && keyVal <= 96) {return RIGHT;}
  else {return UNKNOWN;}
}
void crearPantallaPrincipal(){

    //Creo un escenario para 6 objetos diferentes de pantalla principal
    displayMain = new PantallaPrincipal* [7];
    for(byte i = 0; i < 7; i++){
        
        displayMain[i] = new PantallaPrincipal(&MenuFlejes[i], i);
        Serial.print(*displayMain[i]->_txt); 
        Serial.print("\tDireccion Obj:\t"); Serial.println(long(displayMain[i]));
    }

    Serial.print("\tPuntero Padre:\t"); Serial.println(long(displayMain));
}
void Encoder (){
  currentTime = micros();
   if (currentTime >= (loopTime + timeThreshold))
   {
      stateChannelA = digitalRead(channelPinA);
      stateChannelB = digitalRead(channelPinB);
      if (stateChannelA != prevStateChannelA)  // Para precision simple if((!stateChannelA) && (prevStateChannelA))
      {
         if (stateChannelB) // B es HIGH, es CW
         {
            bool IsCW = true;
            if (value + 1 <= maxSteps) value++; // Asegurar que no sobrepasamos maxSteps
         }
         else  // B es LOW, es CWW
         {
            bool IsCW = false;
            if (value - 1 >= 0) value = value--; // Asegurar que no tenemos negativos
         }

      }
      prevStateChannelA = stateChannelA;   // Guardar valores para siguiente

      // Si ha cambiado el valor, mostrarlo
      if (prevValue != value)
      {
         prevValue = value;
         Serial.println(value);

      }

      loopTime = currentTime;  // Actualizar tiempo
   }
   
 

}
int deCmAPulsos (int Cm){
  int Resultado;
  Resultado = Cm*ConstanteDeConversion;
  return Resultado;
}
int dePulsosACm (int Pulsos){
  int Resultado;
  Resultado = Pulsos*ConstanteDeConversionPulsos;
  return Resultado;
}



void updateEprom(short *index, byte figurado[6][2]){
    byte eeadress = 0;
    eeadress =  sizeof(Fleje) * (*index);

    Fleje Figurado;
    //Asignacion de pesos
    Figurado.weight = *index;
    //ingresar los datos del fleje y ponerlos en 
    //el la eprom segun la posicion indicada
    for(byte i = 0; i < 6; i++){
      Figurado.fleje[i][0] = figurado[i][0] ;
      Figurado.fleje[i][1] = figurado[i][1];
    }

    //Actualice la eprom en la posicion indicada
    EEPROM.put(eeadress, Figurado);

}

void readEEPROM(short *index){
    byte eeadress = 0;
    eeadress =  sizeof(Fleje) * (*index);

    Fleje Figurado;
    EEPROM.get(eeadress, Figurado);
    
    //Sacar los datos del fleje y ponerlos en 
    //el la variable global de fleje
    for(byte i = 0; i < 6; i++){
      fleje[i][0] = Figurado.fleje[i][0];
      fleje[i][1] = Figurado.fleje[i][1];
    }
}


#if startFirstTime
  void putEpromFirstTime(){
      Fleje Figurado1, Figurado2, Figurado3, Figurado4, Figurado5, Figurado6; 
      byte eeadresS = 0;

      //Asignacion de pesos
      Figurado1.weight = 0;
      Figurado2.weight = 1;
      Figurado3.weight = 2;
      Figurado4.weight = 3;
      Figurado5.weight = 5;
      Figurado6.weight = 6;

      EEPROM.put(eeadresS, Figurado1);
      eeadresS += sizeof(Fleje);

      EEPROM.put(eeadresS, Figurado2);
      eeadresS += sizeof(Fleje);

      EEPROM.put(eeadresS, Figurado3);
      eeadresS += sizeof(Fleje);

      EEPROM.put(eeadresS, Figurado4);
      eeadresS += sizeof(Fleje);

      EEPROM.put(eeadresS, Figurado5);
      eeadresS += sizeof(Fleje);

      EEPROM.put(eeadresS, Figurado6);
      eeadresS += sizeof(Fleje);
}
#endif