################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-2000468353:
	@$(MAKE) --no-print-directory -Onone -f tirtos_builds_cc13x2_cc26x2_release_ccs/Debug/configPkg/package/subdir_rules.mk build-2000468353-inproc

build-2000468353-inproc: ../tirtos_builds_cc13x2_cc26x2_release_ccs/Debug/configPkg/package/build.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/ccs1011/xdctools_3_61_02_27_core/xs" --xdcpath="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source;C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/kernel/tirtos/packages;C:/ti/simplelink_cc2640r2_sdk_4_20_00_04/source;C:/ti/simplelink_cc2640r2_sdk_4_20_00_04/kernel/tirtos/packages;C:/ti/simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62/source;C:/ti/simplelink_cc26x2_sdk_2_20_00_36/source;C:/ti/simplelink_cc26x2_sdk_2_20_00_36/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -r debug -c "C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-2000468353 ../tirtos_builds_cc13x2_cc26x2_release_ccs/Debug/configPkg/package/build.cfg
configPkg/compiler.opt: build-2000468353
configPkg/: build-2000468353

tirtos_builds_cc13x2_cc26x2_release_ccs/Debug/configPkg/package/%.obj: ../tirtos_builds_cc13x2_cc26x2_release_ccs/Debug/configPkg/package/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/tibackup/device-halhi-master-0p1/uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs" --include_path="C:/tibackup/device-halhi-master-0p1/uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/posix/ccs" --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=DeviceFamily_CC26X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="tirtos_builds_cc13x2_cc26x2_release_ccs/Debug/configPkg/package/$(basename $(<F)).d_raw" --include_path="C:/tibackup/device-halhi-master-0p1/uartecho_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" --obj_directory="tirtos_builds_cc13x2_cc26x2_release_ccs/Debug/configPkg/package" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


