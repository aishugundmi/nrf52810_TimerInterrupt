#include <stdbool.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"

#include "nrf52810_timer_driver.h"


void (*callback)(void);
    
void start_timer(uint32_t us_interval, void (*cb)(void))
{		
    uint32_t prescaler = 0, cc0_value = 0;
    do{
        cc0_value = (us_interval * 16UL )/ (1<<prescaler);  
        prescaler++;
    } while( cc0_value > 0xFFFF );

    prescaler--;
  
    callback = cb;
    
    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;               // Set the timer in Counter Mode
    NRF_TIMER2->TASKS_CLEAR = 1;                            // clear the task first to be usable for later
    NRF_TIMER2->PRESCALER = prescaler;                      // TimerTick = 1/(16M / 2^prescaler) 
    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;	    // Set counter to 16 bit resolution
    NRF_TIMER2->CC[0] = cc0_value;                    
		
    NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);  // Enable interrupt on Timer 2, CC[0] compare match events
    NVIC_EnableIRQ(TIMER2_IRQn);	
    NRF_TIMER2->TASKS_START = 1;                            // Start TIMER2
}
		

void stop_timer(void)
{
  
   callback = NULL;
  
   NRF_TIMER2->TASKS_STOP = 1;          // Stop the timer
   NRF_TIMER2->TASKS_SHUTDOWN = 1;      // Shutdown the timer module

   NVIC_DisableIRQ(TIMER2_IRQn);        // Disable and clear the timer interrupt
   NVIC_ClearPendingIRQ(TIMER2_IRQn);
}


/** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
 */
void TIMER2_IRQHandler(void)
{
  if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
  {
      NRF_TIMER2->EVENTS_COMPARE[0] = 0;           //Clear compare register 0 event	
      if(callback){
        callback();
      }
  }
}



