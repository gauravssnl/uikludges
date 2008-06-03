"""

	Application manager allows programmatically manipulate Series 60 applications.
				
	
"""


# For field information, see http://epydoc.sourceforge.net/manual-fields.html
__author__ = "Mikko Ohtamaa <mikko@redinnovation.com>"
__copyright__ = "Copyright 2008 Red Innovation Ltd."
__docformat__ = "epytext"
__license__ = "BSD" 

__version__ = "2.1"

import e32

if e32.s60_version_info>=(3,0):
	# Symbian 9.0 way to import things
    import imp
    _applicationmanager=imp.load_dynamic('_applicationmanager', 'c:\\sys\\bin\\_applicationmanager.pyd')    
else:
	# Old way
    import _applicationmanager
    
def application_list(include_hidden=False):
	""" Get all running applications.
	
	@param include_hidden: Set to true to list hidden applications also.	
	@return: List of application caption strings
	"""
	return _applicationmanager.application_list(include_hidden)
	
def switch_to_fg(appname):
	""" Bring application to the foreground.
	
	@param appname: Application caption (as returned by application_list)
	@type appname: string
	@return: True if the operation success
	"""
	return _applicationmanager.switch_to_fg(appname)

def switch_to_bg(appname):
	""" Put application to the background.
	
	@param appname: Application caption (as returned by application_list)
	@type appname: string
	@return: True if the operation success
	"""
	return _applicationmanager.switch_to_bg(appname)

def kill_app(appname):
	""" Forcefully terminate an application.
	
	@param appname: Application caption (as returned by application_list)
	@type appname: string
	@return: True if the operation success	
	"""	
	return _applicationmanager.kill_app(appname)

def end_app(appname):
	""" Ask application to shutdown.
	
	@param appname: Application caption (as returned by application_list)
	@type appname: string
	@return: True if the operation success	
	"""	
	return _applicationmanager.end_app(appname)

def launch_py_background(python_file, launcher=u"uikludges_python_launcher.exe"):
	""" Launch Python as a background process.
	
	This function works around the limitations of e32.start_server process
	which has zero capabilities for the launched EXE.::
	
		ensymble.py altere32 modules\applicationmanager\uikludges_python_launcher.exe --caps=PowerMgmt+ReadDeviceData+WriteDeviceData+TrustedUI+ProtServ+SwEvent+NetworkServices+LocalServices+ReadUserData+WriteUserData+Location+SurroundingsDD+UserEnvironment --uid=0xA0008CDB  modules\applicationmanager\uikludges_python_launcher_mod.exe
	
	The default launcher is built against Series 60 3.0 FP1 and
	PyS60 1.4.3. If you use other configuration you might need
	to recompile python_launcher.exe to match your Application
	Binary Interface. The application EXE drop is in applicationmanager
	folder.
	
	The default launcher is enabled with all dev. cert. cabilities.
	Use Ensymble tool to tune capabilities for Symbian Signed process.
	You need to rewrite the UID also.
	
	@param python_file: Full path to Python file in Symbian path convention
	@type python_file: unicode
	
	@param launcher: Stub EXE used to execute the file
	@type launcher: unicode
	
	@return: None
	@raise SymbianError: If the file does not exist or capability problems
	"""	
	return _applicationmanager.launch_py_background(appname)


#: Exported functions from this module
__all__ = ["switch_to_bg", 
		   "switch_to_fg", 
		   "kill_app", 
		   "end_app", 
		   "application_list",
		   "launch_py_background"]

def _test():
	""" Self-testing for uikludges module. 
		
	"""
	
	print str(application_list())
			
if __name__ == "__main__":
	# Perform self test is this Python module is used as an application entry point	
	_test()
	
	
	
	
	