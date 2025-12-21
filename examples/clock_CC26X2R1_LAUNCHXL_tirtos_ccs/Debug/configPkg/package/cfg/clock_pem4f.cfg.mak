# invoke SourceDir generated makefile for clock.pem4f
clock.pem4f: .libraries,clock.pem4f
.libraries,clock.pem4f: package/cfg/clock_pem4f.xdl
	$(MAKE) -f C:\tibackup\examples\clock_CC26X2R1_LAUNCHXL_tirtos_ccs/src/makefile.libs

clean::
	$(MAKE) -f C:\tibackup\examples\clock_CC26X2R1_LAUNCHXL_tirtos_ccs/src/makefile.libs clean

