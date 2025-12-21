################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1462205697: ../project_zero.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"D:/CCS/ccs/utils/sysconfig_1.21.0/sysconfig_cli.bat" --script "D:/Sathish_Frimware/tibackup/SydantekVer4.1_Workspace0p1/application/project_zero.syscfg" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/error.h: build-1462205697 ../project_zero.syscfg
syscfg: build-1462205697

build-1055094257:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1055094257-inproc

build-1055094257-inproc: ../project_zero_app.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"D:/CCS/xdctools_3_62_01_15_core/xs" --xdcpath="D:/CCS/xdctools_3_50_08_24_core/eclipse;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tirtos:CC26X2R1F -r release -c "D:/CCS/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O4 --opt_for_speed=0 --include_path=\"D:/Sathish_Frimware/tibackup/SydantekVer4.1_Workspace0p1/application\" --include_path=\"D:/Sathish_Frimware/tibackup/SydantekVer4.1_Workspace0p1/application/Release\" --include_path=\"D:/Sathish_Frimware/tibackup/SydantekVer4.1_Workspace0p1/application/Application\" --include_path=\"/source/ti/ble5stack\" --include_path=\"/source/ti/ble5stack/common/cc26xx/\" --include_path=\"/source/ti/ble5stack/boards/CC26X2R1_LAUNCHXL/\" --include_path=\"/source/ti/drivers/nvs/\" --include_path=\"/source/ti/common/cc26xx/oad/\" --include_path=\"/source/ti/ble5stack/profiles/oad/cc26xx\" --include_path=\"/source/ti/\" --include_path=\"/source\" --include_path=\"/kernel/tirtos/packages\" --include_path=\"/source/ti/ble5stack/controller/cc26xx/inc\" --include_path=\"/source/ti/ble5stack/inc\" --include_path=\"/source/ti/ble5stack/rom\" --include_path=\"/source/ti/ble5stack/common/cc26xx\" --include_path=\"/source/ti/ble5stack/icall/inc\" --include_path=\"/source/ti/ble5stack/hal/src/target/_common\" --include_path=\"/source/ti/ble5stack/hal/src/target/_common/cc26xx\" --include_path=\"/source/ti/ble5stack/hal/src/inc\" --include_path=\"/source/ti/ble5stack/heapmgr\" --include_path=\"/source/ti/ble5stack/profiles/dev_info\" --include_path=\"/source/ti/ble5stack/profiles/simple_profile\" --include_path=\"/source/ti/ble5stack/icall/src/inc\" --include_path=\"/source/ti/ble5stack/osal/src/inc\" --include_path=\"/source/ti/ble5stack/services/src/saddr\" --include_path=\"/source/ti/ble5stack/services/src/sdata\" --include_path=\"/source/ti/common/nv\" --include_path=\"/source/ti/common/cc26xx\" --include_path=\"/source/ti/devices/cc13x2_cc26x2\" --include_path=\"/source/ti/posix/ccs\" --include_path=\"D:/CCS/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include\" --define=SECURITY --define=uartlog_FILE=\"\"\"\" --define=DeviceFamily_CC26X2 --define=FLASH_ROM_BUILD --define=NVOCMP_NWSAMEITEM=1 -g --c99 --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1055094257 ../project_zero_app.cfg
configPkg/compiler.opt: build-1055094257
configPkg: build-1055094257


