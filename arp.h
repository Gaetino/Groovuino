uint8_t arpmode = 0;
uint8_t arpspeed = 3;
int8_t arphigh = 0;
uint8_t arpgamme = 0;

int8_t transpo = 0;

int8_t gamme[8];

boolean arpbut[8] = {0,0,0,0,0,0,0,0};
uint8_t length = 8;

void fill_gamme()
{
  if(arpgamme==0)
  {
    gamme[0] = 60 + transpo;
    gamme[1] = 63 + transpo;
	  gamme[2] = 65 + transpo;
	  gamme[3] = 67 + transpo;
	  gamme[4] = 70 + transpo;
	  gamme[5] = 72 + transpo;
	  gamme[6] = 75 + transpo;
	  gamme[7] = 77 + transpo;
  }  
  if(arpgamme==1)
  {
    gamme[0] = 60 + transpo;
	  gamme[1] = 61 + transpo;
	  gamme[2] = 64 + transpo;
	  gamme[3] = 65 + transpo;
	  gamme[4] = 67 + transpo;
	  gamme[5] = 68 + transpo;
  	gamme[6] = 70 + transpo;
  	gamme[7] = 72 + transpo;
  }
  if(arpgamme==2)
  {
    gamme[0] = 60 + transpo;
	  gamme[1] = 62 + transpo;
	  gamme[2] = 64 + transpo;
	  gamme[3] = 65 + transpo;
	  gamme[4] = 67 + transpo;
	  gamme[5] = 69 + transpo;
	  gamme[6] = 71 + transpo;
	  gamme[7] = 72 + transpo;
  }
  if(arpgamme==3)
  {
    gamme[0] = 60 + transpo;
	  gamme[1] = 62 + transpo;
	  gamme[2] = 63 + transpo;
	  gamme[3] = 65 + transpo;
	  gamme[4] = 67 + transpo;
	  gamme[5] = 68 + transpo;
	  gamme[6] = 70 + transpo;
	  gamme[7] = 72 + transpo;
  }
  if(arpgamme==4)
  {
    gamme[0] = 60 + transpo;
	  gamme[1] = 62 + transpo;
  	gamme[2] = 63 + transpo;
	  gamme[3] = 65 + transpo;
	  gamme[4] = 67 + transpo;
	  gamme[5] = 69 + transpo;
	  gamme[6] = 70 + transpo;
	  gamme[7] = 72 + transpo;
  }
}

void load_arpeg()
{
  fill_gamme();
  
  int modulo = 96;
  if(arpspeed==1) modulo = 4;
  if(arpspeed==2) modulo = 6;
  if(arpspeed==3) modulo = 8;
  if(arpspeed==4) modulo = 12;
  if(arpspeed==5) modulo = 24;
  if(arpspeed==6) modulo = 48;
  
  int indgamme=0;

  for(int i = 0; i<96; i++)
  {
	if(i%modulo==0)
	{
	  noteon[i] = true;
	  seqvol[i] = 100;
    seqpit[i] = 50;
    seqlen[i] = length;
	  
	  for(int j = 0; j<8; j++)
    {
	    if(funcbutState[indgamme])
		  {
		    seqpit[i] = gamme[indgamme];
		    j = 9;
		  
		  }
		  if(indgamme<7) indgamme ++;
		  else indgamme = 0;
	  }
	}	
	else noteon[i] = false;
  }
  
}

void change_length_arpeg(uint32_t clength)
{
  if(arpspeed==1) length = clength/26+2;
  if(arpspeed==2) length = clength/16+2;
  if(arpspeed==3) length = clength/11+2;
  if(arpspeed==4) length = clength/7+2;
  if(arpspeed==5) length = clength/3+2;
  if(arpspeed==6) length = clength+2;
	
  for(int i = 0; i<96; i++)
  {
    seqlen[i] = length;
   
  }
}

uint8_t cal_nb_notes()
{
  uint8_t ret = 0;
  for(int i=0; i<8; i++)
  {
    if(funcbutState[i]) ret++;
  }
  return ret;
}
