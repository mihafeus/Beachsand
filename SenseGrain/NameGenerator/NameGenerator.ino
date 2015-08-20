#include <EEPROM.h>
char Char;
String letter;
String name = "";
boolean conso = false;
boolean vowel = false;
int shiftkey = 64;
boolean nameWritten = false;
byte value;

void setup() {
  randomSeed((analogRead(A2)*analogRead(A1)*analogRead(A3))/3);
}

void loop() {
  whatsmyname();
}

void whatsmyname(){
  
  randomSeed((analogRead(A2)*analogRead(A1)*analogRead(A3))/3);
  
  if(name==""){
    shiftkey = 65;
  }
  else{
    shiftkey = 97;
  }

  Char = random(26)+shiftkey;

  letter = String(Char);

  if(letter == "A" || letter == "E" || letter =="I" || letter =="O" || 
    letter =="U" || letter == "a" || letter == "e" || letter =="i" || 
    letter =="o" || letter =="u"){

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

    name += String(random(1,10));
    vowel = false;
    conso = false;

    if(nameWritten == false){
      for (int i = 0; i < name.length(); i++){
        EEPROM.write(i, name[i]);
        nameWritten = true;
      }
    } 
  }
}
