#include <stdint.h>
#include "em_device.h"

#define LED_PORT 1 /* gpioPortB */
#define LED_PIN  0

int main(void)
{
    /* Enable clock for GPIO */
    CMU->CLKEN0_SET = CMU_CLKEN0_GPIO;

    /* Configure PB0 as push-pull output */
    GPIO->P[LED_PORT].MODEL &= ~_GPIO_P_MODEL_MODE0_MASK;
    GPIO->P[LED_PORT].MODEL |= GPIO_P_MODEL_MODE0_PUSHPULL;

    while (1) {
        GPIO->P[LED_PORT].DOUT ^= (1U << LED_PIN);
        for (volatile uint32_t i = 0; i < 100000; ++i) {
            __NOP();
        }
    }
}
