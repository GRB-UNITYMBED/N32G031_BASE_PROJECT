#include "oled.h"
#include "n32g031.h"
#include "n32g031_gpio.h"
#include "n32g031_rcc.h"
#include "utils.h"
#include "fonts.h"

// OLED_SCL_PORT and OLED_SDA_PORT are defined in oled.h,
// which includes the definitions from Reference 1.

// I2C communication functions (software bit-banging)
static void OLED_I2C_Start(void);
static void OLED_I2C_Stop(void);
static void OLED_I2C_Send_Byte(uint8_t byte);
static uint8_t OLED_I2C_Wait_Ack(void);

// Internal function to write command/data to OLED
static void OLED_WR_Byte(uint8_t dat, uint8_t cmd);

// Forward declarations for SCL/SDA functions
void OLED_SCL_Set(void);
void OLED_SCL_Clr(void);
void OLED_SDA_Set(void);
void OLED_SDA_Clr(void);

// Initialize I2C GPIO for OLED (Reference 2, adapted to bare-metal)
static void OLED_I2C_Init(void) {
    // Enable GPIOB clock
    RCC->AHBPCLKEN |= (1U << 18); // Bit 18: IOPBEN (GPIOB clock enable)

    // Configure PB6 (SCL) and PB7 (SDA) as Open-Drain Output
    GPIOB->PMODE &= ~((0x3U << (6 * 2)) | (0x3U << (7 * 2))); 
    GPIOB->PMODE |= ((0x1U << (6 * 2)) | (0x1U << (7 * 2))); // 01: General purpose output

    GPIOB->POTYPE |= ((1U << 6) | (1U << 7)); // 1: Open-drain

    // Set initial state to high (idle)
    OLED_SCL_Set();
    OLED_SDA_Set();
}

// OLED initialization sequence (Reference 3, adapted)
void OLED_Init(void) {
    OLED_I2C_Init();
    delay_ms(100);
    OLED_WR_Byte(0xAE, OLED_CMD); // Display Off
    OLED_WR_Byte(0x20, OLED_CMD); // Set Memory Addressing Mode
    OLED_WR_Byte(0x10, OLED_CMD); // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WR_Byte(0xB0, OLED_CMD); // Set Page Start Address for Page Addressing Mode,0-7
    OLED_WR_Byte(0xC8, OLED_CMD); // Set COM Output Scan Direction
    OLED_WR_Byte(0x00, OLED_CMD); // Set low column address
    OLED_WR_Byte(0x10, OLED_CMD); // Set high column address
    OLED_WR_Byte(0x40, OLED_CMD); // Set Start Line Address
    OLED_WR_Byte(0x81, OLED_CMD); // Set Contrast Control Register
    OLED_WR_Byte(0xDF, OLED_CMD); // Default value 0x7F (200)
    OLED_WR_Byte(0xA1, OLED_CMD); // Set Segment Re-map (A0h/A1h)
    OLED_WR_Byte(0xA6, OLED_CMD); // Normal Display (A6h/A7h)
    OLED_WR_Byte(0xA8, OLED_CMD); // Set Multiplex Ratio(1 to 64)
    OLED_WR_Byte(0x3F, OLED_CMD); // 1/64 duty
    OLED_WR_Byte(0xA4, OLED_CMD); // Output RAM to Display(A4h/A5h)
    OLED_WR_Byte(0xD3, OLED_CMD); // Set Display Offset
    OLED_WR_Byte(0x00, OLED_CMD); //
    OLED_WR_Byte(0xD5, OLED_CMD); // Set Display Clock Divide Ratio/Oscillator Frequency
    OLED_WR_Byte(0xF0, OLED_CMD); //
    OLED_WR_Byte(0xD9, OLED_CMD); // Set Pre-charge Period
    OLED_WR_Byte(0x22, OLED_CMD); //
    OLED_WR_Byte(0xDA, OLED_CMD); // Set COM Pins Hardware Configuration
    OLED_WR_Byte(0x12, OLED_CMD); //
    OLED_WR_Byte(0xDB, OLED_CMD); // Set VCOM Deselect Level
    OLED_WR_Byte(0x20, OLED_CMD); //
    OLED_WR_Byte(0x8D, OLED_CMD); // Set Charge Pump Enable/Disable
    OLED_WR_Byte(0x14, OLED_CMD); // Enable Charge Pump (0x10 Disable)
    OLED_WR_Byte(0xAF, OLED_CMD); // Display ON
    OLED_Clear();
}

// Clear the OLED display
void OLED_Clear(void) {
    uint8_t i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(0xB0 + i, OLED_CMD); // Set page address (0~7)
        OLED_WR_Byte(0x00, OLED_CMD);     // Set low column address
        OLED_WR_Byte(0x10, OLED_CMD);     // Set high column address
        for (n = 0; n < 128; n++) {
            OLED_WR_Byte(0x00, OLED_DATA);
        }
    }
}

// Set cursor position
void OLED_Set_Pos(uint8_t x, uint8_t y) {
    OLED_WR_Byte(0xB0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0F), OLED_CMD);
}

// Turn on a single pixel
void OLED_DrawPoint(uint8_t x, uint8_t y) {
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    OLED_WR_Byte(0xB0 + i, OLED_CMD); // Set page address
    OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); // Set high column address
    OLED_WR_Byte((x & 0x0F), OLED_CMD); // Set low column address
    OLED_WR_Byte(n, OLED_DATA); // Write data to turn on pixel
}

// Display a character
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size) {
    uint8_t c = 0, i = 0;
    c = chr - ' '; // Get character offset
    if (x > 128 - 1) {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16) {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    } else {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }
}

// Display a string
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t Char_Size) {
    uint8_t j = 0;
    while (chr[j] != '\0') {
        OLED_ShowChar(x, y, chr[j], Char_Size);
        if (Char_Size == 16) {
            x += 8;
        } else {
            x += 6;
        }
        if (x > 120) {
            x = 0;
            y += 2;
        }
        j++;
    }
}

// Power up SCL
void OLED_SCL_Set(void) {
    OLED_SCL_PORT->PBSC = OLED_SCL_PIN; // Set bit (ref: N32G031 RM p.140, GPIOx_PBSC register)
}

// Power down SCL
void OLED_SCL_Clr(void) {
    OLED_SCL_PORT->PBC = OLED_SCL_PIN; // Clear bit (ref: N32G031 RM p.140, GPIOx_PBC register)
}

// Power up SDA
void OLED_SDA_Set(void) {
    OLED_SDA_PORT->PBSC = OLED_SDA_PIN; // Set bit (ref: N32G031 RM p.140, GPIOx_PBSC register)
}

// Power down SDA
void OLED_SDA_Clr(void) {
    OLED_SDA_PORT->PBC = OLED_SDA_PIN; // Clear bit (ref: N32G031 RM p.140, GPIOx_PBC register)
}

// Read SDA input (not used in current OLED driver, but good to have)
uint8_t OLED_SDA_Read(void) {
    return (OLED_SDA_PORT->PID & OLED_SDA_PIN) ? 1 : 0; // Read bit (ref: N32G031 RM p.141, GPIOx_PID register)
}

// I2C Start condition
static void OLED_I2C_Start(void) {
    OLED_SDA_Set();
    OLED_SCL_Set();
    delay_us(4);
    OLED_SDA_Clr();
    delay_us(4);
    OLED_SCL_Clr();
}

// I2C Stop condition
static void OLED_I2C_Stop(void) {
    OLED_SCL_Clr();
    OLED_SDA_Clr();
    delay_us(4);
    OLED_SCL_Set();
    delay_us(4);
    OLED_SDA_Set();
    delay_us(4);
}

// I2C Send Byte
static void OLED_I2C_Send_Byte(uint8_t dat) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        OLED_SCL_Clr();
        delay_us(2);
        if (dat & 0x80) {
            OLED_SDA_Set();
        } else {
            OLED_SDA_Clr();
        }
        dat <<= 1;
        delay_us(2);
        OLED_SCL_Set();
        delay_us(2);
    }
    OLED_SCL_Clr();
}

// I2C Wait for ACK
static uint8_t OLED_I2C_Wait_Ack(void) {
    uint8_t ack = 1;
    OLED_SCL_Clr();
    OLED_SDA_Set(); // Release SDA
    delay_us(2);
    OLED_SCL_Set();
    delay_us(2);
    if (OLED_SDA_Read()) { // Check SDA line
        ack = 0; // No ACK
    }
    OLED_SCL_Clr();
    return ack;
}

// Write command or data to OLED
static void OLED_WR_Byte(uint8_t dat, uint8_t cmd) {
    OLED_I2C_Start();
    OLED_I2C_Send_Byte(0x78); // Slave address, write mode
    OLED_I2C_Wait_Ack();
    if (cmd) {
        OLED_I2C_Send_Byte(0x00); // Command mode
    } else {
        OLED_I2C_Send_Byte(0x40); // Data mode
    }
    OLED_I2C_Wait_Ack();
    OLED_I2C_Send_Byte(dat);
    OLED_I2C_Wait_Ack();
    OLED_I2C_Stop();
}

// Draw a bar graph
// x, y: top-left corner of the bar graph
// width, height: dimensions of the bar graph in pixels
// percentage: 0-100, fill level of the bar
void OLED_DrawBarGraph(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percentage) {
    uint8_t i, j;
    uint8_t fill_width = (width * percentage) / 100; // Calculate filled width in pixels

    // Ensure y and height are page-aligned for simplicity (8 pixels per page)
    // This implementation assumes height is a multiple of 8 and y is a multiple of 8.
    // For more flexible height, individual pixel manipulation would be needed.
    uint8_t start_page = y / 8;
    uint8_t end_page = (y + height - 1) / 8;

    for (i = start_page; i <= end_page; i++) {
        OLED_WR_Byte(0xB0 + i, OLED_CMD); // Set page address
        OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); // Set high column address
        OLED_WR_Byte((x & 0x0F), OLED_CMD); // Set low column address

        for (j = 0; j < width; j++) {
            if (j < fill_width) {
                // Fill pixel (all bits set for full height within this page)
                OLED_WR_Byte(0xFF, OLED_DATA); 
            } else {
                // Clear pixel
                OLED_WR_Byte(0x00, OLED_DATA);
            }
        }
    }
}