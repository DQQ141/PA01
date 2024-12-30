#include "./spi_msp.h"

// 获取名字字符串
const char *spi_name_str_get(SPI_TypeDef *Instance)
{
    if (Instance == SPI1)
    {
        return "SPI1";
    }
    else if (Instance == SPI2)
    {
        return "SPI2";
    }
    else if (Instance == SPI3)
    {
        return "SPI3";
    }
    else if (Instance == SPI4)
    {
        return "SPI4";
    }
    else if (Instance == SPI5)
    {
        return "SPI5";
    }
    else if (Instance == SPI6)
    {
        return "SPI6";
    }
    else
    {
        return "SPI_Unknown";
    }
}
// 获取错误字符串
const char *spi_error_str_get(uint32_t ErrorCode)
{
    if (ErrorCode == HAL_SPI_ERROR_NONE)
    {
        return "无错误: SPI操作期间未发生任何错误。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_MODF)
    {
        return "MODF错误: 发生模式错误，表示SPI主/从模式配置不正确。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_CRC)
    {
        return "CRC错误: 发生CRC错误，表示接收到的数据与计算的CRC不匹配。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_OVR)
    {
        return "OVR错误: 发生溢出错误，表示接收缓冲区已满但仍有数据到达。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_FRE)
    {
        return "FRE错误: 发生帧错误，表示SPI帧格式不正确。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_DMA)
    {
        return "DMA传输错误: 发生DMA传输错误，表示与SPI操作相关的DMA传输失败。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_FLAG)
    {
        return "标志错误: 发生RXP/TXP/DXP/FTLVL/FRLVL标志错误，表示SPI标志状态异常。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_ABORT)
    {
        return "中止错误: SPI中止过程中发生错误。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_UDR)
    {
        return "欠载错误: 发生欠载错误，表示接收缓冲区为空但仍有数据请求。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_TIMEOUT)
    {
        return "超时错误: SPI操作期间发生超时，意味着操作未在预期时间内完成。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_UNKNOW)
    {
        return "未知错误: SPI操作期间发生未知错误。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_NOT_SUPPORTED)
    {
        return "不支持的操作: 请求的操作不被支持。";
    }
    else if (ErrorCode & HAL_SPI_ERROR_RELOAD)
    {
        return "重载错误: 发生重载错误，表示SPI重载操作失败。";
    }
    else
    {
        return "未知错误: SPI操作期间发生未知错误。";
    }
}
