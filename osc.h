#include <arduino.h>
#include <tables.h>
#include <clavinet01.h>
#include <ebass01.h>


#define SAMPLE_RATE 44100.0 
#define SAMPLES_PER_CYCLE 600 
#define SAMPLES_PER_CYCLE_FIXEDPOINT (SAMPLES_PER_CYCLE<<20) 
#define TICKS_PER_CYCLE (float)((float)SAMPLES_PER_CYCLE_FIXEDPOINT/(float)SAMPLE_RATE) 



class Osc
{
  
public:
   uint32_t volglb;
   uint32_t volglbsave;
   uint16_t waveform1;
   uint32_t volosc1;
   float fine1;

   uint16_t waveform2;
   uint32_t volosc2;
   float fine2;

   uint16_t waveform3;
   uint32_t volosc3;
   float fine3;
   
   uint32_t ulInput;
   
   uint32_t ulPhaseAccumulator1; 
   uint32_t ulPhaseAccumulator2; 
   uint32_t ulPhaseAccumulator3; 

   volatile uint32_t ulPhaseIncrement1 ;   
   volatile uint32_t ulPhaseIncrement2;   
   volatile uint32_t ulPhaseIncrement3;

   float fFrequency;
   
   void init()
   {
     ulInput = 100;

     volglb = 0;
     volglbsave = 0;
     
     waveform1 = 0;
     volosc1 = 64;
     fine1 = 0;

     waveform2 = 1;
     volosc2 = 64;
     fine2 = 0;

     waveform3 = 0;
     volosc3 = 64;
     fine3 = 0;
     
     ulPhaseAccumulator1 = 0; 
     ulPhaseAccumulator2 = 0; 
     ulPhaseAccumulator3 = 0; 

     ulPhaseIncrement1 = 0;   
     ulPhaseIncrement2 = 0;   
     ulPhaseIncrement3 = 0;
	 
	 
   }
   
   void setNote(uint32_t note, uint32_t vol)
   {
     ulInput = note;
     fFrequency = ((pow(2.0,((note)+(fine1/256)-69.0)/12.0)) * 440.0)*TICKS_PER_CYCLE; 
     ulPhaseIncrement1 = fFrequency; 
     fFrequency = ((pow(2.0,((note)+(fine2/256)-69.0)/12.0)) * 440.0)*TICKS_PER_CYCLE; 
     ulPhaseIncrement2 = fFrequency;
     fFrequency = ((pow(2.0,((note)+(fine3/256)-69.0)/12.0)) * 440.0)*TICKS_PER_CYCLE; 
     ulPhaseIncrement3 = fFrequency;

     if(vol!=0) volglb = vol;

   }
   
   void setVolOsc1(uint32_t vol)
   {
     volosc1 = vol;
   }
   
   void setVolOsc2(uint32_t vol)
   {
     volosc2 = vol;
   }
   
   void setVolOsc3(uint32_t vol)
   {
     volosc3 = vol;
   }
   
   void setWaveform1(uint32_t num)
   {
     if(num<25) waveform1 = 0;
     if(num>=25&&num<50) waveform1 = 1;
     if(num>=50&&num<75) waveform1 = 2;
     if(num>=75&&num<100) waveform1 = 3;
     if(num>=100) waveform1 = 4;
   }
   
   void setWaveform2(uint32_t num)
   {
     if(num<25) waveform2 = 0;
     if(num>=25&&num<50) waveform2 = 1;
     if(num>=50&&num<75) waveform2 = 2;
     if(num>=75&&num<100) waveform2 = 3;
     if(num>=100) waveform2 = 4;
   }
   
   void setWaveform3(uint32_t num)
   {
     if(num<25) waveform3 = 0;
     if(num>=25&&num<50) waveform3 = 1;
     if(num>=50&&num<75) waveform3 = 2;
     if(num>=75&&num<100) waveform3 = 3;
     if(num>=100) waveform3 = 4;
   }
   
   void setFine1(uint32_t val)
   {
     fine1 = val;
   }
   
   void setFine2(uint32_t val)
   {
     fine2 = val;
   }
   
   void setFine3(uint32_t val)
   {
     fine3 = val;
   }
   
   void next()
   {
     ulPhaseAccumulator1 += ulPhaseIncrement1;   
     ulPhaseAccumulator2 += ulPhaseIncrement2;
     ulPhaseAccumulator3 += ulPhaseIncrement3;
	 
     if(ulPhaseAccumulator1 > SAMPLES_PER_CYCLE_FIXEDPOINT) 
     { 
       ulPhaseAccumulator1 -= SAMPLES_PER_CYCLE_FIXEDPOINT; 
     } 
     if(ulPhaseAccumulator2 > SAMPLES_PER_CYCLE_FIXEDPOINT) 
     { 
       ulPhaseAccumulator2 -= SAMPLES_PER_CYCLE_FIXEDPOINT; 
     } 
     if(ulPhaseAccumulator3 > SAMPLES_PER_CYCLE_FIXEDPOINT) 
     { 
       ulPhaseAccumulator3 -= SAMPLES_PER_CYCLE_FIXEDPOINT; 
     } 
   }
   
   uint32_t output()
   {
       uint32_t ulOutputosc1;
  
       if(waveform1 == 0) ulOutputosc1 = nSineTable[ulPhaseAccumulator1>>20];
       if(waveform1 == 1) ulOutputosc1 = nSawTable[ulPhaseAccumulator1>>20];
       if(waveform1 == 2) ulOutputosc1 = nSqTable[ulPhaseAccumulator1>>20];
       if(waveform1 == 3) ulOutputosc1 = clavinet01[ulPhaseAccumulator1>>20];
       if(waveform1 == 4) ulOutputosc1 = ebass01[ulPhaseAccumulator1>>20];
 
       ulOutputosc1 = ulOutputosc1 * volosc1 / 128;
	   
       uint32_t ulOutputosc2;
	   
       if(waveform2 == 0) ulOutputosc2 = nSineTable[ulPhaseAccumulator2>>20];
       if(waveform2 == 1) ulOutputosc2 = nSawTable[ulPhaseAccumulator2>>20];
       if(waveform2 == 2) ulOutputosc2 = nSqTable[ulPhaseAccumulator2>>20];
       if(waveform2 == 3) ulOutputosc2 = clavinet01[ulPhaseAccumulator1>>20];
       if(waveform2 == 4) ulOutputosc2 = ebass01[ulPhaseAccumulator1>>20];
 
       ulOutputosc2 = ulOutputosc2 * volosc2 / 128;
	   
       uint32_t ulOutputosc3;
  
       if(waveform3 == 0) ulOutputosc3 = nSineTable[ulPhaseAccumulator2>>20];
       if(waveform3 == 1) ulOutputosc3 = nSawTable[ulPhaseAccumulator2>>20];
       if(waveform3 == 2) ulOutputosc3 = nSqTable[ulPhaseAccumulator2>>20];
       if(waveform3 == 3) ulOutputosc3 = clavinet01[ulPhaseAccumulator1>>20];
       if(waveform3 == 4) ulOutputosc3 = ebass01[ulPhaseAccumulator1>>20];
 
       ulOutputosc3 = ulOutputosc3 * volosc3 / 128;
	   
       uint32_t ret = ulOutputosc1 + ulOutputosc2 + ulOutputosc3;
	   
       return ret;
   }
   
};
