# invoke SourceDir generated makefile for hello.pem3
hello.pem3: .libraries,hello.pem3
.libraries,hello.pem3: package/cfg/hello_pem3.xdl
	$(MAKE) -f C:\Users\F-YH5B\GOOGLE~1\School\2018_3_Semester_Fall\CPE403\Workspace8\hello_CC1350_LAUNCHXL_tirtos_ccs/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\F-YH5B\GOOGLE~1\School\2018_3_Semester_Fall\CPE403\Workspace8\hello_CC1350_LAUNCHXL_tirtos_ccs/src/makefile.libs clean

