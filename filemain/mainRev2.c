// #include "n32g031.h"
// #include "n32g031_gpio.h"
// #include "n32g031_rcc.h"
// #include "n32g031_usart.h"

// extern uint32_t SystemCoreClock;

// void delay_ms(uint32_t ms)
// {
//     SysTick->LOAD  = (SystemCoreClock / 1000) - 1;
//     SysTick->VAL   = 0;
//     SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
//     while (ms--)
//     {
//         while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
//     }
//     SysTick->CTRL = 0;
// }

// void USART1_SendChar(char ch)
// {
//     while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);
//     USART_SendData(USART1, ch);
// }

// void USART1_SendString(const char* str)
// {
//     while (*str)
//     {
//         USART1_SendChar(*str++);
//         // Flash LED PB1 เมื่อส่งแต่ละตัวอักษร
//         GPIO_WriteBit(GPIOB, GPIO_PIN_1, Bit_SET);
//         delay_ms(10);
//         GPIO_WriteBit(GPIOB, GPIO_PIN_1, Bit_RESET);
//         delay_ms(10);
//     }
// }

// void USART1_Init(void)
// {
//     RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_USART1, ENABLE);

//     GPIO_InitType GPIO_InitStruct;

//     GPIO_InitStruct.Pin        = GPIO_PIN_9;
//     GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.GPIO_Speed = GPIO_SPEED_HIGH;
//     GPIO_InitPeripheral(GPIOA, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin        = GPIO_PIN_10;
//     GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_INPUT;
//     GPIO_InitPeripheral(GPIOA, &GPIO_InitStruct);

//     USART_InitType USART_InitStruct;
//     USART_InitStruct.BaudRate            = 115200;
//     USART_InitStruct.WordLength          = USART_WL_8B;
//     USART_InitStruct.StopBits            = USART_STPB_1;
//     USART_InitStruct.Parity              = USART_PE_NO;
//     USART_InitStruct.HardwareFlowControl = USART_HFCTRL_NONE;
//     USART_InitStruct.Mode                = USART_MODE_TX;
//     USART_Init(USART1, &USART_InitStruct);
//     USART_Enable(USART1, ENABLE);
// }

// void LED_Init(void)
// {
//     RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
//     GPIO_InitType GPIO_InitStruct;
//     GPIO_InitStruct.Pin        = GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7;
//     GPIO_InitStruct.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
//     GPIO_InitStruct.GPIO_Speed = GPIO_SPEED_LOW;
//     GPIO_InitPeripheral(GPIOB, &GPIO_InitStruct);
// }

// // int main(void)
// // {
// //     SystemInit();
// //     SystemCoreClockUpdate();

// //     USART1_Init();
// //     LED_Init();

// //     while (1)
// //     {
// //         USART1_SendString("TEVA UART OK\r\n");

// //         // LED pattern PB6, PB7
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_6, Bit_SET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_7, Bit_RESET);
// //         delay_ms(500);

// //         GPIO_WriteBit(GPIOB, GPIO_PIN_6, Bit_RESET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_7, Bit_SET);
// //         delay_ms(500);
// //     }
// // }
