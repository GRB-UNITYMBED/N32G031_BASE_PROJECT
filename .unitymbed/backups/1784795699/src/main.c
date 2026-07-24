#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "n32g031_adc.h" // Include ADC header
#include "core_cm0.h"
#include "dht11.h"
#include "oled.h"
#include "utils.h"
#include <stdio.h> 

// Function prototypes
void adc_init(void);
uint16_t adc_read_channel(uint8_t channel);
void OLED_DrawBarGraph(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percentage);

int main(void)
{
    uint8_t current_temp = 0;
    uint8_t current_humi = 0;
    
    uint8_t last_temp = 255;
    uint8_t last_humi = 255;
    uint8_t was_error = 1; 
    
    uint8_t error_count = 0; 
    char buffer[20];

    uint16_t adc_raw_value;
    uint8_t  volume_percent;
    uint8_t  last_volume_percent = 255; // To track changes for OLED update

    SystemInit();
    SystemCoreClockUpdate();
    
    delay_ms(500); 
    OLED_Init();
    OLED_Clear();
    DHT11_Init(); 
    adc_init(); // Initialize ADC

    OLED_ShowString(10, 24, "System Ready", 16);
    delay_ms(1500); 
    
    OLED_Clear();
    OLED_ShowString(0, 0, "Volume:", 8); // Label for volume

    while (1)
    {
        // Read DHT11 data
        if (DHT11_Read_Data(&current_temp, &current_humi) == 1)
        {
            error_count = 0; 

            if (was_error) {
                OLED_Clear();
                OLED_ShowString(0, 0, "Volume:", 8); // Re-draw volume label
                OLED_ShowString(0, 24, "Temp:      ", 8);
                OLED_ShowString(0, 48, "Humidity:         ", 8);
                was_error = 0;
                last_temp = 255; 
                last_humi = 255;
            }

            if (current_temp != last_temp) {
                sprintf(buffer, "%02d  C        ", current_temp);
                OLED_ShowString(0, 36, buffer, 16); 
                OLED_ShowString(24, 36, "o", 8); 
                last_temp = current_temp; 
            }

            if (current_humi != last_humi) {
                sprintf(buffer, "%02d %%        ", current_humi);
                OLED_ShowString(0, 60, buffer, 16); 
                last_humi = current_humi; 
            }
        }
        else
        {
            error_count++; 

            if (error_count >= 3) {
                if (!was_error) {
                    OLED_Clear();
                    was_error = 1;
                }
                OLED_ShowString(0, 0,  "Sensor Error!     ", 16);
                OLED_ShowString(0, 24, "Check Wiring.     ", 16);
            }
        }

        // Read ADC value from PA0 (Channel 0)
        adc_raw_value = adc_read_channel(0); // Channel 0 corresponds to PA0
        
        // Scale ADC raw value (0-4095 for 12-bit) to 0-100 percent
        volume_percent = (uint8_t)((adc_raw_value * 100) / 4095);

        // Update OLED only if percentage changed
        if (volume_percent != last_volume_percent) {
            OLED_DrawBarGraph(0, 12, 128, 8, volume_percent); // Draw bar graph at (0,12), width 128, height 8
            last_volume_percent = volume_percent;
        }

        delay_ms(200); // Shorter delay for more responsive volume updates
    }
}

/**
 * @brief Initializes ADC for single-channel conversion on PA0.
 */
void adc_init(void)
{
    // 1. Enable GPIOA clock (for PA0)
    RCC->AHBPCLKEN |= (1U << 17); // Bit 17: IOPAEN (GPIOA clock enable)

    // 2. Configure PA0 as analog input
    GPIOA->PMODE &= ~(0x3U << (0 * 2)); // Clear PMODE0
    GPIOA->PMODE |= (0x3U << (0 * 2));  // Set PMODE0 to 11 (Analog mode)

    // 3. Enable ADC clock
    RCC->APB2PCLKEN |= (1U << 9); // Bit 9: ADCEN (ADC clock enable)

    // 4. Reset ADC
    RCC->APB2PRST |= (1U << 9);  // Bit 9: ADCRST
    RCC->APB2PRST &= ~(1U << 9);

    // 5. Configure ADC settings
    ADC->CTRL1 &= ~(1U << 8); // Clear SCAN mode (bit 8)
    ADC->CTRL2 &= ~(1U << 1); // Clear CONT mode (bit 1)

    // Enable ADC
    ADC->CTRL2 |= (1U << 0); // Set ADCON (bit 0)

    // Wait for ADC to stabilize
    delay_ms(1); 
}

/**
 * @brief Reads a single ADC conversion from the specified channel.
 * @param channel The ADC channel to read (e.g., 0 for PA0).
 * @return The 12-bit ADC conversion value.
 */
uint16_t adc_read_channel(uint8_t channel)
{
    // Set channel for first conversion in sequence
    ADC->SQR3 &= ~(0x1FU << 0); // Clear SQ1 (bits 4:0)
    ADC->SQR3 |= (channel & 0x1F);

    // Start conversion
    ADC->CTRL2 |= (1U << 22); // Set SWSTART (bit 22)

    // Wait for EOC
    while (!(ADC->STS & (1U << 1))); // EOC is bit 1

    // Clear EOC flag
    ADC->STS &= ~(1U << 1);

    return ADC->DAT;
}