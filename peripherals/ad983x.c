#include "ad983x.h"
#include "driverlib.h"


// SPI初始化（配置为16位数据、模式0（CPOL=0, CPHA=0）、主模式）
static void SPI_Init(uint32_t spi_base) {
    // 禁用SPI模块以配置
    SPI_disableModule(spi_base);
    
    // 配置SPI主模式，时钟极性0，时钟相位0
    SPI_setConfig(spi_base, SysCtlClockGet(), SPI_PROT_POL0PHA0,
                  SPI_MODE_MASTER, 10000000, 16);  // 10MHz SPI时钟（AD983x最大支持40MHz）
    
    // 使能SPI模块
    SPI_enableModule(spi_base);
}

// 初始化AD983x（包含SPI和CS引脚）
void AD983x_Init(AD983x_Dev *dev, uint32_t spi_base, uint32_t cs_gpio_base, uint16_t cs_pin) {
    dev->spi_base = spi_base;
    dev->cs_gpio_base = cs_gpio_base;
    dev->cs_pin = cs_pin;
    
    // 配置CS引脚为输出                /.m
    GPIO_setPinConfig(GPIO_20_SPIA_SS);  // 示例：将GPIO20复用为SPIA片选（需根据硬件修改）
    GPIO_setDirectionMode(cs_gpio_base, cs_pin, GPIO_DIR_MODE_OUT);
    GPIO_writePin(cs_gpio_base, cs_pin, 1);  // 初始拉高CS（未选中）
    
    // 初始化SPI
    SPI_Init(spi_base);
    
    // 复位AD983x
    AD983x_Reset(dev);
}

// 通过SPI发送16位命令到AD983x
static void AD983x_SendCmd(AD983x_Dev *dev, uint16_t cmd) {
    GPIO_writePin(dev->cs_gpio_base, dev->cs_pin, 0);  // 拉低CS，选中芯片
    
    // 等待SPI发送缓冲区为空
    while (!SPI_isTxBufferEmpty(dev->spi_base)) {}
    SPI_transmitData(dev->spi_base, cmd);  // 发送命令
    
    // 等待发送完成
    while (!SPI_isTxComplete(dev->spi_base)) {}
    GPIO_writePin(dev->cs_gpio_base, dev->cs_pin, 1);  // 拉高CS，结束通信
}

// 复位AD983x
void AD983x_Reset(AD983x_Dev *dev) {
    AD983x_SendCmd(dev, AD983X_RESET);  // 发送复位命令
}

// 设置频率（freq_reg：AD983X_REG_FREQ0/1；mclk：AD983x的主时钟频率，单位Hz）
void AD983x_SetFrequency(AD983x_Dev *dev, uint16_t freq_reg, uint32_t freq, float mclk) {
    // 频率计算公式：FREQ = (freq * 2^28) / mclk
    uint32_t freq_data = (uint32_t)((float)freq * (1 << 28) / mclk);
    
    // 分两次发送32位频率数据（需设置B28位）
    AD983x_SendCmd(dev, AD983X_B28 | (freq_reg & 0xC000) | (freq_data & 0x0000FFFF));  // 低16位
    AD983x_SendCmd(dev, AD983X_B28 | (freq_reg & 0xC000) | ((freq_data >> 16) & 0x0000FFFF));  // 高16位
}

// 设置相位（phase_reg：AD983X_REG_PHASE0/1；phase：相位值，0~4095对应0~2π）
void AD983x_SetPhase(AD983x_Dev *dev, uint16_t phase_reg, uint16_t phase) {
    // 相位数据低12位有效，高4位为寄存器地址
    AD983x_SendCmd(dev, phase_reg | (phase & 0x0FFF));
}























