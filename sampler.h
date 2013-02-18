#include <arduino.h>

const int chipSelect = 10;
const int bufsize = 1024;


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
   uint32_t volglobal;
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
  
   
   void init()
   {
     volsample = 0;
     indbuf =0;
     bufread = 0;
     lastbuf = 0;
     play = false;
     volglobal = 800;	 
   }
   
   void splay(uint32_t vol, uint16_t note)
   {
     
     play=true;
     volglobal = vol;
     samplenote=note;
   }
   
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
   
   void sstop()
   {
     play=false;
     myFile.close();
   }
   
   void notestop(uint16_t note)
   {
     if(samplenote==note) 
     {
       decrease = possample;
       endofsample = possample + header.num_channels*2048;  
     }
   }
   
   void setVol(uint32_t vol)
   {
     volglobal = vol;
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
		 
       if(possample>=decrease)
       {
         if(volsample>0) volsample -= 1;
       }
		 
      if(volsample<=volglobal&&possample<=1024)
      {
         volsample += 8;
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
      ret = (((buf[bufread][indbuf]+32768)>>4)*volsample)>>10;
    }
    else ret = 0;
		 
    return ret;
  }
};
