#include <arduino.h>

class Env
{
  
public:


  uint32_t envA;
  uint32_t envD;
  uint32_t envS;
  uint32_t envR;

  uint32_t accu;

  boolean phaseA;
  boolean phaseD;
  boolean phaseS;
  boolean phaseR;

  void init()
  {
    envA = 30000;
    envD = 20000;
    envS = 200000;
    envR = 30;
	  accu = 0;
	  phaseA = false;
	  phaseD = false;
	  phaseS = false;
	  phaseR = false;
  }
  
  void start()
  {
    phaseA = true;
    phaseD = false;
    phaseS = false;
    phaseR = false;
  }
  
  void stop()
  {
    phaseA = false;
    phaseD = false;
    phaseS = false;
    phaseR = true;
  }
  
  uint32_t amount()
  {
// ATTACK
    if(phaseA)
    {
      if(accu >= 524288)
      {
        phaseA = false;
        phaseD = true;
      }
      else
      {
        accu += envA;
      }
    }

// SUSTAIN
// We do nothing
  
// DECAY
    if(phaseD)
    {

      accu = accu - envD;
    
      if(accu<=envS)
      {
        phaseD = false;
        phaseS = true;
      }
    
    }   
 
  
// RELEASE
    if(phaseR)
    {
      if(accu>=envR)
      {
        accu -= envR;
      }
      else 
	  {
	    phaseR = false;
		accu = 0;
      }
    }
	
	return accu;
  }
};
