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
* Description:  Header of DownloadGateway class.
*
*/


#ifndef DOWNLOADGATEWAY_H
#define DOWNLOADGATEWAY_H

#include <e32base.h>
#include <downloadmgrclientapiext.h>
#include <ProgDLSource.h>

namespace multimedia
    {
    
    /**
     *  Observer that sends browser events to the Client
     *  @lib EnhancedMediaClient.lib
     *  @since S60 v3.2
     */    
    class MDownloadGatewayObserver
        {
        public:
            virtual void Event(MProgDLSource::TDownloadStatus aStatus) = 0;
            virtual void MoveFileComplete(TInt aError) = 0;
        };

    /**
    *  Implements the Gateway from which the Clients talk to the DownloadMgr.
    *  Provides functionality to the Clients to query attributes
    *  from the Browser
    *  @lib EnhancedMediaClient.lib
    *  @since S60 v3.2
    */    
    class CDownloadGateway : public MHttpDownloadMgrObserver
        {
        public:
            /**
            * Function to Create the Object.
            */
            CDownloadGateway();
            /**
            * Destructor.
            */
            ~CDownloadGateway();

            /**
            * Initialize the Gateway with the Client UID.
            * Here it connect to the DownloadMgr Server.
            * @since S60 v3.2
            * @param aClientUid Client which wants to connect.             
            */            
            TInt Initialize(TUid aClientUid);
            /**
            * Calls Move on the DownloadMgr for the TransactionID
            * @since S60 v3.2
            * @param aId TransactionID for which the File has to be moved.             
            * @param aDestFileName Full Pathname where the file needs to be moved.             
            */            
            TInt MoveFile( TInt aId, const TDesC& aDestFileName );
            /**
            * Called by the Client which wants to Connect to the Download
            * @since S60 v3.2
            * @param aId TransactionID for which the File has to be moved.             
            * @param aObserver Observer where the Client wants the Callbacks             
            * @param aAppUid UID for the App which Connects.             
            */            
            TInt ConnectToDownload(TInt aId, MDownloadGatewayObserver& aObserver, TUid aAppUid);
            /**
            * Disconnect from the Download
            * Here it connect to the DownloadMgr Server.
            * @since S60 v3.2
            * @param aId DownloadID from which the Client wants to Disconnect.             
            */            
            TInt DisconnectFromDownload(TInt aId);
            /**
            * Gets the Current Downloaded Size of the Download
            * @since S60 v3.2
            * @param aId TransactionID of the Download which the FileSize is queried.             
            * @param aSize Size of the Download returned             
            */            
            TInt GetCurrentSize(  TInt aId, TUint& aSize );
            /**
            * Total Size of the File which is being downloaded
            * Here it connect to the DownloadMgr Server.
            * @since S60 v3.2
            * @param aId TransactionID of the Download which the FileSize is queried.             
            * @param aSize Total Size of the File being Downloaded returned             
            */            
            TInt GetDownloadSize( TInt aId, TUint& aSize );
            /**
            * Gets the Download Status for a particular Download
            * @since S60 v3.2
            * @param aId TransactionID of the Download which the FileSize is queried.             
            * @param aStatus Download Status for the Download           
            */            
            TInt GetDownloadStatus(TInt aTxId, MProgDLSource::TDownloadStatus& aStatus);
            /**
            * Cancelling a Particular Download
            * @since S60 v3.2
            * @param aId TransactionID of the Download to be deleted             
            */            
            TInt DeleteDownload(TInt aId);
            /**
            * Resuming a Paused Download
            * @since S60 v3.2
            * @param aId TransactionID of the Download to be Resumed
            */            
            TInt ResumeDownload(TInt aId);
            /**
            * Queries the MimeType of the File from the DownloadMgr
            * @since S60 v3.2
            * @param aId TransactionID of the Download to be queried
            * @param aMimeType MimeType returned from the Browser.           
            */            
            TInt GetMimeType(TInt aId, TDes8& aMimeType);
            /**
            * Tells the Browser if the Download is being Progressively Played
            * @since S60 v3.2
            * @param aId TransactionID of the Dowload to be Resumed
            * @param aMode Whether the Mode is True or False.            
            */            
            TInt SetProgressiveMode(TInt aId, TBool aMode);
            /**
            * Before Calling Move we need to Call this to set the Local FileName
            * @since S60 v3.2
            * @param aId TransactionID of the Dowload for setting the FileName
            * @param aFileName Name to be Set
            */            
            TInt SetLocalFileName( TInt aId, TDesC& aFileName );
            /**
            * Gets the Local FileName from the DownloadMgr
            * @since S60 v3.2
            * @param aId TransactionID of the Dowload to be Resumed
            * @param aFileName Gets the FileName from the DownloadMgr
            */            
            TInt GetLocalFileName(TInt aId, TDes& aFileName );
            // From MhttpDownloadMgrObserver
            /**
            * From MhttpDownloadMgrObserver.
            * Callback from the DownloadManager notifying Client Events.
            * @since S60 v3.2
            * @param aDownload Download for which the Event is meant             
            * @param aEvent Actual Event             
            */            
            void HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent );
            // MhttpDownloadMgrObserver Ends

        private:
            // The Observer where we need to forward the Browser events
            MDownloadGatewayObserver* iObserver;
            // Download Manager instance to which we connect
            RHttpDownloadMgrApiExt  iDownloadMgr;
            // Download for which we connect
            RHttpDownload* iDownload;
            // Status if we are connected
            TBool iDMgrConnected;
            // Previous DLMgr Event
            THttpDownloadEvent iPrevEvent;
            // Current Download ID
            TInt iDownloadId;
            // Current Download Status
            MProgDLSource::TDownloadStatus iDownloadStatus;
        };
    } // namespace multimedia

#endif // DOWNLOADGATEWAY_H

// End of file
