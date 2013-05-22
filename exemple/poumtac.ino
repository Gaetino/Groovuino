// Simple sampler which alternates a kick and a snare

#include <SdFat.h>
#include <sampler.h>
#include <DueTimer.h> 

SdFat sd;
Sampler sampler;

void setup() 
{ 

  Timer0.attachInterrupt(loop1).setFrequency(44100).start();
  analogWrite(DAC1,0); 

    sd.begin(chipSelect, SPI_FULL_SPEED);
    sampler.init();
}

void loop() 
{
   if(to>100000) 
   {
     to=0;
 // POUM
     sampler.load("kick1.wav");
     sampler.splay(100,1);
   }
   
   if(to==50000) 
   {
 // TCHACK
     sampler.load("snare1.wav");
     sampler.splay(100,1);
   }
   
   sampler.buffill();
   
   delayMicroseconds(10);
   
   to++;
} 

void loop1() 
{ 

// 2048 is the 0 value of audio out.
   int16_t ulOutput=2048;

   sampler.next();
   
   ulOutput += sampler.output();

   if(ulOutput>4095) ulOutput=4095;
   dacc_write_conversion_data(DACC_INTERFACE, ulOutput); 
}
