#include <arduino.h>

const int chipSelect = 10;         // For Due, the SS of SD Card is on pin 10. See SdFat library
const int bufsize = 1024;          // buffer size in bytes. You can change this data.

// Header of a wave file.
// Num_channels will tell us if we are in stereo (2) or mono (1)
typedef struct {
  char RIFF[4]; 
  int32_t chunk_size;
  char WAVE[4]; 
  char fmt_[4]; 
  int32_t subchunk1_size;
  int16_t audio_format;
  int16_t num_channels;
  int32_t sample_rate;
  int32_t byte_rate;
  int16_t block_align;
  int16_t bits_per_sample;
  char DATA[4]; 
  int32_t subchunk2_size;
} wave_header;



// Here is our sampler class
class Sampler
{
	
public:
   uint32_t volsample;         // volume applied to the sample (including enveloppe volume)
   uint32_t volglobal;         // volume max of the sample (when sustain)
   boolean play;               // is the sample playing or not ?
   int indbuf;                 // buffer number
   uint16_t bufread;           // buffer being read number
   uint16_t lastbuf;           // last buffer read number
   int16_t buf[2][bufsize];    // buffer
   uint32_t possample;         // sample position
   uint32_t endofsample;       // end of sample
   uint32_t decrease;          // decrease speed
   uint16_t samplenote;        // sample note
   
   SdFile myFile;              // The wave file
   wave_header header;         // The wave header
  

// Init of the sampler object. Here you can put your default values.
   void init()
   {
     volsample = 0;
     indbuf =0;
     bufread = 0;
     lastbuf = 0;
     play = false;
     volglobal = 800;	 
   }

// Play the sample, giving volume and note (volume 0-1024; note 0-128)
   void splay(uint32_t vol, uint16_t note)
   {
     
     play=true;
     volglobal = vol;
     samplenote=note;
   }

// load the wave header in the header data, and the beginning of the wave file in the buffer.
   void load(const char* samplename)
   {
     bufread = 0;
     lastbuf = 0;
     possample = 44;
     myFile.open(samplename, O_READ);
     myFile.read(&header, sizeof(header));
     endofsample = header.chunk_size;
     myFile.read(buf[0], sizeof(buf[0]));
     volsample = 0;
     decrease = endofsample-(2048*header.num_channels);
   }

//Stop playing the sample
   void sstop()
   {
     play=false;
     myFile.close();
   }

// Stop playing the sample, giving a note.
   void notestop(uint16_t note)
   {
     if(samplenote==note) 
     {
       decrease = possample;
       endofsample = possample + header.num_channels*2048;  
     }
   }

// Set the volume of the sample
   void setVol(uint32_t vol)
   {
     volglobal = vol;
   }

//Set the end of the sample
   void setEnd(uint32_t theEnd)
   {
     endofsample = (endofsample*theEnd)>>7;
   }

// Fill the buffer if it has to be. 
// This method must not be called in the sound generating loop.
   void buffill()
   {
     if(play)
     {
       if(bufread==lastbuf)
       {
         if(lastbuf==0) lastbuf = 1;
         else lastbuf = 0;
	 myFile.read(buf[lastbuf], sizeof(buf[lastbuf]));
       }
     }
   }
   
   
// Compute the position of the next sample. Must be called in the sound generating loop, before the method "output()"   
   void next()
   {
     if(play)  
     {
       // If the wave file is stereo, we take only one side (left). 
       indbuf += header.num_channels; 
       possample += (header.num_channels + header.num_channels);
	
       // The sample doesn't stop playing abruptly, because it can make some "clicks"
       // So it's decreasing rapidly after we stop it (like a release of an enveloppe)
       if(possample>=decrease)
       {
         if(volsample>0) volsample -= 8;
       }

      // The sample doesn't start abruptly. There is a quick attack to avoid "clicks"
      if(volsample<=volglobal&&possample<=1024)
      {
         volsample += 8;
      }
	
      // The reading of the file must stop at the end of file.
      if(possample>=(endofsample-1))
      {
         play = false;
         myFile.close();
      }

      // If a buffer is finished to be read, we read the other buffer.
      if(indbuf>=bufsize)
      {
        if(bufread==0) bufread = 1;
        else bufread = 0;
	       
        indbuf=0;
      }
    } 
  }

// Read the wave file at a position. Returns the volume (12 bits)
  int16_t output()
  {
    int16_t ret;
	   
    if(play)
    {
      ret = ((buf[bufread][indbuf]>>4)*volsample)>>7;
    }
    else ret = 0;
		 
    return ret;
  }
};
