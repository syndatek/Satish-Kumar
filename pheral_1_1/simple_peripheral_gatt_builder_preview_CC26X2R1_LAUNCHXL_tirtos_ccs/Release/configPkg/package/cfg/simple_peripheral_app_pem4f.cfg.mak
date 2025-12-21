# invoke SourceDir generated makefile for simple_peripheral_app.pem4f
simple_peripheral_app.pem4f: .libraries,simple_peripheral_app.pem4f
.libraries,simple_peripheral_app.pem4f: package/cfg/simple_peripheral_app_pem4f.xdl
	$(MAKE) -f C:\tibackup\pheral_1_1\simple_peripheral_gatt_builder_preview_CC26X2R1_LAUNCHXL_tirtos_ccs/src/makefile.libs

clean::
	$(MAKE) -f C:\tibackup\pheral_1_1\simple_peripheral_gatt_builder_preview_CC26X2R1_LAUNCHXL_tirtos_ccs/src/makefile.libs clean

