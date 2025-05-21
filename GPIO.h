#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h>
#include "stm32f4xx.h"

// GPIO Pin Definitions
#define GPIO_NUMBER 16U

#define GPIO_PIN_0      ((uint16_t)0x0001)
#define GPIO_PIN_1      ((uint16_t)0x0002)
#define GPIO_PIN_2      ((uint16_t)0x0004)
#define GPIO_PIN_3      ((uint16_t)0x0008)
#define GPIO_PIN_4      ((uint16_t)0x0010)
#define GPIO_PIN_5      ((uint16_t)0x0020)
#define GPIO_PIN_6      ((uint16_t)0x0040)
#define GPIO_PIN_7      ((uint16_t)0x0080)
#define GPIO_PIN_8      ((uint16_t)0x0100)
#define GPIO_PIN_9      ((uint16_t)0x0200)
#define GPIO_PIN_10     ((uint16_t)0x0400)
#define GPIO_PIN_11     ((uint16_t)0x0800)
#define GPIO_PIN_12     ((uint16_t)0x1000)
#define GPIO_PIN_13     ((uint16_t)0x2000)
#define GPIO_PIN_14     ((uint16_t)0x4000)
#define GPIO_PIN_15     ((uint16_t)0x8000)
#define GPIO_PIN_All    ((uint16_t)0xFFFF)
#define GPIO_PIN_MASK   0x0000FFFFU

// GPIO Modes
#define GPIO_MODE               0x00000003U
#define GPIO_OUTPUT_TYPE   		  0x00000010U

#define GPIO_MODE_INPUT         0x00000000U
#define GPIO_MODE_OUTPUT_PP     0x00000001U
#define GPIO_MODE_OUTPUT_OD     0x00000011U
#define GPIO_MODE_AF_PP         0x00000002U
#define GPIO_MODE_AF_OD         0x00000012U
#define GPIO_MODE_ANALOG        0x00000003U


// GPIO Speed
#define GPIO_SPEED_FREQ_LOW         0x00000000U
#define GPIO_SPEED_FREQ_MEDIUM      0x00000001U
#define GPIO_SPEED_FREQ_HIGH        0x00000002U
#define GPIO_SPEED_FREQ_VERY_HIGH   0x00000003U

// GPIO Pull-Up/Pull-Down
#define GPIO_NOPULL      0x00000000U
#define GPIO_PULLUP      0x00000001U
#define GPIO_PULLDOWN    0x00000002U

// Pin State
typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET
} GPIO_PinState;

// GPIO Initialization Structure
typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
    uint32_t Alternate;
} GPIO_InitTypeDef;

// Function(s)
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

#endif
