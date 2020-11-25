#include <stdbool.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"

#include "nrf52810_timer_driver.h"
static volatile uint32_t count;

void timer_callback(void)
{
    ++count;
}

int main(void)
{	
	start_timer(1000*509UL, timer_callback);              //Configure and start timer
	
	while(true) 
	{
             
		// Enter System ON sleep mode
		__WFE();
		// Make sure any pending events are cleared
		__SEV();
		__WFE();
	}
}






