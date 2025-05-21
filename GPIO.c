#include "GPIO.h"


void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    // Enable peripheral clock based on GPIO port
    if (GPIOx == GPIOA)
        RCC->AHB1ENR |= (1 << 0);
    else if (GPIOx == GPIOB)
        RCC->AHB1ENR |= (1 << 1);

    for (uint32_t position = 0; position < GPIO_NUMBER; position++)
    {
        uint32_t ioposition = 1U << position;

        if ((GPIO_Init->Pin & ioposition) != 0U)
        {
            // Mode configuration
            GPIOx->MODER &= ~(0x3U << (position * 2));
            GPIOx->MODER |= (GPIO_Init->Mode & 0x3U) << (position * 2);

            // Output type configuration
            GPIOx->OTYPER &= ~(1U << position);
            GPIOx->OTYPER |= ((GPIO_Init->Mode >> 4U) & 0x1U) << position;

            // Speed configuration
            GPIOx->OSPEEDR &= ~(0x3U << (position * 2));
            GPIOx->OSPEEDR |= (GPIO_Init->Speed & 0x3U) << (position * 2);

            // Pull-up/pull-down configuration
            GPIOx->PUPDR &= ~(0x3U << (position * 2));
            GPIOx->PUPDR |= (GPIO_Init->Pull & 0x3U) << (position * 2);
        }
    }
}



void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    if (PinState == GPIO_PIN_SET) {
        GPIOx->BSRR = GPIO_Pin;
    } else {
        GPIOx->BSRR = (uint32_t)GPIO_Pin << 16U;
    }
}
