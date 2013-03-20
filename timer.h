uint32_t bpm = 120;

void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency)
{
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency;
  TC_SetRA(tc, channel, rc/2);
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

void setFrequency(Tc *tc, uint32_t channel, uint32_t frequency)
{
  uint32_t rc = VARIANT_MCK/128/frequency;
  TC_SetRA(tc, channel, rc/2);
  TC_SetRC(tc, channel, rc); 
  TC_Start(tc, channel);  
}

void setTimerBPM()
{
  uint32_t bps = bpm*16/60;
  setFrequency(TC1, 0, bps);
  
}
