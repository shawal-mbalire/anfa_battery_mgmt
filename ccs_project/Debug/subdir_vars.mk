################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../main.syscfg 

C_SRCS += \
../main.c \
./ti_msp_dl_config.c \
/home/shawal/ti/mspm0_sdk_2_05_01_00/source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c 

GEN_CMDS += \
./device_linker.lds 

GEN_FILES += \
./device_linker.lds \
./device.opt \
./ti_msp_dl_config.c 

C_DEPS += \
./main.d \
./ti_msp_dl_config.d \
./startup_mspm0g350x_gcc.d 

GEN_OPTS += \
./device.opt 

OBJS += \
./main.o \
./ti_msp_dl_config.o \
./startup_mspm0g350x_gcc.o 

GEN_MISC_FILES += \
./device.lds.genlibs \
./ti_msp_dl_config.h 

OBJS__QUOTED += \
"main.o" \
"ti_msp_dl_config.o" \
"startup_mspm0g350x_gcc.o" 

GEN_MISC_FILES__QUOTED += \
"device.lds.genlibs" \
"ti_msp_dl_config.h" 

C_DEPS__QUOTED += \
"main.d" \
"ti_msp_dl_config.d" \
"startup_mspm0g350x_gcc.d" 

GEN_FILES__QUOTED += \
"device_linker.lds" \
"device.opt" \
"ti_msp_dl_config.c" 

C_SRCS__QUOTED += \
"../main.c" \
"./ti_msp_dl_config.c" \
"/home/shawal/ti/mspm0_sdk_2_05_01_00/source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c" 

SYSCFG_SRCS__QUOTED += \
"../main.syscfg" 


