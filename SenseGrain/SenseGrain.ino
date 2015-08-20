#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <VirtualWire.h>
#include <EEPROM.h>

volatile boolean f_wdt = 1;

//Variables
const int switchPin = 3;            // Interrupt pin
const int powerPin = 1;             // Power saving for the sensor
int Sensor1Data;
int Sensor1DataOld;
char Sensor1CharMsg[4]; 

int timer = 0;                      // Sets the number of wake up times after which to send data
int counter = 0;                    // Counts the times the microcontroller has woken up
int masterSwitch;                   // This switch on either interrupt sleep or timer sleep
int wdSetup;                        // Watchdog setup number // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec

String SendStr;
String NameStr = "";                // The name of the transmitter has to consist of 5 characters // If no name is specified, read the given name from the EEPROM

String timerStr = "Not set";

char letter;
byte value;

boolean treshold = false;

void setup() {                      // Set Up Instrictions


  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);     // Sensor power on

  timerSetup();                     // Runs the function to set up the timer 

  digitalWrite(powerPin, LOW);      // Power saving mode enabled

  if(NameStr == ""){                // If no name is specified, read the given name from the EEPROM
    for (int i = 0; i < 5; i++){
      value = EEPROM.read(i);
      letter = value;
      NameStr += String(letter); 
    }
  }

  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);

  // Transmitter code
  //vw_set_ptt_inverted(true);       // Note to self: I might need to re-enable this
  vw_set_tx_pin(0);                  // Transmitter pin
  vw_setup(1000);                    // Speed of data transfer Kbps

  transmitFunc(NameStr);
  transmitFunc(timerStr);

}


void loop() {

  if(masterSwitch==0){               // Timer sleep mode

    if (f_wdt==1) {                  // Wait for timed out watchdog / flag is set when a watchdog timeout occurs
      f_wdt=0;                       // Reset flag

      if(counter>=timer){            // If the counter has not reached the desired timer value, it goes back to sleep
        counter=0;                   // If counter has reached the timer value, it resets the counter and
        SensorRead();                // Tiggers reading the sensor values
      }
      counter++;
      system_sleep();
    }
  }

  if(masterSwitch==1){                // Interrupt sleep mode
    SensorRead();
    sleep(); 
  }
}

void SensorRead(){

  digitalWrite(powerPin, HIGH);       // Provides power to the sensor
  Sensor1Data = analogRead(A2);       // Reading the sensor

  if(timerStr=="Treshold"){  

    if (analogRead(A2)<analogRead(A1) && treshold==false) {
      treshold=true;
      SendStr=NameStr+Sensor1Data;    // Combinig name with sensor values
      transmitFunc(SendStr);
    }
    else if(analogRead(A2)>analogRead(A1) && treshold==true){
      treshold=false;
      SendStr=NameStr+Sensor1Data;    // Combinig name with sensor values
      transmitFunc(SendStr);
    }
  }
  else{
    SendStr=NameStr+Sensor1Data;      // Combinig name with sensor values
    transmitFunc(SendStr);
  }
  digitalWrite(powerPin, LOW);        // Cuts power from the sensor
}
