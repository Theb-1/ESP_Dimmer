#include "Dimmer.h"

Dimmer::Dimmer( int _ac_pin, int _button_pin)
{
  ac_pin = _ac_pin;
  button_pin = _button_pin;
}

void Dimmer::SetLevel(int _level, boolean _store)
{
  if ( _store == 1 ) {
  LastLevel = level;
  }
  level = _level;
}

int Dimmer::GetLastLevel()
{
  return LastLevel;
}

void Dimmer::On()
{
  digitalWrite(ac_pin, HIGH);
}

boolean Dimmer::GetState()
{
  return state;
}

void Dimmer::Off()
{
  digitalWrite(ac_pin, LOW);
}

void Dimmer::SetState(boolean _state)
{
  state = _state;
}

boolean Dimmer::CheckButton()
{
  reading = digitalRead(button_pin);
if ( reading == !prevstate ){
    prevstate = !prevstate;
    state = !state;
    level = 0;    
    return true;
}
else
    return false;

prevstate = reading;    
}

void Dimmer::ResetCounter()
{
  i=0;
}

void Dimmer::ZeroCross(boolean _zero_cross)
{
  zero_cross = _zero_cross;
}

void Dimmer::Dim()
{
  if ( state == true ) {
    if(zero_cross == true) {              
    if(i >= level) {                     
      digitalWrite(ac_pin, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
    } 
    else {
      i++; // increment time step counter                     
    }                                
  } 
  }
}

void Dimmer::Begin()
{
  pinMode(ac_pin, OUTPUT); 
  pinMode(button_pin, INPUT_PULLUP);
}
