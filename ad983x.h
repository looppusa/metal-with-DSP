#ifndef AD983X_H
#define AD983X_H

#include "driverlib.h"
#include "device.h"
#include "spi.h"

// 寄存器定义（与GD32保持一致）
#define AD983X_REG_CMD      (0 << 14)
#define AD983X_REG_FREQ0    (1 << 14)  // 0x4000，与原DSP定义一致
#define AD983X_REG_FREQ1    (2 << 14)  // 0x8000，与原DSP定义一致
#define AD983X_REG_PHASE0   (6 << 13)  // 0xC000，与原DSP定义一致
#define AD983X_REG_PHASE1   (7 << 13)  // 0xE000，与原DSP定义一致

// 命令控制位（补充GD32中的定义）
#define AD983X_B28          (1 << 13)  // 0x2000，与原DSP定义一致
#define AD983X_HLB          (1 << 12)
#define AD983X_FSEL0        (0 << 11)
#define AD983X_FSEL1        (1 << 11)
#define AD983X_PSEL0        (0 << 10)
#define AD983X_PSEL1        (1 << 10)
#define AD983X_PIN_SW       (1 << 9)
#define AD983X_RESET        (1 << 8)   // 0x0100，与原DSP定义一致
#define AD983X_SLEEP1       (1 << 7)   // 对应原DSP的AD983X_SLEEP
#define AD983X_SLEEP12      (1 << 6)
#define AD983X_OPBITEN      (1 << 5)
#define AD983X_SIGN_PIB     (1 << 4)
#define AD983X_DIV2         (1 << 3)
#define AD983X_MODE         (1 << 1)

// 输出波形配置
#define AD983X_OUT_SINUS    ((0 << 5) | (0 << 1) | (0 << 3))
#define AD983X_OUT_TRIANGLE ((0 << 5) | (1 << 1) | (0 << 3))
#define AD983X_OUT_MSB      ((1 << 5) | (0 << 1) | (1 << 3))
#define AD983X_OUT_MSB2     ((1 << 5) | (0 << 1) | (0 << 3))


// ########################## 关键：硬件引脚映射 ##########################
// 格式：引脚编号（GPIO_x） + 对应的PinConfig（复用配置）

#define AD983X_FSYNC0_PIN    13                 // 第1个芯片CS引脚编号
#define AD983X_FSYNC0_CONFIG GPIO_13_GPIO13     // 第1个芯片CS引脚复用配置      作用是 配置FSYNC0_PIN这个物理引脚13复用为普通GPIO

#define AD983X_FSYNC1_PIN    25                 // 第2个芯片CS引脚编号
#define AD983X_FSYNC1_CONFIG GPIO_25_GPIO25     // 第2个芯片CS引脚复用配置

#define AD983X_FSYNC2_PIN    26                 // 第3个芯片CS引脚编号
#define AD983X_FSYNC2_CONFIG GPIO_26_GPIO26     // 第3个芯片CS引脚复用配置

// 主时钟频率（与GD32一致，25MHz）
#define F_MCLK_DEFAULT 25000000.0f

// 修正：设备结构体（移除GPIO基地址，只需引脚编号）
typedef struct {
    uint32_t spi_base;      // SPI模块基地址（如SPIB_BASE）#define SPIB_BASE                 0x00006110U
    uint32_t cs_pin;        // CS引脚编号（如GPIO_20，对应上面的AD983X_FSYNC0_PIN）
} AD983x_Dev;

// 函数声明（适配DSP风格，使用设备结构体指针）
void AD983x_Init(AD983x_Dev *dev, uint32_t spi_base, uint32_t cs_pin, uint32_t cs_pin_config);
void AD983x_SetRegisterValue(AD983x_Dev *dev, uint16_t regValue);
uint32_t AD983x_FreqToReg(double hz, float mclk);
uint16_t AD983x_PhaseToReg(double rad);
void AD983x_SetFrequency(AD983x_Dev *dev, uint16_t freq_reg, uint32_t freq_val);
void AD983x_SetPhase(AD983x_Dev *dev, uint16_t phase_reg, uint16_t phase_val);
void AD983x_SetWaveform(AD983x_Dev *dev, uint16_t waveform);

#endif














