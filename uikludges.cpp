/**
 * UI enhancements for Python for Series 60
 * 
 *
 * 
 * Author: Mikko Ohtamaa <mikko@redinnovation.com>
 * 
 * Copyright 2008 Red Innovation Ltd.
 * 
 * Copyright 2007 Simo Salminen, cyke64
 * 
 * For licensing information, see uikludges.py.
 * 
 */
#include <e32std.h>
#include <e32base.h>
#include <EIKBTGPC.H>
#include <avkon.hrh>
#include <aknnavi.h>
#include <aknnavide.h>
#include <eikspane.h>
#include <aknappui.h> 
#include <aknquerydialog.h>
#include <aknmessagequerydialog.h>

#include <f32file.h> // For start_server

#include "appuifwmodule.hrh"
#include <uikludges.rsg>

#include "Python.h"
#include "symbian_python_ext_util.h"

#include "logger.h"

/**
 * Set soft key text in the main view
 *
 */
static PyObject* uikludges_set_softkey_text(PyObject* /*self*/, PyObject *args)
{
  char* text_ = 0;	
  TInt key;
  TInt textlen = 0;  
  if (!PyArg_ParseTuple(args, "iu#", &key, &text_, &textlen))
  {
    return 0;
  }
  TPtrC text((TUint16*)text_, textlen);


  TRAPD(err,
        CEikButtonGroupContainer::Current()->SetCommandL(key, text);
        CEikButtonGroupContainer::Current()->DrawDeferred();
        );
  
  
  RETURN_ERROR_OR_PYNONE(err);
}

/**
 * Set soft key visibiltiy 
 * 
 */
static PyObject* uikludges_set_softkey_visibility(PyObject* /*self*/, PyObject *args)
{
  TInt key;
  TInt visibility;
  if (!PyArg_ParseTuple(args, "ii", &key, &visibility))
  {
    return 0;
  }

  TRAPD(err,
        CEikButtonGroupContainer::Current()->MakeCommandVisible(key, visibility);
        CEikButtonGroupContainer::Current()->DrawDeferred();
        );
  
  
  RETURN_ERROR_OR_PYNONE(err);
}

/*
 *
 * Implementation of appuifw.query()
 *
 */
_LIT8(KTextFieldType, "text");
_LIT(KUcTextFieldType, "text");
_LIT8(KNumberFieldType, "number");
_LIT(KUcNumberFieldType, "number");
_LIT8(KFloatFieldType, "float");
_LIT(KUcFloatFieldType, "float");
_LIT8(KDateFieldType, "date");
_LIT(KUcDateFieldType, "date");
_LIT8(KTimeFieldType, "time");
_LIT(KUcTimeFieldType, "time");
_LIT8(KCodeFieldType, "code");
_LIT(KUcCodeFieldType, "code");
_LIT8(KQueryFieldType, "query");
_LIT(KUcQueryFieldType, "query");
_LIT8(KMessageFieldType, "message"); // added
_LIT(KUcMessageFieldType, "message");
_LIT8(KComboFieldType, "combo");
_LIT8(KErrorNoteType, "error");
_LIT8(KInformationNoteType, "info");
_LIT8(KConfirmationNoteType, "conf");
_LIT8(KFlagAttrName, "flags");
_LIT(KAppuifwEpoch, "19700000:");
_LIT8(KNormal, "normal");
_LIT8(KAnnotation, "annotation");
_LIT8(KTitle, "title");
_LIT8(KLegend, "legend");
_LIT8(KSymbol, "symbol");
_LIT8(KDense, "dense");
_LIT8(KCheckboxStyle, "checkbox");
_LIT8(KCheckmarkStyle, "checkmark");


static TReal epoch_local_as_TReal()
{
#ifndef EKA2
  TLocale loc;
  return (epoch_as_TReal()+
          ((1000.0*1000.0)*
           TInt64(loc.UniversalTimeOffset().Int()).GetTReal()));
#else
  TLocale loc;
  return (epoch_as_TReal()+
          ((1000.0*1000.0)*
           TReal64(TInt64(loc.UniversalTimeOffset().Int()))));
#endif /*EKA2*/
}


static TReal datetime_as_TReal(const TTime& aTime)
{
#ifndef EKA2
  TDateTime dt = aTime.DateTime();
  return TInt64((dt.Hour()*3600)+(dt.Minute()*60)).GetTReal();
#else
  TDateTime dt = aTime.DateTime();
  return TReal64(TInt64((dt.Hour()*3600)+(dt.Minute()*60)));
#endif /*EKA2*/
}

static TReal datetime_with_secs_as_TReal(const TTime& aTime)
{
#ifndef EKA2
  TDateTime dt = aTime.DateTime();
  return TInt64((dt.Hour()*3600)+(dt.Minute()*60)+(dt.Second())).GetTReal();
#else
  TDateTime dt = aTime.DateTime();
  return TReal64(TInt64((dt.Hour()*3600)+(dt.Minute()*60)+(dt.Second())));
#endif /*EKA2*/
}

static TInt rid_by_type(const TDesC8& aName)
{
  if (aName.Compare(KTextFieldType) == 0)
    return R_APPUIFW_DATA_QUERY;

  if (aName.Compare(KNumberFieldType) == 0)
    return R_APPUIFW_NUMBER_QUERY;
  
  if (aName.Compare(KFloatFieldType) == 0)
    return R_APPUIFW_FLOAT_QUERY;
  
  if (aName.Compare(KCodeFieldType) == 0)
    return R_APPUIFW_CODE_QUERY;
  
  if (aName.Compare(KDateFieldType) == 0)
    return R_APPUIFW_DATE_QUERY;
  
  if (aName.Compare(KTimeFieldType) == 0)
    return R_APPUIFW_TIME_QUERY;

  if (aName.Compare(KQueryFieldType) == 0)
    return R_APPUIFW_CONFIRMATION_QUERY;

  if (aName.Compare(KMessageFieldType) == 0)
	  // My .rss entry doesn't work :(
	  // It crashes either compiler or NewLC with user panic 0
	  // return R_APPUIFW_MESSAGE_QUERY;   
	  return R_AVKON_MESSAGE_QUERY_DIALOG;
  return -1;
}

static CAknQueryDialog::TTone tone_by_value(int itone) {
	if(itone == 0) {
		return CAknQueryDialog::ENoTone;
	} else if(itone == 1003) {
		return CAknQueryDialog::EConfirmationTone;
	} else if(itone == 1004) {
		return CAknQueryDialog::EWarningTone;
	} else if(itone == 1005) {
		return CAknQueryDialog::EErrorTone;
	}
	return CAknQueryDialog::ENoTone;
}


extern "C" PyObject *
query(PyObject* /*self*/, PyObject *args)
{
  int l_label, l_type, l_header;
  char *b_label, *b_type, *b_header;
  int show_left_softkey;
  int itone;
  CAknQueryDialog::TTone tone;
  PyObject* inival = NULL;
  PyObject* retval = NULL;

  if (!PyArg_ParseTuple(args, "u#s#Ou#ii", 		  				
		  				&b_label, &l_label,
                        &b_type, &l_type, 
                        &inival,
		  				&b_header, &l_header,
		  				&show_left_softkey,
		  				&itone))
    return NULL;

  TInt error = KErrNone;
  CAknQueryDialog* dlg = NULL;
  TPtr buf(NULL, 0);
  TInt num = 0;
  TReal num_real = 0;
  TTime time;
  TInt rid = rid_by_type(TPtrC8((TUint8*)b_type, l_type));
  tone = tone_by_value(itone);
  

  LOG(_L("Switch"))
  switch(rid) {
  case R_APPUIFW_DATA_QUERY:
    if (retval = PyUnicode_FromUnicode(NULL, KAppuifwMaxTextField)) {
      buf.Set(PyUnicode_AsUnicode(retval), 0, KAppuifwMaxTextField);
      if (inival && PyUnicode_Check(inival))
        buf.Copy(PyUnicode_AsUnicode(inival), PyUnicode_GetSize(inival));
      TRAP(error, {
        dlg = CAknTextQueryDialog::NewL(buf, tone);
        ((CAknTextQueryDialog*)dlg)->SetMaxLength(KAppuifwMaxTextField);
      });
      if (error == KErrNone)
        dlg->SetPredictiveTextInputPermitted(ETrue);
    }
    else
      error = KErrPython;
    break;
  case R_APPUIFW_NUMBER_QUERY:
    if (inival && PyInt_Check(inival))
      num = PyLong_AsLong(inival);
    TRAP(error, {
      dlg = CAknNumberQueryDialog::NewL(num, tone);
    });
    break;
  case R_APPUIFW_FLOAT_QUERY:
    if (inival && PyFloat_Check(inival))
      num_real = PyFloat_AsDouble(inival);
    TRAP(error, {
      dlg = CAknFloatingPointQueryDialog::NewL(num_real, tone);
    });
    break;
  case R_APPUIFW_CODE_QUERY:
    if (retval = PyUnicode_FromUnicode(NULL, KAppuifwMaxTextField)) {
      buf.Set(PyUnicode_AsUnicode(retval), 0, KAppuifwMaxTextField);
      TRAP(error, {
        dlg = CAknTextQueryDialog::NewL(buf, tone);
        ((CAknTextQueryDialog*)dlg)->SetMaxLength(KAppuifwMaxTextField);
      });
    }
    else
      error = KErrPython;
    break;
  case R_APPUIFW_DATE_QUERY:
  case R_APPUIFW_TIME_QUERY:
    time = (rid == R_APPUIFW_DATE_QUERY) ? epoch_local_as_TReal() : 0.0;
    if (inival && PyFloat_Check(inival))
      time += TTimeIntervalSeconds(PyFloat_AsDouble(inival));
    TRAP(error, {
      dlg = CAknTimeQueryDialog::NewL(time, tone);
    });
    break;
  case R_APPUIFW_CONFIRMATION_QUERY:
    TRAP(error, {
    	dlg = CAknQueryDialog::NewL(tone);
    });
    break;
  case R_AVKON_MESSAGE_QUERY_DIALOG:
  	TRAP(error, {
  		TPtrC16 body((TUint16 *)b_label, l_label);
  		dlg = CAknMessageQueryDialog::NewL(body, tone);
  	}
  	);
  break;    
  default:
    PyErr_SetString(PyExc_ValueError, "unknown query type");
    return NULL;
  }
      
  TInt user_response = 0;
  
  if (error == KErrNone) {
    TRAP(error, {
    
    if(rid != R_AVKON_MESSAGE_QUERY_DIALOG) {
    	dlg->SetPromptL(TPtrC((TUint16 *)b_label, l_label));
    } 
    
    LOG(_L("prompt ok"))
	
		dlg->PrepareLC(rid);

		if(error == KErrNone) {
		  	// need to use PrepareLD instead of ExecuteLD to avoid panic
		  	dlg->MakeLeftSoftkeyVisible(show_left_softkey);
		}

	    if(rid == R_AVKON_MESSAGE_QUERY_DIALOG) {
	    	dlg->QueryHeading()->SetTextL(TPtrC((TUint16 *)b_header, l_header));
	    }
	Py_BEGIN_ALLOW_THREADS	    
	  user_response = dlg->RunLD();
	Py_END_ALLOW_THREADS
	  });
  }
  
  if (error != KErrNone) {
    delete dlg;
    Py_XDECREF(retval);
    return SPyErr_SetFromSymbianOSErr(error);
  }
  else if (!user_response) {
    Py_XDECREF(retval);
    Py_INCREF(Py_None);
    return Py_None;
  }
  else {
    switch(rid) {
    case R_APPUIFW_DATA_QUERY:
    case R_APPUIFW_CODE_QUERY:
      PyUnicode_Resize(&retval, buf.Length());
      break;
    case R_APPUIFW_NUMBER_QUERY:
      retval = Py_BuildValue("i", num);
      break;
    case R_APPUIFW_FLOAT_QUERY:
      retval = Py_BuildValue("d", num_real);
      break;
    case R_APPUIFW_DATE_QUERY:
      retval = Py_BuildValue("d", time_as_UTC_TReal(time));
      break;
    case R_APPUIFW_TIME_QUERY:
      retval = Py_BuildValue("d", datetime_as_TReal(time));
      break;
    case R_APPUIFW_CONFIRMATION_QUERY:
    default:
      retval = Py_True;
      Py_INCREF(Py_True);
      break;
    }
    return retval;
  }
}

/*
 *
 * Utilities for e32.start_server() and e32.start_exe()
 *
 */

class CE32ProcessWait : public CActive
{
public:
  CE32ProcessWait():CActive(EPriorityStandard) {
    CActiveScheduler::Add(this);
  }
#if defined(__WINS__) && !defined(EKA2)  
  TInt Wait(RThread& aProcess) {
#else
  TInt Wait(RProcess& aProcess) {
#endif
    aProcess.Logon(iStatus);
    aProcess.Resume();
    SetActive();
#ifdef HAVE_ACTIVESCHEDULERWAIT   
    iWait.Start();
#else
    CActiveScheduler::Start();
#endif
    return iStatus.Int();
  }

  private:
  void DoCancel() {;}
  void RunL() {
#ifdef HAVE_ACTIVESCHEDULERWAIT
    iWait.AsyncStop();
#else
    CActiveScheduler::Stop();
#endif
  }
#ifdef HAVE_ACTIVESCHEDULERWAIT
  CActiveSchedulerWait iWait;
#endif
};

static TInt ProcessLaunch(const TDesC& aFileName, const TDesC& aCommand,
                     TInt aWaitFlag=0)
{
  TInt error;
  Py_BEGIN_ALLOW_THREADS
#if defined(__WINS__) && !defined(EKA2)
  RThread proc;
  RLibrary lib;
  HBufC* pcommand = aCommand.Alloc();
  error = lib.Load(aFileName);
  if (error == KErrNone) {
    TThreadFunction func = (TThreadFunction)(lib.Lookup(1));
    error = proc.Create(_L(""), func, 0x1000, (TAny*) pcommand, &lib,
                        RThread().Heap(), 0x1000, 0x100000, EOwnerProcess);
    lib.Close();
  }
  else
    delete pcommand;
#else
  RProcess proc;
  error = proc.Create(aFileName, aCommand);
#endif
  if (error == KErrNone)
    if (aWaitFlag) {
      CE32ProcessWait* w = new CE32ProcessWait();
      if (w) {
        error = w->Wait(proc);
        delete w;
      }
      else
        error = KErrNoMemory;
    }
    else
      proc.Resume();
  proc.Close();
  Py_END_ALLOW_THREADS
  return error;
}


static const PyMethodDef uikludges_methods[] =
{
	{"set_softkey_text", (PyCFunction)uikludges_set_softkey_text, METH_VARARGS},		
	{"set_softkey_visibility", (PyCFunction)uikludges_set_softkey_visibility, METH_VARARGS},
	{"query", (PyCFunction)query, METH_VARARGS},		
	{0, 0} /* sentinel */
};

DL_EXPORT(void) init_uikludges()
{
	PyObject* module = Py_InitModule("_uikludges", (PyMethodDef*) uikludges_methods);

}


#ifndef EKA2
// DLL entry point is needed only for Series 60 3.0 predecessors (non-EKA2)
GLDEF_C TInt E32Dll(TDllReason)
{
  return KErrNone;
}
#endif // EKA2
