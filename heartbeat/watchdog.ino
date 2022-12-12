
/**
 * Configure and start the watchdog.
 */
void configWatchdog() {
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn);

  // Configure and enable WDT GCLK:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;
  // set GCLK->GENCTRL.reg and GCLK->CLKCTRL.reg
  GCLK->GENCTRL.reg = GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_ID(5) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSCULP32K;
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_ID_WDT;
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;

  // Configure and enable WDT:
  WDT->CONFIG.reg = 0xAu;
  WDT->CTRL.reg = WDT_CTRL_ENABLE;
  while (WDT->STATUS.bit.SYNCBUSY)
    ;
}

/**
 * Pet the watchdog to reset the watchdog timer.
 */
void petWatchdog() {
#ifndef TESTING
  WDT->CLEAR.reg |= WDT_CLEAR_CLEAR_KEY;
#endif
}
