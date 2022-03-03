#include <avr/wdt.h> // Incluir la librería que contiene el watchdog (wdt.h)
#include <LiquidCrystal.h>
#include <EEPROM.h>

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
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned int cont_inicios = 0;
void setup() {
   Serial.begin(115200);
   lcd.begin(16, 2);
   
   cont_inicios = EEPROM.read(0);
   cont_inicios++;
   EEPROM.put(0, cont_inicios);

  Serial.print("Re-Inicios\t");
  Serial.print(cont_inicios, DEC);
  Serial.println();

  lcd.setCursor(0,0);
  lcd.print("Re-Inicios ");
  lcd.print(cont_inicios, DEC);
   
   wdt_disable();// Desactivar el watchdog mientras se configura, para que no se resetee
   //wdt_enable(WDTO_4S);
   pinMode(channelPinA, INPUT_PULLUP);
   pinMode(channelPinB, INPUT_PULLUP);
   pinMode(A1, INPUT_PULLUP);
   pinMode(A5, OUTPUT); // Pin I6 PLC para alimentar
   pinMode(A4, OUTPUT); // Pin I7 PLC para giro 45°
   pinMode(11, OUTPUT); // Pin I8 PLC para giro 90°
   pinMode(12, OUTPUT); // Pin I11 PLC para giro retraer

   digitalWrite(A5, HIGH); //para apagarlos de inicio ya que es negado
   digitalWrite(A4, HIGH); //para apagarlos de inicio ya que es negado
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

char dato = 0;
void loop(){
//  lcd.clear();
  //wdt_reset();
  if(!digitalRead(A1)){
    Serial.println("Pulsado");
  }
  
  if(Serial.available()>0){
    dato = Serial.read();
    Serial.print("dato: ");Serial.println(dato);
  }

  switch(dato){
    case 'a':
      digitalWrite(A5, 0);
      delay(400);
      digitalWrite(A5, 1);
    break;

    case 's':
      digitalWrite(A5, 0);
      delay(300);
      digitalWrite(A5, 1);
    break;

    case '4':
      digitalWrite(A4, 0);
      delay(300);
      digitalWrite(A4, 1);
      
    break;

    case '9':
      digitalWrite(11, 0);
      delay(300);
      digitalWrite(11, 1);
    break;

    case 'r':
      digitalWrite(12, 0);
      delay(500);
      digitalWrite(12, 1);
    break;

    case 'e':
      Encoder();
      break;

    case 'b':
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }

       cont_inicios = EEPROM.read(0);
       
      Serial.print("Re-Inicios\t");
      Serial.print(cont_inicios, DEC);
      Serial.println();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Re-Inicios ");
      lcd.print(cont_inicios, DEC);
      break;
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
         Serial.println(value);

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
