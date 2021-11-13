volatile int contador = 0;
volatile int contador2 = 0;
volatile int contador3 = 0;
volatile int n = contador ;
volatile int m = contador2 ;
volatile int o = contador3 ;
void setup()
   {   Serial.begin(9600);
       //attachInterrupt( 0, ServicioBoton, RISING);
       //attachInterrupt( 1, ServicioBoton2, RISING);
       pinMode(2, INPUT_PULLUP);
       pinMode(3, INPUT_PULLUP);
       pinMode(A4, INPUT_PULLUP);
       
       Serial.println("contador");
   }
void loop()
   {
       if (!digitalRead(A4))
          {     contador3++;

                if(contador3 >= 8)
                Serial.print("Vuelta\t");
                Serial.print(contador3);
                Serial.print("A\t");
                Serial.print(contador);
                Serial.print("B\t");
                Serial.println(contador2);
                o = contador3 ;
          }
          
//       if (n != contador)
//          {     Serial.print(contador);
//                Serial.print("\t");
//                n = contador ;
//          }
//
//      if (m != contador2)
//          {     Serial.println(contador2);
//                m = contador2 ;
//          }
      
   }

void ServicioBoton() 
   {    contador++ ;
   }
void ServicioBoton2() 
   {    contador2++ ;
   }
