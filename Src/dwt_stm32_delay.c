#include "dwt_stm32_delay.h"


/**
 * @brief  Initializes DWT_Clock_Cycle_Count for DWT_Delay_us function
 * @return Error DWT counter
 *         1: clock cycle counter not started
 *         0: clock cycle counter works
 */
uint32_t DWT_Delay_Init(void) {
  /* Disable TRC */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
  /* Enable TRC */
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

  /*
  The DWT unit consists of four comparators. They are configurable as:
  • a hardware watchpoint or
  • a trigger to an ETM or
  • a PC sampler or
  • a data address sampler
  The DWT also provides some means to give some profiling informations. For this, some
  counters are accessible to give the number of:
  • Clock cycle
  • Folded instructions
  • Load store unit (LSU) operations
  • Sleep cycles
  • CPI (clock per instructions)
  • Interrupt overhead
 */

  /* Disable clock cycle counter */
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  /*
  Enable the CYCCNT counter. If not enabled, the counter does not count and
  no event is generated for PS sampling or CYCCNTENA. In normal use, the
  debugger must initialize the CYCCNT counter to 0.
  */
  /* Enable  clock cycle counter */
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

  /* Reset the clock cycle counter value */
  DWT->CYCCNT = 0;
  /* CYCLE CURRENT COUNT 
  The DWT Current PC Sampler Cycle Count Register to count the number of core cycles. This count can
  measure elapsed execution time.
   */

  /* 3 NO OPERATION instructions */
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");

  /* Check if clock cycle counter has started */
  if(DWT->CYCCNT){
    return 0; /*clock cycle counter started*/
  }else{
    return 1; /*clock cycle counter not started*/
  }
}

/* Use DWT_Delay_Init (); and DWT_Delay_us (microseconds) in the main */
