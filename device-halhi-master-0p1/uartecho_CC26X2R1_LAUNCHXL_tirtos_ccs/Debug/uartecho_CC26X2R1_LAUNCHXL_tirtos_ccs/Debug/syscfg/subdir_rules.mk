################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg/%.obj: ../uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/tibackup/device-halhi-master-0p1/uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs" --include_path="C:/tibackup/device-halhi-master-0p1/uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/posix/ccs" --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=DeviceFamily_CC26X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg/$(basename $(<F)).d_raw" --include_path="C:/tibackup/device-halhi-master-0p1/uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" --obj_directory="uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


