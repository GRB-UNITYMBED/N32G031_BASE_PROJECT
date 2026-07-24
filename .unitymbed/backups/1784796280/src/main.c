#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "core_cm0.h"
#include "dht11.h"
#include "oled.h"
#include "utils.h"
#include <stdio.h> 

int main(void)
{
    uint8_t current_temp = 0;
    uint8_t current_humi = 0;
    
    uint8_t last_temp = 255;
    uint8_t last_humi = 255;
    uint8_t was_error = 1; 
    
    // 1. เพิ่มตัวแปรนับจำนวนครั้งที่อ่านพลาด
    uint8_t error_count = 0; 

    char buffer[20];

    SystemInit();
    SystemCoreClockUpdate();
    
    delay_ms(500); 
    OLED_Init();
    OLED_Clear();
    DHT11_Init(); 
    
    OLED_ShowString(10, 24, "System Ready", 16);
    delay_ms(1500); 
    
    OLED_Clear();

    while (1)
    {
        if (DHT11_Read_Data(&current_temp, &current_humi) == 1)
        {
            // 2. ถ้าอ่านสำเร็จ ให้รีเซ็ตตัวนับ Error กลับเป็น 0 ทันที
            error_count = 0; 

            if (was_error) {
                OLED_Clear();
                OLED_ShowString(0, 0, "Temp:      ", 8);
                OLED_ShowString(0, 32, "Humidity:         ", 8);
                was_error = 0;
                last_temp = 255; 
                last_humi = 255;
            }

            if (current_temp != last_temp) {
                sprintf(buffer, "%02d  C        ", current_temp);
                OLED_ShowString(0, 12, buffer, 16); 
                OLED_ShowString(24, 12, "o", 8); 
                last_temp = current_temp; 
            }

            if (current_humi != last_humi) {
                sprintf(buffer, "%02d %%        ", current_humi);
                OLED_ShowString(0, 44, buffer, 16); 
                last_humi = current_humi; 
            }
        }
        else
        {
            // 3. ถ้าอ่านพลาด ให้นับสะสมไป 1
            error_count++; 

            // 4. ถ้าพลาดติดกัน 3 ครั้งขึ้นไป (สายหลุดจริงๆ) ค่อยเคลียร์จอโชว์ Error
            if (error_count >= 3) {
                if (!was_error) {
                    OLED_Clear();
                    was_error = 1;
                }
                OLED_ShowString(0, 0,  "Sensor Error!     ", 16);
                OLED_ShowString(0, 24, "Check Wiring.     ", 16);
            }
        }

        delay_ms(2000); 
    }
}
