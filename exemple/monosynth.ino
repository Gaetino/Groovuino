#include <MIDI.h>
#include <osc.h>
#include <env.h>
#include <Filter.h>


#define POLIPHONY 1

Osc oscA[POLIPHONY];
Env env1[POLIPHONY];

uint32_t notes[POLIPHONY];
boolean noteplay[POLIPHONY];

boolean mono = true;
boolean samp = true;
boolean midion = true;

Filter LP;


#include <midiev.h>

void setup() 
{ 
  Serial.begin(9600); 
  
  pinMode(A0, INPUT);
 
  createNoteTable(SAMPLE_RATE); 
  createSineTable();
  createSawTable();  
  createSqTable();  
   
  /* turn on the timer clock in the power management controller */ 
  pmc_set_writeprotect(false); 
  pmc_enable_periph_clk(ID_TC4); 
 
  /* we want wavesel 01 with RC */ 
  TC_Configure(/* clock */TC1,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2); 
  TC_SetRC(TC1, 1, 235); // sets  <> 44.1 Khz interrupt rate 
  TC_Start(TC1, 1); 
   
  // enable timer interrupts on the timer  
  TC1->TC_CHANNEL[1].TC_IER=TC_IER_CPCS; 
  TC1->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS; 
   
  /* Enable the interrupt in the nested vector interrupt controller */ 
  /* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1 */ 
  NVIC_EnableIRQ(TC4_IRQn); 
 
  // this is a cheat - enable the DAC 
  analogWrite(DAC1,0); 
  
  MIDI.begin();              // Launch MIDI with default options
				// (input channel is default set to 1)

  LP.setCutoffFreq(255);
  LP.setResonance(1);
  LP.setType(0);
  
  for(int i=0; i<POLIPHONY; i++)
  {
    oscA[i].init();
    env1[i].init();
    notes[i]=0;
    noteplay[i]=false;
  }

} 
 
void loop() 
{ 
  if(midion) 
  {
    midievent();
    delay(2);
  }
} 

  
void TC4_Handler() 
{ 
  // We need to get the status to clear it and allow the interrupt to fire again 
  TC_GetStatus(TC1, 1); 
 
  uint32_t ulOutput=0;
  
  oscA[0].next();

  ulOutput += oscA[0].output();
   
  if(ulOutput>10) ulOutput = (uint32_t)LP.next((int)ulOutput - 2048)+2048;
  
  ulOutput = (ulOutput * env1[0].amount())>>19;
  
  if(ulOutput>4095) ulOutput=4095;
   
  dacc_write_conversion_data(DACC_INTERFACE, ulOutput); 
}
