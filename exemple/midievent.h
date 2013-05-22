const uint8_t MIDIVolOsc[NUM_OSC];
const uint8_t MIDIWaveform[NUM_OSC];
const uint8_t MIDIFine[NUM_OSC];

MIDIVolOsc[0] = 1;
MIDIWaveform[0] = 2;
MIDIFine[0] = 3;

MIDIVolOsc[1] = 9;
MIDIWaveform[1] = 10;
MIDIFine[1] = 11;

MIDIVolOsc[2] = 17;
MIDIWaveform[2] = 18;
MIDIFine[2] = 19;

const uint8_t MIDIEnvA = 69;
const uint8_t MIDIEnvD = 82;
const uint8_t MIDIEnvS = 83;
const uint8_t MIDIEnvR = 84;

const uint8_t MIDIFilterCut = 91;
const uint8_t MIDIFilterRes = 92;
const uint8_t MIDIFilterType = 93;

const uint8_t MIDIGlideTime = 97;


void midievent()
{
  if (MIDI.read()) {                    // Is there a MIDI message incoming ?
    
    switch(MIDI.getType()) {  	// Get the type of the message we caught
      uint32_t voldata;
      uint32_t notedata;

///////////////////////////////
////////  Note On /////////////
///////////////////////////////
      case NoteOn:               // If it is a NoteOn
        
        voldata = MIDI.getData2();
        notedata = MIDI.getData1();

// Monophonic
//-----------
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

// Polyphonic
//-----------
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
          for(int i=0; i<POLIPHONY; i++)
          {
            if(noteplay[i]&&notes[i]==notedata)
            {
              env1[i].stop();
              noteplay[i] = false;
            }
			oscA[i].stop(notedata);
          }
        }
       }

        break;

///////////////////////////////
////////  Note Off ////////////
///////////////////////////////

     case NoteOff:
        voldata = MIDI.getData2();
        notedata = MIDI.getData1();
        
       
// Monophonic
//-----------         
       if(mono)
       {
 
           if(notes[0]==notedata)
           {
             env1[0].stop();
             noteplay[0] = false;
           }
		   oscA[0].stop(notedata);
       }

// Polyphonic
//-----------
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
//////////////////////////////////////
////////  Control Change /////////////
//////////////////////////////////////
      case ControlChange:
      
// Oscillateurs
//-------------

// Volume
	   for(int i=0; i<NUM_OSC; i++)
	   {
        if(MIDI.getData1()==MIDIVolOsc[i]) 
        {
          for(int j=0; j<POLIPHONY; j++)
          {
            oscA[j].setVolOsc(i,MIDI.getData2());
          }
        }
	   }

// Waveform  
	   for(int i=0; i<NUM_OSC; i++)
	   {
        if(MIDI.getData1()==MIDIWaveform[i]) 
        {
          for(int j=0; j<POLIPHONY; j++)
          {
            oscA[j].setWaveform(i,MIDI.getData2());
          }
        }
	   }

// Fine tuning
	   for(int i=0; i<NUM_OSC; i++)
	   {
        if(MIDI.getData1()==MIDIFine[i]) 
        {
          for(int j=0; j<POLIPHONY; j++)
          {
            oscA[j].setFine(i,MIDI.getData2());
          }
        }
	   }
        
// ADSR
//-----
        if(MIDI.getData1()==MIDIEnvA) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envA = 10000/(MIDI.getData2()*16+1);
          }
        }
        if(MIDI.getData1()==MIDIEnvD) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envD = (524288-env1[i].envS)*50/((MIDI.getData2()+1)*44100);
          }
        }
        if(MIDI.getData1()==MIDIEnvS) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envS = MIDI.getData2()*4096;
          }
        }
        if(MIDI.getData1()==MIDIEnvR) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            env1[i].envR = env1[i].envS*200/((MIDI.getData2()+1)*44100);
          }
        }
        
//filter
//------
        if(MIDI.getData1()==MIDIFilterCut) {LP.setCutoffFreq(MIDI.getData2()<<1);}
        if(MIDI.getData1()==MIDIFilterRes) {LP.setResonance(MIDI.getData2()<<1);}
        if(MIDI.getData1()==MIDIFilterType) {LP.setType(MIDI.getData2());}
		
//Glide
//-----
		if(MIDI.getData1()==MIDIGlideTime) 
        {
          for(int i=0; i<POLIPHONY; i++)
          {
            oscA[i].setGlideTime(MIDI.getData2());
          }
        }
      default:
        break;
    }
  }
}
