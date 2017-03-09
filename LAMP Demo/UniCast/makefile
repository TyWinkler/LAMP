################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/TI/ccsv7/tools/compiler/ti-cgt-arm_16.12.0.STS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./LCD.obj" \
"./LED.obj" \
"./LPD8806.obj" \
"./main.obj" \
"./pinmux.obj" \
"./salowcc3200_ili9341.obj" \
"./startup_ccs.obj" \
"./timer_if.obj" \
"./uart_if.obj" \
"./udma_if.obj" \
"./wdt_if.obj" \
"../cc3200v1p32.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-l"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/grlib/ccs-cm3/Debug/grlib-cm3.lib" \
-l"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/third_party/fatfs/ccs/Release/fatfs.a" \
-l"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/driverlib/ccs/Release/driverlib.a" \
-l"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/simplelink/ccs/OS/simplelink.a" \
-l"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/oslib/ccs/free_rtos/free_rtos.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
LAMP\ Demo.out \

EXE_OUTPUTS__QUOTED += \
"LAMP Demo.out" \

BIN_OUTPUTS += \
LAMP\ Demo.bin \

BIN_OUTPUTS__QUOTED += \
"LAMP Demo.bin" \


# All Target
all: LAMP\ Demo.out

# Tool invocations
LAMP\ Demo.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/TI/ccsv7/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --define=ccs --define=NOTERM --define=USE_FREERTOS --define=SL_PLATFORM_MULTI_THREADED --define=cc3200 -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"LAMP Demo.map" --heap_size=0x17000 --stack_size=0x500 -i"C:/TI/ccsv7/tools/compiler/ti-cgt-arm_16.12.0.STS/lib" -i"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/grlib/ccs-cm3/Debug" -i"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/driverlib/ccs/Release" -i"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/grlib" -i"C:/TI/CC3200SDK_1.2.0/cc3200-sdk/inc" -i"C:/TI/ccsv7/tools/compiler/ti-cgt-arm_16.12.0.STS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="LAMP Demo_linkInfo.xml" --rom_model -o "LAMP Demo.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

LAMP\ Demo.bin: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/TI/ccsv7/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armhex"  -o "LAMP Demo.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "LCD.obj" "LED.obj" "LPD8806.obj" "main.obj" "pinmux.obj" "salowcc3200_ili9341.obj" "startup_ccs.obj" "timer_if.obj" "uart_if.obj" "udma_if.obj" "wdt_if.obj" 
	-$(RM) "LCD.d" "LED.d" "LPD8806.d" "main.d" "pinmux.d" "salowcc3200_ili9341.d" "startup_ccs.d" "timer_if.d" "uart_if.d" "udma_if.d" "wdt_if.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-@echo 'convert to bin file'
	-"C:/TI/ccsv7/utils/tiobj2bin/tiobj2bin" "LAMP Demo.out" "LAMP Demo.bin" "C:/TI/ccsv7/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armofd" "C:/TI/ccsv7/tools/compiler/ti-cgt-arm_16.12.0.STS/bin/armhex" "C:/TI/ccsv7/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

