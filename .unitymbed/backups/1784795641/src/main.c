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

        // Read ADC value from PA0 (ADC channel 0)
        adc_raw_value = adc_read_channel(ADC_CH_0); // ref: N32G031 RM p.229 (ADC_CH_0 corresponds to PA0)
        
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
 * @brief Initializes ADC1 for single-channel conversion on PA0.
 */
void adc_init(void)
{
    // 1. Enable GPIOA clock (for PA0)
    RCC->AHBPERIPH_CLK_ENABLE |= RCC_AHBPERIPH_CLK_ENABLE_GPIOA; // ref: N32G031 RM p.102 (AHBPERIPH_CLK_ENABLE register)

    // 2. Configure PA0 as analog input
    // Clear PMODE[1:0] for PA0 (bits 1:0) to 00 (Analog mode)
    GPIOA->PMODE &= ~(GPIO_PMODE_PMODE0_Msk); // ref: N32G031 RM p.138 (GPIOx_PMODE register)
    GPIOA->PMODE |= (0x3u << (0 * 2)); // Set PA0 to Analog mode (0b11)

    // 3. Enable ADC1 clock
    RCC->APB2_PERIPH_CLK_ENABLE |= RCC_APB2_PERIPH_CLK_ENABLE_ADC1; // ref: N32G031 RM p.104 (APB2_PERIPH_CLK_ENABLE register)

    // 4. Reset ADC (optional, good practice)
    RCC->APB2_PERIPH_RST |= RCC_APB2_PERIPH_RST_ADC1; // ref: N32G031 RM p.105 (APB2_PERIPH_RST register)
    RCC->APB2_PERIPH_RST &= ~RCC_APB2_PERIPH_RST_ADC1;

    // 5. Configure ADC settings
    // Set ADC clock prescaler (e.g., PCLK2/2)
    ADC1->CTRL2 &= ~ADC_CTRL2_ADCPRE_Msk; // Clear ADCPRE bits
    ADC1->CTRL2 |= ADC_CTRL2_ADCPRE_DIV2; // Set prescaler to PCLK2/2 (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Set data alignment (right alignment)
    ADC1->CTRL2 &= ~ADC_CTRL2_ALIGN_Msk; // Clear ALIGN bit
    ADC1->CTRL2 |= ADC_CTRL2_ALIGN_RIGHT; // Right alignment (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Set resolution (12-bit, default)
    ADC1->CTRL1 &= ~ADC_CTRL1_RSLTSEL_Msk; // Clear RSLTSEL bits
    ADC1->CTRL1 |= ADC_CTRL1_RSLTSEL_12BIT; // 12-bit resolution (ref: N32G031 RM p.222, ADC_CTRL1 register)

    // Disable scan mode (single channel)
    ADC1->CTRL1 &= ~ADC_CTRL1_SCAN_Msk; // ref: N32G031 RM p.222 (ADC_CTRL1 register)

    // Disable continuous conversion mode (single conversion on demand)
    ADC1->CTRL2 &= ~ADC_CTRL2_CONT_Msk; // ref: N32G031 RM p.224 (ADC_CTRL2 register)

    // Enable ADC
    ADC1->CTRL2 |= ADC_CTRL2_ADCON_Msk; // Set ADCON bit to enable ADC (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Wait for ADC to stabilize (power-up time)
    delay_ms(1); 
}

/**
 * @brief Reads a single ADC conversion from the specified channel.
 * @param channel The ADC channel to read (e.g., ADC_CH_0 for PA0).
 * @return The 12-bit ADC conversion value.
 */
uint16_t adc_read_channel(uint8_t channel)
{
    // Clear previous channel selection and set the desired channel for regular conversion
    ADC1->SQR3 &= ~ADC_SQR3_SQ1_Msk; // Clear SQ1 bits (first conversion in sequence)
    ADC1->SQR3 |= (channel << ADC_SQR3_SQ1_Pos); // Set SQ1 to the desired channel (ref: N32G031 RM p.229, ADC_SQR3 register)

    // Start the conversion
    ADC1->CTRL2 |= ADC_CTRL2_SWSTART_Msk; // Set SWSTART bit to start conversion (ref: N32G031 RM p.224, ADC_CTRL2 register)

    // Wait for the conversion to complete
    while (!(ADC1->STS & ADC_STS_EOC_Msk)); // Wait until End Of Conversion flag is set (ref: N32G031 RM p.221, ADC_STS register)

    // Clear the EOC flag
    ADC1->STS &= ~ADC_STS_EOC_Msk; // Clear EOC flag by writing 0 (ref: N32G031 RM p.221, ADC_STS register)

    // Return the converted data
    return ADC1->DAT; // Read the conversion result (ref: N32G031 RM p.230, ADC_DAT register)
}