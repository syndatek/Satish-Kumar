################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-2075553567: ../simple_peripheral_oad_onchip.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1011/ccs/utils/sysconfig_1.6.0/sysconfig_cli.bat" -b "/ti/boards/.meta/CC26X2R1_LAUNCHXL" -s "C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/.metadata/product.json" -s "C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/.metadata/product.json" -o "syscfg" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_ble_config.h: build-2075553567 ../simple_peripheral_oad_onchip.syscfg
syscfg/ti_ble_config.c: build-2075553567
syscfg/ti_build_config.opt: build-2075553567
syscfg/ti_ble_app_config.opt: build-2075553567
syscfg/ti_radio_config.c: build-2075553567
syscfg/ti_radio_config.h: build-2075553567
syscfg/ti_drivers_config.c: build-2075553567
syscfg/ti_drivers_config.h: build-2075553567
syscfg/ti_utils_build_linker.cmd.genlibs: build-2075553567
syscfg/syscfg_c.rov.xs: build-2075553567
syscfg/ti_utils_runtime_model.gv: build-2075553567
syscfg/ti_utils_runtime_Makefile: build-2075553567
syscfg/: build-2075553567

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" --cmd_file="C:/tibackup/pheral_1_1/simple_peripheral_oad_onchip_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg/ti_ble_app_config.opt" --cmd_file="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/config/build_components.opt" --cmd_file="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/config/factory_config.opt" --cmd_file="C:/tibackup/pheral_1_1/simple_peripheral_oad_onchip_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg/ti_build_config.opt"  -mv7M4 --code_state=16 -me -O4 --opt_for_speed=0 --include_path="C:/tibackup/pheral_1_1/simple_peripheral_oad_onchip_CC26X2R1_LAUNCHXL_tirtos_ccs" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/controller/cc26xx/inc" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/inc" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/rom" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx/rcosc" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/icall/inc" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/examples/rtos/CC26X2R1_LAUNCHXL/ble5stack/simple_peripheral_oad_onchip/Application" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/cc26xx" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/cc26xx/flash_interface" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/cc26xx/oad/" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/common/cc26xx/menu" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/profiles/dev_info" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/profiles/oad/cc26xx" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/profiles/simple_profile" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/hal/src/target/_common" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/hal/src/inc" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/heapmgr" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/icall/src/inc" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/osal/src/inc" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/services/src/saddr" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/ble5stack/services/src/sdata" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/common/nv" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/devices/cc13x2_cc26x2" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_4_30_00_54/source/ti/drivers/nvs" --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=DeviceFamily_CC26X2 --define=FLASH_ROM_BUILD --define=NVOCMP_NWSAMEITEM=1 --define=SECURITY --define=OAD_IMG_B --define=OAD_ONCHIP -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/tibackup/pheral_1_1/simple_peripheral_oad_onchip_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


