# invoke SourceDir generated makefile for app.pem3
app.pem3: .libraries,app.pem3
.libraries,app.pem3: package/cfg/app_pem3.xdl
	$(MAKE) -f C:\Users\THE_WO~1\GOOGLE~1\School\2018_3_Semester_Fall\CPE403\Workspace8\coprocessor_cc1350lp\Tools/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\THE_WO~1\GOOGLE~1\School\2018_3_Semester_Fall\CPE403\Workspace8\coprocessor_cc1350lp\Tools/src/makefile.libs clean

