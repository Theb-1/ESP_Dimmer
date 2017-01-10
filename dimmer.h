#include "Arduino.h"

#ifndef DIMMER_H
#define DIMMER_H
class Dimmer
{
  public:
    Dimmer(int _ac_pin, int _button_pin);
    void Begin();
    void SetLevel(int _level, boolean _store);
    void SetState(boolean _state);
    void ResetCounter();
    int GetLastLevel();
    void ZeroCross(boolean _zero_cross);
    boolean GetState();
    boolean CheckButton();
    void On();
    void Off();
    void Dim(); 
  private:
    volatile int i = 0;
    volatile boolean zero_cross;
    int ac_pin;
    int button_pin;
    int LastLevel;
    boolean state = 0;
    boolean prevstate = 1;
    int level;
    int reading;
    
    

};

#endif

