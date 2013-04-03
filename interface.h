#include <arduino.h>

int funcbut[8] ;
int funcLED[8] ;
boolean funcbutPressed[8];
boolean funcbutState[8];
boolean funcbutRead[8];

boolean butstate[4][8];


int slider[4] ;

int modebut[4] ;
int modeLED[4] ;
boolean modebutPressed[4];
boolean modebutState[4];
boolean modebutRead[4];





int mode = 0;
int function = 0;

void init_interface()
{
  funcbut[0] = 23;
  funcbut[1] = 25;
  funcbut[2] = 27;
  funcbut[3] = 29;
  funcbut[4] = 31;
  funcbut[5] = 33;
  funcbut[6] = 35;
  funcbut[7] = 37;

  funcLED[0] = 22;
  funcLED[1] = 24;
  funcLED[2] = 26;
  funcLED[3] = 28;
  funcLED[4] = 30;
  funcLED[5] = 32;
  funcLED[6] = 34;
  funcLED[7] = 36;

  slider[0] = A3;   
  slider[1] = A4;   
  slider[2] = A5; 
  slider[3] = A6;  

  modebut[0] = 38;
  modebut[1] = 40;
  modebut[2] = 42;
  modebut[3] = 44;

  modeLED[0] = 39;
  modeLED[1] = 41;
  modeLED[2] = 43;
  modeLED[3] = 45;

  
  for (int a = 0; a < 8; a++)
  {
    pinMode(funcbut[a], INPUT);
    pinMode(funcLED[a], OUTPUT);
    
    funcbutPressed[a] = false;
    funcbutState[a] = false;
    funcbutRead[a] = false;
    
    digitalWrite(funcLED[a], LOW);

  }
  funcbutState[0] = true;
  digitalWrite(funcLED[0], HIGH);
  
  for (int a = 0; a < 4; a++)
  {
    pinMode(modebut[a], INPUT);
    pinMode(modeLED[a], OUTPUT);
  pinMode(slider[a], INPUT);
	
	modebutPressed[a] = false;
    modebutState[a] = false;
    modebutRead[a] = false;
	
	digitalWrite(modeLED[a], LOW);
  }
  modebutState[0] = true;
  digitalWrite(modeLED[0], HIGH);
  
  for (int a = 0; a < 4; a++)
  {
    for (int b = 0; b < 8; b++)
    {
	  butstate[a][b] = false;
	}
  }
  butstate[0][0] = true;
}

void load_funcbut()
{
  for (int a = 0; a < 8; a++)
  {
	digitalWrite(funcLED[a], LOW);
  }
  
  for (int a = 0; a < 8; a++)
  {
    funcbutState[a] = butstate[mode][a];
	if(funcbutState[a])	digitalWrite(funcLED[a], HIGH);
  }
}

void read_input()
{
  for (int a = 0; a < 8; a++)
  {
    funcbutRead[a] = digitalRead(funcbut[a]);
	
	if(funcbutRead[a] && !funcbutPressed[a] && !funcbutState[a])
	{
	  funcbutPressed[a] = true;
	  funcbutState[a] = true;
	  butstate[mode][a] = true;
	  digitalWrite(funcLED[a], HIGH);
	  
	  function = a;
	  
	  for(int b = 0; b < 8; b++)
      {
        if(b!=a && funcbutState[b])
        {
          funcbutState[b] = false;
		  butstate[mode][b] = false;
          digitalWrite(funcLED[b], LOW);
        }
      }
	}
	
	if(funcbutRead[a] && !funcbutPressed[a] && funcbutState[a])
    {
      funcbutPressed[a] = true;
    }
	
    if(!funcbutRead[a] && funcbutPressed[a])
    {
      funcbutPressed[a] = false;
    }
  }

  
  
  for (int a = 0; a < 4; a++)
  {
    modebutRead[a] = digitalRead(modebut[a]);
	
	if(modebutRead[a] && !modebutPressed[a] && !modebutState[a])
	{
	  modebutPressed[a] = true;
	  modebutState[a] = true;
	  digitalWrite(modeLED[a], HIGH);
	  
	  mode = a;
	  
	  for(int b = 0; b < 4; b++)
      {
        if(b!=a && modebutState[b])
        {
          modebutState[b] = false;
          digitalWrite(modeLED[b], LOW);
        }
      }
      load_funcbut();
	}
	
	if(modebutRead[a] && !modebutPressed[a] && modebutState[a])
    {
      modebutPressed[a] = true;
    }
	
    if(!modebutRead[a] && modebutPressed[a])
    {
      modebutPressed[a] = false;
    }
  }
}

void read_input_arp()
{
  for (int a = 0; a < 8; a++)
  {
    funcbutRead[a] = digitalRead(funcbut[a]);
	
	if(funcbutRead[a] && !funcbutPressed[a] && !funcbutState[a])
	{
	  funcbutPressed[a] = true;
	  funcbutState[a] = true;
	  butstate[mode][a] = true;
	  digitalWrite(funcLED[a], HIGH);
	}
	
	if(funcbutRead[a] && !funcbutPressed[a] && funcbutState[a])
	{
	  funcbutPressed[a] = true;
	  funcbutState[a] = false;
	  butstate[mode][a] = false;
	  digitalWrite(funcLED[a], LOW);
	}
	
	if(funcbutRead[a] && !funcbutPressed[a] && funcbutState[a])
    {
      funcbutPressed[a] = true;
    }
	
    if(!funcbutRead[a] && funcbutPressed[a])
    {
      funcbutPressed[a] = false;
    }
  }

  
  
  for (int a = 0; a < 4; a++)
  {
    modebutRead[a] = digitalRead(modebut[a]);
	
	if(modebutRead[a] && !modebutPressed[a] && !modebutState[a])
	{
	  modebutPressed[a] = true;
	  modebutState[a] = true;
	  digitalWrite(modeLED[a], HIGH);
	  
	  mode = a;
	  
	  for(int b = 0; b < 4; b++)
      {
        if(b!=a && modebutState[b])
        {
          modebutState[b] = false;
          digitalWrite(modeLED[b], LOW);
        }
      }
	  load_funcbut();
	}
	
	if(modebutRead[a] && !modebutPressed[a] && modebutState[a])
    {
      modebutPressed[a] = true;
    }
	
    if(!modebutRead[a] && modebutPressed[a])
    {
      modebutPressed[a] = false;
    }
  }
}
