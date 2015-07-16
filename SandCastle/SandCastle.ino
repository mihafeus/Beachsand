//Bits of the code borowed from:
//http://forum.arduino.cc/index.php?topic=167953.0 //some receiver code by jmc
//http://stackoverflow.com/questions/25989202/arduino-virtualwire-separating-received-data // some receiver code by Wasd9595

#include <VirtualWire.h>

int Sensor1Data; // Sensors 
char Sensor1CharMsg[4]; // RF Transmission container

int LBit = 5; //for littleBits integration

int powerPin = 11; //to give power to the receiver if you decide to plug it streight into the Arduino

void setup() {
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(13); // Connect this pin to the DATA pin on your receiver

  vw_setup(1000);  // Bits per second - the speed of the communication 
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH); //power on
  Serial.begin(9600); // set baud rate of serial communication 
  vw_rx_start();       // Start the receiver PLL running    

  pinMode(LBit, OUTPUT);
  digitalWrite(LBit, false);

} // END void setup

void loop(){

  uint8_t buflen = VW_MAX_MESSAGE_LEN; //Maximum length of the message
  uint8_t buf[buflen]; // The buffer that holds the message

  if (vw_get_message(buf, &buflen)){   

    buf[buflen] = '\0'; //Null terminate the char array, otherwhise problems could occur with values of different lenghts
    String str((char *)buf); //Converts received values to a string

    Serial.println((char *)buf);//send raw values to serial

    String name = str.substring(0,5); //splits the input integet into two parts name and value
    String value = str.substring(5);

    Serial.print("Name: ");
    Serial.print(name);

    Sensor1Data = value.toInt(); 

    Serial.print("  Value: ");
    Serial.println(Sensor1Data);

    int sensScale = Sensor1Data/4; //for the littleBits integration, converts the analogIn value for analogWrite
    analogWrite(LBit, sensScale); //littleBits integration, this passes through a low-pass filter on the ciruit 0.1uF, 4.7KOhm

  } // END if vw_get_message
} // END loop

