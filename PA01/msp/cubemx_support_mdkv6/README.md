让cubemx生成的项目支持MDKv6，需要用一下目录的文件

```
C:\Users\%UserName%\STM32Cube\Repository\STM32Cube_FW_H7_V1.11.2\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM4F

port.c portmacro.h
```

替换

```
.\Middlewares\Third_Party\FreeRTOS\Source\portable\RVDS\ARM_CM4F
```
