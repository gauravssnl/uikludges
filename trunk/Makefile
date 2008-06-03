#
#
# Reserved UIDs used in
# - uikludges.pkg
# - modules\uikludges\uikludges.mmp
# - modules\applicationmanager\applicationmanager.mmp
#

# Where is your stuff installed
PYTHON=c:\python25\python
EPYDOC=c:\python25\scripts\epydoc
ENSYMBLE=c:\ensymble\ensymble.py


# Certificate files needed to sign the SIS
CERT_PATH=C:\certificates
CERT=${CERT_PATH}\JussiAndOthers.cer
PRIV_KEY=${CERT_PATH}\PrivateKeyNoPassphrase.pem

# Required capabilties in Ensymble format.
# List all Dev. Cert. capabilities
CAPABILITIES="PowerMgmt+ReadDeviceData+WriteDeviceData+TrustedUI+ProtServ+SwEvent+NetworkServices+LocalServices+ReadUserData+WriteUserData+Location+SurroundingsDD+UserEnvironment"

MODULES="uikludges applicationmanager"

UNSIGNED_SIS=build\uikludges_unsigned.sis
SIGNED_SIS=build\uikludges_signed.sis

all: extensions sis docs 

#
# Use Epydoc to generate API documentation.
# 
# Note that apidoc folder is marked as svn:ignore. Committing API documentation 
# to repository is waste of bandwidth and disk space, since plenty of HTML files
# are generated. Instead, a zipped archive is built and committed.		
#
docs:
	${PYTHON} ${EPYDOC} -v --fail-on-warning --no-private --name "${PROJECTNAME}" --parse-only --html --graph all -o apidoc lib/*
	zip -r apidoc.zip apidoc

#
# Compile native extensions to DLL (PYD) files
# 	
extensions:
	- rmdir /s /q build
	mkdir build

	cd modules\uikludges ; make -f Makefile.extension DLLNAME=uikludges
	cd ..\..

	cd modules\applicationmanager ; make -f Makefile.extension DLLNAME=applicationmanager
	cd ..\..
	
#
# Bundle .py and .dll for SIS installation
#
sis: 
	makesis uikludges.pkg 
	copy uikludges.SIS ${UNSIGNED_SIS}
	
#
# Sign SIS for phone installation
#
sign_sis:
	${PYTHON} ${ENSYMBLE} signsis --cert ${CERT} --privkey=${PRIV_KEY} --passphrase=${SIGN_PASS} --execaps=${CAPABILITIES} --dllcaps=${CAPABILITIES} ${UNSIGNED_SIS} ${SIGNED_SIS}
	
	