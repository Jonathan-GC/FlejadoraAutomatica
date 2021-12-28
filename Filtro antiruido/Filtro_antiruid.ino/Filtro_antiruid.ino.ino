/*
  Input Pull-up Serial

  This example demonstrates the use of pinMode(INPUT_PULLUP). It reads a digital
  input on pin 2 and prints the results to the Serial Monitor.

  The circuit:
  - momentary switch attached from pin 2 to ground
  - built-in LED on pin 13

  Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
  20K-ohm resistor is pulled to 5V. This configuration causes the input to read
  HIGH when the switch is open, and LOW when it is closed.

  created 14 Mar 2012
  by Scott Fitzgerald

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/InputPullupSerial
*/

void setup() {
  //start serial connection
  Serial.begin(9600);
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(A1, INPUT_PULLUP);
  pinMode(13, OUTPUT);

}
static unsigned long tnow = 0;
static unsigned long t_ant=0;
int presionado = 0;
void loop() {
  if (digitalRead(A1) == 1)     //Pregunta si el pulsador está presionado
      {
        presionado = 1;     //La variable cambia de valor
      }
      delay(80);
      if (digitalRead(A1) == 0 && presionado == 1)
      {
        Serial.println("presionado");
        //Realiza la acción deseada
        presionado = 0;    //La variable vuelve a su valor original
     }
  /*
  tnow = millis();
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(A1);
  //print out the value of the pushbutton
  if(!sensorVal){
      tnow = millis();
      if (tnow-t_ant > 3000) {
        Serial.println(sensorVal);
        t_ant = tnow;
      }
  }
  
  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:
  if (sensorVal == HIGH) {
    digitalWrite(13, LOW);
  } else {
    digitalWrite(13, HIGH);
  }
  */
}
