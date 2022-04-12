#include <PIDController.h>

#define channelPinA 2
#define channelPinB 3

#define pinAlimentar A5 // Pin I8 PLC para giro 90°
#define pinRetraer   12 // pin interfaz LCD
#define frecuenciaPWM 11

/*In this section we have defined the gain values for the 
 * proportional, integral, and derivative controller I have set
 * the gain values with the help of trial and error methods.
*/ 
#define __Kp 8.8 // Proportional constant
#define __Ki 0.25 // Integral Constant
#define __Kd 1.45 // Derivative Constant

volatile long int encoder_count = 0; // stores the current encoder count
unsigned int integerValue = 0; // stores the incoming serial value. Max value is 65535
char incomingByte; // parses and stores each character one by one
int motor_pwm_value = 10; // after PID computation data is stored in this variable.

PIDController pidcontroller;

void setup(){
  Serial.begin(9600); // Serial for Debugging
  pinMode(channelPinA, INPUT_PULLUP);
  pinMode(channelPinB, INPUT_PULLUP);
  pinMode(pinAlimentar, OUTPUT);
  pinMode(pinRetraer, OUTPUT);  

  attachInterrupt(digitalPinToInterrupt(channelPinA), encoder, RISING);
  pidcontroller.begin(); // initialize the PID instance
  pidcontroller.tune(__Kp , __Ki , __Kd); // Tune the PID, arguments: kP, kI, kD
  pidcontroller.limit(-255, 255);
  
}


void loop(){
  while (Serial.available() > 0) {
    integerValue = Serial.parseInt(); // stores the integerValue
    incomingByte = Serial.read(); // stores the /n character
    if (incomingByte == '\n'){ // if we receive a newline character we will continue in the loop
      integerValue = map(integerValue, 0, 360, 0, 1000);
      continue;
    }
  }
  
  pidcontroller.setpoint(integerValue);
  motor_pwm_value = pidcontroller.compute(encoder_count);
  Serial.print(motor_pwm_value); // print the calculated value for debugging
  Serial.print("   ");
 
  if (motor_pwm_value > 8) // if the motor_pwm_value is greater than zero we rotate the  motor in clockwise direction
    motor_ccw(motor_pwm_value);
  else // else we move it in a counter clockwise direction
    motor_cw(abs(motor_pwm_value));
  
  Serial.println(encoder_count);  
  
}

void motor_cw(int power) {
  if (power > 8) {
    analogWrite(frecuenciaPWM, power);
    digitalWrite(pinAlimentar, LOW);
    
    digitalWrite(pinRetraer, HIGH);
  }
// both of the pins are set to low
  else {
    digitalWrite(pinAlimentar, HIGH);
    digitalWrite(pinRetraer, HIGH);
  }
}

void motor_ccw(int power) {
  if (power > 1) {
    analogWrite(frecuenciaPWM, power);
    digitalWrite(pinRetraer, LOW);
    
    digitalWrite(pinAlimentar, HIGH);
  }
// both of the pins are set to low
  else {
    digitalWrite(pinRetraer, HIGH);
    digitalWrite(pinAlimentar, HIGH);
  }
}

void encoder() {
  if (digitalRead(channelPinB) == HIGH) // if ENCODER_B is high increase the count
    encoder_count++; // increment the count
  else // else decrease the count
    encoder_count--;  // decrement the count
}
