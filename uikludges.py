"""
	UIKludges 2.0
	
	http://code.google.com/p/uikludges/
	
	UIKludges provides additional UI functions over standard Series 60 appuifw.
	
	This module provides self testing capabilities. Run this from Python shell::
	
		>>> import uikludges
		>>> uikludges.test()
				
	
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
    _uikludges=imp.load_dynamic('_uikludges', 'c:\\sys\\bin\\_uikludges.pyd')    
else:
	# Old way
    import _uikludges

def _nextval():
	""" Helper function to enumerate soft key pseudo constants """
	global _val
	_val += 1
	return _val


#
# Soft key command definitions.
#

# From Epoc32\include\eikon.hrh
EAknSoftkeyOk = -2
EAknSoftkeyCancel = -1
EAknSoftkeySelect = -6

# From Epoc32\include\avkon.hrh
_val = 2999
EAknSoftkeyOptions = _nextval() # Running from 3000
EAknSoftkeyBack = _nextval()
EAknSoftkeyMark = _nextval()
EAknSoftkeyUnmark = _nextval()
EAknSoftkeyInsert = _nextval()
EAknSoftkeyYes = _nextval()
EAknSoftkeyNo = _nextval()
EAknSoftkeyDone = _nextval()
EAknSoftkeyClose = _nextval()
EAknSoftkeyExit = _nextval()
EAknSoftkeyClear = _nextval()
EAknSoftkeyUnlock = _nextval()
EAknSoftkeySave = _nextval()
EAknSoftkeyShow = _nextval()
EAknSoftkeyListen = _nextval()
EAknSoftkeyRead = _nextval()
EAknSoftkeySearch = _nextval()
EAknSoftkeyEmpty = _nextval()
EAknSoftkeyLock =_nextval()
EAknSoftkeyCall =_nextval()
EAknSoftkeyAgain =_nextval()
EAknSoftkeyQuit =_nextval()
EAknSoftkeyDetails = _nextval()
EAknSoftkeyNext =_nextval()
EAknSoftkeyOther =_nextval()
EAknSoftkeyEdit =_nextval()
EAknSoftkeyOpen =_nextval()
EAknSoftkeySend = _nextval()
EAknSoftkeyView =_nextval()
EAknSoftkeyContextOptions = _nextval()
EAknSoftkeyChange = _nextval()

#: Tone constants for query()
ENoTone = 0	# aknquerydialog.h
#: Tone constants for query()
EConfirmationTone = 1003 # avkon.hrh
#: Tone constants for query()
EWarningTone = 1004 # avkon.hrh
#: Tone constants for query()
EErrorTone = 1005 # avkon.hrh

def set_softkey_text(command, text):
	""" 
	Set soft key label.
	
	Softkey labels are defined per command. So, you are not changing
	the softkey itself, but command it presents.
	
	@param command: One of EAknSoftKey* constants in uikludges.py
	@param text: new label as unicode text 
	
	@type command: int
	@type text: unicode
	
	@raise SymbianError: if native code execution fails
	@return: None
	"""
	_uikludges.set_softkey_text(command, text)
		
def set_softkey_visibility(command, visibility):
	""" 
	Hides or shows soft key label.
	
	Softkey labels are defined per command. So, you are not changing
	the softkey itself, but command it presents.
	
	@param command: One of EAknSoftKey* constants in uikludges.py
	@param visibility: True for visible, False for hidden
		
	@type command: int
	@type visibility: boolean
	
	@raise SymbianError: if native code execution fails
	@return: None
	"""
	_uikludges.set_softkey_visibility(command, visibility)

def query(text=u"", type="query", defval=None, header=u"", show_left_softkey=True, tone=ENoTone):
	"""  Opens a query dialog.
	
	Query dialogs prompts user to enter or choose something.
	
	Example::
	
		uikludges.query("Python rocks?", type="query", tone=uikludges.EConfirmationTone)
	
	@param text: Unicode text string in the query.
	@param type: Non-unicode text string of: confirmation, text, code, number, data, time, float, message
	@param defval: The default value of the prompt of None
	@param header: Dialog header. Applies only for type 'message'.
	@param show_left_softkey: Should left soft key choice be available for the user
	@param tone: One of uikludges constants: ENoTone, EConfirmationTone, EWarningTone, EErrorTone

	@type text: unicode
	@type type: string
	@type header: unicode
	@type defval: Object
	@type show_left_softkey: boolean�
	@type tone: integer
	
	@raise SymbianError: if native code execution fails
	
	@return: Queried value as Python object or None if user cancels the query
	
	"""
	# Provide appuifw compatibility with default values
	return _uikludges.query(text, type, defval, header, show_left_softkey, tone)


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
	
	
	
	
	