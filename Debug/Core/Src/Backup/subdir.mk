################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Backup/IOC_refcode.c 

OBJS += \
./Core/Src/Backup/IOC_refcode.o 

C_DEPS += \
./Core/Src/Backup/IOC_refcode.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Backup/%.o Core/Src/Backup/%.su Core/Src/Backup/%.cyclo: ../Core/Src/Backup/%.c Core/Src/Backup/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -IC:/Users/Administrator/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/Administrator/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/Administrator/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/Administrator/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Backup

clean-Core-2f-Src-2f-Backup:
	-$(RM) ./Core/Src/Backup/IOC_refcode.cyclo ./Core/Src/Backup/IOC_refcode.d ./Core/Src/Backup/IOC_refcode.o ./Core/Src/Backup/IOC_refcode.su

.PHONY: clean-Core-2f-Src-2f-Backup

