#include "./i2c_msp.h"

// 获取名字字符串
const char *i2c_name_str_get(I2C_TypeDef *Instance)
{
    if (Instance == I2C1)
    {
        return "I2C1";
    }
    else if (Instance == I2C2)
    {
        return "I2C2";
    }
    else if (Instance == I2C3)
    {
        return "I2C3";
    }
    else if (Instance == I2C4)
    {
        return "I2C4";
    }
    else
    {
        return "I2C_Unknown";
    }
}
const char *i2c_error_str_get(uint32_t ErrorCode)
{
    if (ErrorCode == HAL_I2C_ERROR_NONE)
    {
        return "无错误: I2C操作期间未发生任何错误。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_BERR)
    {
        return "BERR错误: I2C操作期间发生总线错误，表示数据传输过程中出现对齐错误。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_ARLO)
    {
        return "ARLO错误: 发生仲裁丢失错误，意味着在多主机通信中，I2C主设备失去了仲裁。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_AF)
    {
        return "ACKF错误: 发生应答失败错误，表示I2C从设备在传输过程中未应答某个字节。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_OVR)
    {
        return "OVR错误: 发生溢出/欠载错误，意味着I2C外设接收数据的速度快于处理速度，或反之。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_DMA)
    {
        return "DMA传输错误: 发生DMA传输错误，表示与I2C操作相关的DMA传输失败。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_TIMEOUT)
    {
        return "超时错误: I2C操作期间发生超时，意味着操作未在预期时间内完成。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_SIZE)
    {
        return "大小管理错误: 发生大小管理错误，表示数据传输的大小存在问题。";
    }
    else if (ErrorCode & HAL_I2C_ERROR_DMA_PARAM)
    {
        return "DMA参数错误: 发生DMA参数错误，表示为I2C操作配置的DMA参数存在问题。";
    }
    else
    {
        return "未知错误: I2C操作期间发生未知错误。";
    }
}
