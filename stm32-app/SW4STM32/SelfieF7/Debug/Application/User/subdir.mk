################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/mice/workspace/SelfieF7/Src/adc.c \
C:/Users/mice/workspace/SelfieF7/Src/can.c \
C:/Users/mice/workspace/SelfieF7/Src/dma.c \
C:/Users/mice/workspace/SelfieF7/Src/freertos.c \
C:/Users/mice/workspace/SelfieF7/Src/gpio.c \
C:/Users/mice/workspace/SelfieF7/Src/i2c.c \
C:/Users/mice/workspace/SelfieF7/Src/main.c \
C:/Users/mice/workspace/SelfieF7/Src/spi.c \
C:/Users/mice/workspace/SelfieF7/Src/stm32f7xx_hal_msp.c \
C:/Users/mice/workspace/SelfieF7/Src/stm32f7xx_hal_timebase_TIM.c \
C:/Users/mice/workspace/SelfieF7/Src/stm32f7xx_it.c \
C:/Users/mice/workspace/SelfieF7/Src/tim.c \
C:/Users/mice/workspace/SelfieF7/Src/usart.c \
C:/Users/mice/workspace/SelfieF7/Src/usb_device.c \
C:/Users/mice/workspace/SelfieF7/Src/usbd_cdc_if.c \
C:/Users/mice/workspace/SelfieF7/Src/usbd_conf.c \
C:/Users/mice/workspace/SelfieF7/Src/usbd_desc.c 

OBJS += \
./Application/User/adc.o \
./Application/User/can.o \
./Application/User/dma.o \
./Application/User/freertos.o \
./Application/User/gpio.o \
./Application/User/i2c.o \
./Application/User/main.o \
./Application/User/spi.o \
./Application/User/stm32f7xx_hal_msp.o \
./Application/User/stm32f7xx_hal_timebase_TIM.o \
./Application/User/stm32f7xx_it.o \
./Application/User/tim.o \
./Application/User/usart.o \
./Application/User/usb_device.o \
./Application/User/usbd_cdc_if.o \
./Application/User/usbd_conf.o \
./Application/User/usbd_desc.o 

C_DEPS += \
./Application/User/adc.d \
./Application/User/can.d \
./Application/User/dma.d \
./Application/User/freertos.d \
./Application/User/gpio.d \
./Application/User/i2c.d \
./Application/User/main.d \
./Application/User/spi.d \
./Application/User/stm32f7xx_hal_msp.d \
./Application/User/stm32f7xx_hal_timebase_TIM.d \
./Application/User/stm32f7xx_it.d \
./Application/User/tim.d \
./Application/User/usart.d \
./Application/User/usb_device.d \
./Application/User/usbd_cdc_if.d \
./Application/User/usbd_conf.d \
./Application/User/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/adc.o: C:/Users/mice/workspace/SelfieF7/Src/adc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/can.o: C:/Users/mice/workspace/SelfieF7/Src/can.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/dma.o: C:/Users/mice/workspace/SelfieF7/Src/dma.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/freertos.o: C:/Users/mice/workspace/SelfieF7/Src/freertos.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/gpio.o: C:/Users/mice/workspace/SelfieF7/Src/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/i2c.o: C:/Users/mice/workspace/SelfieF7/Src/i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/main.o: C:/Users/mice/workspace/SelfieF7/Src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/spi.o: C:/Users/mice/workspace/SelfieF7/Src/spi.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/stm32f7xx_hal_msp.o: C:/Users/mice/workspace/SelfieF7/Src/stm32f7xx_hal_msp.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/stm32f7xx_hal_timebase_TIM.o: C:/Users/mice/workspace/SelfieF7/Src/stm32f7xx_hal_timebase_TIM.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/stm32f7xx_it.o: C:/Users/mice/workspace/SelfieF7/Src/stm32f7xx_it.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/tim.o: C:/Users/mice/workspace/SelfieF7/Src/tim.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/usart.o: C:/Users/mice/workspace/SelfieF7/Src/usart.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/usb_device.o: C:/Users/mice/workspace/SelfieF7/Src/usb_device.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/usbd_cdc_if.o: C:/Users/mice/workspace/SelfieF7/Src/usbd_cdc_if.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/usbd_conf.o: C:/Users/mice/workspace/SelfieF7/Src/usbd_conf.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/usbd_desc.o: C:/Users/mice/workspace/SelfieF7/Src/usbd_desc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Gyro"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


