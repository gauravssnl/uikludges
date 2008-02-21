#
# Automatized build process for Python extensions
#
# Runs under Symbian SDK make and Cygwin make
#
# API documentation building needs Epydoc and zip command from Cygwin
# - http://sourceforge.net/project/showfiles.php?group_id=32455&package_id=24617&release_id=572670
# - www.cygwin.com
#
# Copyright 2008 Red Innovation Ltd. 
#
# Author: Mikko Ohtamaa <mikko@redinnovation.com>
#
#
#

# Human readable name for the extension
PROJECTNAME="Uikludges 2.0"

# DLL, pkg, etc. filename base
DLLNAME=uikludges

# Where is your stuff installed
PYTHON=c:\python25\python
EPYDOC=c:\python25\scripts\epydoc

#
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
# Creates SIS file which allows install the custom DLL. The resulting file is 
# placed in a _build subfolder from where you can Bluetooth it to your phone.
#
sis:
	- rmdir /s /q _build > nul
	mkdir _build
	copy $EPOCROOTepoc32\release\gcce\urel\_${DLLNAME}.pyd _build
	makesis ${DLLNAME}.pkg

#
# Use Epydoc to generate API documentation.
# 
# Note that apidoc folder is marked as svn:ignore. Committing API documentation 
# to repository is waste of bandwidth and disk space, since plenty of HTML files
# are generated. Instead, a zipped archive is built and committed.		
#
docs:
	${PYTHON} ${EPYDOC} -v --fail-on-warning --no-private --name "${PROJECTNAME}" --parse-only --html --graph all -o apidoc ${DLLNAME}.py
	zip -r apidoc.zip apidoc
	