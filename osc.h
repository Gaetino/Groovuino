#include <arduino.h>
#include <tables.h>

// Change this to change the number of oscillators per voice
#define NUM_OSC 1 

// Can adjust the Sample rate
#define SAMPLE_RATE 44100.0 

// Number of samples played in one cycle (a cycle depends on the frequency of the note played)
#define SAMPLES_PER_CYCLE 600 

#define SAMPLES_PER_CYCLE_FIXEDPOINT (SAMPLES_PER_CYCLE<<20) 
#define TICKS_PER_CYCLE (float)((float)SAMPLES_PER_CYCLE_FIXEDPOINT/(float)SAMPLE_RATE) 


// This class represents a voice of synth. It can have several oscillators
class Osc
{
	
public:
   int32_t volglb;                               // Volume global
   int32_t volglbsave;                           // Volume global temporary save  
   uint16_t waveform[NUM_OSC];                    // Waveform of each oscillator 
   int32_t volosc[NUM_OSC];                      // Volume of each oscillator
   float fine[NUM_OSC];                           // Fine of each oscillator
   uint32_t ulPhaseAccumulator[NUM_OSC];          // Position in the reading of each oscillator table 
   volatile uint32_t ulPhaseIncrement[NUM_OSC] ;  // Reading speed of each oscillaotr table 
   int8_t octave[NUM_OSC];                        // Octave of each oscillator

//GLIDE
   uint8_t noteplaying;                           // true : a note is already playing - false : no note is playing
   boolean glideon;                               // true : glide is ON - false : glide is OFF
   boolean glidestart;                            // true : glide already started - false : glide is not started
   boolean play;                                  
   uint32_t glidetime;                            // glide time in ms
   uint32_t Incrementglide[NUM_OSC];              // glide speed
   uint32_t Incrementfin[NUM_OSC];                // target frequency


   float fFrequency;
   
// initialize the synth. Here you can put your default values.   
   void init()
   {
     volglb = 0;
     volglbsave = 0;
     
     waveform[0] = 0;
     volosc[0] = 64;
     fine[0] = 0;

     waveform[1] = 0;
     volosc[1] = 0;
     fine[1] = 0;

     waveform[2] = 0;
     volosc[2] = 0;
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
     glideon = false;
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
   
   void setFrequency(uint32_t note)
   {
      for(int i=0; i<NUM_OSC; i++)
      {
         fFrequency = ((pow(2.0,((note+12*octave[i])+(fine[i]/256)-69.0)/12.0)) * 440.0)*TICKS_PER_CYCLE; 
         ulPhaseIncrement[i] = fFrequency; 
      }
      noteplaying = note;
   }

// Set the note (frequency), and volume 
   void setNote(uint32_t note, int32_t vol)
   {
// If we are in glide mode and the synth is not playing, we have to compute the glide increment (= glide speed)	
// We do not have to change the frequency value, it's the glide increment which will change it
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
//If we are not in glide mode, compute the frequency of the new note
     else
     {
       for(int i=0; i<NUM_OSC; i++)
       {
         fFrequency = ((pow(2.0,((note+12*octave[i])+(fine[i]/256)-69.0)/12.0)) * 440.0)*TICKS_PER_CYCLE; 
         ulPhaseIncrement[i] = fFrequency; 
	 Incrementfin[i] = fFrequency; 
       }
       // The synth is playing
       play = true;
     }
//Fill the volume data
     noteplaying = note;
     
   }

// Stop the playing of the synth
   void stop(uint8_t note)
   {
     if(note==noteplaying) play = false;
   }

// Set the volume of one oscillator
   void setVolOsc(uint8_t num, int32_t vol)
   {
     volosc[num] = vol>>1;
   }

// Set the waveform of one oscillator
   void setWaveform(uint8_t num, uint32_t val)
   {
     if(val<40) waveform[num] = 0;
     if(val>=40&&val<80) waveform[num] = 1;
     if(val>=80) waveform[num] = 2;
   }

// Set the fine tune of one oscillator
   void setFine(uint8_t num, uint32_t val)
   {
     fine[num] = val;
   }

//Set the glide time
   void setGlideTime(uint32_t glt)
   {
     glidetime = glt * 10+1;
   }

// Compute the table position in the wavetable from increment values
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

//Return the sample value from the wavetable 
   int32_t output()
   {
     int32_t ret=0;
     for(int i=0; i<NUM_OSC; i++)
     {
       if(waveform[i] == 0) ret+= (nSineTable[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
       if(waveform[i] == 1) ret+= (nSawTable[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
       if(waveform[i] == 2) ret+= (nSqTable[ulPhaseAccumulator[i]>>20]* volosc[i])>>7;
     }
     return ret;
   }
   
};
