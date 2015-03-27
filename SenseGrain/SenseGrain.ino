//Bits of the code borowed from
//http://www.instructables.com/id/Multimeter-Backlit-LCD-Mod/step12/Program-The-ATtiny/ //watchdog code by kukubee
//https://bigdanzblog.wordpress.com/2014/08/10/attiny85-wake-from-sleep-on-pin-state-change-code-example/ //interrupt code by Dan TheMan

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <VirtualWire.h>

volatile boolean f_wdt = 1;

//Variables
const int switchPin = 3; //interrupt pin
const int powerPin = 1; //power saving for sensor
int Sensor1Data;
int Sensor1DataOld;
char Sensor1CharMsg[4]; 

int timer = 0; // Sets the number of wake up times after which to send data
int counter = 0; // counts the times the microcontroller has woken up
int masterSwitch; //this switch on either interrupt sleep or timer sleep
int wdSetup; //watchdog setup number
// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms 6=1 sec, 7=2 sec, 8=4 sec, 9=8sec

String str = "Door"; //Name of the transmitter
String timerStr = "Not set";

//Set Up Instrictions
void setup() {  

  //sensor power on
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  int aRead = analogRead(A1);
  if(aRead<1000){
    masterSwitch = 0;

    if(aRead<125){
      wdSetup= 0; //16ms
      timer = 0;  //wakes up every 16ms, sends every 0.3sec
      timerStr = "Continous";
    }
    else if(aRead<250){
      wdSetup= 6; //1sec
      timer = 5;  //wakes up every 5sec, sends every 5.2sec
      timerStr = "T:5sec";
    }
    else if(aRead<375){
      wdSetup= 7; //2sec
      timer = 15;  //wakes up every 30sec, sends every 31sec
      timerStr = "T:30sec";
    }
    else if(aRead<500){
      wdSetup= 8;  //4sec
      timer = 75;  //wakes up every 5min
      timerStr = "T:5min";
    }
    else if(aRead<625){
      wdSetup= 9;   //8sec
      timer = 225;  //wakes up every 30min
      timerStr = "T:30min";
    }
    else if(aRead<750){
      wdSetup= 9;   //8sec
      timer = 900;  //wakes up every 2h
      timerStr = "T:2h";
    }
    else if(aRead<875){
      wdSetup= 9;   //8sec
      timer = 2700;  //wakes up every 6h
      timerStr = "T:6h";
    }
    else{
      wdSetup= 9;     //8sec
      timer = 10800;  //wakes up every 24h
      timerStr = "T:24h";
    };

    //for the timer setup
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
    setup_watchdog(wdSetup); //set up the watchdog grrrrrr

  }
  else{
    masterSwitch = 1; //this sets it to become an interrupt
    timerStr="Interrupt";
  }

  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);

  digitalWrite(powerPin, LOW); //power saving mode enabled

  //transmitter code
  //vw_set_ptt_inverted(true); //I might need to re-enable this
  vw_set_tx_pin(0);
  vw_setup(1000);// speed of data transfer Kbps

  // SEND time setup values--------------- 
  int str_len = timerStr.length() + 1;  // Length (with one extra character for the null terminator) 
  char char_array[str_len]; // Prepare the character array (the buffer)   
  timerStr.toCharArray(char_array, str_len); // Copy it over
  vw_send((uint8_t *)char_array, strlen(char_array));
  vw_wait_tx(); // Wait until the whole message is gone

}


//Main Program Loop -----------------------------------------------------
void loop() {

  if(masterSwitch==0){//timer setups

    //Now put the system to sleep
    if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
      f_wdt=0;       // reset flag

      if(counter>=timer){ //if the counter has not reached the desired timer value, it goes back to sleep
        counter=0; //if counter has reached the timer value, it resets the counter and
        transmit(); //tiggers the transmit function
      }
      counter++;

      system_sleep();
    }
  }

  if(masterSwitch==1){//interrupt setup

    transmit();
    sleep(); 
  }
}


//----------------------Watchdog timer sleep code-------------------------------------------

// set system into the sleep state
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

    sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}

//----------------------Interrupt deep-sleep code-------------------------------------------

void sleep() {


  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts

} // sleep

ISR(PCINT0_vect) {
  // This is called when the interrupt occurs, but I don't need to do anything in it
}

//----------------------Transmit code-------------------------------------------

void transmit(){

  digitalWrite(powerPin, HIGH);  //provides power to the sensor


  if(wdSetup!=0 || masterSwitch==1){ //send values 

    // SEND CODE --------------- 
    int str_len = str.length() + 1;  // Length (with one extra character for the null terminator) 
    char char_array[str_len]; // Prepare the character array (the buffer)   
    str.toCharArray(char_array, str_len); // Copy it over
    vw_send((uint8_t *)char_array, strlen(char_array));
    vw_wait_tx(); // Wait until the whole message is gone

    Sensor1Data = analogRead(A2);

    itoa(Sensor1Data,Sensor1CharMsg,10);  // Convert integer data to Char array directly 

    vw_send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));
    vw_wait_tx(); // Wait until the whole message is gone

    digitalWrite(powerPin, LOW); //cuts power from the sensor

  }
  else{//sends continues values only as they change and the timer is set to 16ms

    Sensor1Data = analogRead(A2);

    if(Sensor1Data!=Sensor1DataOld){
      itoa(Sensor1Data,Sensor1CharMsg,10);  // Convert integer data to Char array directly 

      vw_send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));
      vw_wait_tx(); // Wait until the whole message is gone

      Sensor1DataOld=Sensor1Data;

    }
    digitalWrite(powerPin, LOW); //cuts power from the sensor
  }
}

