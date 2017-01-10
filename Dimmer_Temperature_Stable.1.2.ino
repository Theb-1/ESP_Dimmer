  #include "dimmer.h"
  #include "hw_timer.h"
  #include <DallasTemperature.h>
  #include <OneWire.h>  
  #include <EEPROM.h>
  #include <SPI.h>
  
  #define MY_DEBUG 
  #define MY_BAUD_RATE 9600
  #define MY_GATEWAY_ESP8266
  #define MY_ESP8266_SSID "privatny"
  #define MY_ESP8266_PASSWORD "yehfx8i4"
  #define MY_PORT 5003      
  
  #include <ESP8266WiFi.h>
  #include <MySensors.h>
  
  #define SN "2 Channel Dimmer & Temp"
  #define SV "1.2"
  #define CHANNELS 2
  volatile float temperature;
  int freqStep = 100;
  unsigned long previousMillis = 0;
  const long interval = 300000;
  unsigned long previousMillis2 = 0;
  const long interval2 = 70;
  int Status;
  
  OneWire oneWire(13);
  DallasTemperature sensors(&oneWire);
  MyMessage msg(0,V_TEMP);
  MyMessage dimmerMsg[CHANNELS] = {{0,V_DIMMER},{1,V_DIMMER}};
  MyMessage lightMsg[CHANNELS] = {{0,V_LIGHT},{1,V_LIGHT}};
  Dimmer channel[CHANNELS] = {{5,12},{16,14}};
  
  
  void ICACHE_RAM_ATTR zero_cross_detect() {
        
  for ( int i = 0 ; i < CHANNELS ; i++ ){
    channel[i].Off();                                 //Turn off SSR right after zero crossed
    channel[i].ZeroCross(true);                       //Tell dimmer to start counting 
    channel[i].ResetCounter();                        //From 0

  }
  } 
  void ICACHE_RAM_ATTR dim_check() {                   
  for ( int i = 0 ; i < CHANNELS ; i++ ){             // start Dim per channel
    channel[i].Dim();                                 //
  }
   
  }  
  void Button(){
   for ( int i = 0 ; i < CHANNELS ; i++ ){            //Read Buttons change per channel
    if (channel[i].CheckButton() == true)             // and if state changed
    send(lightMsg[i].set(channel[i].GetState()));     // Send current state to controller.
   } 
  }
  
  void setup() {
  //Serial.begin(9600);
  noInterrupts();
  sensors.begin();
  sensors.setResolution(12);
  sensors.setWaitForConversion(false);
    
  for ( int i = 0 ; i < CHANNELS ; i++ ){
    channel[i].Begin();
    channel[i].SetLevel(70,true);
    send(lightMsg[i].set(0));
  }
    attachInterrupt(4, zero_cross_detect, RISING);    // Attach an Interupt to GPIO 4 for Zero Cross Detection
    
    
  
  //hw_timer_init(FRC1_SOURCE, 1);
  hw_timer_init(NMI_SOURCE, 1);
  hw_timer_set_func(dim_check);
  hw_timer_arm(freqStep);

    //timer1_attachInterrupt(dim_check);
    //timer1_isr_init();
    //timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
    //timer1_write(clockCyclesPerMicrosecond() * 100); 
    //timer1_write((clockCyclesPerMicrosecond() / 16) * 85); 
    interrupts();
    
    //delay(10000);
    //wdt_enable(15000);
  }                            
                                   
  void GetTemp(){
    temperature = sensors.getTempCByIndex(0);       //Get temperature in C
    sensors.requestTemperatures();                  //Request temperature for next loop
    send(msg.set(temperature,1));                   //Send current readings to controller
  }   
  
  void presentation() {
  present( 0, S_TEMP);        // present temp to controller
  present( 0, S_DIMMER );     // present dimmer channel 1 to controller
  present( 1, S_DIMMER );     // present dimmer channel 2 to controller
  sendSketchInfo(SN, SV);     // present sketch info to controller
  
  } 
  
  void receive(const MyMessage &message)
  {
      if (message.type == V_LIGHT)  {                   // Only if valid V_LIGHT message Type
        Status = message.getBool();                     // Get Boolean 
        channel[message.sensor].SetState(Status);       //Set Boolean Status
      }
      
      if (message.type == V_DIMMER) {                                                     // Only if valid V_DIMMER message type
          int requestedLevel = atoi( message.data );                                      // convert message
          channel[message.sensor].SetLevel(map ( requestedLevel, 0, 100, 100, 1), true);  // Set diming level ( reversed as 0 is full open and 100 is no dimming at all )
          channel[message.sensor].SetState(true);                                         // Turn on dimming 
      }
  }
  
  void loop() {     
    
  unsigned long currentMillis = millis();
  
    if (currentMillis - previousMillis > interval) {
      GetTemp();
      previousMillis = currentMillis;
      
    }
    
    if (currentMillis - previousMillis2 > interval2) {
      Button();
      previousMillis2 = currentMillis;
    }
    delay(0);
  }
  
  
  

