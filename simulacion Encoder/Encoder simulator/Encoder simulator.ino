const int pinArranqueDer = 7, pinIndicador = 13, pinIndicador2 = 4, pinArranqueIzq = 8;
const int pinA = 2, pinB = 3;



void setup() {
  pinMode(pinArranqueDer, INPUT);
  pinMode(pinArranqueIzq, INPUT);
  pinMode(A0, INPUT);
  pinMode(pinIndicador, OUTPUT);
  pinMode(pinIndicador2, OUTPUT);  
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);

  Serial.begin(9600);
     
}
unsigned long x = 0;
boolean orden = 1;
void loop() {
  /*
  int lectura = digitalRead(pinArranque);
  if(lectura == 0){
    delay(1000);
    orden = 1;
  }
  */
  digitalWrite(pinIndicador, LOW);
  digitalWrite(pinIndicador2, LOW);

  while (!digitalRead(pinArranqueDer)){
    digitalWrite(pinIndicador, HIGH);
    x = analogRead(A0);
    
    x < 10? x = 63000000000 : x = map(x, 0, 1023, 100000, 1000);
    //Serial.println(x);
    
    digitalWrite(pinA, 1);
    delayMicroseconds(x);
    digitalWrite(pinB, 1);
    delayMicroseconds(x);
    digitalWrite(pinA, 0);
    delayMicroseconds(x);
    digitalWrite(pinB, 0);
    delayMicroseconds(x);
    
  }
  digitalWrite(pinIndicador, LOW);
  digitalWrite(pinIndicador2, LOW);
  
  while (!digitalRead(pinArranqueIzq)){
    digitalWrite(pinIndicador2, HIGH);
    x = analogRead(A0);
    
    x < 10? x = 63000000000 : x = map(x, 0, 1023, 100000, 250);
    //Serial.println(x);
    
    digitalWrite(pinB, 1);
    delayMicroseconds(x);
    digitalWrite(pinA, 1);
    delayMicroseconds(x);
    digitalWrite(pinB, 0);
    delayMicroseconds(x);
    digitalWrite(pinA, 0);
    delayMicroseconds(x);
    
  }




}
