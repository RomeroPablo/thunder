#include <stdint.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"

int main(void)
{
    CHIP_Init();
    // Enable clock for GPIO
    CMU_ClockEnable(cmuClock_GPIO, true);

    // Configure PB0 as push-pull output
    GPIO_PinModeSet(gpioPortB, 0, gpioModePushPull, 0);

    while (1) {
        GPIO_PinOutToggle(gpioPortB, 0);
        for(volatile uint32_t i=0; i<100000; ++i) {
            __asm__("nop");
        }
    }
}
