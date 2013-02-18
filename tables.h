#define SAMPLE_RATE 44100.0 
#define SAMPLES_PER_CYCLE 600 
#define SAMPLES_PER_CYCLE_FIXEDPOINT (SAMPLES_PER_CYCLE<<20) 
#define TICKS_PER_CYCLE (float)((float)SAMPLES_PER_CYCLE_FIXEDPOINT/(float)SAMPLE_RATE) 

#define WAVE_SAMPLES 600 
uint16_t nSineTable[WAVE_SAMPLES]; 
uint16_t nSawTable[WAVE_SAMPLES]; 
uint16_t nSqTable[WAVE_SAMPLES];

#define MIDI_NOTES 128 
uint32_t nMidiPhaseIncrement[MIDI_NOTES]; 


void createSineTable() 
{ 
  for(uint32_t nIndex = 0;nIndex  < WAVE_SAMPLES;nIndex++) 
  { 
    // normalised to 12 bit range 0-4095 
    nSineTable[nIndex] = (uint16_t)  (((1+sin(((2.0*PI)/WAVE_SAMPLES)*nIndex))*4095.0)/2); 
    //Serial.println(nSineTable[nIndex]); 
  } 
} 

void createSawTable() 
{ 
  for(uint32_t nIndex = 0;nIndex  < WAVE_SAMPLES;nIndex++) 
  { 
    nSawTable[nIndex] = (uint16_t)  (nIndex*(4095.0/600)); 
  } 
} 

void createSqTable() 
{ 
  for(uint32_t nIndex = 0; nIndex  < WAVE_SAMPLES; nIndex++) 
  { 
    if(nIndex<(WAVE_SAMPLES/2)) nSqTable[nIndex] = 0;
    if(nIndex>=(WAVE_SAMPLES/2)) nSqTable[nIndex] = 4095; 
  } 
} 
