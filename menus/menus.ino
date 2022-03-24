/***   Global variables   ***/
int Contador;
int Contador_2;

/***   Function declaration   ***/

void setup()
{
  Contador=0;

  Contador_2=0;



  pinMode(2,INPUT);

  Serial.begin(9600);

  pinMode(8,OUTPUT);

  pinMode(10,OUTPUT);

  pinMode(3,INPUT);

}


void loop()
{
    Serial.println(digitalRead(2));
    if (Contador == 2) {
      digitalWrite(8,LOW);
     }
    if (Contador_2 == 2) {
      digitalWrite(10,LOW);
     }
    if (Contador == 1) {
      digitalWrite(8,HIGH);
     }
    if (Contador_2 == 1) {
      digitalWrite(10,HIGH);
     }
    if (digitalRead(2) == 1) {
      delay(250);
      Contador=Contador + 1;
     }
    if (digitalRead(3) == 1) {
      delay(250);
      Contador_2=Contador_2 + 1;
     }
    delay(100);
    if (Contador == 2) {
      Contador=0;
     }
    if (Contador_2 == 2) {
      Contador_2=0;
     }

}

/***   Function definition   ***/