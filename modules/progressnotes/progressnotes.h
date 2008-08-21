#include <eikprogi.h>
#include <aknwaitdialog.h> 
#include <aknprogressdialog.h>
 
class  CProgressNotes : public MProgressDialogCallback
{	
public: 
   ~CProgressNotes();
   CProgressNotes() : iProgressDialog(NULL), iProgressInfo(NULL), iWaitDialog(NULL), iCallback(NULL) {};
   void ConstructL();
   void SetCallback( MProgressDialogCallback* aCallback );
   void StartWaitNoteL();
   void StartProgressNoteL(TInt aFinalValue);
   void UpdateProcessL(TInt aProgress,const TDesC& aProgressText);
   void FinishL(TBool aFinish);
   void DialogDismissedL (TInt aButtonId);
   
private:	
   CAknProgressDialog*  iProgressDialog;
   CEikProgressInfo*    iProgressInfo;
   CAknWaitDialog*      iWaitDialog;
   MProgressDialogCallback* iCallback;
   
   TInt iRscOffset;
};

