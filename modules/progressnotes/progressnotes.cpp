
#include <eikenv.h>
#include <progressbar.rsg>
#include "progressnotes.h"

CProgressNotes::~CProgressNotes()
{
    
    // If this fails we have some serious trouble anyway
    TRAPD(err, FinishL(ETrue); );
    
    // Free the resource
    CEikonEnv::Static()->DeleteResourceFile( iRscOffset );
    
}


#ifdef __WINSCW__
_LIT( KRscProgressNotes, "Z:\\resource\\apps\\progressnotes.rsc" );
#else
_LIT( KRscProgressNotes, "\\resource\\apps\\progressnotes.rsc" );
#endif


void CProgressNotes::ConstructL()
{
    iRscOffset = CEikonEnv::Static()->AddResourceFileL( KRscProgressNotes );    
}

void CProgressNotes::FinishL( TBool aFinish )
{
        
    if(iProgressDialog)
    {
        if( aFinish )
        {
        iProgressDialog->SetCallback( NULL );
        iProgressDialog->ProcessFinishedL(); 
        }
        iProgressDialog = NULL;
    }
    
    if(iProgressInfo)
    {
        iProgressInfo = NULL;
    }
    
    if(iWaitDialog)
    {
        if( aFinish )
        {
        iWaitDialog->SetCallback( NULL );
        iWaitDialog->ProcessFinishedL();
        }
        
        iWaitDialog = NULL;
    }    
}
 
void CProgressNotes::StartWaitNoteL()	
{
    
    iWaitDialog = new (ELeave) CAknWaitDialog( (REINTERPRET_CAST(CEikDialog**, &iWaitDialog) ), ETrue);
    iWaitDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);        
    iWaitDialog->SetCallback( this );
    iWaitDialog->RunLD();
    
}
 
_LIT( KStrEmpty, "" );
void CProgressNotes::StartProgressNoteL(TInt aFinalValue)	
{
    
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

