#
# Building SIS file from gcce urel files for Windows XP/Symbian SDK
#
#

all: build_pyd sis


#
# Builds target GGCE UREL DLL file (comes to ${EPOCROOT}epoc32\release\gcce\urel\).
# Cleans everything to make sure that all gets build properly.
#
build_pyd:
	bldmake bldfiles 
	call abld clean
	call abld build gcce urel

#
# Creates SIS file which allows install the custom DLL
#
sis:
	- rmdir /s /q _build > nul
	mkdir _build
	copy ${EPOCROOT}epoc32\release\gcce\urel\_uikludges.pyd _build
	makesis uikludges.pkg