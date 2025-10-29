// #ifndef AD983X_H
// #define AD983X_H




// #include "driverlib.h"
// #include "device.h"
// #include "board.h"
// #include "c2000ware_libraries.h"
// #include "sysctl.h"

// // #include "F28x_Project.h"  // F28377头文件
// #include "spi.h"           // TI SPI driverlib

// // AD983x寄存器与命令位定义
// #define AD983X_REG_FREQ0    0x4000  // 频率寄存器0
// #define AD983X_REG_FREQ1    0x8000  // 频率寄存器1
// #define AD983X_REG_PHASE0   0xC000  // 相位寄存器0
// #define AD983X_REG_PHASE1   0xE000  // 相位寄存器1
// #define AD983X_B28          0x2000  // 32位频率写入使能
// #define AD983X_RESET        0x0100  // 复位命令
// #define AD983X_SLEEP        0x0080  // 睡眠模式

// // AD983x设备结构体（包含SPI句柄和CS引脚）
// typedef struct {
//     uint32_t spi_base;      // SPI模块基地址（如SPIA_BASE）
//     uint32_t cs_gpio_base;  // CS引脚GPIO基地址（如GPIOA_BASE）
//     uint16_t cs_pin;        // CS引脚号（如GPIO_PIN_0）
// } AD983x_Dev;

// // 函数声明
// void AD983x_Init(AD983x_Dev *dev, uint32_t spi_base, uint32_t cs_gpio_base, uint16_t cs_pin);
// void AD983x_SetFrequency(AD983x_Dev *dev, uint16_t freq_reg, uint32_t freq, float mclk);
// void AD983x_SetPhase(AD983x_Dev *dev, uint16_t phase_reg, uint16_t phase);
// void AD983x_Reset(AD983x_Dev *dev);

// #endif












