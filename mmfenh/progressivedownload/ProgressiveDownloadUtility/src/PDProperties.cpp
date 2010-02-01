/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Progressive Download Utility
*
*/



#include "PdProperties.h"

//Percentage (increments) of the file [TBD]
//that needs to be downloaded/buffered before playing begins
//Should try at 25%,50%,75%,100%
const TInt KPercentageToBufferIncrement =25;


//Percentage of the file needed for header [TBD]
//const TInt KPercentageNeededForHeader = 33;
const TInt KBytesNeededForHeader = 200000;//102400;

//const TInt KMinFileSize = 100000;

//Max attempts to openfile
const TInt KRetryOpen = 1;


//===============================================================
// CPdProperties implementation begins
//
//===============================================================
EXPORT_C CPdProperties* CPdProperties::NewL()
    {
    CPdProperties* self = new(ELeave) CPdProperties();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPdProperties* CPdProperties::NewLC()
    {
    CPdProperties* self = new (ELeave) CPdProperties();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

EXPORT_C CPdProperties::~CPdProperties()
    {
    delete iFileName;

    }

void CPdProperties::ConstructL()
    {

    iFileOpened        = EFalse;
    iAttemptsToOpen    = 0;
    iState             = EStopped;
    iPercentageToBuffer= KPercentageToBufferIncrement;
    iDownloadCompleted = EFalse;
    iFileBytePosition  = 0;
    iTimePosition      = 0;
    iBytesDownloaded   = 0;
    iOpenInProgress    = EFalse;

    }

CPdProperties::CPdProperties()
    {

    }

EXPORT_C CAudioPdPlayUtility* CPdProperties::OpenL(const TDesC& aFileName,
                                          MAudioPdPlayUtilityCallback& aCallback)
    {
    iUseFileHandle = EFalse;
    iFileName = HBufC::NewL ( aFileName.Length() );
    iFileName->Des().Copy( aFileName );

    iCallback = &aCallback;

    iAudioPdPlayUtility = CAudioPdPlayUtility::NewL(*this);

    return iAudioPdPlayUtility;
    }

EXPORT_C CAudioPdPlayUtility* CPdProperties::OpenL(const RFile& aFileHandle,
                                          MAudioPdPlayUtilityCallback& aCallback)
    {
    iUseFileHandle = ETrue;
    iFileHandle.Duplicate(aFileHandle);

    iCallback = &aCallback;

    iAudioPdPlayUtility = CAudioPdPlayUtility::NewL(*this);


    return iAudioPdPlayUtility;
    }


void CPdProperties::HandleDownloadEventL(TUint /*aTransactionID*/,
                                         TBrCtlDownloadEvent aEvent,
                                         TUint aValue)
    {
/*
Event                    Value (optional)       Comment
EDownloadEventStarted     Total Bytes           Size of content as specified in header.
EDownloadEventCompleted   Total Bytes Received  Total number received should be same as specified at start.
EDownloadEventProgress    Bytes Received        Number of bytes received so far. May be used to determine % received for progress bar. This will be sent multiple times. Frequency to be determined. Perhaps on every chunk received.
EDownloadEventCanceled    N/A                   Cancelled by Download Manager
EDownloadEventError       N/A                   Cancelled by BrowserEngine due to network/http error.
EDownloadEventPaused,                           aValue is the size downloaded so far
EDownloadEventResumed,                          aValue is the size downloaded
EDownloadEventPausable                          aValue = true when it is pausable
*/

    switch (aEvent)
        {
        case EDownloadEventStarted: //Started
           {
           if(aValue)
             iFileSize = aValue;
           break;
           }

        case EDownloadEventCompleted: //Completed
           {
           iDownloadCompleted = ETrue;
           iBytesDownloaded = aValue;

           if((!iFileOpened) && (!iOpenInProgress))
              {
              //Used for when metadata is at the end of file
              //or when the file is in cache and already downloaded.
              iAudioPdPlayUtility->SetFileSize(iFileSize);
              iAudioPdPlayUtility->SetBytesDownloaded(iBytesDownloaded, iDownloadCompleted);
              iAudioPdPlayUtility->DlCompleteOpenFileL(iFileName->Des());
              iOpenInProgress = ETrue;
              iAttemptsToOpen++;
              }
           break;
           }

        case EDownloadEventProgress: //Progress
           {
           if(aValue)
             iBytesDownloaded = aValue;
           break;
           }
        case EDownloadEventCanceled: //Cancelled
        case EDownloadEventError: //Error
            {
            iBytesDownloaded = 0;
            iFileSize = 0;
            break;
            }

        case EDownloadEventPaused:   //Paused
        case EDownloadEventResumed:  //Resumed
        case EDownloadEventPausable: //Pausable
           //do nothing
        break;

        default:
        break;

        } // switch(aEvent)


    #if _DEBUG
       RDebug::Print(_L("CPdProperties::HandleDownloadEventL"));
       RDebug::Print(_L("file byte position     %d"),iFileBytePosition);
       RDebug::Print(_L("bytes downloaded       %d"),iBytesDownloaded);
       RDebug::Print(_L("file size              %d"),iFileSize);
       RDebug::Print(_L("percentage to buffer   %d"),iPercentageToBuffer);
       RDebug::Print(_L("download state         %d"),aEvent);
    #endif

    HandleDownloadUpdateL();
    }

TInt CPdProperties::CalculatePercentageDownloaded()
    {
    TInt percentageDone = 0;
    if((iFileSize>0) && (iBytesDownloaded>0))
       percentageDone= ((100*iBytesDownloaded)/iFileSize);

    return percentageDone;
    }

void CPdProperties::SavePosition()
    {
    #if _DEBUG
      RDebug::Print(_L("CPdProperties::SavePosition"));
    #endif
    TTimeIntervalMicroSeconds timeposition;
    if(iState==EPlaying)
        {
        iAudioPdPlayUtility->GetFilePosition(iFileBytePosition);

        iAudioPdPlayUtility->GetPosition(timeposition);
        if(timeposition >iTimePosition)
           iTimePosition = timeposition;
        }

    #if _DEBUG
      RDebug::Print(_L("CPdProperties::SavePosition iTimePosition %d"),I64INT(iTimePosition.Int64()));
    #endif

    }

void CPdProperties::HandleDownloadUpdateL()
    {
    TInt percentageDone=0;

    percentageDone = CalculatePercentageDownloaded();

    #if _DEBUG
        RDebug::Print(_L("iFileOpened %d "),iFileOpened);
        RDebug::Print(_L("iState  %d "),iState);
        RDebug::Print(_L("percentage downloaded %d "),percentageDone);
        RDebug::Print(_L("iBytesDownloaded %d "),iBytesDownloaded);
        RDebug::Print(_L("iAttemptsToOpen  %d "),iAttemptsToOpen);
        RDebug::Print(_L("iOpenInProgress %d "),iOpenInProgress);
        RDebug::Print(_L("iFileSize %d "),iFileSize);
    #endif
    //if((percentageDone>KPercentageNeededForHeader)&&
    if((iBytesDownloaded>KBytesNeededForHeader)&&
      (!iFileOpened) && iAttemptsToOpen<KRetryOpen && (!iOpenInProgress))
        {
        TInt err(KErrNone);
        if(iUseFileHandle)
            {
#ifdef RD_PD_FOR_AUDIO_CONTENT_VIA_HELIX_ENGINE
            iAudioPdPlayUtility->SetFileSize(iFileSize);
#endif
            iAudioPdPlayUtility->OpenFileL(iFileHandle);
            }
        else
            {
#ifdef RD_PD_FOR_AUDIO_CONTENT_VIA_HELIX_ENGINE
            iAudioPdPlayUtility->SetFileSize(iFileSize);
#endif
            TRAP(err,iAudioPdPlayUtility->OpenFileL(iFileName->Des()));
            }

        if(err != KErrNone)
            {
            MapcInitComplete(err,0);
            return;
            }
        iOpenInProgress = ETrue;
        iAttemptsToOpen++;

        }

    //Pass bytes downloaded to ProgressiveDownload DataSource
    iAudioPdPlayUtility->SetBytesDownloaded(iBytesDownloaded, iDownloadCompleted);

    if(iFileOpened)
        {//if the file is opened/playing save the
        //file byte position and the time position
        SavePosition();
        switch (iState)
            {
            case EPlaying:
                {
                if((iFileBytePosition >=iBytesDownloaded) &&
                   (percentageDone<=100))
                    {
                    //Should have paused, but is causing problems
                    //with DRM1 FL
                    #if _DEBUG
                      RDebug::Print(_L("Should have paused"));
                    #endif
                    //StartPaused();
                    }
                break;
                }
            // Try to automatically resume only if we are stopped due to lac
            // of data (KErrUnderflow)
            //case EPaused:
            //case EStopped:
            case EStoppedAutoResume:
                {
                //if stopped attempt to play at 25%,50%,75%,100%
                if( percentageDone >= iPercentageToBuffer )
                    {
                    StartPlaying();
                    }
                break;
                }
            default:
                break;
            }//switch iState

        if( percentageDone > iPercentageToBuffer )
            {
            iPercentageToBuffer += KPercentageToBufferIncrement;
            }
        }//iFileOpened

    }

/*
void CPdProperties::StartPaused()
    {
    #if _DEBUG
       RDebug::Print(_L("ProgressiveDownload audio paused"));
    #endif
    iState = EPaused;
    iAudioPdPlayUtility->Pause(); //Audio Pause
    //iCallback->Paused();
    }
*/

void CPdProperties::StartPlaying()
    {
    #if _DEBUG
       RDebug::Print(_L("ProgressiveDownload audio play"));
    #endif

    //Will disable automatic play if manual play was started.
    TInt state = iAudioPdPlayUtility->CheckAudioPlayerState();

    if(iState!=EPlaying)
        {
        iState = EPlaying;

        TTimeIntervalMicroSeconds timeposition(0);
        TInt error = iAudioPdPlayUtility->GetPosition(timeposition);

    #if _DEBUG
       RDebug::Print(_L("ProgressiveDownload SetPosition SavePos[%d] CurPos[%d]"), I64INT(iTimePosition.Int64()),I64INT(timeposition.Int64()));
    #endif

        if(timeposition != iTimePosition)
        {
        iAudioPdPlayUtility->SetPosition(iTimePosition);
        }

        iAudioPdPlayUtility->Play();
        //iCallback->Playing();
        }
    }

//From MMdaAudioPlayerCallback
void CPdProperties::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
    {

#if _DEBUG
   RDebug::Print(_L("ProgressiveDownload file opened err =%d"),aError);
#endif
    //iError = aError;
    iOpenInProgress = EFalse;

    iState = EStopped;

    if( aError == KErrNone )
        {
        if( aDuration > 0 )
            {
            iFileOpened = ETrue;
            }
        else
            {
#if _DEBUG
            RDebug::Print(_L("ProgressiveDownload - Open failed - could not calculate duration"));
            RDebug::Print(_L("metadata is at the end of file PD not supported"));
#endif
            iFileOpened = EFalse;
            return;
            }

        if( iFileSize > 0 )
            {
            TInt err = iAudioPdPlayUtility->SetFileSize(iFileSize);
#if _DEBUG
            RDebug::Print(_L("CPdProperties::SetFileSize=%d err %d"),iFileSize,err);
#endif
            }
        // After init is complete successfully, playback is to be started
        // automatically.
        iState = EStoppedAutoResume;
        }
    iCallback->MapcInitComplete(aError,aDuration);

    //Last chance to automatically play a file with very fast download
    //or when the file is in cache and there are only a couple of download events.
    if ( (iDownloadCompleted) && (aError == KErrNone ) )
        {
        StartPlaying();
        }
    }

void CPdProperties::MapcPlayComplete(TInt aError)
    {
#if _DEBUG
    RDebug::Print(_L("ProgressiveDownload play complete err=%d"),aError);
#endif
    //iError = aError;
    // Playback stopped due to lac of data (KErrUnderflow)
    if( aError == KErrUnderflow )
        {// By now, Controller would've gone to STOPPED state and would've reset
        // play position to zero. There is no point in querying position here.
        TTimeIntervalMicroSeconds timeposition;
        TInt error = iAudioPdPlayUtility->GetPosition(timeposition);
        if(timeposition > iTimePosition)
            {
            iTimePosition = timeposition;
            }
#if _DEBUG
        RDebug::Print(_L("ProgressiveDownload GetPosition return [%d] error[%d]"), I64INT(iTimePosition.Int64()), error);
#endif
        iState = EStoppedAutoResume;
        iCallback->Paused();
        }
    else
        {
        iState = EStopped;
        iCallback->MapcPlayComplete(aError);
        }
    }

void CPdProperties::Playing()
    {
    iCallback->Playing();
    }

void CPdProperties::Paused()
    {
    iCallback->Paused();
    }
