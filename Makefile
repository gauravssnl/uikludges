
# Where is your stuff installed
PYTHON=c:\python25\python
EPYDOC=c:\python25\scripts\epydoc

MODULES="uikludges applicationmanager"

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
	
extensions:
	echo "FUCK"
	cd modules\uikludges ; make -f Makefile.extension DLLNAME=uikludges
	cd ..\..

sis: 
	makesis uikludges