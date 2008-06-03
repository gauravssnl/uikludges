#include <e32std.h>
#include <e32base.h>
#include <apgtask.h>
#include <apgcli.h>
#include <eikenv.h>
#include <apgwgnam.h>
#include <f32file.h>

#include "Python.h"
#include "unicodeobject.h"
#include "symbian_python_ext_util.h"

enum TTaskOperations
    {
    ETaskForeground,
    ETaskBackground,
    ETaskKill,
    ETaskEnd,    
    };


void GetRunningApplicationTitlesL(CDesCArray& aArray, TBool aIncludeHidden)
{  
    // this code is somewhat borrowed from Switcher application

    RWsSession ws(CEikonEnv::Static()->WsSession()); 
    RApaLsSession rs;
    CApaWindowGroupName* wgName; 
    TApaAppInfo ai;
    
    User::LeaveIfError(rs.Connect());    
    CleanupClosePushL(rs);

    CArrayFixFlat<TInt>* wgl = new (ELeave) CArrayFixFlat<TInt>(5);
    CleanupStack::PushL(wgl);

    User::LeaveIfError(ws.WindowGroupList(wgl));
    wgName = CApaWindowGroupName::NewLC(ws); 
    
    for(TInt i = 0; i < wgl->Count(); i++)
    {
        wgName->ConstructFromWgIdL(wgl->At(i)); 

        if(!aIncludeHidden && wgName->Hidden())
        {
            continue;
        }      
        
        TPtrC caption;      
        caption.Set(wgName->Caption());        
        if(!caption.Length()) 
        {
            continue; 
        } 
        aArray.AppendL(caption);
    }

    CleanupStack::PopAndDestroy(wgName);
    CleanupStack::PopAndDestroy(wgl);
    CleanupStack::PopAndDestroy(); // rs
    
}

static PyObject*  _perform_task_operation(TTaskOperations aTask, PyObject *args)
{    
	char* b = NULL;	
	TInt l = 0;

	if (!PyArg_ParseTuple(args, "u#", &b, &l))
	{
		return 0;
	}
	
	TPtrC caption((TUint16*)b, l);
    
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
	TApaTask task(taskList.FindApp(caption));

    if (!task.Exists())
        {
		Py_INCREF(Py_False);
		return Py_False;
        }

    switch (aTask)
        {
        case ETaskForeground:
            task.BringToForeground();
            break;
        case ETaskBackground:
            task.SendToBackground();
            break;
        case ETaskKill:
            task.EndTask();
            break;
        case ETaskEnd:
            task.KillTask();
            break;
        }

	Py_INCREF(Py_True);
	return Py_True;
}


static PyObject* switch_to_fg(PyObject* /*self*/, PyObject *args)
{
    return _perform_task_operation(ETaskForeground, args);
}

static PyObject* switch_to_bg(PyObject* /*self*/, PyObject *args)
{
    return _perform_task_operation(ETaskBackground, args);
}

static PyObject* kill_app(PyObject* /*self*/, PyObject *args)
{
    return _perform_task_operation(ETaskKill, args);
}

static PyObject* end_app(PyObject* /*self*/, PyObject *args)
{
    return _perform_task_operation(ETaskEnd, args);
}

static PyObject* application_list(PyObject* /*self*/, PyObject * args)
{
    TBool includeHidden;

	if (!PyArg_ParseTuple(args, "i", &includeHidden))
	{
		return 0;
	}


    CDesCArray* array = new (ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL(array);

    TRAPD(err, GetRunningApplicationTitlesL(*array, includeHidden));
    if (err)
        {
        return SPyErr_SetFromSymbianOSErr(err);        
        }
    
    
    PyObject *appstuple;
    appstuple = PyTuple_New(array->Count());

    for (TInt i = 0; i < array->Count(); i++)
        {
        PyObject *str = Py_BuildValue("u#", (*array)[i].Ptr(), (*array)[i].Length());        
        PyTuple_SET_ITEM(appstuple, i, str);
        }
    
    CleanupStack::PopAndDestroy(array);

	return appstuple;
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

/*
 *
 * 
 *
 */
extern "C" PyObject *
launch_py_background(PyObject* /*self*/, PyObject* args)
{
  PyObject* it;
  PyObject* launcher_obj;

  if (!PyArg_ParseTuple(args, "OO", &it, &launcher_obj))
    return NULL;

  PyObject* fn = PyUnicode_FromObject(it);
  if (!fn)
    return NULL;
  
  PyObject* launcher_fn = PyUnicode_FromObject(launcher_obj);

  TPtrC name(PyUnicode_AsUnicode(fn), PyUnicode_GetSize(fn));
  TPtrC launcher_name(PyUnicode_AsUnicode(launcher_fn), PyUnicode_GetSize(launcher_fn));

  TParse p;
  p.Set(name, NULL, NULL);
  
  if (!(p.Ext().CompareF(_L(".py")) == 0)) {
    Py_DECREF(fn);
    PyErr_SetString(PyExc_TypeError, "Python script name expected");
    return NULL;
  }
  
  TInt error;
  RFs rfs;
  
  if ((error = rfs.Connect()) == KErrNone) {
    TBool f_exists;
    f_exists = BaflUtils::FileExists(rfs, name);
    rfs.Close();
    if (!f_exists){
      Py_DECREF(fn);
      return SPyErr_SetFromSymbianOSErr(KErrNotFound);
    }
  }  
  
  error =
#if defined(__WINS__) && !defined(EKA2)
    ProcessLaunch(launcher_name, name);
#else
    ProcessLaunch(launcher_name, name);
#endif

  Py_DECREF(fn);
  Py_DECREF(launcher_fn);
  
  RETURN_ERROR_OR_PYNONE(error);
}


static const PyMethodDef appswitch_methods[] =
{
	{"switch_to_fg", (PyCFunction)switch_to_fg, METH_VARARGS},
    {"switch_to_bg", (PyCFunction)switch_to_bg, METH_VARARGS},
    {"end_app", (PyCFunction)end_app, METH_VARARGS},
    {"kill_app", (PyCFunction)kill_app, METH_VARARGS},        
    {"application_list", (PyCFunction)application_list, METH_VARARGS},
    {"launch_py_background", (PyCFunction)launch_py_background, METH_VARARGS},    
	{0, 0} /* sentinel */
};

DL_EXPORT(void) init_appswitch()
{
	Py_InitModule("_applicationmanager", (PyMethodDef*) appswitch_methods);	
}

#ifndef EKA2
// DLL entry point is needed only for Series 60 3.0 predecessors (non-EKA2)
GLDEF_C TInt E32Dll(TDllReason)
{
  return KErrNone;
}
#endif // EKA2


