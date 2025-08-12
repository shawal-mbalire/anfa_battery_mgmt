################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/usr/bin/gcc" -c @"device.opt"  -mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft -I"/home/shawal/GitHub/anfa_battery_management/ccs_project" -I"/home/shawal/GitHub/anfa_battery_management/ccs_project/Debug" -I"/home/shawal/ti/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"/home/shawal/ti/mspm0_sdk_2_05_01_00/source" -I"/usr/bin/arm-none-eabi/include/newlib-nano" -I"/usr/bin/arm-none-eabi/include" -O2 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1011103585: ../main.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/home/shawal/ti/ccs2020/ccs/utils/sysconfig_1.24.0/sysconfig_cli.sh" --script "/home/shawal/GitHub/anfa_battery_management/ccs_project/main.syscfg" -o "." -s "/home/shawal/ti/mspm0_sdk_2_05_01_00/.metadata/product.json" --compiler gcc
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.lds: build-1011103585 ../main.syscfg
device.opt: build-1011103585
device.lds.genlibs: build-1011103585
ti_msp_dl_config.c: build-1011103585
ti_msp_dl_config.h: build-1011103585

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/usr/bin/gcc" -c @"device.opt"  -mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft -I"/home/shawal/GitHub/anfa_battery_management/ccs_project" -I"/home/shawal/GitHub/anfa_battery_management/ccs_project/Debug" -I"/home/shawal/ti/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"/home/shawal/ti/mspm0_sdk_2_05_01_00/source" -I"/usr/bin/arm-none-eabi/include/newlib-nano" -I"/usr/bin/arm-none-eabi/include" -O2 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_gcc.o: /home/shawal/ti/mspm0_sdk_2_05_01_00/source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/usr/bin/gcc" -c @"device.opt"  -mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft -I"/home/shawal/GitHub/anfa_battery_management/ccs_project" -I"/home/shawal/GitHub/anfa_battery_management/ccs_project/Debug" -I"/home/shawal/ti/mspm0_sdk_2_05_01_00/source/third_party/CMSIS/Core/Include" -I"/home/shawal/ti/mspm0_sdk_2_05_01_00/source" -I"/usr/bin/arm-none-eabi/include/newlib-nano" -I"/usr/bin/arm-none-eabi/include" -O2 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


