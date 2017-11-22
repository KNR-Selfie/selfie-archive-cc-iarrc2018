################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
C:/Users/mice/git/Selfie-autonomous-car/stm32-app/SW4STM32/startup_stm32f745xx.s 

OBJS += \
./Application/SW4STM32/startup_stm32f745xx.o 


# Each subdirectory must supply rules for building sources it contributes
Application/SW4STM32/startup_stm32f745xx.o: C:/Users/mice/git/Selfie-autonomous-car/stm32-app/SW4STM32/startup_stm32f745xx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -I"C:/Users/mice/git/Selfie-autonomous-car/stm32-app/SW4STM32/SelfieF7/Application/User/Gyro" -I"C:/Users/mice/git/Selfie-autonomous-car/stm32-app/SW4STM32/SelfieF7/Application/User/Lighting" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


