const int BUFRIB = 30;

uint32_t stockribbon[BUFRIB];
uint32_t indicestock = 0;

uint32_t volsynth = 127;

uint32_t pot1save = 0;
uint32_t pot2save = 0;
uint32_t pot3save = 0;
uint32_t pot4save = 0;
boolean noteplay[1];



int ctrlmode = 2;

uint32_t moyenneStock()
{
  uint32_t ret = 0;
  for(int i=0; i<BUFRIB; i++)
  {
    ret += stockribbon[i];
  }
  return (ret/BUFRIB);
}

uint32_t ecartStock(int val)
{
  uint32_t ret = 0;
  for(int i=0; i<BUFRIB; i++)
  {
    if(ret < abs((int)stockribbon[i]-val)) ret = abs((int)stockribbon[i]-val);
  }
  return (ret);
}

void analogcontroller_init()
{
  for(int i=0; i<BUFRIB; i++)
  {
    stockribbon[i]=508;
  }
}

void analogcontroller_read()
{
    uint32_t ribbondata = analogRead(0);
    uint32_t distdata = analogRead(1);
    uint32_t pressdata = analogRead(2);
	
//--------------------------------
//           MODE 0
//--------------------------------
    if(mode==0)
	{
// ----- CONTROL MODE 0
    if(ctrlmode==0)
    {
    if(ecartStock(508)<9)
    {
      env1[0].stop();
      noteplay[0]=false;
    }
    else
    {
      if(ecartStock(moyenneStock())<3)
      {
      if(!noteplay[0])
      {
        oscA[0].setNote((ribbondata/84)+36, 100);
        env1[0].start();
        noteplay[0]=true;
      }
      else
      {
        oscA[0].setNote((ribbondata/84)+36, 100);
      }
      }
    }
    if(indicestock == (BUFRIB-1)) indicestock = 0;
    else indicestock++;
    stockribbon[indicestock] = ribbondata;
    delay(3);
    }
	
// ----- CONTROL MODE 1	
    if(ctrlmode==1)
    {
      if(distdata>300)
      {
        uint32_t calrib = (ribbondata/84)+36;
        volsynth = distdata>>3;
        
        if(!noteplay[0])
        {
          noteplay[0] = true;
          notes[0] = calrib;
          oscA[0].setNote(notes[0], volsynth);
          env1[0].start();
        }
        else
        {
          
          if(notes[0] != calrib)
          oscA[0].setNote(calrib, volsynth);
        }
        
      }
      if(distdata<=300)
      {
        if(noteplay[0])
        {
          env1[0].stop();
          noteplay[0]=false;
        }
      }
      delay(3);
    }
// ----- CONTROL MODE 2
    if(ctrlmode==2)
    {
      if(pressdata<1000)
      {
        uint32_t calrib = (ribbondata/84)+36;
        volsynth = (1023-pressdata)>>3;
        
        if(!noteplay[0])
        {
          noteplay[0] = true;
          notes[0] = calrib;
          oscA[0].setNote(notes[0], volsynth);
          env1[0].start();
        }
        else
        {
          
          if(notes[0] != calrib)
          oscA[0].setNote(calrib, volsynth);
        }
        
      }
      if(pressdata>=1000)
      {
        if(noteplay[0])
        {
          env1[0].stop();
          noteplay[0]=false;
        }
      }
      delay(3);
    }
	}
 
 //--------------------------------
//           MODE 1
//--------------------------------
    if(mode==1)
    {
      if(ecartStock(508)>9)
      {
        if(ecartStock(moyenneStock())<3)
        {
          transpo = (((int)ribbondata-508)/150)*12;
	  load_arpeg();
	}
      }
      if(indicestock == (BUFRIB-1)) indicestock = 0;
      else indicestock++;
      stockribbon[indicestock] = ribbondata;
      delay(3);
	  
      change_length_arpeg((1024-distdata)>>3);
    }

//***************************************************************************
	
    uint32_t pot1 = analogRead(3)>>3;
    uint32_t pot2 = analogRead(4)>>3;
    uint32_t pot3 = analogRead(5)>>3;
    uint32_t pot4 = analogRead(6)>>3;
    
	
//================================
//       POTENTIOMETER 1
//================================	
    if(pot1>(pot1save+3)||pot1<(pot1save-3))
    {
      Serial.println("pot1");
 //------ SYNTH MODE -------
      if(mode==0)
      {
        // OSC1
        if(function==0)
        {
          oscA[0].setVolOsc(0,pot1);
        }
        // OSC2
        if(function==1)
        {
          oscA[0].setVolOsc(1,pot1);
        }
		// OSC3
        if(function==2)
        {
          oscA[0].setVolOsc(2,pot1);
        }
		// Attack volume enveloppe 
        if(function==3)
        {
          env1[0].setA(pot1);
        }
		// filter
        if(function==6)
        {
          LP.setCutoffFreq(pot1<<1);
        }
      }
  //------ ARPEGGIATOR MODE -------- 
      if(mode==1)
      {
	if(pot1<25)
	{
	  arpmode = 0;
	}

        if(pot1>=25 && pot1<50)
        {
	  arpmode = 1;
	  load_arpeg();
	}
		
	if(pot1>=50 && pot1<75)
	{
	  arpmode = 2;
	  load_arpeg();
	}
		
	if(pot1>=75 && pot1<100)
	{
	  arpmode = 3;
	  load_arpeg();
	}
		
	if(pot1>=100 && pot1<128)
	{
	  arpmode = 4;
	  load_arpeg();
	}
      }
      pot1save = pot1;
    }
	
//================================
//       POTENTIOMETER 2
//================================	
    if(pot2>(pot2save+3)||pot2<(pot2save-3))
    {
 //------ SYNTH MODE -------
      if(mode==0)
      {
        // OSC1
        if(function==0)
        {
          oscA[0].setWaveform(0,pot2);
        }
        if(function==1)
        {
          oscA[0].setWaveform(1,pot2);
        }
        if(function==2)
        {
          oscA[0].setWaveform(2,pot2);
        }
		// Decay volume enveloppe 
        if(function==3)
        {
          env1[0].setD(pot2);
        }
		// filter
        if(function==6)
        {
          LP.setResonance(pot2<<1);
        }
		if(function==7)
        {
          bpm = pot2*2+60;
		  setTimerBPM();
		  delay(100);
        }
      }
//------ ARPEGGIATOR MODE -------- 
      if(mode==1)
      {
      if(pot2<20)
	{
	  arpspeed = 1;
	  load_arpeg();
	}
	if(pot2>=20 && pot2<40)
	{
	  arpspeed = 2;
	  load_arpeg();
	}
	if(pot2>=40 && pot2<60)
	{
	  arpspeed = 3;
	  load_arpeg();
	}
	if(pot2>=60 && pot2<80)
	{
	  arpspeed = 4;
	  load_arpeg();
	}
	if(pot2>=80 && pot2<100)
	{
	  arpspeed = 5;
	  load_arpeg();
	}
	if(pot2>=100 && pot2<128)
	{
	  arpspeed = 6;
	  load_arpeg();
	}
      }
      pot2save = pot2;
    }
	
//================================
//       POTENTIOMETER 3
//================================	
    if(pot3>(pot3save+3)||pot3<(pot3save-3))
    {
 //------ SYNTH MODE -------
      if(mode==0)
      {
        // OSC1
        if(function==0)
        {
          oscA[0].setFine(0,pot3);
        }
        if(function==1)
        {
          oscA[0].setFine(1,pot3);
        }
        if(function==2)
        {
          oscA[0].setFine(2,pot3);
        }
		// Release volume enveloppe 
        if(function==3)
        {
          env1[0].setS(pot3);
		  
        }
      }
//------ ARPEGGIATOR MODE -------- 
      if(mode==1)
      {
	if(pot3<25)
	{
	  arpgamme = 0;
	  load_arpeg();
	}
	if(pot3>=25 && pot3<50)
	{
	  arpgamme = 1;
	  load_arpeg();
	}
	if(pot3>=50 && pot3<75)
	{
	  arpgamme = 2;
	  load_arpeg();
	}
	if(pot3>=75 && pot3<100)
	{
	  arpgamme = 3;
	  load_arpeg();
	}
	if(pot3>=100 && pot3<128)
	{
	  arpgamme = 4;
	  load_arpeg();
	}
      }
      pot3save = pot3;
    }

//================================
//       POTENTIOMETER 4
//================================	
    if(pot4>(pot4save+3)||pot4<(pot4save-3))
    {
 //------ SYNTH MODE -------
      if(mode==0)
      {
	if(function==3)
        {
          env1[0].setR(pot4);
        }
      }
    }
}


