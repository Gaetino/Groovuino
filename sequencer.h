boolean noteon[96];
uint8_t seqvol[96];
uint8_t seqpit[96];
uint8_t seqlen[96];
uint8_t durrest;



void init_step()
{
  for(int i=0; i<96; i++)
  {
    noteon[i] = false;
    seqvol[i] = 0;
	  seqpit[i] = 0;
	  seqlen[i] = 0;
  }
  
  noteon[0] = true;
  seqvol[0] = 100;
  seqpit[0] = 50;
  seqlen[0] = 8;
  noteon[15] = true;
  seqvol[15] = 100;
  seqpit[15] = 50;
  seqlen[15] = 8;
  noteon[31] = true;
  seqvol[31] = 100;
  seqpit[31] = 50;
  seqlen[31] = 8;
}
