/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the ClientProgDLSource class.
*
*/


#include <e32std.h>
#include <MCustomCommand.h>
#include <SourceControlObserver.h>
#include "ClientProgDLSource.h"
#include "EventNotifier.h"
#include "EventAO.h"
#include "ErrorCode.h"
#include "ProgDLSourceUid.h"

#define RETURN_IF_ERROR(x) if(x != KErrNone) return x

// CONSTANTS
const TInt KLocalFile = -1;
const TInt KTranferRateMonitorFrequency = 2;
const TInt KFileSizeChangeDelta = 10000;
const TInt KNofityAfterPercentage = 5;

using namespace multimedia;

CClientProgDLSource::CClientProgDLSource()
    {
    iSourceState = CMultimediaDataSource::ECLOSED;
    isDownloadComplete = false;
    iProgressiveMode = false;
    }

CClientProgDLSource::~CClientProgDLSource()
    {
    delete iEventNotifier;
    delete iMonitor;
    delete iFileName;
    
    if(iStateEvent->IsActive())
        {
        iStateEvent->Cancel();
        }
    
    delete iStateEvent;
    }

TInt CClientProgDLSource::PostConstructor()
    {
    TInt status(KErrNone);
    // Make sure this doesn't get called second time around.
    if ( !iEventNotifier )
        {
        TRAP(status, iEventNotifier = CEventNotifier::NewL());
        }
    
    RETURN_IF_ERROR(status);
    
    TRAP(status, iStateEvent  = CEventAO::NewL(*this)); 
    
    return status;
    }

TInt CClientProgDLSource::AddObserver( MControlObserver& aObserver )
    {
    return iEventNotifier->AddObserver( aObserver );
    }

TInt CClientProgDLSource::RemoveObserver( MControlObserver& aObserver )
    {
    return iEventNotifier->RemoveObserver( aObserver );
    }

TUid CClientProgDLSource::Type()
    {
    return KProgDLSourceControl;
    }

TControlType CClientProgDLSource::ControlType()
    {
	return ESourceControl;
    }
    
TInt CClientProgDLSource::GetSize( TUint& aSize )
    {
    TInt status(KErrNone);
    aSize = iSourceSize;
    return status;
    }

TInt CClientProgDLSource::GetMimeType( TDes8& aMimeType )
    {
    TInt status = iDownloadGateway->GetMimeType(iDownloadId,aMimeType);
    return status;
    }

TInt CClientProgDLSource::Close()
    {
    TInt status(KErrNone);
    if(iMonitor)
        {
        iMonitor->Stop();
        }
    // Reset the Progressive Mode flag always
    iDownloadGateway->SetProgressiveMode(iDownloadId,EFalse);
    iProgressiveMode = EFalse;
    iDownloadId = 0;
    iCurrentBytes = 0;
    isDownloadPaused = EFalse;
    isDownloadComplete = EFalse;
    iCurrentSize = 0;
    iSourceSize = 0;
    iServerSourceExists = EFalse;
    return status;
    }

TInt CClientProgDLSource::SetDownloadGateway(CDownloadGateway* aDownloadGateway)
    {
    iDownloadGateway = aDownloadGateway;
    return KErrNone;
    }


TInt CClientProgDLSource::Open(const TDesC& aFileName,TInt32 aDLTxId )
    {
    TInt status(KErrNone);
    if(aDLTxId != KLocalFile)
        {
        status = iDownloadGateway->ConnectToDownload(aDLTxId,*this,KNullUid);
        if(status == KErrNone)
        	{
		    iDownloadId = aDLTxId;
		    isDownloadComplete = EFalse;
	        //Gets current downloaded size
	        status = iDownloadGateway->GetDownloadSize(iDownloadId,iSourceSize);
	        RETURN_IF_ERROR(status);
	    	}
	    else
	    	{
	    	iDownloadId = 0;	
	    	return status;	
	    	}	
        }
    else
        {
        isDownloadComplete = ETrue;
        }        
    
    if(iFileName)
        {
        delete iFileName;
        iFileName = NULL;
        }

    TRAP(status,iFileName = HBufC::NewL(aFileName.Size()));
	
	if(!status)
	    {
	    TPtr des = iFileName->Des();
	    des.Copy(aFileName);
	    }    
    
    return status;
    }

TInt CClientProgDLSource::MoveFile(const TDesC& aDestFileName )
    {
    TInt status(KErrNone);
    iDestFileName.Copy(aDestFileName);
    status = iDownloadGateway->MoveFile(iDownloadId,aDestFileName);
    return status;
    }

TInt CClientProgDLSource::IsDownloadComplete(TBool& aBool)
    {
    TInt status(KErrNone);
    
    TDownloadStatus dlStatus;
    status = iDownloadGateway->GetDownloadStatus(iDownloadId,dlStatus);
    if(dlStatus == ECompleted)
        {
        aBool = true;
        }
    else
        {
        aBool = false;
        }
            
    return status;
    }

TInt CClientProgDLSource::GetPercentageDownloaded(TUint& aPercent)
    {
    TInt status(KErrNone);
    TUint curSize;
    TUint fullSize;
    if(!isDownloadComplete)
        {
        status = iDownloadGateway->GetCurrentSize(iDownloadId,curSize);
        RETURN_IF_ERROR(status);
        status = iDownloadGateway->GetDownloadSize(iDownloadId,fullSize);    
        RETURN_IF_ERROR(status);
        }
    else
        {
        curSize = iCurrentSize;
        fullSize = iSourceSize;
        }
    
    if(fullSize)
        {
        aPercent = (curSize*100/fullSize);
        }
    else
        {
        aPercent = 0;
        }        
    return status;
    }

TInt CClientProgDLSource::GetDownloadingRate(TUint& aRate)
    {
    TInt status(KErrNone);
    if(iMonitor)
        {
        aRate = iMonitor->TransferRate();
        }
    else
        {
        status = KErrNotFound;
        }
    return status;
    }    

TInt CClientProgDLSource::GetCurrentFileSize( TUint& aSize )
    {
    TInt status(KErrNotFound);
    if(!isDownloadComplete)
        {
        status = iDownloadGateway->GetCurrentSize(iDownloadId,aSize);
        }
    else
        {
        iCurrentSize = iSourceSize;
        status = KErrNone;
        }
    return status;
    }
     
TInt CClientProgDLSource::GetExpectedFileSize( TUint& aSize )
    {
    TInt status(KErrNotFound);
    if(!isDownloadComplete)
        {
        status = iDownloadGateway->GetDownloadSize(iDownloadId,aSize);
        }
    else
        {
        aSize = iSourceSize;
        status = KErrNone;
        }
    return status;
    }
    
MProgDLSource::TDownloadStatus CClientProgDLSource::GetDownloadStatus()
    {
    MProgDLSource::TDownloadStatus dlStatus;
    iDownloadGateway->GetDownloadStatus(iDownloadId,dlStatus);
    return dlStatus;
    }
    

TInt CClientProgDLSource::GetPercentageBuffered(TUint& aPercent)
    {
    //Source Custom Command
    TPckgBuf<TUint> pckg;
    if(iServerSourceExists)
        {
        iCustomCommand->CustomCommandSync( iSourceHandle, EGetPercentageBuffered, KNullDesC8, KNullDesC8, pckg );
        aPercent = pckg();
        return KErrNone;
        }
    else
        {
        aPercent = 0;
        return KErrNotFound;    
        }
    }            

TInt CClientProgDLSource::GetBitRate(TUint& aRate)
    {
    //Source Custom Command
    TPckgBuf<TUint> pckg;
    if(iServerSourceExists)
        {
        iCustomCommand->CustomCommandSync( iSourceHandle, EGetBitRate, KNullDesC8, KNullDesC8, pckg );
        aRate = pckg();
        return KErrNone;
        }
    else
        {
        aRate = 0;
        return KErrNotFound;
        }
    }

TInt CClientProgDLSource::FileName(TPtr& aFileName)
    {
    TInt status(KErrNone);
    aFileName.Set(iFileName->Des());
    return status;
    }


TInt CClientProgDLSource::CancelDownload()
    {
    TInt status(KErrNone);
    status = iDownloadGateway->DeleteDownload(iDownloadId);
    return status;
    }

TInt CClientProgDLSource::ResumeDownload()
    {
    TInt status(KErrNone);
    if(isDownloadPaused)
        {
        iDownloadGateway->ResumeDownload(iDownloadId);
        }
    return status;
    }    

void CClientProgDLSource::ServerSourceCreated( MCustomCommand& aCustomCommand,
                                                   TMMFMessageDestination& aSourceHandle )
    {
    iServerSourceExists = ETrue;
    iCustomCommand = &aCustomCommand;
    iSourceHandle = aSourceHandle;
    
    if(!isDownloadComplete)
        {
        TRAPD(err1,iMonitor = CTransferRateMonitor::NewL(*this, KTranferRateMonitorFrequency, KFileSizeChangeDelta));
        if(!err1)
    	    {
    		iMonitor->Start();
    		}
    	
        // Send data to server source
        TPckgBuf<TInt> pckg(iSourceSize);
        iCustomCommand->CustomCommandSync( iSourceHandle, ESetActualSize, pckg, KNullDesC8 );
        
        iStateEvent->SetActive();
        iStatePckg() = iSourceState;	  
        iCustomCommand->CustomCommandAsync(
            iSourceHandle,
            (TInt)EGetSourceState,
            iStatePckg,
            KNullDesC8,
            iStatePckg,
            iStateEvent->iStatus);
            
         }
     else
        {
        iCustomCommand->CustomCommandSync( iSourceHandle, ESetDownloadState, KNullDesC8, KNullDesC8 );        
        }
    }

void CClientProgDLSource::ServerSourceDeleted()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CClientProgDLSource::ServerSourceDeleted()"));    
#endif    
    iServerSourceExists = EFalse;
    iCustomCommand = NULL;
    
    if(iMonitor)
        {
        iMonitor->Stop();
        }
    }

TBool CClientProgDLSource::IsEncrypted()
    {
    return EFalse;
    }
    
TUid CClientProgDLSource::GetSourceUid()
    {
    return KMmfProgDLSource;
    }
    
TInt CClientProgDLSource::GetHeaderData(TPtr& /*aPtr*/)
    {
    return KErrNotSupported;
    }
    
void CClientProgDLSource::MoveFileComplete(TInt aError)
    {
    if(iServerSourceExists)
        {
        TPtr des = iFileName->Des();
        if(aError == KErrNone)
            {
    	    des.Copy(iDestFileName);
            }
	    else
            {
            iDownloadGateway->GetLocalFileName(iDownloadId,des);
            }
        TPckgBuf<TFileName> pckg(iDestFileName);
        iCustomCommand->CustomCommandSync( iSourceHandle, ESetUpdateFileName, pckg, KNullDesC8 );
        }
    
    CErrorCode* event1 = new CErrorCode( aError );       
    iEventNotifier->Event(this,MSourceControlObserver::KFileMoveCompleteEvent,event1);
    }

void CClientProgDLSource::Event(TDownloadStatus aStatus)
    {   
#ifdef _DEBUG
    RDebug::Print(_L("CClientProgDLSource::Event DownloadStatus[%d]"),aStatus);
#endif            

    switch(aStatus)
        {
            case ECompleted:
                {
                if(iMonitor)
                    {
                    iMonitor->Stop();	
                    }
                TUint size(0);
                GetCurrentFileSize(size);
                TInt value = size;
                TPckgBuf<TInt> pckg(value);
				if(iServerSourceExists)
				{
                	if (iSourceState != CMultimediaDataSource::ESTOPPED)
                    	{
	                    iCustomCommand->CustomCommandSync( iSourceHandle, ESetDownloadSize, pckg, KNullDesC8 );
    	                }
        	        iCustomCommand->CustomCommandSync( iSourceHandle, ESetDownloadState, KNullDesC8, KNullDesC8 );
				}
                isDownloadComplete = ETrue;               
                break;                    
                }
            case EStarted:
                {
                isDownloadPaused = EFalse;    
                break;
                }
            case EPaused:
                {
                isDownloadPaused = ETrue;
                break;    
                }
            case EDeleted:
                {
                if(iMonitor)
                    {
                    iMonitor->Stop();
                    }
                break;
                }

            case EUnknown:
                break;
                
        }
        //Send only Known Events
        if(aStatus != EUnknown)
            {
            CErrorCode* event1 = new CErrorCode( KErrNone );       
            iEventNotifier->Event(this,MSourceControlObserver::KDownloadStatusChangedEvent,event1); 
            }
    }

TInt CClientProgDLSource::GetCurrentSize( TUint& aCurrentSize )
    {
    //RDebug::Print(_L("CClientProgDLSource::GetCurrentSize"));			
    TUint value(0);
    GetExpectedFileSize(value);
    
    GetCurrentFileSize(aCurrentSize);
    
    if(aCurrentSize - iCurrentBytes > ((KNofityAfterPercentage/100) * value)) //Currently checking for 5%
        {
            iCurrentBytes = aCurrentSize;
            CErrorCode* event1 = new CErrorCode( KErrNone );       
            iEventNotifier->Event(this,MSourceControlObserver::KPercentageDownloadedChangedEvent,event1);
        }
    
    iCurrentSize = aCurrentSize;
    
    if ( iServerSourceExists && iSourceState != CMultimediaDataSource::ESTOPPED)
        {
        // Send data to server source
        
        TPckgBuf<TInt> pckg(iCurrentSize);
        iCustomCommand->CustomCommandSync( iSourceHandle, ESetDownloadSize, pckg, KNullDesC8 );
        
        }			
#ifdef _DEBUG
    RDebug::Print(_L("CClientProgDLSource::GetCurrentSize:[%d]"), aCurrentSize);	
#endif    
    
    return KErrNone;			
    }

void CClientProgDLSource::TransferRateChanged()
    {
#ifdef _DEBUG    
    RDebug::Print(_L("CClientProgDLSource::TransferRateChanged:[%d]"), iMonitor->TransferRate());
#endif    
    if ( iServerSourceExists  && !isDownloadComplete && iSourceState != CMultimediaDataSource::ESTOPPED)
        {
        // Send data to server source
        
        TPckgBuf<TUint> pckg1(iMonitor->TransferRate());
        iCustomCommand->CustomCommandSync( iSourceHandle, EDownloadRate, pckg1, KNullDesC8 );
        
        }			
    }

void CClientProgDLSource::SourceStateChanged()		
    {
#ifdef _DEBUG
    RDebug::Print(_L("CClientProgDLSource::SourceStateChanged() :PrevState[%d] NewState[%d]"), iSourceState,iStatePckg());
#endif    
    iSourceState = iStatePckg();
    
    if(iSourceState == CMultimediaDataSource::ESTOPPED || iSourceState == CMultimediaDataSource::ECLOSED)        
        {
        //iDownloadGateway->SetProgressiveMode(iDownloadId,EFalse);
        iProgressiveMode = EFalse;
        }
    else if(!iProgressiveMode && !isDownloadComplete)
        {
        iDownloadGateway->SetProgressiveMode(iDownloadId,ETrue);
        TPtr ptrFileName = iFileName->Des();
        iDownloadGateway->SetLocalFileName( iDownloadId, ptrFileName );
        iProgressiveMode = ETrue;
        }
    
        
    if(iStateEvent->Error() != KErrDied && iServerSourceExists)
        {
        iStateEvent->SetActive();	    
        iCustomCommand->CustomCommandAsync(
            iSourceHandle,
            (TInt)EGetSourceState,
            iStatePckg,
            KNullDesC8,
            iStatePckg,
            iStateEvent->iStatus);    
        }                               
    }
// End of file
