// C64 USB Keyboard mod 
// Original by Pyofer 
// See original thread @ 
// http://www.lemon64.com/forum/viewtopic.php?t=55650 

#include <HID.h>
#include <Keyboard.h>
#include "keydefs.h"

int RowPinMap[4] = {0,1,2,3};
int ColPinMap[12] = {4,5,6,7,8,9,10,11,12,13,14,15};

uint16_t keyMap[48]={
TAB,  Q,W,E,R,T,Y,U,I,O,P,  BACKSPACE,
CTRL, A,S,D,F,G,H,J,K,L,    SEMICOLON, QUOTE,
SHIFT,Z,X,C,V,B,N,M,        COMMA, PERIOD, SLASH, SHIFT,
Z,    X,X,E,R,T,Y,U,I,X,X,  BACKSPACE,
};

// Select 0 for normal or 1 for the left HybridKey allowing all f keys and cursor keys in windows mode.
//(Also has a shifted restore key)
int HybridKeyboard=1;
// Position of the Hybrid Select key in the keymap, Left Shift = 17, Right Shift = 64
int HybridKey=17;
// Position of the Hybrid2 Select key in the keymap, CTRL = 72
int HybridKey2=72;

uint16_t Hybridkeys[7]{
A, F8, F2, F4, F6, A, F12,
};

uint16_t Hybridkeys2[5]{
PAGE_DOWN, F11, D, PAGE_UP, F9,
};

int inChar=0;
int keyPosition=0;
int keyDown[80];
int debounceDelay=50;
int shift=0;
int Row=0;
int i;
long lastDebounceTime[80];

void setup(){
  Keyboard.begin();

  for (i=0; i<80; i++) keyDown[i]=0; // Set all keys as up
  
  for (int Row=0; Row<sizeof(RowPinMap); Row++)
    pinMode(RowPinMap[Row], INPUT_PULLUP); 

  for (int Col=0; Col<sizeof(ColPinMap); Col++)
    pinMode(ColPinMap[Col], INPUT_PULLUP);
}

void loop(){
  int InputValue;
  int RowPin;

  for (int Row=0; Row<sizeof(RowPinMap); Row++){
    RowPin = RowPinMap[Row];
    pinMode(RowPin,OUTPUT);
    digitalWrite(RowPin,LOW);

    for (int Col=0; Col<sizeof(ColPinMap); Col++){
      keyPosition=Col+(Row*sizeof(ColPinMap));
      inChar=keyMap[keyPosition];
      InputValue = digitalRead(ColPinMap[Col]);

      if ((millis()-lastDebounceTime[keyPosition])>debounceDelay){ // debounce for each key individually
        if ((InputValue == LOW) && keyDown[keyPosition]==0){// if a key is pressed and wasn't already down
          keyDown[keyPosition]=inChar;
          if (HybridKeyboard==1){
            if (keyDown[HybridKey]&&keyDown[2])  {Keyboard.release (keyDown[HybridKey]);keyDown[keyPosition]=Hybridkeys[0];}

            if (keyDown[HybridKey2]&&keyDown[3])  {Keyboard.release (keyDown[HybridKey2]);keyDown[keyPosition]=Hybridkeys2[0];}
          }
            lastDebounceTime[keyPosition] = millis(); // reset the debounce delay
            Keyboard.press(keyDown[keyPosition]);    // pass the keypress to windows
        }
        if ((InputValue == HIGH) && keyDown[keyPosition]!=0){ // key is up and a character is stored in the keydown position
            lastDebounceTime[keyPosition] = millis();  // reset keybounce delay
            Keyboard.release(keyDown[keyPosition]);    // pass key release to windows
            keyDown[keyPosition]=0;
        }
      }
    }
    digitalWrite(RowPin,HIGH);
    delay(1);

    // Set output pin back to INPUT with pullup to make sure it stays HIGH
    pinMode(RowPin, INPUT_PULLUP);
  }
}
