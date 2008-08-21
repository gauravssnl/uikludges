
#include <logman/logmanutils.h>

#include <eikenv.h>
#include <progressbar.rsg>
#include "progressnotes.h"

CProgressNotes::~CProgressNotes()
{
    LOGMAN_SENDLOG( "enter ~CProgressNotes" )
    FinishL(ETrue);
    
    // Free the resource
    CEikonEnv::Static()->DeleteResourceFile( iRscOffset );
    
    LOGMAN_SENDLOG( "exit ~CProgressNotes" )
}

void CProgressNotes::ConstructL()
{
    LOGMAN_SENDLOG( "enter CProgressNotes::ConstructL" )
    iRscOffset = CEikonEnv::Static()->AddResourceFileL(_L("\\resource\\apps\\progressnotes.rsc"));
    LOGMAN_SENDLOG( "exit  CProgressNotes::ConstructL" )
}

void CProgressNotes::FinishL( TBool aFinish )
{
    LOGMAN_SENDLOGF( "enter Finish:%d", aFinish )
    
    if(iProgressDialog)
    {
        if( aFinish )
        {
        iProgressDialog->SetCallback( NULL );
        LOGMAN_SENDLINE( iProgressDialog->ProcessFinishedL();  )
        }
        LOGMAN_SENDLINE( iProgressDialog = NULL; )
    }
    
    if(iProgressInfo)
    {
        LOGMAN_SENDLINE( iProgressInfo = NULL; )
    }
    
    if(iWaitDialog)
    {
        if( aFinish )
        {
        iWaitDialog->SetCallback( NULL );
        LOGMAN_SENDLINE( iWaitDialog->ProcessFinishedL(); )
        }
        
        LOGMAN_SENDLINE( iWaitDialog = NULL; )
    }
    LOGMAN_SENDLOG( "exit Finish" )
}
 
void CProgressNotes::StartWaitNoteL()	
{
    LOGMAN_SENDLOG( "enter StartWaitNoteL" )
    
    iWaitDialog = new (ELeave) CAknWaitDialog( (REINTERPRET_CAST(CEikDialog**, &iWaitDialog) ), ETrue);
    iWaitDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);        
    iWaitDialog->SetCallback( this );
    iWaitDialog->RunLD();
    
    LOGMAN_SENDLOG( "exit StartWaitNoteL" )
}
 
_LIT( KStrEmpty, "" );
void CProgressNotes::StartProgressNoteL(TInt aFinalValue)	
{
    LOGMAN_SENDLOG( "enter StartProgressNoteL" )
     
    iProgressDialog = new (ELeave) CAknProgressDialog(
     (REINTERPRET_CAST(CEikDialog**, &iProgressDialog)),
     ETrue);
     
    iProgressDialog->PrepareLC( R_PROGRESS_NOTE);
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    // Make sure there is no text
    iProgressDialog->SetTextL( KStrEmpty );
    
    iProgressDialog->SetCallback( this );
    
    iProgressDialog->RunLD();
    iProgressInfo->SetFinalValue(aFinalValue);
    
    LOGMAN_SENDLOG( "exit StartProgressNoteL" )
    
}
 
void CProgressNotes::UpdateProcessL(TInt aProgress,const TDesC& aProgressText)
{
   if(iProgressDialog)
   {
      iProgressDialog->SetTextL(aProgressText);
   }
   
   if(iWaitDialog)
   {
      iWaitDialog->SetTextL(aProgressText);
   }
   
   if(iProgressInfo && iProgressDialog)
   {
      iProgressInfo->SetAndDraw(aProgress);	
   }
}
 
 
void CProgressNotes::SetCallback( MProgressDialogCallback* aCallback )
{
    this->iCallback = aCallback;
} 

void CProgressNotes::DialogDismissedL (TInt aButtonId)
{
    if( this->iCallback )
    {
        this->iCallback->DialogDismissedL( aButtonId );
    }
}

