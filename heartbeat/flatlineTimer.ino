
/**
 * Detects when there has not been a heartbeat for a long time.
 *
 * Is to be restarted at every heartbeat.
 * Will trigger interrupt every 5 seconds.
 */

const int CLOCKFREQ = 8000 / (2) * 1000; // FIXME;

/**
 * Set up the flatline timer to be able to run.
 */
void setupFlatlineTimer() {
  // Configure and enable GCLK4 for TC:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(4); // do not divide gclk 4
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;
  // use GCLK->GENCTRL.reg and GCLK->CLKCTRL.reg
  GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_ID(4) | GCLK_GENCTRL_IDC | GCLK_GENCTRL_SRC_OSC8M;
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(4) | GCLK_CLKCTRL_ID(0x1B);
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;

  // Disable TC (for now)
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
    ;
  TC3->COUNT16.INTENCLR.reg |= TC_INTENCLR_MC0;
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
    ;

  // Set up NVIC
  NVIC_SetPriority(TC3_IRQn, 0);
  NVIC_EnableIRQ(TC3_IRQn);

  Serial.println("Initialized timer!");
}

/**
 * Starts the flatline timer to go off every 5 seconds.
 */
void startFlatlineTimer() {
  float freq = 0.1;

  // Turn off interrupts to TC3 on MC0 when configuring
  TC3->COUNT16.INTENCLR.reg |= TC_INTENCLR_MC0;

  // Configure / turn on timer
  TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE | TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_PRESCSYNC_PRESC | TC_CTRLA_WAVEGEN_MFRQ;
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
    ;
  int regNum = 5 * CLOCKFREQ / (1024 * freq);
  TC3->COUNT16.CC[0].reg = (int)CLOCKFREQ / (1024 * freq);
  Serial.println(regNum);
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
    ;

  // Turn interrupts to TC3 on MC0 back on when done configuring
  TC3->COUNT16.INTENSET.reg |= TC_INTENSET_MC0;
}

/** Restart the timer from the beginning. */
void restartFlatlineTimer() {
#ifndef TESTING
  TC3->COUNT16.INTENCLR.reg |= TC_INTENCLR_MC0;
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;

  startFlatlineTimer();
#endif
}
