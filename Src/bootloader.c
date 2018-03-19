
#include <bootloader.h>
#include <platform.h>
#include <config.h>
#include <led.h>


void jump_to_st_usb_bootloader()
{
    // Disable interrupts
    __disable_irq();

    // Disable RCC and reset it to default values
    RCC->CFGR = 0;
    RCC->CR = 0x83;

    //Disable systick timer and reset it to default values
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    // Disable USB
    USB->CNTR = USB_CNTR_PDWN;

    // Disable CAN
    CAN->MCR = CAN_MCR_SLEEP;

    // Disable LEDs
    // TODO: Surprisingly, this doesn't switch off the LED.
//    HAL_GPIO_WritePin(LED_ACTIVITY_PORT, LED_ACTIVITY_PIN, GPIO_PIN_RESET);

    // Enable interrupts
//    __enable_irq();

    __DSB();

    // Re-map system memory to address 0x0
    SYSCFG->CFGR1 = (SYSCFG->CFGR1 & ~SYSCFG_CFGR1_MEM_MODE) | SYSCFG_CFGR1_MEM_MODE_0;

    // Fetch and apply bootloader's stack pointer
    __set_MSP(*(uint32_t*)(0x00));

    __DSB();
    __ISB();

    // Invoke bootloader's reset handler
    void (*SysMemBootJump)(void);
    SysMemBootJump = (void (*)(void)) (*((uint32_t*)(4)));
    SysMemBootJump();
}
