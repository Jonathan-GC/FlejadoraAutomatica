#define ConstanteDeConversion 13
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

int Fleje1 [6] = {24,5,5,5,5,5};

void setup() {
   Serial.begin(115200);
   pinMode(channelPinA, INPUT_PULLUP);
   pinMode(channelPinB, INPUT_PULLUP);
   pinMode(A1, INPUT_PULLUP);
   pinMode(A2, OUTPUT);
   pinMode(A3, OUTPUT);
   pinMode(11, OUTPUT);
   pinMode(12, OUTPUT);

   digitalWrite(A2, HIGH); //para apagarlos de inicio ya que es negado
   digitalWrite(A1, HIGH); //para apagarlos de inicio ya que es negado
   digitalWrite(11, HIGH); //para apagarlos de inicio ya que es negado
   digitalWrite(12, HIGH); //para apagarlos de inicio ya que es negado
   currentTime = micros();
   loopTime = currentTime;
   value = 0;
   prevValue = 0;
}

void loop() {
   if(digitalRead(A1) == 0){
      for(int i = 0; i < 3; i++){
        int Medida = 0;
        Medida = Fleje1[i];
        Serial.println(deCmAPulsos(Medida));
        Medida = deCmAPulsos(Medida);
        
        digitalWrite(A2, LOW);
        while(value < Medida){
          Encoder();
        }
        digitalWrite(A2, HIGH);
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
