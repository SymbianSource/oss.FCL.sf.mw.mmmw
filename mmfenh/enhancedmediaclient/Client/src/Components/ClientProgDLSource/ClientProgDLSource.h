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
* Description:  Definition of the ClientProgDLSource class.
*
*/


#ifndef C_CCLIENTAUDIOPROGDLSOURCE_H
#define C_CCLIENTAUDIOPROGDLSOURCE_H

#include <e32base.h>
#include <ProgDLSource.h>
#include <DownloadMgrClient.h>
#include <mmfcontrollerframework.h>
#include <MultimediaDataSource.h>

#include "SourceBase.h"
#include "TransferRateMonitor.h"
#include "ProgDLSourceCustomCommands.h"
#include "EventAO.h"
#include "DownloadGateway.h"


class MCustomCommand;

namespace multimedia
    {
    
    class CEventNotifier;

    class MTransferRateMonitor;
    
    /**
    *  Implements the ClientSide Source for the ProgDL Source.
    *  Provides functionality to the Clients to query attributes
    *  from the ServerSide source
    *  @lib EnhancedMediaClient.lib
    *  @since S60 v3.2
    */    
    class CClientProgDLSource : public CSourceBase,
                                public MProgDLSource,
                                public MDownloadGatewayObserver,
                                public MTransferRateObserver,
                                public MSourceStateObserver
        {
        public:
            /**
            * Function to Create the Object.
            */
            CClientProgDLSource();
            /**
            * Destructor.
            */
            ~CClientProgDLSource();
            /**
            * Post Contructor. This is to Support the new type of Construction
            * of the Object. Notice there is no Two phased Constructor in this class
            */
            TInt PostConstructor();
            
            /**
            * Associates the DownloadGateway with the Progressive Download Source.
            * This Gateway class sends all the Browser Events to the Source.
            */
            TInt SetDownloadGateway(CDownloadGateway* aDownloadGateway);
            
            // From MControl begins
            /**
            * From MControl.
            * Adds the Observer to this Object
            * @since S60 v3.2
            * @param aObserver Client which set the Observer.             
            */            
            TInt AddObserver( MControlObserver& aObserver );
            /**
            * From MControl.
            * Removes the Observer to this Object
            * @since S60 v3.2
            * @param aObserver Client which set the Observer.             
            */            
            TInt RemoveObserver( MControlObserver& aObserver );
            /**
            * From MControl.
            * Returns the Type of this Object.
            * request and that it is now outstanding 
            * @since S60 v3.2
            */            
            TUid Type();
            /**
            * From MControl.
            * Returns the Control Type of this Object.
            *
            * @since S60 v3.2
            */            
            TControlType ControlType();
            // From MControl ends
            
            // From MSourceControl begins
            /**
            * From MSourceControl.
            * Returns the Size in Bytes of the Source.
            * Since this is ProgDL Source, the size represents
            * the file size which will be passed by the Client
            *  to the Source
            * @since S60 v3.2
            * @param aSize Size returned by the Source.             
            */            
            TInt GetSize( TUint& aSize );
            /**
            * From MSourceControl.
            * Returns the Mime Type of the Data for which
            * the Source is initialized.
            * @since S60 v3.2
            * @param aMimeType MimeType returned by the Source.             
            */            
            TInt GetMimeType( TDes8& aMimeType );
            /**
            * From MSourceControl.
            * Closes the Source.
            *
            * @since S60 v3.2
            */            
            TInt Close();
            // From MSourceControl ends
            
            // From MProgDLSource begins
            /**
            * From MProgDLSource.
            * Opens the Source with the FileName and TransactionID
            * Here it connects to the Browser and attaches to the 
            * download corresponding to the TransactionID
            *
            * @since S60 v3.2
            * @param aFileName      FileName to be Opened by the Source
            * @param aDLTxId        TransactionID to connect to the Browser
            */            
            TInt Open( const TDesC& aFileName,TInt32 aDLTxId );
            /**
            * From MProgDLSource.
            * Moves the Finished Download to the new Location. This is done
            * by calling the browser which is invoked through the Gateway.
            *
            * @since S60 v3.2
            * @param aFileName      FileName to be Opened by the Source
            */            
            TInt MoveFile(const TDesC& aDestFileName );
            /**
            * From MProgDLSource.
            * Cancels the Ongoing Download
            *
            * @since S60 v3.2
            */            
            TInt CancelDownload();
            /**
            * From MProgDLSource.
            * Resumes the Paused download
            *
            * @since S60 v3.2
            */            
            TInt ResumeDownload();
            /**
            * From MProgDLSource.
            * Gets the Current FileSize from the Browser which is getting 
            * Downloaded
            *
            * @since S60 v3.2
            * @param aSize  Size of the File already downloaded
            */            
            TInt GetCurrentFileSize( TUint& aSize );
            /**
            * From MProgDLSource.
            * Gets the Expected FileSize from the Browser which is getting 
            * Downloaded
            *
            * @since S60 v3.2
            * @param aSize  Final Size of the File
            */            
            TInt GetExpectedFileSize( TUint& aSize );
            /**
            * From MProgDLSource.
            * Return the Current Download Status of the Download
            *
            * @since S60 v3.2
            */            
            TDownloadStatus GetDownloadStatus();
            /**
            * From MProgDLSource.
            * Queries if the Download is Complete from the Gateway
            *
            * @since S60 v3.2
            * @param aBool      Download Complete Status
            */            
            TInt IsDownloadComplete(TBool& aBool);
            /**
            * From MProgDLSource.
            * Gets the Percentage of the File already Downloaded
            *
            * @since S60 v3.2
            * @param aPercent   Percentage of the File downloaded
            */            
            TInt GetPercentageDownloaded(TUint& aPercent);
            /**
            * From MProgDLSource.
            * Gets the Percentage of the File Buffered before Playback starts.
            * This query is to the ServerSide Source which is in Buffering State
            *   
            * @since S60 v3.2
            * @param aPercent      Percentage of Data Buffered before Playback starts.
            */            
            TInt GetPercentageBuffered(TUint& aPercent);            
            /**
            * From MProgDLSource.
            * This is the DownloadRate that tells you how fast data is downloaded
            * by the Download Manager
            *
            * @since S60 v3.2
            * @param aRate  Rate at which Data is Downloaded.
            */            
            TInt GetDownloadingRate(TUint& aRate);
            /**
            * From MProgDLSource.
            * Returns the BitRate of the Data played from the Source
            *
            * @since S60 v3.2
            * @param aRate  BitRate of the Data Played from the Source
            */            
            TInt GetBitRate(TUint& aRate); 
            /**
            * From MProgDLSource.
            * Returns the FileName associated with the Download.
            * This value is set by the Client when it calls Open().
            *
            * @since S60 v3.2
            * @param aFileName  FileName to be Opened by the Source
            */            
            TInt FileName(TPtr& aFileName);
            // From MProgDLSource ends
            
            // From MDownloadGatewayObserver
            /**
            * From MDownloadGatewayObserver.
            * The Gateway Observer sends event which it receives from the
            * Download Manager.
            *
            * @since S60 v3.2
            * @param aStatus  Current Download Status
            */            
            void Event(TDownloadStatus aStatus);
            /**
            * From MDownloadGatewayObserver.
            * Notifies the Client that the Move of the File Complete
            * with an ErrorCode returned.
            *
            * @since S60 v3.2
            * @param aError  Move Complete with this ErrorCode.
            */            
            void MoveFileComplete(TInt aError);
            // MDownloadGatewayObserver Ends
            
            
            // From CSourceBase begins
            /**
            * From CSourceBase.
            * Callback from the StreamControl when the Source is Created
            *
            * @since S60 v3.2
            * @param aCustomCommand    Custom Command handle to send message to the Source             
            * @param aSourceHandle     Handle Identifying the Source             
            */            
            void ServerSourceCreated( MCustomCommand& aCustomCommand,
                                    TMMFMessageDestination& aSourceHandle );
            /**
            * From CSourceBase.
            * Callback from the StreamControl when the ServerSource is Deleted
            *
            * @since S60 v3.2
            */            
            void ServerSourceDeleted();
            /**
            * From CSourceBase.
            * Returns the Content Protection information to the Stream Control
            * Currently returns KErrNotSupported 
            * @since S60 v3.2
            */            
            TBool IsEncrypted();
            /**
            * From CSourceBase.
            * Returns the SourceUID
            *
            * @since S60 v3.2
            */            
            TUid GetSourceUid();
            /**
            * From CSourceBase.
            * Returns the Header Data passed when the Client calls Open
            *  Currently return KErrNotSupported
            * @since S60 v3.2
            */            
            TInt GetHeaderData(TPtr& aPtr);
            // From CSourceBase ends
            
            //MTransferRateObserver begins
            /**
            * From MTransferRateObserver.
            * Callback from the TransferRate Monitor that the Rate Changed
            * @since S60 v3.2
            */            
            void TransferRateChanged();
            /**
            * From MTransferRateObserver
            * Method from the Transfer Rate Monitor to query CurrentFile Size.
            * @since S60 v3.2
            * @param aCurrentSize Current File Size returned to the Caller             
            */            
            TInt GetCurrentSize( TUint& aCurrentSize );
            // MTransferRateObserver ends
            
            // MSourceStateObserver begins
            /**
            * From MSourceStateObserver.
            * Callback from the Active Object that the Source State changed.
            * @since S60 v3.2
            */            
            void SourceStateChanged();    
            // MSourceStateObserver ends		
            
        private:
            // Variable to determine if the ServerSource exists
            TBool iServerSourceExists;

            // FileSize from the Client
            TUint iSourceSize;
            // Current Size from the Browser
            TInt iCurrentSize;
            // Destination File Name            
            TFileName iDestFileName;

            // Handle to send messages to the Server Side Source
            MCustomCommand* iCustomCommand;
            // Handle Identifying the Server Side Source
            TMMFMessageDestination iSourceHandle;
            
            // Current State of the Source
            TPckgBuf<CMultimediaDataSource::TState> iStatePckg;
            // Observer monitoring the State change event
            CEventAO* iStateEvent;
            // Current State of the Source
            CMultimediaDataSource::TState iSourceState;
            // Event Notifier to notify Clients
            CEventNotifier* iEventNotifier;
            // Storing the Download Complete Status
            TBool isDownloadComplete;
            // Storing the Download Paused status
            TBool isDownloadPaused;
            // Storing the DownloadID/Transaction ID
            TInt32 iDownloadId;
            // FileName passed to Open the Source
            HBufC* iFileName;
            // Gateway Attached to this Source to talk to Browser
            CDownloadGateway* iDownloadGateway;
            // Transfer Rate monitor
            CTransferRateMonitor* iMonitor;
            // Current Size of the File in Bytes.
            TUint iCurrentBytes;
            // Is the Playback in Progressive Mode
            TBool iProgressiveMode;
        };
    } // namespace multimedia

#endif // CLIENTAUDIOPROGDLSOURCE_H

// End of file
