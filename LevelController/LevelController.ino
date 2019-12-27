/* Copyright (C) 2019 Mievo - All Rights Reserved
 */
 
#include "Includes.h"

//__CONFIG(INTIO & WDTDIS & PWRTDIS & MCLREN & BORDIS & UNPROTECT & UNPROTECT);
__CONFIG(INTIO & WDTDIS & PWRTDIS & MCLRDIS & BORDIS & UNPROTECT & UNPROTECT);

//voltage control values
#define tankLidUpper   5.2
#define tankLidLower  3.0

#define tankBottomUpper 1.0
#define tankBottomLower 0.0

#define manualUpper   5.2
#define manualLower   4.8

#define semiUpper     3.2
#define semiLower     2.9

#define autoUpper     1.3
#define autoLower     0.8

//relay variable
int relayOn = 0;

//mode variable
#define manualMode  0
#define semiMode    1
#define autoMode    2

int mode            = manualMode;
int interruptCount  = 0;
/*
unsigned max(unsigned int a, unsigned int b, unsigned int c)
{
  int temp;
  if(a > b)
    temp = a;
  else
    temp = b;
  if(temp < c)
    temp = c;
  return temp;
}
*/
void interrupt ISR(void)
{
  if(INTF && interruptCount >= 1)
  { 
    switch(mode)
    {
      case (int)manualMode:
        if(relayOn == 0)
          relayOn = 1;
        else
          relayOn = 0;
      break;

      case (int)semiMode:
          relayOn = 1;
      break;
    }
  }
  if(interruptCount <= 0)
    interruptCount++;
  INTF  = 0;
}

void main()
{ 
  float tankLid     = 0.0;
  float tankBottom    = 0.0;
  float slider      = 0.0;
  
  unsigned int ADC_value1 = 0;
  unsigned int ADC_value2 = 0;
  unsigned int ADC_value3 = 0;
  
  unsigned int val1 = 0;
  unsigned int val2 = 0;
  unsigned int val3 = 0;
  
  InitADC();
  
  //TRIS0 = 1;
  //TRIS1 = 1;
  TRIS2 = 1;
  //TRIS4 = 1;    
  //setting relay for output
  TRIS5 = 0;
  GPIO5 = 0;
  //setting interrupt
  INTCON  |= 0X90;

  while(1)
  {   
    ADC_value3 = GetADCValue(AN3);
    DelayMs(50);          // Half second delay before next reading
    
    slider = (5.0 * ADC_value3)/1024.0;

    if(slider >= (float)manualLower && slider <= (float)manualUpper)
    {
      mode = (int)manualMode;
    }
    else if(slider >= (float)semiLower && slider <= (float)semiUpper)
    {
      mode = (int)semiMode;
    }
    else if(slider >= (float)autoLower && slider <= (float)autoUpper)
    {
      mode = (int)autoMode;
    }
    else
    {
      mode = manualMode;
    } 

    switch(mode)
    {
      case (int)manualMode: 
        if(relayOn == 1)
        { GPIO5 = 1; }
        else
        { GPIO5 = 0; }
      break;
    
      case (int)semiMode:
        /*val1 = GetADCValue(AN0);
        DelayMs(50);
        val2 = GetADCValue(AN0);
        DelayMs(50);
        val3 = GetADCValue(AN0);
        DelayMs(50);
        ADC_value1 = max(val1,val2,val3);*/
        ADC_value1 = GetADCValue(AN0);
        DelayMs(50);
        
        tankLid = (5.0 * ADC_value1)/1024.0;

        if(tankLid >= (float)tankLidLower && tankLid <= (float)tankLidUpper)
        {
          relayOn = 0;
        }   

        if(relayOn == 1)
        { GPIO5 = 1; }
        else
        { GPIO5 = 0; }
      break;

      case (int)autoMode:
        ADC_value1 = GetADCValue(AN0);
        DelayMs(50);
        ADC_value2 = GetADCValue(AN1);
        DelayMs(50);

        tankLid   = (5.0 * ADC_value1)/1024.0;
        tankBottom  = (5.0 * ADC_value2)/1024.0;

        if(tankLid >= (float)tankLidLower && tankLid <= (float)tankLidUpper)
        {
          relayOn  = 0;
        } 
        if(tankBottom >= (float)tankBottomLower && tankBottom <= (float)tankBottomUpper)
        {
          relayOn = 1;
        }

        if(relayOn == 1)
        { GPIO5 = 1; }
        else
        { GPIO5 = 0; }
      break;
    }
  }
}
              
