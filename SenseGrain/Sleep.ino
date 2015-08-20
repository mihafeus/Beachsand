
// Watchdog code by kukubee:     http://www.instructables.com/id/Multimeter-Backlit-LCD-Mod/step12/Program-The-ATtiny/ 
// Interrupt code by Dan TheMan: https://bigdanzblog.wordpress.com/2014/08/10/attiny85-wake-from-sleep-on-pin-state-change-code-example/


//----------------------Timer set-up sleep code-------------------------------------------

void timerSetup(){

  int aRead = analogRead(A1);

  if(aRead<1000){
    masterSwitch = 0;

    if(aRead<125){
      wdSetup= 7;               //2sec
      timer = 0;                //wakes up every 2sec to see if the values have crossed the treshold
      timerStr = "Treshold";
    }
    else if(aRead<250){
      wdSetup= 6;               //1sec
      timer = 5;                //wakes up every 5sec, sends every 5.2sec
      timerStr = "T:5sec";
    }
    else if(aRead<375){
      wdSetup= 7;               //2sec
      timer = 15;               //wakes up every 30sec, sends every 31sec
      timerStr = "T:30sec";
    }
    else if(aRead<500){
      wdSetup= 8;               //4sec
      timer = 75;               //wakes up every 5min
      timerStr = "T:5min";
    }
    else if(aRead<625){
      wdSetup= 9;               //8sec
      timer = 225;              //wakes up every 30min
      timerStr = "T:30min";
    }
    else if(aRead<750){
      wdSetup= 9;               //8sec
      timer = 900;              //wakes up every 2h
      timerStr = "T:2h";
    }
    else if(aRead<875){
      wdSetup= 9;               //8sec
      timer = 2700;             //wakes up every 6h
      timerStr = "T:6h";
    }
    else{
      wdSetup= 9;               //8sec
      timer = 10800;            //wakes up every 24h
      timerStr = "T:24h";
    };

    // The timer set up needs this bit
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
    setup_watchdog(wdSetup);           // Set up the watchdog grrrrrr

  }
  else{
    masterSwitch = 1;                 // This sets it to become an interrupt
    timerStr="Interrupt";
  }

} 

//----------------------Watchdog timer sleep code-------------------------------------------

// set system into the sleep state
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // Switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

    sleep_disable();                   // System continues execution here when watchdog timed out
  sbi(ADCSRA,ADEN);                    // Switch Analog to Digitalconverter ON
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

}

ISR(PCINT0_vect) {
  // This is called when the interrupt occurs, but I don't need to do anything in it
}



