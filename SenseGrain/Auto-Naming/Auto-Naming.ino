//#include <TrueRandom.h>
#include <EEPROM.h>
char crka;
String letter;
String name = "";
boolean conso = false;
boolean vowel = false;
int shiftkey = 64;
boolean nameWritten = false;

byte value;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
}

void loop() {
  whatsmyname();
  //delay(50);
}

void whatsmyname(){

  //crka = random(25)+65;
  if(name==""){
    shiftkey = 65;
  }
  else{
    shiftkey = 97;
  }

  crka = random(26)+shiftkey;
  //crka = TrueRandom.random(0,26)+shiftkey;

  letter = String(crka);
  //Serial.print(letter);

  if(letter == "A" || letter == "E" || letter =="I" || letter =="O" || 
  letter =="U" || letter == "a" || letter == "e" || letter =="i" || letter =="o" || 
  letter =="u"){

    if(vowel == false){
      name += letter;
      vowel = true;
      conso = false;
    }
  }
  else{

    if(conso == false){
      name += letter;
      vowel = false;
      conso = true;
    }
  }

  if(name.length()==4){
    //name += String(TrueRandom.random(1,10));
    name += String(random(1,10));
    //Serial.println(name);
    vowel = false;
    conso = false;
    //name="";

    if(nameWritten == false){
      for (int i = 0; i < name.length(); i++){
        EEPROM.write(i, name[i]);
        nameWritten = true;
      }
    } 
    name="";
    for (int i = 0; i < 5; i++){
      value = EEPROM.read(i);
      crka = value;
      name += String(crka); 
      Serial.println(name);
    }
  }
}

