################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/DS18B20_Driver/DS18B20.c 

C_DEPS += \
./Drivers/DS18B20_Driver/DS18B20.d 

OBJS += \
./Drivers/DS18B20_Driver/DS18B20.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/DS18B20_Driver/DS18B20.o: ../Drivers/DS18B20_Driver/DS18B20.c Drivers/DS18B20_Driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L452xx -c -I../Core/Inc -I"C:/STM32_Projects/L452RE_AgroStp/Drivers/Sensor_Driver" -I"C:/STM32_Projects/L452RE_AgroStp/Drivers/DS18B20_Driver" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DS18B20_Driver/DS18B20.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

