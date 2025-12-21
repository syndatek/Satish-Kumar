################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-566520319:
	@$(MAKE) --no-print-directory -Onone -f Tools/subdir_rules.mk build-566520319-inproc

build-566520319-inproc: ../Tools/simple_peripheral_oad_onchip_app.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/ccs1011/xdctools_3_61_02_27_core/xs" --xdcpath="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source;C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/kernel/tirtos/packages;C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.simplelink:CC2642R1F -r release -c "C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS" --cfgArgs "{OAD_IMG_B:1}" --compileOptions "-mv7M4 --code_state=16 -me -O4 --opt_for_speed=0 --include_path=\"C:/tibackup/pheral_1_1/simple_peripheral_oad_onchip_CC26X2R1_LAUNCHXL_tirtos_ccs\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/controller/cc26xx/inc\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/inc\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/rom\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx/rcosc\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/icall/inc\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/examples/rtos/CC26X2R1_LAUNCHXL/ble5stack/simple_peripheral_oad_onchip/Application\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/cc26xx\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/cc26xx/flash_interface\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/cc26xx/oad/\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx/menu\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/profiles/dev_info\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/profiles/oad/cc26xx\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/profiles/simple_profile\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/hal/src/target/_common\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/hal/src/target/_common/cc26xx\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/hal/src/inc\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/heapmgr\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/icall/src/inc\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/osal/src/inc\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/services/src/saddr\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/services/src/sdata\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/nv\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/devices/cc13x2_cc26x2\" --include_path=\"C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/drivers/nvs\" --include_path=\"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include\" --define=DeviceFamily_CC26X2 --define=FLASH_ROM_BUILD --define=NVOCMP_NWSAMEITEM=1 --define=SECURITY --define=OAD_IMG_B --define=OAD_ONCHIP -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-566520319 ../Tools/simple_peripheral_oad_onchip_app.cfg
configPkg/compiler.opt: build-566520319
configPkg/: build-566520319


