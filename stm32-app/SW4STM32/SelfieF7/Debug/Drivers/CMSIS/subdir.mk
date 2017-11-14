################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/mice/workspace/SelfieF7/Src/system_stm32f7xx.c 

OBJS += \
./Drivers/CMSIS/system_stm32f7xx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32f7xx.o: C:/Users/mice/workspace/SelfieF7/Src/system_stm32f7xx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F745xx -I"C:/Users/mice/workspace/SelfieF7/Inc" -I"C:/Users/mice/workspace/SelfieF7/SW4STM32/SelfieF7/Application/User/Lighting" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Device/ST/STM32F7xx/Include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/mice/workspace/SelfieF7/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/mice/workspace/SelfieF7/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


