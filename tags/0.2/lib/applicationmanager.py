"""

	Application manager allows programmaticalyl manipalte Series 60 applications.
				
	
"""


# For field information, see http://epydoc.sourceforge.net/manual-fields.html
__author__ = "Mikko Ohtamaa <mikko@redinnovation.com>"
__copyright__ = "Copyright 2008 Red Innovation Ltd."
__docformat__ = "epytext"
__license__ = "BSD" 
__version__ = "2.0"

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

#: Exported functions from this module
__all__ = ["switch_to_bg", 
		   "switch_to_fg", 
		   "kill_app", 
		   "end_app", 
		   "application_list"]

# Export all E*** pseudo constants automatically
def _test():
	""" Self-testing for uikludges module. 
		
	"""
	
	print str(application_list())
	
		
if __name__ == "__main__":
	# Perform self test is this Python module is used as an application entry point	
	_test()
	
	
	
	
	