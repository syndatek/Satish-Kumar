# invoke SourceDir generated makefile for project_zero_app.pem4f
project_zero_app.pem4f: .libraries,project_zero_app.pem4f
.libraries,project_zero_app.pem4f: package/cfg/project_zero_app_pem4f.xdl
	$(MAKE) -f C:\tibackup\SY52FC~1.1_W\application/src/makefile.libs

clean::
	$(MAKE) -f C:\tibackup\SY52FC~1.1_W\application/src/makefile.libs clean

