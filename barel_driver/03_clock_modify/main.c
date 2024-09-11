#include "include/led.h"
#include "include/clock.h"
#include "include/my_printf.h"
#include "include/uart.h"

uint32_t count = 10;

int main()
{
   Uart_Init();

   led_init();
   set_arm_clk_root_81mhz();

   while (count--)
   {
      printf("count = %d\n", count);
      led_toggle();
      delay(3);
   }

   set_arm_clk_root_648mhz();

   while (1)
   {
      printf("count = %d\n", count);
      led_toggle();
      delay(3);
   }

   return 0;
}