#define ConstanteDeConversion 12.8 // con velocida de 26 en Driver Variador
#define ConstanteDeConversionPulsos 0,0612612

const int channelPinA = 2;
const int channelPinB = 3;

unsigned char stateChannelA;
unsigned char stateChannelB;
unsigned char prevStateChannelA = 0;

const int maxSteps = 9999;
int prevValue;
int value;

const int timeThreshold = 200; 
unsigned long currentTime;
unsigned long loopTime;

bool IsCW = true;

//int Fleje1 [6] = {29,10,10,10,10,5};
int Fleje1 [6] = {200,200,200,200,200,200};

void setup() {
   Serial.begin(115200);
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
}

int presionado = 0;

void loop() {
   if(digitalRead(A1) == 1){
      presionado = 1; 
      delay(200);
   }
   if (digitalRead(A1) == 0 && presionado == 1){
    Serial.println("Entro");
    Serial.print("Pin A2: I6 ");Serial.println(digitalRead(A2));
    Serial.print("Pin A3: I7 ");Serial.println(digitalRead(A3));
    presionado = 0;
      for(int i = 0; i < 6; i++){
        int Medida = 0;
        Medida = Fleje1[i];
        Serial.println(deCmAPulsos(Medida));
        Medida = deCmAPulsos(Medida);
        
        digitalWrite(A2, LOW);
        while(value < Medida){
          Encoder();
        }
        digitalWrite(A2, HIGH);
        digitalWrite(A3, LOW);
        delay(500);
        digitalWrite(A3, HIGH);
        value = 0;
        delay(2000);
        
     }
     while(1);
     
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
