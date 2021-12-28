#include <avr/wdt.h> // Incluir la librería que contiene el watchdog (wdt.h)
//#include <LiquidCrystal.h>
#define ConstanteDeConversion 10 //12.8 // con velocida de 26 en Driver Variador
#define ConstanteDeConversionPulsos 0,0612612

#define channelPinA  2
#define channelPinB  3

volatile boolean stateChannelA;
volatile boolean stateChannelB;
volatile boolean prevStateChannelA = 0;

#define maxSteps 5000
//const int maxSteps = 5000;
volatile int prevValue;
volatile int value;

#define timeThreshold 200
//const int timeThreshold = 200; 
unsigned long currentTime;
unsigned long loopTime;

static bool IsCW = true;



int presionado = 0; //Ayuda para filtro de entrada a proceso
//int Fleje1 [6] = {29,10,10,10,10,5};
int Fleje1 [6] = {1,10,10,10,10,22};

//*****************lcd*******************************
#define rs 8 
#define en 9
#define d4 4 
#define d5 5 
#define d6 6 
#define d7 7
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
   Serial.begin(9600);

   wdt_disable();// Desactivar el watchdog mientras se configura, para que no se resetee
   //wdt_enable(WDTO_4S);
   pinMode(channelPinA, INPUT_PULLUP);
   pinMode(channelPinB, INPUT_PULLUP);
   pinMode(A1, INPUT_PULLUP);
   pinMode(A2, OUTPUT); // Pin I6 PLC para alimentar
   pinMode(A3, OUTPUT); // Pin I7 PLC para giro 45°
   pinMode(11, OUTPUT); // Pin I8 PLC para giro 90°
   pinMode(12, OUTPUT); // Pin I11 PLC para giro retraer

   digitalWrite(A2, HIGH); //para apagarlos de inicio ya que es negado
   digitalWrite(A3, HIGH); //para apagarlos de inicio ya que es negado
   digitalWrite(11, HIGH); //para apagarlos de inicio ya que es negado
   digitalWrite(12, HIGH); //para apagarlos de inicio ya que es negado
   currentTime = micros();
   loopTime = currentTime;
   value = 0;
   prevValue = 0;
   Serial.println("Bienvenido");
//   lcd.begin(16, 2);
//   lcd.setCursor(0, 0);
//   lcd.print("    TecnoBot   ");
//
//   delay(3000);
}

void loop(){
//  lcd.clear();
  //wdt_reset();
  if(inOrden()){
    for(int i = 0; i < 5; i++){
        Serial.print("Giro: ");Serial.println(i+1);
        //Toma medidas
        int Medida = 0;
        Medida = Fleje1[i];
        Serial.println(deCmAPulsos(Medida));
        Medida = deCmAPulsos(Medida);
        
        digitalWrite(A2, LOW);
        //Alimente mientras este lleno
        while(value < Medida){
          Encoder();
        }
        digitalWrite(A2, HIGH);
        value = 0;

        //Eleccion de ángulo
        if(i == 0 || i == 4 ){
          digitalWrite(A3, LOW);
          delay(500);
          digitalWrite(A3, HIGH);
        }
        
        else{
          digitalWrite(11, LOW);
          delay(500);
          digitalWrite(11, HIGH);
        }

        while(!inOrden());
        
        //Esperar al que el plc de la orden de 
        //volver a arrancar

        
     }
        //Reset en el contador encoder0
        value = 0;
        Serial.println("SAlio del for a corte");
        //Toma medidas
        int Medida = 0;
        
        Medida = Fleje1[5];
        Serial.println(deCmAPulsos(Medida));
        Medida = deCmAPulsos(Medida);
        
        digitalWrite(12, LOW);
        //Alimente mientras este lleno
        while(value < Medida){
          Encoder();
        }
        digitalWrite(12, HIGH);
        value = 0;

     //while(1);
     
  }
  
  
}

boolean inOrden(){
   if(digitalRead(A1) == 1){
      presionado = 1; 
      delay(150);
   }
   if (digitalRead(A1) == 0 && presionado == 1){
      presionado = 0;
      return true;
   }
   else{
      return false;
   }
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
            IsCW = true;
            if (value + 1 <= maxSteps) value++; // Asegurar que no sobrepasamos maxSteps
         }
         else  // B es LOW, es CWW
         {
            
            IsCW = false;
            if (value - 1 >= 0) value = value--; // Asegurar que no tenemos negativos
         }

      }
      prevStateChannelA = stateChannelA;   // Guardar valores para siguiente

      // Si ha cambiado el valor, mostrarlo
      if (prevValue != value)
      {
         prevValue = value;
         //Serial.println(value);

      }

      loopTime = currentTime;  // Actualizar tiempo
   }
   
   // Otras tareas
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
