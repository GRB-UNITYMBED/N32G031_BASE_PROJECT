// #include "n32g031.h"
// #include "n32g031_gpio.h"
// #include "n32g031_rcc.h"
// #include "n32g031_usart.h"
// #include "core_cm0.h"

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

//     SysTick->CTRL  = 0;
// }

// void USART1_SendChar(char ch)
// {
//     while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET);
//     USART_SendData(USART1, ch);
// }

// // int main(void)
// // {
// //     SystemInit();
// //     SystemCoreClockUpdate();

// //     // ✅ เปิด Clock ให้ GPIOA และ USART1 โดยตรง
// //     RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_USART1, ENABLE);

// //     // ✅ ตั้งค่า GPIO PA9 เป็น USART1_TX
// //     GPIO_InitType GPIO_InitStructure;
// //     GPIO_InitStructure.Pin         = GPIO_PIN_9;
// //     GPIO_InitStructure.GPIO_Mode   = GPIO_MODE_AF_PP;
// //     GPIO_InitStructure.GPIO_Speed  = GPIO_SPEED_HIGH;
// //     GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

// //     // ✅ ตั้งค่า USART1 แบบ TX-only
// //     USART_InitType USART_InitStructure;
// //     USART_InitStructure.BaudRate            = 115200;
// //     USART_InitStructure.WordLength          = USART_WL_8B;
// //     USART_InitStructure.StopBits            = USART_STPB_1;
// //     USART_InitStructure.Parity              = USART_PE_NO;
// //     USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
// //     USART_InitStructure.Mode                = USART_MODE_TX;
// //     USART_Init(USART1, &USART_InitStructure);
// //     USART_Enable(USART1, ENABLE);

// //     // ✅ ตั้ง GPIO PB1, PB6, PB7 เป็น output
// //     GPIO_InitStructure.Pin        = GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7;
// //     GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
// //     GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_LOW;
// //     GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

// //     // ✅ Loop หลัก
// //     while (1)
// //     {
// //         USART1_SendChar('T');
// //         USART1_SendChar('E');
// //         USART1_SendChar('V');
// //         USART1_SendChar('A');
// //         USART1_SendChar('\n');

// //         GPIO_WriteBit(GPIOB, GPIO_PIN_1, Bit_SET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_6, Bit_RESET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_7, Bit_RESET);
// //         delay_ms(1000);

// //         GPIO_WriteBit(GPIOB, GPIO_PIN_1, Bit_RESET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_6, Bit_SET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_7, Bit_RESET);
// //         delay_ms(1000);

// //         GPIO_WriteBit(GPIOB, GPIO_PIN_1, Bit_RESET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_6, Bit_RESET);
// //         GPIO_WriteBit(GPIOB, GPIO_PIN_7, Bit_SET);
// //         delay_ms(1000);
// //     }
// // }
