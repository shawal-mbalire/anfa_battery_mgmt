################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.c 

OBJS += \
./Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.o 

C_DEPS += \
./Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USBPD_Library/Core/src/%.o Middlewares/ST/STM32_USBPD_Library/Core/src/%.su Middlewares/ST/STM32_USBPD_Library/Core/src/%.cyclo: ../Middlewares/ST/STM32_USBPD_Library/Core/src/%.c Middlewares/ST/STM32_USBPD_Library/Core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32G0C1xx -DUSBPD_PORT_COUNT=2 -D_SNK -DUSBPDCORE_LIB_PD3_FULL -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I../USBPD/App -I../USBPD/Target -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USBPD_Library/Core/inc -I../Middlewares/ST/STM32_USBPD_Library/Devices/STM32G0XX/inc -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-STM32_USBPD_Library-2f-Core-2f-src

clean-Middlewares-2f-ST-2f-STM32_USBPD_Library-2f-Core-2f-src:
	-$(RM) ./Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.cyclo ./Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.d ./Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.o ./Middlewares/ST/STM32_USBPD_Library/Core/src/usbpd_trace.su

.PHONY: clean-Middlewares-2f-ST-2f-STM32_USBPD_Library-2f-Core-2f-src

