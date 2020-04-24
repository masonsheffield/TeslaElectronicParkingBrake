#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield md;
const int buttonPin = 3; 
const int OpenTimeMS = 4000;
const int CloseTimeMS = 7000;
const int maxCloseCurrentMA = 5000;
const int maxOpenCurrentMA = 5000;

int buttonState = 0;  
int initButtonState = 0;

void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    while(1);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Dual VNH5019 Motor Shield");
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  md.init();
  initButtonState = digitalRead(buttonPin);
}
/*
 * Motor1 Moves to full speed for .8s and then senses current for clamping force
 * Motor1 Retracts for 3 seconds
 */
void loop()
{
 
 //Accelerate close speed from 0 to 400
   // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  if(buttonState != initButtonState){ 
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == HIGH) {//Engage Parking Brake
      for (int i = 0; i <= 400; i++)//Move Motor1 from 0 to 400 speed over 800ms  //accelerate without stall detection
      {       
        md.setM1Speed(i);
        md.setM2Speed(i);
        stopIfFault();
        if (i%200 == 100)
        {
          Serial.print("Motor Accelerating   Motor current:");
          Serial.println(md.getM1CurrentMilliamps());
          Serial.println(md.getM2CurrentMilliamps());
        }
        delay(2);
      }
    
    //Continue closing at max speed for 7 seconds or until motor stalls
      for (int i = 0; i <= (CloseTimeMS*0.5); i++)// Keep moving Motor1 at full speed until amp spike
      {
        if (i%200 == 100)
        {
          Serial.print("M1 Steady Clamping");
          Serial.print("M1 current: ");
          Serial.println(md.getM1CurrentMilliamps());
          if(md.getM1CurrentMilliamps() > maxCloseCurrentMA)
          {
            Serial.print("M1 Stalled");
            initButtonState = digitalRead(buttonPin);
            md.setM1Speed(0);
          }
          if(md.getM2CurrentMilliamps() > maxCloseCurrentMA)
          {
            Serial.print("M2 Stalled");
            initButtonState = digitalRead(buttonPin);
            md.setM2Speed(0);
          }
        }
        delay(2);
        
      }
 
      md.setM1Speed(0);
      md.setM2Speed(0);
      initButtonState = digitalRead(buttonPin);
    }

    if (buttonState == LOW){// RELEASE Parking Brake
      //accelerate motor in reverse for 0.8s
      for (int i = 0; i >= -400; i--)//Move Motor1 from 0 to -400 speed over 800ms
      {
        md.setM1Speed(i);
        md.setM2Speed(i);
        stopIfFault();
        if (i%200 == 100)
        {
          Serial.print("M1 current: ");
          Serial.println(md.getM1CurrentMilliamps());
          Serial.println(md.getM2CurrentMilliamps());
        }
        delay(2);
      }
    
    //continue reversing at max speed for 2s
    for (int i = 0; i <= (OpenTimeMS*0.5); i++)// Keep moving Motor1 at full speed until amp spike
    {
      if (i%200 == 100)
      {
        Serial.print("M1 current: ");
        Serial.println(md.getM1CurrentMilliamps());
        Serial.println(md.getM2CurrentMilliamps());
        if(md.getM1CurrentMilliamps() > maxOpenCurrentMA) md.setM1Speed(0);
        if(md.getM2CurrentMilliamps() > maxOpenCurrentMA) md.setM2Speed(0);
      }
      delay(2);     
    }
    //decelerate to 0 speed for 0.8s
        for (int i = -400; i >= 0; i++)//Move Motor1 from -400 to 0 speed over 1600ms
      {
        md.setM1Speed(i);
        md.setM2Speed(i);
        stopIfFault();
        if (i%200 == 100)
        {
          Serial.print("M1 current: ");
          Serial.println(md.getM1CurrentMilliamps());
          Serial.println(md.getM2CurrentMilliamps());       
        }
        delay(2);
      }
      md.setM1Speed(0);
      md.setM2Speed(0);
      initButtonState = digitalRead(buttonPin);
    }
  }
}
