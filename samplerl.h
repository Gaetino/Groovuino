// If you want to play only one sample per voice, use this samplerl.h
// Else, if you want to swap between several samples on the same voice, use sampler.h

#include <arduino.h>

const int chipSelect = 10;
const int bufsize = 1024;
uint16_t n = 4;

const char* samplefile[]= {"kick1.wav","hithat1.wav" , "snare1.wav","snare2.wav", "kick2.wav", "snare3.wav" };

bool readChunk(void* buf, uint32_t startBlock, uint16_t blockCount)
{
  uint8_t* dst = (uint8_t*)buf;
  if(!sd.card()->readStart(startBlock)) return false;
  for(uint16_t i = 0; i < blockCount; i++)
  {
    if(!sd.card()->readData(dst + i*512L)) return false;
  }
  return sd.card()->readStop();
}


class Samplerl
{
  
public:
   uint32_t volglobal;
   uint32_t ulPhaseAccumulator; 
   volatile uint32_t ulPhaseIncrement ;  
   boolean play;
   int indbuf;
   uint16_t bufread;
   uint16_t lastbuf;
   int16_t buf[2][bufsize];

   uint32_t bgnBlock;
   uint32_t posBlock;
   uint32_t endBlock;
   
   uint32_t endData;
   uint32_t posData;
   uint32_t bgnData;
   
   SdFile myFile;
   
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


   void splay(uint32_t vol)
   {
     bufread = 0;
     lastbuf = 0;
	 
     openfile=true;
     volglobal = vol;
	 
     posData = 0;
     posBlock = bgnBlock;
     indbuf = bgnData;
   }


   void load(const char* samplename)
   {
     bufread = 0;
     lastbuf = 0;
     samplen = samplename;
	 
     myFile.open(samplen, O_READ);
     Serial.println(myFile.contiguousRange(&bgnBlock, &endBlock));
      
     posBlock = bgnBlock;

     readChunk(buf[0], posBlock, n);
	 
     int dataind=18;

     if((char)(buf[0][18]&0x00ff) != 'd')
     {
       for (int i = 18; i < 1023; i++) { 
         if((char)(buf[0][i]&0x00ff)=='d') if((char)((buf[0][i]&0xff00)>>8)=='a') {dataind = i; i=1025;}
	 if((char)((buf[0][i]&0xff00)>>8)=='d') if((char)(buf[0][i+1]&0x00ff)=='a')  {dataind = i; i=1025;}
       }
       endData = ((uint16_t)buf[0][dataind+2])>>1;
     } 
     else endData = ((uint16_t)buf[0][20])>>1;

     bgnData = dataind+4; //  the buffer is 1024 16 bits integer long. So 4 x 512 bytes. (4 blocks)
     posData = 0;
     indbuf = bgnData;
	 
     posBlock +=4;
	 
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
   
   boolean buffill()
   {
     boolean ret = false;
     if(play||openfile)
     {
       if(bufread==lastbuf)
       {
         ret = true;
         if(lastbuf==0) lastbuf = 1;
         else lastbuf = 0;

         readChunk(buf[lastbuf], posBlock, n);
         posBlock +=4;		   

 	 if(openfile) 
	 {
	   play = true; 
	   openfile=false; 
	 }
       }
     }
     return ret;
   }
   
   
   
   void next()
   {
     if(play)  
     {
		 
       indbuf ++;
       posData ++;
	 
       if(posData>=(endData)) play = false; 

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
       if(play) ret = ((buf[bufread][indbuf]>>4)*volglobal)>>7;
       return ret;
   }
};
