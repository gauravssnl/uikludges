#include <e32std.h>
#include <e32base.h>
#include <apgtask.h>
#include <apgcli.h>
#include <eikenv.h>
#include <apgwgnam.h>

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

static const PyMethodDef appswitch_methods[] =
{
	{"switch_to_fg", (PyCFunction)switch_to_fg, METH_VARARGS},
    {"switch_to_bg", (PyCFunction)switch_to_bg, METH_VARARGS},
    {"end_app", (PyCFunction)end_app, METH_VARARGS},
    {"kill_app", (PyCFunction)kill_app, METH_VARARGS},        
    {"application_list", (PyCFunction)application_list, METH_VARARGS},
	{0, 0} /* sentinel */
};

DL_EXPORT(void) init_appswitch()
{
	Py_InitModule("appswitch", (PyMethodDef*) appswitch_methods);	
}

#ifndef EKA2
// DLL entry point is needed only for Series 60 3.0 predecessors (non-EKA2)
GLDEF_C TInt E32Dll(TDllReason)
{
  return KErrNone;
}
#endif // EKA2


