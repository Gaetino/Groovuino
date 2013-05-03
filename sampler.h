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
   
   boolean openfile ;          // Are we opening file ?
   boolean closefile ;         // Are we closing file ?
   const char* samplen;        // Storing the sample name
  

// Init of the sampler object. Here you can put your default values.
   void init()
   {
     indbuf =0;
     bufread = 0;
     lastbuf = 0;
     play = false;
     volglobal = 800;	 
     openfile = false;	
     closefile = false;	 
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
     possample = 0;
     samplen = samplename;
     closefile = false;
     openfile = true;
   }

//Stop playing the sample
   void sstop()
   {
     if(play)
     {
       closefile = true;
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
// This method must not be called in the main program loop.
// The return of the method indicates that it is reading a sound file
   boolean buffill()
   {
     boolean ret = false;
     if(play)
     {
       // The buffer must be filled only if the previous buffer was finished to be read
       if(bufread==lastbuf)
       {
	 ret = true;
       	 // If we the sample read has arrived to the end of the file, we must stop the sample reading
         if(possample>=(endofsample-bufsize)) closefile=true;

         // OPEN FILE
	 if(openfile)
	 {
	   myFile.open(samplen, O_READ);
	   myFile.read(&header, sizeof(header));
	   endofsample = header.chunk_size-bufsize;
	   myFile.read(buf[0], sizeof(buf[0]));
	   possample = sizeof(buf[0]) + sizeof(header);
	   openfile = false;
 	 }
	 else
	 {
	   // CLOSE FILE
           if(closefile) 
           {
	     myFile.close();
	     play = false;
	   }
	   //READ FILE
	   else
	   {
	     if(lastbuf==0) lastbuf = 1;
	     else lastbuf = 0;
			 
	     int me = myFile.read(buf[lastbuf], sizeof(buf[lastbuf]));
	     possample += sizeof(buf[lastbuf]);
	     // If there is an error while reading sample file, we stop the reading and close the file
             if(me<=0) 
	     {
	       play = false;
	       myFile.close();
	     }
	   }
	 }
       }
     }
     return ret;
   }
 
   
// Compute the position of the next sample. Must be called in the sound generating loop, before the method "output()"   
   void next()
   {
     if(play)  
     {
       // If the wave file is stereo, we take only one side (left). 
       indbuf += header.num_channels; 
       possample += (header.num_channels + header.num_channels);
	
       // The reading of the file must stop at the end of file.
      if(possample>=(endofsample-bufsize))
      {
         closefile = true;
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
      ret = ((buf[bufread][indbuf]>>4)*volglobal)>>7;
    }
    else ret = 0;
		 
    return ret;
  }
};
