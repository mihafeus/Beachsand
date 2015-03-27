//Bits of the code borowed from:
//http://forum.arduino.cc/index.php?topic=167953.0 //some receiver code by jmc
//http://stackoverflow.com/questions/25989202/arduino-virtualwire-separating-received-data // some receiver code by Wasd9595


#include <VirtualWire.h>

int ledPin = 13; // LED
int Sensor1Data; // Sensors 
char Sensor1CharMsg[4]; // RF Transmission container

int LBit = 5;

void setup() {
  vw_set_ptt_inverted(true); // Required for DR3100
  
  vw_set_rx_pin(12); // Connect this pin to the DATA pin on your receiver
  
  vw_setup(1000);  // Bits per second - the speed of the communication 
  pinMode(ledPin, OUTPUT); // LED
  Serial.begin(9600); // set baud rate of serial communication 
  vw_rx_start();       // Start the receiver PLL running    

  pinMode(LBit, OUTPUT);
  digitalWrite(LBit, false);

} // END void setup

void loop(){

  uint8_t buflen = VW_MAX_MESSAGE_LEN; //Maximum length of the message
  uint8_t buf[buflen]; // The buffer that holds the message

  if (vw_get_message(buf, &buflen)){   

    digitalWrite(13, true); //if a message is received the LED will blink

    buf[buflen] = '\0'; //Null terminate the char array, otherwhise problems could occur with values of different lenghts
    String str((char *)buf); //Converts received values to a string

     //Serial.println((char *)buf);//send raw values to serial

    if(str=="Door"){ //Put in the name of the transmitter is you want to listen to

      Serial.print("Success! You are listening to ");
      Serial.println(str);

    }
    else{//This part is listening to the value of the sensor

      
      Sensor1Data = atoi((char *)buf); //Converts received values to an integer
      Serial.println(Sensor1Data);

      int sensScale = Sensor1Data/4; //for the littleBits integration, converts the analogIn value for analogWrite
      analogWrite(LBit, sensScale); //littleBits integration, this passes through a low-pass filter on the ciruit 0.1uF, 4.7KOhm

      digitalWrite(13, false); // Turn off light to and await next message 

    } // END if (string or int)
  } // END if vw_get_message
} // END loop




