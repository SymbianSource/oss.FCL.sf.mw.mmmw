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
* Description:  Implementation of DownloadGateway class.
*
*/


#include "DownloadGateway.h"
#include <e32std.h>

#define RETURN_IF_ERROR(x) if(x != KErrNone) return x

using namespace multimedia;

// CONSTANTS
CDownloadGateway::CDownloadGateway()
    {
    iDMgrConnected = EFalse;
    iDownload = NULL;
    iDownloadId = 0;
    }

CDownloadGateway::~CDownloadGateway()
    {
    if ( iDMgrConnected )
        {
        iDownloadMgr.Close();
        }
    }

TInt CDownloadGateway::Initialize(TUid aClientUid)
    {
    TInt status(KErrNone);
    if ( !iDMgrConnected )
        {
        TRAP(status,iDownloadMgr.ConnectL( aClientUid, *this, EFalse ));
        if(!status)
            {
            iDMgrConnected = ETrue;
            }
        }
    return status;
    }

TInt CDownloadGateway::MoveFile( TInt aId, const TDesC& aDestFileName )
    {
    TInt status(KErrNotFound);
    // Check here if the download is complete and the Source is in Stopped State
    if(iDownloadId == aId && iDownload)
        {
        status = iDownload->SetStringAttribute( EDlAttrDestFilename, aDestFileName );
        if(!status)
            {
            iDownload->Move();
            }
        }
    return status;
    }

TInt CDownloadGateway::ConnectToDownload(TInt aId, MDownloadGatewayObserver& aObserver, TUid /*aAppUid*/)
    {
    TInt status(KErrNotFound);
    TRAP(status, iDownload = &(iDownloadMgr.AttachToDownloadL( aId )));
    if(status == KErrNone)
    	{
	    iObserver = &aObserver;
	    iDownloadId = aId;
    	}
	else
		{
		iDownload = NULL;	
		}    	
    return status;
    }

TInt CDownloadGateway::DisconnectFromDownload(TInt aId)
    {
    TInt status(KErrNotFound);
    if(iDownloadId == aId && iDownload)
        {
        iDownload = NULL;
        iDownloadId = 0;
        status = KErrNone;
        }
    return status;
    }

TInt CDownloadGateway::GetMimeType(TInt aId, TDes8& aMimeType)
    {
    TInt status(KErrNotFound);
    if(iDownloadId == aId && iDownload)
        {
        status = iDownload->GetStringAttribute( EDlAttrContentType, aMimeType  );
        }
    return status;
    }

TInt CDownloadGateway::GetCurrentSize(  TInt aId, TUint& aSize )
    {
    TInt status(KErrNotFound);
    TInt32 value(0);
    if(iDownloadId == aId && iDownload)
        {
         status = iDownload->GetIntAttribute( EDlAttrDownloadedSize, value );
         if(!status)
            {
            aSize = value;
            }
        }
    return status;
    }

TInt CDownloadGateway::GetDownloadSize( TInt aId, TUint& aSize )
    {
    TInt status(KErrNotFound);
    TInt32 value(0);
    if(iDownloadId == aId && iDownload)
        {
        status = iDownload->GetIntAttribute( EDlAttrLength, value );
        if(status == KErrNone)
            {
            aSize = value;
            }
        }
    return status;
    }

TInt CDownloadGateway::GetDownloadStatus(TInt aId, MProgDLSource::TDownloadStatus& aStatus)
    {
    TInt status(KErrNotFound);
    if(iDownloadId == aId && iDownload)
        {
        aStatus = iDownloadStatus;
        }
    return status;
    }

TInt CDownloadGateway::DeleteDownload(TInt aId)
    {
    TInt status(KErrNotFound);
    if(iDownloadId == aId && iDownload)
        {
        status = iDownload->Delete();
        }
    return status;
    }

TInt CDownloadGateway::ResumeDownload(TInt aId)
    {
    TInt status(KErrNotFound);
    if(iDownloadId == aId && iDownload)
        {
        status = iDownload->Start();
        }
    return status;
    }

TInt CDownloadGateway::SetProgressiveMode(TInt aId, TBool aMode)
    {
    TInt status(KErrNotFound);
    if(iDownloadId == aId && iDownloadStatus != MProgDLSource::EDeleted && iDownload)
        {
        status = iDownload->SetBoolAttribute( EDlAttrProgressive, aMode );
        }
    return status;
    }

TInt CDownloadGateway::SetLocalFileName( TInt aId, TDesC& aFilName )
    {
    TInt status(KErrNotFound);
    if( iDownloadId == aId  && iDownload)
        {
        status = iDownload->SetStringAttribute( EDlAttrLocalFileName, aFilName );
        }
    return status;
    }

TInt CDownloadGateway::GetLocalFileName( TInt aId, TDes& aFileName )
    {
    TInt status(KErrNotFound);
    if( iDownloadId == aId  && iDownload)
        {
        status = iDownload->GetStringAttribute( EDlAttrDestFilename, aFileName );
        }
    return status;
    }

void CDownloadGateway::HandleDMgrEventL( RHttpDownload& /*aDownload*/, THttpDownloadEvent aEvent )
    {
#ifdef _DEBUG
        RDebug::Print(_L("CDownloadGateway::HandleDMgrEventL DownloadState[%d] ProgressState[%d]"),aEvent.iDownloadState,aEvent.iProgressState);
#endif        
        MProgDLSource::TDownloadStatus status;

        // When the new evnet is different in one of the states 
        // then the current event 
        if(iPrevEvent.iDownloadState != aEvent.iDownloadState)
            {
                switch(aEvent.iDownloadState)
                {
                    case EHttpDlCreated:
                            status = MProgDLSource::EConnecting;
                            break;
                    case EHttpDlInprogress:
                            status = MProgDLSource::EStarted;
                            break;
                    case EHttpDlPaused:
                            status = MProgDLSource::EPaused;
                            break;
                    case EHttpDlCompleted:
                            status = MProgDLSource::ECompleted;
                            break;
                    case EHttpDlFailed:
                            status = MProgDLSource::EFailed;
                            break;
                    case EHttpDlDeleting:
                            status = MProgDLSource::EDeleted;
                            break;                        
                    default:
                            status = MProgDLSource::EUnknown;
                            break;
                };
                
                iDownloadStatus = status;
                iObserver->Event(status);
            }
        
        if(iPrevEvent.iProgressState != aEvent.iProgressState)
            {
                switch(aEvent.iProgressState)
                {
                    
                    case EHttpProgContentFileMoved:
                            {
                            TInt32 value(0);
                            TInt err = iDownload->GetIntAttribute( EDlAttrErrorId, value );
#ifdef _DEBUG
                            RDebug::Print(_L("Delete File Code 2[%d][%d]"),err,value);
#endif                            
                            if(!err)
                                {
                                iObserver->MoveFileComplete(value);
                                }
                            }
                            break;
                   case EHttpProgCodLoadEnd:
                            // Case for COD download complete
                            iDownloadStatus = MProgDLSource::ECompleted;
                            iObserver->Event(MProgDLSource::ECompleted);
                            break;
                    case EHttpProgContentFileMovedAndDestFNChanged:
                            {
                            iObserver->MoveFileComplete(KErrAlreadyExists);
                            break;
                            }
                };
            }
        
        iPrevEvent = aEvent;
    }

// End of file
