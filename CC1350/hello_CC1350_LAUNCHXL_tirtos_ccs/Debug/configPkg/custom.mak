## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em3 linker.cmd package/cfg/hello_pem3.oem3

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/hello_pem3.xdl
	$(SED) 's"^\"\(package/cfg/hello_pem3cfg.cmd\)\"$""\"C:/Users/F-YH5B/Google Drive/School/2018_3_Semester_Fall/CPE403/Workspace8/hello_CC1350_LAUNCHXL_tirtos_ccs/Debug/configPkg/\1\""' package/cfg/hello_pem3.xdl > $@
	-$(SETDATE) -r:max package/cfg/hello_pem3.h compiler.opt compiler.opt.defs
