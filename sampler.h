#include <arduino.h>

const int chipSelect = 10;
const int bufsize = 512;


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




class Sampler
{
  
public:
   uint32_t volsample;
   uint32_t ulPhaseAccumulator; 
   volatile uint32_t ulPhaseIncrement ;  
   boolean play;
   int indbuf;
   uint16_t bufread;
   uint16_t lastbuf;
   int16_t buf[2][bufsize];
   uint32_t possample;
   uint32_t endofsample;
   
   SdFile myFile;
   
   wave_header header;
  
   
   void init()
   {
     volsample = 64;
     ulPhaseAccumulator = 0; 
     indbuf =0;
	 bufread = 0;
	 lastbuf = 0;
     ulPhaseIncrement = 1;  
     play = false;	 
	 endofsample = header.chunk_size;
   }
   
   void splay(uint32_t vol)
   {
     play=true;
	 volsample = vol;
   }
   
   void load(const char* samplename)
   {
     bufread = 0;
	 lastbuf = 0;
	 possample = 44;
     myFile.open(samplename, O_READ);
	 myFile.read(&header, sizeof(header));
	 myFile.read(buf[0], sizeof(buf[0]));
   }
   
   void sstop()
   {
     play=false;
	 myFile.close();
   }
   
   void setNote(uint32_t note, uint32_t vol)
   {
     ulPhaseIncrement = 1; 
	 if(vol!=0) volsample = vol;
   }
   
   void setVol(uint32_t vol)
   {
     volsample = vol;
   }
   
   void setEnd(uint32_t theEnd)
   {
     endofsample = (endofsample*theEnd)>>7;
   }
      
   void buffill()
   {
     if(play)
	 {
       if(bufread==lastbuf)
	   {
	     if(lastbuf==0) lastbuf = 1;
	     else lastbuf = 0;
         /*if(myFile.read(buf[lastbuf], sizeof(buf[lastbuf]))<=0) 
	     {
	       play = false;
	       myFile.close();
		 }*/
		 myFile.read(buf[lastbuf], sizeof(buf[lastbuf]));
	   }
	 }
   }
   
   
   
   void next()
   {
     if(play)  
     {
	     indbuf += header.num_channels;
		 possample += (header.num_channels + header.num_channels);
		 
		 if(possample>=(endofsample-128))
		 {
		   if(volsample>0) volsample--;
		 }
		 
		 if(possample>=(endofsample-1))
		 {
		   play = false;
	       myFile.close();
		 }

	     if(indbuf>=bufsize)
	     {
		   if(bufread==0) bufread = 1;
	       else bufread = 0;
	       
		   indbuf=0;
		 }
     } 
   }
   
   uint32_t output()
   {
       uint32_t ret;
	   
       if(play)
	   {
         ret = (((buf[bufread][indbuf]+32768)>>4)*volsample)>>7;
       }
	   else ret = 0;
		 
	   return ret;
   }
   
};
