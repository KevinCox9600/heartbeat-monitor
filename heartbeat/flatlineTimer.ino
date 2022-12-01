
/**
 * Detects when there has not been a heartbeat for a long time.
 *
 * Is to be restarted at every heartbeat.
 * Will trigger interrupt every 5 seconds. TODO: fix and verify this
 */

const int CLOCKFREQ = 8000 / (2) * 1000; // FIXME;

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

void startFlatlineTimer() {
  int freq = 1; // FIXME: should be a frequency of 0.2, but make sure this works first

  // Turn off interrupts to TC3 on MC0 when configuring
  TC3->COUNT16.INTENCLR.reg |= TC_INTENCLR_MC0;

  // Configure / turn on timer
  TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE | TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV2 | TC_CTRLA_PRESCSYNC_PRESC | TC_CTRLA_WAVEGEN_MFRQ;
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
    ;
  TC3->COUNT16.CC[0].reg = CLOCKFREQ / (2 * freq);
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY)
    ;

  // Turn interrupts to TC3 on MC0 back on when done configuring
  TC3->COUNT16.INTENSET.reg |= TC_INTENSET_MC0;

  Serial.println("Started timer");
}

/** Restart the timer from the beginning. */
void restartFlatlineTimer() {
  TC3->COUNT16.INTENCLR.reg |= TC_INTENCLR_MC0;
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;

  startFlatlineTimer();
  Serial.println("Restarted timer!");

  // TODO (step 8a): Reference pin with PORT->Group[PORTB].register_name.reg
  // PORT->Group[PORTB].OUTCLR.reg |= 1 << PB_PIN;
}