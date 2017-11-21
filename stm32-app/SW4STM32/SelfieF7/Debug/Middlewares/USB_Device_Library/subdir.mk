################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c 

OBJS += \
./Middlewares/USB_Device_Library/usbd_cdc.o \
./Middlewares/USB_Device_Library/usbd_core.o \
./Middlewares/USB_Device_Library/usbd_ctlreq.o \
./Middlewares/USB_Device_Library/usbd_ioreq.o 

C_DEPS += \
./Middlewares/USB_Device_Library/usbd_cdc.d \
./Middlewares/USB_Device_Library/usbd_core.d \
./Middlewares/USB_Device_Library/usbd_ctlreq.d \
./Middlewares/USB_Device_Library/usbd_ioreq.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/USB_Device_Library/usbd_cdc.o: C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_core.o: C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_ctlreq.o: C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/USB_Device_Library/usbd_ioreq.o: C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


