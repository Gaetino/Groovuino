#include <arduino.h>

#define SAMPLE_LFO_RATE 500.0 
#define SAMPLES_LFO_PER_CYCLE 600 
#define SAMPLES_LFO_PER_CYCLE_FIXEDPOINT (SAMPLES_LFO_PER_CYCLE<<20) 
#define TICKS_LFO_PER_CYCLE (float)((float)SAMPLES_LFO_PER_CYCLE_FIXEDPOINT/(float)SAMPLE_LFO_RATE) 



class Lfo
{
  
public:
   uint16_t waveform;
   uint32_t ulPhaseAccumulator; 
   volatile uint32_t ulPhaseIncrement ;   
   float fFrequency;
   boolean play;
   uint32_t lfovol;
   uint32_t maincutoff;
   
   void init()
   {
    
     waveform = 0;

     ulPhaseAccumulator = 0; 
     ulPhaseIncrement = 0; 
     maincutoff = 64;	 

     lfovol = 100;	 
	 
	 play=true;
   }

   void setfreq(uint32_t freq)
   {
     fFrequency = (sqrt((float)freq+1.0)*50.0-48.0)/25.0*TICKS_LFO_PER_CYCLE;
     ulPhaseIncrement = fFrequency; 
   }

   void stop()
   {
     play = false;
   }
   
   void start()
   {
     play = true;
   }
   
   void setvol(uint32_t _vol)
   {
     lfovol = _vol;
   }
   
   void setmaincutoff(uint32_t data)
   {
     maincutoff = data;
   }
   
   void setWaveform(uint32_t val)
   {
     if(val<40) waveform = 0;
     if(val>=40&&val<80) waveform = 1;
     if(val>=80) waveform = 2;
   }
   
   void next()
   {
     if(play)
	 {
       ulPhaseAccumulator += ulPhaseIncrement;   
       if(ulPhaseAccumulator > SAMPLES_LFO_PER_CYCLE_FIXEDPOINT) 
       { 
         ulPhaseAccumulator -= SAMPLES_LFO_PER_CYCLE_FIXEDPOINT; 
       }
     }	   
   }		 

   
   uint16_t output()
   {
     int32_t ret=0;
     if(play)
	 {
       if(waveform == 0) ret+= (((nSineTable[ulPhaseAccumulator>>20]+2048)*lfovol)>>12)+maincutoff;
       if(waveform == 1) ret+= (((nSawTable[ulPhaseAccumulator>>20]+2048)*lfovol)>>12)+maincutoff;
       if(waveform == 2) ret+= (((nSqTable[ulPhaseAccumulator>>20]+2048)*lfovol)>>12)+maincutoff;
	   if(ret>=128) ret = 128;
	   if(ret<=1) ret = 1;
     }
     return ret;
   }
   
};
