################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1718096031:
	@$(MAKE) --no-print-directory -Onone -f tirtos_builds_cc13x2_cc26x2_release_ccs/subdir_rules.mk build-1718096031-inproc

build-1718096031-inproc: ../tirtos_builds_cc13x2_cc26x2_release_ccs/release.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/ccs1011/xdctools_3_61_02_27_core/xs" --xdcpath="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source;C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/kernel/tirtos/packages;C:/ti/simplelink_cc2640r2_sdk_4_20_00_04/source;C:/ti/simplelink_cc2640r2_sdk_4_20_00_04/kernel/tirtos/packages;C:/ti/simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62/source;C:/ti/simplelink_cc26x2_sdk_2_20_00_36/source;C:/ti/simplelink_cc26x2_sdk_2_20_00_36/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -r debug -c "C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1718096031 ../tirtos_builds_cc13x2_cc26x2_release_ccs/release.cfg
configPkg/compiler.opt: build-1718096031
configPkg/: build-1718096031


