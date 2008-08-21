/**
 * Interprocess communications module for PyS60
 *
 * __author__    = "Jussi Toivola <jussi@redinnovation.com>"
 * __copyright__ = "2008 Red Innovation Ltd."
 * __license__   = "BSD"
 */

#include <Python.h>
#include <symbian_python_ext_util.h>
#include <logman/logmanutils.h>

#include "progressnotes.h"

////////////////////////////////////////////////////////////////////

#define ProgressNote_TypeString "ProgressNote.type_ProgressNote"
#define ProgressNoteTypeObject (*(PyTypeObject *)SPyGetGlobalString(ProgressNote_TypeString))

/// Message for Python exception
const char* KErrProgressNoteNotOpen = "No active note.";

/// Message for Symbian panic.
_LIT( KMsgInvalidNotifier, "Invalid notifier type" );

enum TProgressType
{
    ENoteTypeWait,
    ENoteTypeProgress
};

class CCallbackWrapper : public MProgressDialogCallback
{
    public:
    // Python function callback
    PyObject* iCallback;
    CProgressNotes* iParent;
    
    CCallbackWrapper( PyObject* aCallback, CProgressNotes* aParent ) : iCallback( aCallback ), iParent( aParent )
    {
        Py_XINCREF(iCallback);
    }
    // Called when user presses a button
    void DialogDismissedL (TInt aButtonId)
    {
        LOGMAN_SENDLOG( "enter DialogDismissedL" )
        iParent->FinishL( EFalse );
        
        if (aButtonId == EAknSoftkeyCancel)
        {
            PyObject *result;
        
            PyEval_RestoreThread(PYTHON_TLS->thread_state);
            result = PyEval_CallObject( iCallback, NULL );
            PyEval_SaveThread();

            Py_XDECREF(result);
        }
        LOGMAN_SENDLOG( "exit DialogDismissedL" )
    }
    
    ~CCallbackWrapper()
    {
        LOGMAN_SENDLOG( "enter ~CCallbackWrapper" )
        if ( iCallback )
        {
            Py_XDECREF( iCallback );
        }
        LOGMAN_SENDLOG( "exit ~CCallbackWrapper" )
    };
    
};

/// RProgressNote class wrapper declaration
struct Type_ProgressNote
{
    /// Python type header.
    PyObject_HEAD;
    /// RProgressNote instance.
    CProgressNotes* iProgressNotes;
    CCallbackWrapper* iCallbackWrapper;    
    /// Type of the notifier
    //TInt iType;
    /// Is a note activated?
    TBool iIsActive;
};

// Prototypes
static PyObject* ProgressNote_Progress( Type_ProgressNote* self, PyObject* args  );
static PyObject* ProgressNote_UpdateProgress( Type_ProgressNote* self, PyObject* args );
static PyObject* ProgressNote_Finish( Type_ProgressNote* self );
static PyObject* ProgressNote_Wait( Type_ProgressNote* self );
static PyObject* ProgressNote_SetCancelCallback( Type_ProgressNote* self, PyObject* args );

static const PyMethodDef ProgressNote_methods[] =
{
    {"progress",  (PyCFunction)ProgressNote_Progress,   METH_VARARGS, "" },    
    {"update",    (PyCFunction)ProgressNote_UpdateProgress,   METH_VARARGS, "" },
    {"wait",      (PyCFunction)ProgressNote_Wait, METH_NOARGS, "" },
    {"finish",    (PyCFunction)ProgressNote_Finish,   METH_NOARGS, "" },
    {"cancel_callback", (PyCFunction)ProgressNote_SetCancelCallback,  METH_VARARGS, "" },
    {NULL, NULL}
};

/**
  * Initialize object data.
  */
static void Initialize( Type_ProgressNote* self )
{
    if ( self->iIsActive )
    {
        self->iIsActive = EFalse;
    }
}

static PyObject* ProgressNote_Finish(Type_ProgressNote* self)
{
    TRAPD( err, self->iProgressNotes->FinishL( ETrue ) );
    if ( err != KErrNone )
    {
        return SPyErr_SetFromSymbianOSErr(err);
    }
    Py_INCREF(Py_True);
    return Py_True;
}


static PyObject* ProgressNote_Wait(Type_ProgressNote* self)
{
    TRAPD( err, self->iProgressNotes->StartWaitNoteL( ) );
    if ( err != KErrNone )
    {
        return SPyErr_SetFromSymbianOSErr(err);
    }
    Py_INCREF(Py_True);
    return Py_True;
}

static PyObject* ProgressNote_UpdateProgress(Type_ProgressNote* self, PyObject* args)
{
    
    TInt newpos = 0;
    char* title       = NULL;
    TInt title_length = 0;
    
    if (!PyArg_ParseTuple(args, "iu#", &newpos, &title, &title_length) )
    {
        return 0;
    }
    
    TPtrC aQueueName((TUint16*)title, title_length );
    TRAPD( err, self->iProgressNotes->UpdateProcessL( newpos, aQueueName ) );
    if ( err != KErrNone )
    {
        return SPyErr_SetFromSymbianOSErr(err);
    }
    
    Py_INCREF(Py_True);
    return Py_True;
}

/**
  * Show progress note-
  */
static PyObject* ProgressNote_Progress(Type_ProgressNote* self, PyObject* args)
{
    TInt maxvalue = 0;

    if (!PyArg_ParseTuple(args, "i", &maxvalue))
    {
        return 0;
    }

    Initialize( self );

    // Construct the message queue handle    
    TRAPD( err, self->iProgressNotes->StartProgressNoteL( maxvalue ) );

    if ( err != KErrNone )
    {
        return SPyErr_SetFromSymbianOSErr(err);
    }

    self->iIsActive = ETrue;

    Py_INCREF(Py_True);
    return Py_True;
}


/**
  * Sets cancel callback.
  * @param self
  * @param args  Contains the callback object.
*/
PyObject *ProgressNote_SetCancelCallback(Type_ProgressNote *self, PyObject *args )
{
    
    LOGMAN_SENDLOG( "enter ProgressNote_SetCancelCallback" )
        
    PyObject *callback = NULL;
    if (!PyArg_ParseTuple(args, "O", &callback))
    {
        return NULL;
    }
 
    TRAPD(err, self->iCallbackWrapper = new (ELeave) CCallbackWrapper(callback, self->iProgressNotes ) );
    if ( err )
    {
        SPyErr_SetFromSymbianOSErr( err );
        return NULL;
    }
    
    self->iProgressNotes->SetCallback( self->iCallbackWrapper );
    
    LOGMAN_SENDLOG( "exit ProgressNote_SetCancelCallback" )
        
    Py_INCREF(Py_True);
    return Py_True;
}
     

////////////////////////////////////////////////////////////////////////////////
// ProgressNote types definitions
////////////////////////////////////////////////////////////////////////////////


/**ProgressNote python constructor*/
static PyObject* Type_ProgressNote_Construct(PyObject* /*self*/, PyObject * args)
{
    //LOGMAN_SENDLOG( "enter Type_ProgressNote_Construct" )
    
    Type_ProgressNote *self = PyObject_New(Type_ProgressNote, &ProgressNoteTypeObject);

    if (!self)
    {
        return SPyErr_SetFromSymbianOSErr(KErrNone);
    }
        
    TRAPD( err, self->iProgressNotes = new (ELeave)CProgressNotes();
                self->iProgressNotes->ConstructL(); );  
    if ( err )
    {
        SPyErr_SetFromSymbianOSErr( err );
        return NULL;
    }
    self->iIsActive = EFalse;
    self->iCallbackWrapper = NULL;
    
    return (PyObject*)self;
}

/** ProgressNote python destructor */
static void dealloc_ProgressNote(Type_ProgressNote* self)
{
    LOGMAN_SENDLOG( "enter dealloc_ProgressNote" )
    if ( self->iProgressNotes )
    {

        if ( self->iIsActive )
        {
            self->iIsActive = EFalse;
        }

        delete self->iProgressNotes;
        self->iProgressNotes = NULL;
    }
 
    if ( self->iCallbackWrapper )
    {
        delete self->iCallbackWrapper;
    }

    PyObject_Del(self);
    LOGMAN_SENDLOG( "exit dealloc_ProgressNote" )
}

static PyObject *getattr_ProgressNote(PyObject *self, char *name)
{
    return Py_FindMethod(const_cast<PyMethodDef*>(&ProgressNote_methods[0]), self, name);
}

static const PyTypeObject type_template_ProgressNote =
{
    /*******************************************************/
    PyObject_HEAD_INIT(0)    /* initialize to 0 to ensure Win32 portability */
    0,                 /*ob_size*/
    "_progressnotes.ProgressNote",            /*tp_name*/
    sizeof(Type_ProgressNote), /*tp_basicsize*/
    0,                 /*tp_itemsize*/
    /* methods */
    (destructor)dealloc_ProgressNote, /*tp_dealloc*/
    0, /*tp_print*/
    (getattrfunc)getattr_ProgressNote, /*tp_getattr*/

    /* implied by ISO C: all zeros thereafter */
};

static const PyMethodDef _progressnotes_methods[] =
{
    {"ProgressNote", (PyCFunction)Type_ProgressNote_Construct, METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

DL_EXPORT(void) init_progressnotes(void)
{
    LOGMAN_SENDLOG( "init_progressnotes" )
    PyObject* m = Py_InitModule("_progressnotes", (PyMethodDef*)_progressnotes_methods);

    // Create ProgressNote class type
    PyTypeObject *ProgressNote_type = PyObject_New(PyTypeObject, &PyType_Type);
    if (!ProgressNote_type)
        return;

    *ProgressNote_type = type_template_ProgressNote;

    TInt err = SPyAddGlobalString(ProgressNote_TypeString, (PyObject *)ProgressNote_type);
    if (err != KErrNone ) // 0 is success
    {
        PyObject_Del(ProgressNote_type);
        PyErr_SetString(PyExc_Exception, "SPyAddGlobalString failed");
        return;
    }

    ProgressNoteTypeObject.ob_type = &PyType_Type;

}

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
{
    return KErrNone;
}
#endif /*EKA2*/
