# invoke SourceDir generated makefile for project_zero_app.pem4f
project_zero_app.pem4f: .libraries,project_zero_app.pem4f
.libraries,project_zero_app.pem4f: package/cfg/project_zero_app_pem4f.xdl
	$(MAKE) -f C:\tibackup\SydantekVer4.1_Workspace0p4_final\application/src/makefile.libs

clean::
	$(MAKE) -f C:\tibackup\SydantekVer4.1_Workspace0p4_final\application/src/makefile.libs clean

