// A large part of this code was taken from Duane B on RCArduino forum
// You must have a SD card connected to Arduino Due, with wavfiles samples on it :
// kick1.wav, snare1.wav, hihat1.wav


#include <MIDI.h>
#include <osc.h>
#include <env.h>
#include <SdFat.h>
#include <sampler.h>

#define POLIPHONY 1

const int samplernumber = 3;

SdFat sd;
Sampler sampler[samplernumber];


Osc oscA[POLIPHONY];
Env env1[POLIPHONY];

uint32_t notes[POLIPHONY];
boolean noteplay[POLIPHONY];

// Activate monophonic mode
boolean mono = true;
boolean samp = true;


void setup() 
{ 
  Serial.begin(9600); 
 
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
  
  sd.begin(chipSelect, SPI_FULL_SPEED);
  for(int i=0; i<samplernumber; i++)
  {
    sampler[i].init();
  }
  
  MIDI.begin(0);              // Launch MIDI with default options
				// (input channel is set to OMNI)

  fb = q+q/(1.0 - f);
  
  for(int i=0; i<POLIPHONY; i++)
  {
    oscA[i].init();
    env1[i].init();
    notes[i]=0;
    noteplay[i]=false;
  }
  
  
//  lastnote=0;
} 


//////////////////////////////////////
////////  MIDI ///////////////////////
//////////////////////////////////////

void loop() 
{ 

  if (MIDI.read()) {                    // Is there a MIDI message incoming ?
     
    switch(MIDI.getType()) {		// Get the type of the message we caught
      uint32_t voldata;
      uint32_t notedata;
      uint16_t channeldata;
      case NoteOn:               // If it is a Program Change
        
        voldata = MIDI.getData2();
        notedata = MIDI.getData1();
        channeldata = MIDI.getChannel();
        
// Drum channel = 2        
       if(channeldata==2)
       {
        if(voldata!=0)
        {
          if(notedata==40)
          {
            sampler[0].load("kick1.wav");
            sampler[0].splay(voldata<<3, notedata);
          }
          if(notedata==41)
          {
            sampler[1].load("snare1.wav");
            sampler[1].splay(voldata<<3,notedata);
          }
          if(notedata==42)
          {
            sampler[2].load("hithat1.wav");
            sampler[2].splay(voldata<<3,notedata);
          }
          
        }
        else
        {
          for(int i=0; i<samplernumber; i++)
          {
            sampler[i].notestop(notedata);
          }
        }
       }

// Synth channel = 1
       if(channeldata==1)
       { 
        
       if(mono)
       {
         if(voldata!=0)
         {
           if(noteplay[0])
           {
             oscA[0].setNote(notedata, voldata);
           }
           else
           {
             oscA[0].setNote(notedata, voldata);
             env1[0].start();
             noteplay[0]=true;
           }
           notes[0] = notedata;
         }
         else
         {
           if(notes[0]==notedata)
           {
             env1[0].stop();
             noteplay[0] = false;
           }
         }
       }
       else
       {
        if(voldata!=0)
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            if(!noteplay[i])
            {
              notes[i] = notedata;
              oscA[i].setNote(notedata, voldata);
              env1[i].start();
              noteplay[i]=true;
              i=100;
            }
          }
        }
        else
        {
          //env1[0].stop();
          
          for(int i=0; i<POLIPHONY; i++)
          {
            if(noteplay[i]&&notes[i]==notedata)
            {
              env1[i].stop();
              noteplay[i] = false;
            }
          }
        }
       }
       }

        break;
        
      case NoteOff:
        voldata = MIDI.getData2();
        notedata = MIDI.getData1();
        channeldata = MIDI.getChannel();
        
       
       if(channeldata==2)
       {
 
          for(int i=0; i<samplernumber; i++)
          {
            sampler[i].notestop(notedata);
          }
       }
       if(channeldata==1)
       { 
        
       if(mono)
       {
 
           if(notes[0]==notedata)
           {
             env1[0].stop();
             noteplay[0] = false;
           }
       }
       else
       {
           
          for(int i=0; i<POLIPHONY; i++)
          {
            if(noteplay[i]&&notes[i]==notedata)
            {
              env1[i].stop();
              noteplay[i] = false;
            }
          }
        }
       }
        break;


      case ControlChange:
       if(channeldata==1)
       {

      // Oscillateurs
        if(MIDI.getData1()==1) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setVolOsc1(MIDI.getData2());
          }
        }
        if(MIDI.getData1()==9) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setVolOsc2(MIDI.getData2());
          }
        }
        if(MIDI.getData1()==17) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setVolOsc3(MIDI.getData2());
          }
        }
      // Waveform  
        if(MIDI.getData1()==2) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setWaveform1(MIDI.getData2());
          }
        }
        if(MIDI.getData1()==10) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setWaveform2(MIDI.getData2());
          }
        }
        if(MIDI.getData1()==18) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setWaveform3(MIDI.getData2());
          }
        }

        // FINE
        if(MIDI.getData1()==3) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setFine1(MIDI.getData2());
          }
        }
        if(MIDI.getData1()==11) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setFine2(MIDI.getData2());
          }
        }
        if(MIDI.getData1()==19) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setFine3(MIDI.getData2());
          }
        }
        
        // ADSR
        if(MIDI.getData1()==69) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envA = 10000/(MIDI.getData2()*16+1);
          }
        }
        if(MIDI.getData1()==82) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envD = (524288-env1[i].envS)*50/((MIDI.getData2()+1)*44100);
          }
        }
        if(MIDI.getData1()==83) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envS = MIDI.getData2()*4096;
          }
        }
        if(MIDI.getData1()==84) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envR = env1[i].envS*200/((MIDI.getData2()+1)*44100);
          }
        }
        
        //filter
        if(MIDI.getData1()==91) {f = MIDI.getData2()/128.0; fb = q+q/(1.0 - f);}
        if(MIDI.getData1()==92) {q = MIDI.getData2()/128.0; fb = q+q/(1.0 - f);}
       // See the online reference for other message types
       }
      default:
        break;
    }
  }
  for(int i=0; i<samplernumber; i++)
   {
     sampler[i].buffill();
     delayMicroseconds(100);
   }
} 

float nextfilter(float in)
{
	buf0 = buf0 + f*(in-buf0 + fb*(buf0-buf1));
  buf1 = buf1 + f*(buf0-buf1);
                
	return (buf1*4096);
                
}

///////////////////////////////////
////// Sound generation ///////////
///////////////////////////////////
 
void TC4_Handler() 
{ 
 TC_GetStatus(TC1, 1); 
 
 uint32_t ulOutput=0;
 
  
  for(int i=0; i<POLIPHONY; i++)
  {
    oscA[i].next();
  }
  
  for(int i=0; i<POLIPHONY; i++)
  {
    ulOutput += (oscA[i].output() * env1[i].amount())>>19;
  }
  
  if(ulOutput>10) ulOutput = nextfilter(ulOutput)/16384;
  
   for(int i=0; i<samplernumber; i++)
  {
    sampler[i].next();
  }
   
  for(int i=0; i<samplernumber; i++)
  {
    ulOutput += sampler[i].output();
  }
  
  if(ulOutput>4095) ulOutput=4095;
   

  dacc_write_conversion_data(DACC_INTERFACE, ulOutput); 
}

