/**
 * Progress dialogs for PyS60
 *
 * __author__    = "Jussi Toivola <jussi@redinnovation.com>"
 * __copyright__ = "2008 Red Innovation Ltd."
 * __license__   = "BSD"
 */

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "progressnotes.h"

////////////////////////////////////////////////////////////////////

//#define ProgressNote_TypeString "ProgressNote.type_ProgressNote"
//#define ProgressNoteTypeObject (*(PyTypeObject *)SPyGetGlobalString(ProgressNote_TypeString))

/** Wrapper for the Python callback. 
  * Forwards the DialogDismissedL callback to Python function 
  */
class CCallbackWrapper : public MProgressDialogCallback
{
    public:
    /// Python function callback
    PyObject* iCallback;
    
    /// The caller class
    CProgressNotes* iParent;
    
    CCallbackWrapper( PyObject* aCallback, CProgressNotes* aParent ) :
                      iCallback( aCallback ), 
                      iParent( aParent )
    {
        // Increase reference to Python object
        Py_INCREF(iCallback);
    }
    // Called when user presses the cancel button
    void DialogDismissedL (TInt aButtonId)
    {
        iParent->FinishL( EFalse );
        
        if (aButtonId == EAknSoftkeyCancel)
        {
            // See: http://www.python.org/doc/ext/callingPython.html
            PyObject *result;
            
            // Restore Python context before calling Python code!
            PyEval_RestoreThread(PYTHON_TLS->thread_state);
            result = PyEval_CallObject( iCallback, NULL );
            PyEval_SaveThread();
            
            // Reduce reference to Python object
            Py_XDECREF(result);
        }
    }
    
    ~CCallbackWrapper()
    {
        // Reduce reference to callback object
        Py_XDECREF( iCallback );
    };    
};

/// CProgressNote class wrapper declaration
struct Type_ProgressNote
{
    /// Python type header.
    PyObject_HEAD;
    /// RProgressNote instance.
    CProgressNotes* iProgressNotes;
    /// Our callback interface
    CCallbackWrapper* iCallbackWrapper;            
};

// Prototypes
static PyObject* ProgressNote_Progress( Type_ProgressNote* self, PyObject* args  );
static PyObject* ProgressNote_UpdateProgress( Type_ProgressNote* self, PyObject* args );
static PyObject* ProgressNote_Finish( Type_ProgressNote* self );
static PyObject* ProgressNote_Wait( Type_ProgressNote* self );
static PyObject* ProgressNote_SetCancelCallback( Type_ProgressNote* self, PyObject* args );

static const PyMethodDef ProgressNote_methods[] =
{
    {"progress",  (PyCFunction)ProgressNote_Progress, METH_VARARGS, "" },    
    {"update",    (PyCFunction)ProgressNote_UpdateProgress, METH_VARARGS, "" },
    {"wait",      (PyCFunction)ProgressNote_Wait, METH_NOARGS, "" },
    {"finish",    (PyCFunction)ProgressNote_Finish, METH_NOARGS, "" },
    {"cancel_callback", (PyCFunction)ProgressNote_SetCancelCallback,  
                                     METH_VARARGS, "" },
    {NULL, NULL}
};


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

static PyObject* ProgressNote_UpdateProgress(Type_ProgressNote* self, 
                                             PyObject* args)
{
    
    TInt aNewpos      = 0;
    char* title       = NULL;
    TInt title_length = 0;
    
    if (!PyArg_ParseTuple(args, "iu#", &aNewpos, &title, &title_length) )
    {
        return 0;
    }
    
    // Convert C string into Symbian descriptor
    TPtrC aTitle((TUint16*)title, title_length );
    
    // TRAP leaves
    TRAPD( err, self->iProgressNotes->UpdateProcessL( aNewpos, aTitle ) );
    if ( err != KErrNone )
    {
        // Returns NULL
        return SPyErr_SetFromSymbianOSErr(err);
    }
    // Remember to increase reference count
    Py_INCREF(Py_True);
    return Py_True;
}
/**
  * Show progress note
  */
static PyObject* ProgressNote_Progress(Type_ProgressNote* self, PyObject* args)
{
    TInt maxvalue = 0;

    if (!PyArg_ParseTuple(args, "i", &maxvalue))
    {
        return 0;
    }

    // Construct the message queue handle    
    TRAPD( err, self->iProgressNotes->StartProgressNoteL( maxvalue ) );

    if ( err != KErrNone )
    {
        return SPyErr_SetFromSymbianOSErr(err);
    }

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
        
    PyObject *callback = NULL;
    if (!PyArg_ParseTuple(args, "O", &callback))
    {
        return NULL;
    }
 
    TRAPD(err, self->iCallbackWrapper = new (ELeave) CCallbackWrapper(callback, self->iProgressNotes ) );
    if ( err )
    {
        return SPyErr_SetFromSymbianOSErr( err );
    }
    
    self->iProgressNotes->SetCallback( self->iCallbackWrapper );
    
        
    Py_INCREF(Py_True);
    return Py_True;
}
     

////////////////////////////////////////////////////////////////////////////////
// ProgressNote types definitions
////////////////////////////////////////////////////////////////////////////////


/** ProgressNote python destructor */
static void dealloc_ProgressNote(Type_ProgressNote* self)
{
    if ( self->iProgressNotes )
    {
        delete self->iProgressNotes;
        self->iProgressNotes = NULL;
    }
    if ( self->iCallbackWrapper )
    {
        delete self->iCallbackWrapper;
        self->iCallbackWrapper = NULL;
    }
    PyObject_Del(self);
}

static PyObject *getattr_ProgressNote(PyObject *self, char *name)
{
    return Py_FindMethod(const_cast<PyMethodDef*>(&ProgressNote_methods[0]), self, name);
}

static PyTypeObject progressnotes_ProgressNoteType =
{
    /*******************************************************/
    PyObject_HEAD_INIT(0)          /* initialize to 0 to ensure Win32 portability */
    0,                             /*ob_size*/
    "_progressnotes.ProgressNote", /*tp_name*/
    sizeof(Type_ProgressNote),     /*tp_basicsize*/
    0,                             /*tp_itemsize*/
    /* methods */
    (destructor)dealloc_ProgressNote, /*tp_dealloc*/
    0,                                 /*tp_print*/
    (getattrfunc)getattr_ProgressNote, /*tp_getattr*/

    /* implied by ISO C: all zeros thereafter */
};

/**ProgressNote python constructor*/
static PyObject* Type_ProgressNote_Construct(PyObject* /*self*/, PyObject * args)
{
    
    Type_ProgressNote *self = PyObject_New(Type_ProgressNote, 
                                            &progressnotes_ProgressNoteType);

    if (!self)
    {
        return SPyErr_SetFromSymbianOSErr(KErrNone);
    }
        
    TRAPD( err, self->iProgressNotes = new (ELeave)CProgressNotes();
                self->iProgressNotes->ConstructL(); );  
    if ( err )
    {
        return SPyErr_SetFromSymbianOSErr( err );        
    }
    // GCCE does not set to NULL by default
    self->iCallbackWrapper = NULL;
    
    return (PyObject*)self;
}

static const PyMethodDef _progressnotes_methods[] =
{
    {"ProgressNote", (PyCFunction)Type_ProgressNote_Construct, METH_VARARGS},
    {NULL,              NULL}           /* sentinel */
};

DL_EXPORT(void) init_progressnotes(void)
{
    // &PyType_Type is not constant so we have to assing it here 
    // instead of in the progressnotes_ProgressNoteType definition.
    progressnotes_ProgressNoteType.ob_type = &PyType_Type;
    
    // Initialize module
    PyObject* m = Py_InitModule("_progressnotes", 
                    (PyMethodDef*)_progressnotes_methods);    
}

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
{
    return KErrNone;
}
#endif /*EKA2*/
