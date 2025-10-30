#include "ad983x.h"
#include "driverlib.h"
#include "board.h"



// // SPI初始化（配置为16位数据、模式2（CPOL=1, CPHA=0），10MHz时钟）
// static void SPI_Init(uint32_t spi_base) {
//     SPI_disableModule(spi_base);
    
//     // 参数说明：SPI基地址、系统时钟、SPI模式（模式2）、主从模式、波特率、数据宽度
//     SPI_setConfig(spi_base, SysCtlClockGet(), SPI_PROT_POL1PHA0,
//                   SPI_MODE_MASTER, 10000000, 16);
    
//     SPI_enableModule(spi_base);
// }


// 初始化AD983x设备（包含SPI和CS引脚，关键修正部分）
void AD983x_Init(AD983x_Dev *dev, uint32_t spi_base, uint32_t cs_pin, uint32_t cs_pin_config) {
    dev->spi_base = spi_base;
    dev->cs_pin = cs_pin;  // 存储CS引脚编号（而非基地址+引脚号）
    // ########################## 修正1：GPIO_setPinConfig ##########################
    // 函数定义：void GPIO_setPinConfig(uint32_t pinConfig) → 仅需1个参数（32位引脚配置值）
    GPIO_setPinConfig(cs_pin_config);  // 传入预定义的引脚配置（如AD983X_FSYNC0_CONFIG）
    
    // ########################## 修正2：GPIO_setDirectionMode ##########################
    // 函数定义：void GPIO_setDirectionMode(uint32_t pin, GPIO_Direction pinIO)
    // 第1参数：引脚编号（如GPIO_20）；第2参数：方向（GPIO_DIR_MODE_OUT=输出）

    GPIO_setDirectionMode(cs_pin, GPIO_DIR_MODE_OUT);

    // ########################## 修正3：GPIO_writePin（TI标准库函数） ##########################
    // 函数定义：void GPIO_writePin(uint32_t pin, uint16_t value)
    // 第1参数：引脚编号；第2参数：电平（1=高，0=低）
    GPIO_writePin(cs_pin, 1);  // 初始拉高CS（未选中芯片）
     
    // 初始化SPI
    //SPI_Init();
    
    // 复位芯片（与GD32逻辑一致）
    AD983x_SetRegisterValue(dev, AD983X_B28 | AD983X_RESET);
}

// 向AD983x写入16位寄存器值（修正CS引脚控制）
void AD983x_SetRegisterValue(AD983x_Dev *dev, uint16_t regValue) {
    GPIO_writePin(dev->cs_pin, 0);  // 拉低CS选中芯片（直接用引脚编号）

    // 等待发送缓冲区为空（替代SPI_isTxBufferEmpty()）
    while ((HWREGH(dev->spi_base + SPI_O_STS) & SPI_STS_BUFFULL_FLAG) != 0U) {}

     // 发送命令（替代SPI_transmitData()，使用非FIFO阻塞发送）
    SPI_writeDataBlockingNonFIFO(dev->spi_base, regValue);// 发送16位数据
    
    // 等待发送完成（替代SPI_isTxComplete()）
    // 发送完成后，SPI_STS_INT_FLAG会置位（表示发送空且接收完成）
    while ((HWREGH(dev->spi_base + SPI_O_STS) & SPI_STS_INT_FLAG) == 0U) {}

    // 清除发送完成标志（可选，根据需要）
    SPI_clearInterruptStatus(dev->spi_base, SPI_INT_RX_DATA_TX_EMPTY);
    
    GPIO_writePin(dev->cs_pin, 1);  // 拉高CS结束通信
}


// 频率值转寄存器值（与GD32的freq_2_reg逻辑一致）
uint32_t AD983x_FreqToReg(double hz, float mclk) {
    return (uint32_t)((hz * (1 << 28)) / mclk);
}

// 相位值（弧度）转寄存器值（与GD32的rad_2_reg逻辑一致）
uint16_t AD983x_PhaseToReg(double rad) {
    return (uint16_t)((rad * 4096.0) / (2 * 3.1415926));
}

// 设置频率（对应GD32的ad983x_set_frequency）
void AD983x_SetFrequency(AD983x_Dev *dev, uint16_t freq_reg, uint32_t freq_val) {
    uint16_t freqLo = AD983X_B28 | freq_reg | (freq_val & 0x3FFF);
    uint16_t freqHi = AD983X_B28 | freq_reg | ((freq_val >> 14) & 0x3FFF);
    
    AD983x_SetRegisterValue(dev, freqLo);  // 写低14位
    AD983x_SetRegisterValue(dev, freqHi);  // 写高14位
}

// 设置相位（对应GD32的ad983x_set_phase）
void AD983x_SetPhase(AD983x_Dev *dev, uint16_t phase_reg, uint16_t phase_val) {
    uint16_t phase = phase_reg | (phase_val & 0x0FFF);  // 相位低12位有效
    AD983x_SetRegisterValue(dev, phase);
}

// 设置输出波形（补充GD32的波形配置功能）
void AD983x_SetWaveform(AD983x_Dev *dev, uint16_t waveform) {
    AD983x_SetRegisterValue(dev, waveform);
}

// 初始化3个AD983x芯片（对应GD32的ad983x_init）
void AD983x_MultiInit(AD983x_Dev *dev0, AD983x_Dev *dev1, AD983x_Dev *dev2) {
    // 初始化芯片0：SPIA + GPIO20（CS）
    AD983x_Init(dev0, mySPI0_BASE, AD983X_FSYNC0_PIN, AD983X_FSYNC0_CONFIG);
    // 初始化芯片1：SPIA + GPIO21（CS）
    AD983x_Init(dev1, mySPI0_BASE, AD983X_FSYNC1_PIN, AD983X_FSYNC1_CONFIG);
    // 初始化芯片2：SPIA + GPIO22（CS）
    AD983x_Init(dev2, mySPI0_BASE, AD983X_FSYNC2_PIN, AD983X_FSYNC2_CONFIG);
    
    // ------------ 以下逻辑与GD32完全一致 ------------
    // 设置默认频率300KHz
    uint32_t freq_val = AD983x_FreqToReg(300000, F_MCLK_DEFAULT);
    AD983x_SetFrequency(dev0, AD983X_REG_FREQ0, freq_val);
    AD983x_SetFrequency(dev1, AD983X_REG_FREQ0, freq_val);
    AD983x_SetFrequency(dev2, AD983X_REG_FREQ0, freq_val);
    
    // 设置默认相位
    AD983x_SetPhase(dev0, AD983X_REG_PHASE0, 0);          // 0rad
    AD983x_SetPhase(dev1, AD983X_REG_PHASE0, 0);          // 0rad（GD32注释为pi/2，按代码对齐）
    AD983x_SetPhase(dev2, AD983X_REG_PHASE0, 4096 / 4);   // pi/2 rad
    
    // 退出复位，正弦波输出
    AD983x_SetRegisterValue(dev0, AD983X_B28 | AD983X_OUT_SINUS);
    AD983x_SetRegisterValue(dev1, AD983X_B28 | AD983X_OUT_SINUS);
    AD983x_SetRegisterValue(dev2, AD983X_B28 | AD983X_OUT_SINUS);
}




