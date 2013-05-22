//This exemple is a simple mono 3-osc synth MIDI controlled
// It needs the DueTimer library

#include <MIDI.h>
#include <osc.h>
#include <env.h>
#include <Filter.h>
#include <DueTimer.h>


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
   
  Timer0.attachInterrupt(loop1).setFrequency(44100).start();
 
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

  
void loop1() 
{ 
  uint32_t ulOutput=2048;
  
  oscA[0].next();

  ulOutput += LP.next((oscA[0].output()*(int32_t)env1[0].amount())/524288);
  
  if(ulOutput>4095) ulOutput=4095;
  if(ulOutput<0) ulOutput=0;
   
  dacc_write_conversion_data(DACC_INTERFACE, ulOutput); 
}
