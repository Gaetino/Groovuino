#include <arduino.h>
#include <tables.h>
#include <clavinet01.h>
#include <ebass01.h>

#define NUM_OSC 3 

#define SAMPLE_RATE 44100.0 
#define SAMPLES_PER_CYCLE 600 
#define SAMPLES_PER_CYCLE_FIXEDPOINT (SAMPLES_PER_CYCLE<<20) 
#define TICKS_PER_CYCLE (float)((float)SAMPLES_PER_CYCLE_FIXEDPOINT/(float)SAMPLE_RATE) 



class Osc
{
	
public:
   uint32_t volglb;
   uint32_t volglbsave;
   uint16_t waveform[NUM_OSC];
   uint32_t volosc[NUM_OSC];
   float fine[NUM_OSC];
   uint32_t ulPhaseAccumulator[NUM_OSC]; 
   volatile uint32_t ulPhaseIncrement[NUM_OSC] ;   
   int8_t octave[NUM_OSC];
//GLIDE
   uint8_t noteplaying;
   boolean glideon;
   boolean glidestart;
   boolean play;
   uint32_t glidetime;
   uint32_t Incrementglide[NUM_OSC];
   uint32_t Incrementfin[NUM_OSC];


   float fFrequency;
   
   
   void init()
   {
     volglb = 0;
     volglbsave = 0;
     
     waveform[0] = 0;
     volosc[0] = 64;
     fine[0] = 0;

     waveform[1] = 1;
     volosc[1] = 64;
     fine[1] = 0;

     waveform[2] = 0;
     volosc[2] = 64;
     fine[2] = 0;
     
     ulPhaseAccumulator[0] = 0; 
     ulPhaseAccumulator[1] = 0; 
     ulPhaseAccumulator[2] = 0; 

     ulPhaseIncrement[0] = 0;   
     ulPhaseIncrement[1] = 0;   
     ulPhaseIncrement[2] = 0;
	 
     octave[0] = 0;
     octave[1] = 0;
     octave[2] = -1;
	 
//GLIDE
     glideon = true;
     glidestart = false;
     Incrementglide[0]=0;
     Incrementfin[0]=0;
     Incrementglide[1]=0;
     Incrementfin[1]=0;
     Incrementglide[2]=0;
     Incrementfin[2]=0;
     noteplaying = 0;
     glidetime = 1000; // (en ms)
     play = false;
   }
   
   void setNote(uint32_t note, uint32_t vol)
   {
	
     if(note!=noteplaying && glideon && play)
     {
       for(int i=0; i<NUM_OSC; i++)
       {
         fFrequency = ((pow(2.0,((note+12*octave[i])+(fine[i]/256)-69.0)/12.0)) * 440.0)*TICKS_PER_CYCLE; 
         Incrementfin[i] = fFrequency;
	 Incrementglide[i] = abs((int)Incrementfin[i] - (int)ulPhaseIncrement[i])*1000/44100/glidetime;
	 if(Incrementglide[i]==0) Incrementglide[i] = 1;
       }
     }
     else
     {
       for(int i=0; i<NUM_OSC; i++)
       {
         fFrequency = ((pow(2.0,((note+12*octave[i])+(fine[i]/256)-69.0)/12.0)) * 440.0)*TICKS_PER_CYCLE; 
         ulPhaseIncrement[i] = fFrequency; 
	 Incrementfin[i] = fFrequency; 
       }
       play = true;
     }
     if(vol!=0) 
     {
       volglb = vol;
       noteplaying = note;
     }
     else play = false;
   }

   void stop(uint8_t note)
   {
     if(note==noteplaying) play = false;
   }
   
   void setVolOsc(uint8_t num, uint32_t vol)
   {
     volosc[num] = vol>>1;
   }
   
   void setWaveform(uint8_t num, uint32_t val)
   {
     if(val<25) waveform[num] = 0;
     if(val>=25&&val<50) waveform[num] = 1;
     if(val>=50&&val<75) waveform[num] = 2;
     if(val>=75&&val<100) waveform[num] = 3;
     if(val>=100) waveform[num] = 4;
   }
  
   void setFine(uint8_t num, uint32_t val)
   {
     fine[num] = val;
   }
   
   void setGlideTime(uint32_t glt)
   {
     glidetime = glt * 10+1;
   }
   
   void next()
   {
     for(int i=0; i<NUM_OSC; i++)
     {
       ulPhaseAccumulator[i] += ulPhaseIncrement[i];   
       if(ulPhaseAccumulator[i] > SAMPLES_PER_CYCLE_FIXEDPOINT) 
       { 
         ulPhaseAccumulator[i] -= SAMPLES_PER_CYCLE_FIXEDPOINT; 
       } 
       if(ulPhaseIncrement[i]<Incrementfin[i]) 
       {
         if((Incrementfin[i]-ulPhaseIncrement[i])<=Incrementglide[i])
         {
           ulPhaseIncrement[i] = Incrementfin[i];
         }
         else ulPhaseIncrement[i] += Incrementglide[i];
       }
       if(ulPhaseIncrement[i]>Incrementfin[i]) 
       {
         if((ulPhaseIncrement[i]-Incrementfin[i])<=Incrementglide[i])
         {
           ulPhaseIncrement[i] = Incrementfin[i];
         }
         else ulPhaseIncrement[i] -= Incrementglide[i];
       }
     }		 
   }
   
   uint32_t output()
   {
     uint32_t ret=0;
     for(int i=0; i<NUM_OSC; i++)
	 {
       if(waveform[i] == 0) ret+= (nSineTable[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
       if(waveform[i] == 1) ret+= (nSawTable[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
       if(waveform[i] == 2) ret+= (nSqTable[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
       if(waveform[i] == 3) ret+= (clavinet01[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
       if(waveform[i] == 4) ret+= (ebass01[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
     }
     return ret;
   }
   
};
