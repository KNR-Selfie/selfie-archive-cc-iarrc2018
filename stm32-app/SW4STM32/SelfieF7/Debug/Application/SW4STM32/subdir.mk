################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
C:/Users/mice/workspace/SelfieF7/SW4STM32/startup_stm32f745xx.s 

OBJS += \
./Application/SW4STM32/startup_stm32f745xx.o 


# Each subdirectory must supply rules for building sources it contributes
Application/SW4STM32/startup_stm32f745xx.o: C:/Users/mice/workspace/SelfieF7/SW4STM32/startup_stm32f745xx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


