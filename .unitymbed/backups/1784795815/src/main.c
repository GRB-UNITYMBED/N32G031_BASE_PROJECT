#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "n32g031_adc.h" // Include ADC header
#include "core_cm0.h"
#include "oled.h"
#include "utils.h"
#include <stdio.h> 

// Function prototypes
void adc_init(void);
uint16_t adc_read_channel(uint8_t channel);
void OLED_DrawBarGraph(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percentage);

int main(void)
{
    char buffer[20];

    uint16_t adc_raw_value;
    uint8_t  volume_percent;
    uint8_t  last_volume_percent = 255; // To track changes for OLED update

    SystemInit();
    SystemCoreClockUpdate();
    
    delay_ms(500); 
    OLED_Init();
    OLED_Clear();
    adc_init(); // Initialize ADC

    OLED_ShowString(10, 24, "System Ready", 16);
    delay_ms(1500); 
    
    OLED_Clear();
    OLED_ShowString(0, 0, "Volume:", 8); // Label for volume

    while (1)
    {
        // Read ADC value from PA0 (Channel 0)
        adc_raw_value = adc_read_channel(0); // Channel 0 corresponds to PA0
        
        // Scale ADC raw value (0-4095 for 12-bit) to 0-100 percent
        volume_percent = (uint8_t)((adc_raw_value * 100) / 4095);

        // Update OLED only if percentage changed
        if (volume_percent != last_volume_percent) {
            OLED_DrawBarGraph(0, 12, 128, 8, volume_percent); // Draw bar graph at (0,12), width 128, height 8
            sprintf(buffer, "%3d%%", volume_percent); // Display percentage value
            OLED_ShowString(80, 0, buffer, 8); // Show percentage next to "Volume:"
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
    GPIOA->PMODE &= ~(0x3U << (0 * 2)); // Clear PMODE0 (bits 1:0)
    GPIOA->PMODE |= (0x3U << (0 * 2));  // Set PMODE0 to 11 (Analog mode)

    // 3. Enable ADC clock
    RCC->APB2PCLKEN |= (1U << 9); // Bit 9: ADCEN (ADC clock enable)

    // 4. Reset ADC
    RCC->APB2PRST |= (1U << 9);  // Bit 9: ADCRST
    RCC->APB2PRST &= ~(1U << 9);

    // 5. Configure ADC settings
    // Set ADC clock prescaler (e.g., PCLK2/2)
    ADC->CTRL2 &= ~(0x3U << 14); // Clear ADCPRE bits (bits 15:14)
    ADC->CTRL2 |= (0x0U << 14);  // Set prescaler to PCLK2/2 (00) (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Set data alignment (right alignment)
    ADC->CTRL2 &= ~(1U << 11); // Clear ALIGN bit (bit 11)
    ADC->CTRL2 |= (0U << 11);  // Right alignment (0) (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Set resolution (12-bit, default)
    ADC->CTRL1 &= ~(0x3U << 24); // Clear RSLTSEL bits (bits 25:24)
    ADC->CTRL1 |= (0x0U << 24);  // 12-bit resolution (00) (ref: N32G031 RM p.222, ADC_CTRL1 register)

    // Disable scan mode (single channel)
    ADC->CTRL1 &= ~(1U << 8); // Clear SCAN bit (bit 8) (ref: N32G031 RM p.222, ADC_CTRL1 register)

    // Disable continuous conversion mode (single conversion on demand)
    ADC->CTRL2 &= ~(1U << 1); // Clear CONT bit (bit 1) (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Enable ADC
    ADC->CTRL2 |= (1U << 0); // Set ADCON bit (bit 0) to enable ADC (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Wait for ADC to stabilize (power-up time)
    delay_ms(1); 
}

/**
 * @brief Reads a single ADC conversion from the specified channel.
 * @param channel The ADC channel to read (e.g., 0 for PA0).
 * @return The 12-bit ADC conversion value.
 */
uint16_t adc_read_channel(uint8_t channel)
{
    // Set channel for first conversion in regular sequence (SQ1 in RSEQ3)
    // SQ1 bits are 4:0 in ADC_RSEQ3 (ref: N32G031 RM p.229, ADC_RSEQ3 register)
    ADC->RSEQ3 &= ~(0x1FU << 0); // Clear SQ1 (bits 4:0)
    ADC->RSEQ3 |= (channel & 0x1F); // Set SQ1 to the desired channel

    // Start conversion
    ADC->CTRL2 |= (1U << 22); // Set SWSTART (bit 22) (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Wait for EOC (End Of Conversion)
    // EOC is bit 1 in ADC_STS (ref: N32G031 RM p.221, ADC_STS register)
    while (!(ADC->STS & (1U << 1))); 

    // Clear EOC flag by writing 0
    ADC->STS &= ~(1U << 1);

    return ADC->DAT; // Read the conversion result (ref: N32G031 RM p.230, ADC_DAT register)
}