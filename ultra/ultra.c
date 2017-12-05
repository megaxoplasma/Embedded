

/*
	This is the main user software application
	
 */

#include "cpu.h"
#include "board_led.h"
#include "ultra_hal.h"

#include "foo.h"

int main()
{
	int ultra_flag;

	
	
	
  /*
    Initialize the PLL, clock tree to all peripherals, flash and Systick 1 ms time reference:
   */
  cpu_init();
  /*
    Initialize the GPIO (General-Purpose I/O) subsystem pins that are connected to the LEDs on the board:
   */
  board_led_init();

  int i = 0;

  /*
    Set up the USER button on the Discovery board (PA0) as an EXTI line, sensitive to both RISING
    and FALLING edges.
   */
  ultra_init();

  
  while(1)
  {
    /*
      Carry out a simple unit test of foo() declared in foo.h:
     */
    if(TEST_FOO(i, i+1) < 0)
    {
      /*
        If the above fails there is either a hardware, code or other undefined error.
        Now we're in an undefined state regarding processor core behavior...
       */
      while(1); // We probably have had a radiation hit or a major malfunction on the ALU of the processor...
    }
    else
    {
		
		//ultra_flag = ultra_change();
		//if(ultra_flag == 1){
			//board_led_toggle(LED2);
			
		//} 
		//else if(ultra_flag == 0) {
			//board_led_toggle(LED1);
		//}
		//else{} 
        
     
    

      ++i; // Increment i for the next test iteration...
    }
  }

  return 0;
}
