# invoke SourceDir generated makefile for empty.pem4f
empty.pem4f: .libraries,empty.pem4f
.libraries,empty.pem4f: package/cfg/empty_pem4f.xdl
	$(MAKE) -f C:\Users\THE_WO~1\GOOGLE~1\School\2018_3_Semester_Fall\CPE403\TIVAC_RTOS_ws\blink_target_RTOS/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\THE_WO~1\GOOGLE~1\School\2018_3_Semester_Fall\CPE403\TIVAC_RTOS_ws\blink_target_RTOS/src/makefile.libs clean

