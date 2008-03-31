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





#: Exported functions from this module
__all__ = ["set_softkey_text", "set_softkey_visibility", "query"]

# Export all E*** pseudo constants automatically
for x in locals().keys():
	if x.startswith("E"):
		__all__.append(x)

def _test():
	""" Self-testing for uikludges module. 
		
	"""
	
	print "UIKludges self-testing"
	
	import appuifw
	
	c = appuifw.Canvas()
	appuifw.app.body = c
	c.text([0, 0], "Testing UI operations")

	query(u"Simple query",  "query")
	query(u"Simple query 2",  "query", tone=EErrorTone)
	query(u"Simple query 3",  "query", header=u"Test header")
	
	long_text = u"This a long long text text long long text text long long text text long long text text. "
	long_text = long_text * 4
	
	query(long_text,  "message")

	# Test message query
	query(long_text, 
	      "message", None, header=u"A header", show_left_softkey=False, tone=EErrorTone)
	
	# Use visual triaging method for error detection
	set_softkey_text(EAknSoftkeyExit, u"Hello")
	e32.ao_sleep(1)
	set_softkey_text(EAknSoftkeyOptions, u"Moo was here")
	e32.ao_sleep(1)	
	set_softkey_visibility(EAknSoftkeyOptions, False)
	e32.ao_sleep(1)	
	set_softkey_visibility(EAknSoftkeyOptions, True)
	
	try:
		set_softkey_text(1001, u"Bad id")
	except:
		set_softkey_text(EAknSoftkeyOptions, u"Bad id test ok")
		pass
	e32.ao_sleep(1)			
		
if __name__ == "__main__":
	# Perform self test is this Python module is used as an application entry point	
	_test()
	
	
	
	
	