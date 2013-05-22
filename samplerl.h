#include <arduino.h>

const int chipSelect = 10;
const int bufsize = 1024;

const char* samplefile[]= {"kick1.wav", "hithat1.wav", "snare1.wav", "snare2.wav"};

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

class Samplerl
{
  
public:
   uint32_t volsample;
   uint32_t volglobal;
   uint32_t ulPhaseAccumulator; 
   volatile uint32_t ulPhaseIncrement ;  
   boolean play;
   int indbuf;
   uint16_t bufread;
   uint16_t lastbuf;
   int16_t buf[2][bufsize];
   uint32_t possample;
   uint32_t endofsample;
   uint32_t decrease;
   uint16_t samplenote;
   
   SdFile myFile;
   
   wave_header header;
   
   boolean openfile ;
   boolean closefile ;
   const char* samplen;
  
   
   void init()
   {
     ulPhaseAccumulator = 0; 
     indbuf =0;
	   bufread = 0;
	   lastbuf = 0;
     ulPhaseIncrement = 1;  
     play = false;
     volglobal = 800;	
     openfile = false;	
     closefile = false;	 
   }


   void splay(uint32_t vol, uint16_t note)
   {
     possample = sizeof(header);
     myFile.seekSet(sizeof(header));
	 
	   bufread = 0;
	   lastbuf = 0;
	 
     openfile=true;
	   volglobal = vol;
	   samplenote=note;
   }


   void load(const char* samplename)
   {
     bufread = 0;
	   lastbuf = 0;
	   samplen = samplename;
	 
     myFile.open(samplen, O_READ);
     myFile.read(&header, sizeof(header));
     endofsample = header.chunk_size-bufsize;
  
     myFile.read(buf[0], sizeof(buf[0]));
     possample = sizeof(buf[0]) + sizeof(header);
	 
	   volglobal = 0;
   }
   
    
   void sstop()
   {
     play=false;
   }
   
   void setVol(uint32_t vol)
   {
     volglobal = vol;
   }
   
   void setEnd(uint32_t theEnd)
   {
     endofsample = (endofsample*theEnd)>>7;
   }
   
   void close()
   {
     myFile.close();
	   play = false;
   }
   
      
   boolean buffill()
   {
     boolean ret = false;
     if(play||openfile)
	   {
       if(bufread==lastbuf)
	     {
	       if(possample>=(endofsample-bufsize)) play=false;
	       ret = true;
       
         if(lastbuf==0) lastbuf = 1;
         else lastbuf = 0;
			 
	       int me = myFile.read(buf[lastbuf], sizeof(buf[lastbuf]));
		     possample += sizeof(buf[lastbuf]);
		 
		     if(openfile) {play = true; openfile=false;}

         if(me<=0) 
         {
           play = false;
	       }
	     }
	   }
  	 return ret;
   }
   
   
   
   void next()
   {
     if(play)  
     {
		 
	     indbuf += header.num_channels;
	 
		   if(possample>=(endofsample-bufsize))
	  	 {
	 	     play = false;
		   }

	     if(indbuf>=bufsize)
	     {
		     if(bufread==0) bufread = 1;
	       else bufread = 0;
	       
		     indbuf=0;
		   }
     } 
   }
   
   int16_t output()
   {
 	   int16_t ret=0;

     if(play)
     {
	     ret = ((buf[bufread][indbuf]>>4)*volglobal)>>7;
     }
	   return ret;
   }
};
