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
* Description:  Header of ProgDLMultimediaSource.
*
*/




#ifndef __PROGDLMULTIMEDIASOURCE_H_
#define __PROGDLMULTIMEDIASOURCE_H_


#include <centralrepository.h>  

class CReadWriteRequest;
class CTransferBufferCopy;
class MGenericFile;




/**
@publishedAll
@released

MultiMedia framework class.  Represents a physical file.
This may be source file from which data is read or destination to which data is written.
Intended to be used by controller plugin developers for source and sink plugins.*/

class CProgDLMultimediaSource : public CMultimediaDataSource
    {
    public:
        // From MDataSource
        IMPORT_C static CProgDLMultimediaSource* NewL(MDataSource& aDataSource);
        
        // From CMultimediaDataSource begins
        TInt SetObserver( MMultimediaDataSourceObserver& aObserver );
        TInt GetObserver( MMultimediaDataSourceObserver*& aObserver );
        void Event( TUid aEvent );
        TInt SetDataTypeCode(TFourCC aSourceFourCC );
        TInt GetDataTypeCode(TFourCC& aSourceFourCC );
        TInt GetSize( TUint& aSize );
        TInt Open();
        TInt Close();
        TInt Prime();
        TInt Play();
        TInt Stop();
        TInt FillBuffer( CMMFBuffer* aBuffer);
        TInt Seek( TUint aPosInBytes );
        TInt GetSeekingSupport( TBool& aSeekSupport );
        TInt GetRandomSeekingSupport( TBool& aSeekSupport );
        // From CMultimediaDataSource ends
        
        TInt GetDRMProtection( TBool& aIsProtected );
        TInt EvaluateIntent( ContentAccess::TIntent aIntent );
        TInt ExecuteIntent( ContentAccess::TIntent aIntent );
        TInt SetAgentProperty( ContentAccess::TAgentProperty aProperty,
            TInt aValue);
        const TDesC& UniqueId() const;
        // From MCustomInterface
        TAny* CustomInterface( TUid aInterfaceUid );
        void SourceCustomCommand(TMMFMessage& aMessage);

        void FillBufferL( CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId ) ;
        
       
    public :
        virtual ~CProgDLMultimediaSource() ;
        //	static MDataSource* NewSourceL(/*TUid aType*/) ;
        
        void SetTransferBuffer (TBool aTBuffer) ;
        TBool CanUseTransferBuffer () ;
        TInt ReadRequestStatus(CReadWriteRequest* aRequest,TRequestStatus& aStatus);
        
    protected:
    /**
    Indicates for which purpose the object instance is being created
        */
        enum TMMFileMode
            {
            /** The object is created for being a source */
            ESourceMode,
                /** The object is created for being a sink */
                ESinkMode
            };

        void ConstructL(const TDesC8& aInitData, TMMFileMode aFileMode) ;
        void ConstructL();
        CProgDLMultimediaSource(MDataSource& aDataSource);
        
        void StoreRequestL( CReadWriteRequest* aRequest ) ;
        void CancelRequests();
        
        TInt EmptySinkQueue();
        CTransferBufferCopy* ObtainCopyOfTransferBufferL(TInt aMaxLength);
        
        // Calculate BufferedDataSize
        void CalculateBufferedDataSize();
        void StateChanged(TState newState);
        
        // A common function that implements FillBuffer requests
        TInt AppendBufferToSinkQueue( CMMFBuffer* aBuffer,
            MMultimediaDataSourceObserver* aObserver,
            MDataSink* aConsumer,TBool aTop);

        TInt ServiceFillBuffer();
    private:
        
        // New Functions
        const TDesC& FileName() const ;
        const TDesC& Extension() const ;
        const TDesC& FilePath() const ;
        const TDesC& FileDrive() const ;
        const TFileName FullName() const;
        RFile& FileL() ;
        
    private:
        class CProgDLMultimediaSourceAsyncEventHandler : public MAsyncEventHandler
            {
            public:
                CProgDLMultimediaSourceAsyncEventHandler(CProgDLMultimediaSource* aParent);
                virtual ~CProgDLMultimediaSourceAsyncEventHandler();
            public:
                virtual TInt SendEventToClient(const TMMFEvent& aEvent);
            private:
                CProgDLMultimediaSource* iParent;
            };
        
        class CCAFParameters : public CBase
            {
            public:
                CCAFParameters() : iUniqueId(NULL), iEnableUI(EFalse) {}
                ~CCAFParameters() {	delete iUniqueId;	}
            public:
                HBufC* iUniqueId;
                TBool iEnableUI;
            };
        
        void GetAudioConfiguration();
        TInt ReOpenCAF();
        
    private:
        MGenericFile* iFile;
        RFs iFsSession;
        
        TBool iFileHandle;
        RFile iHandle;
        TBool iReserved;
        
        TFileName iFullFileName ;
        HBufC* iFileName ;
        HBufC* iFileExt ;
        HBufC* iFilePath ;
        HBufC* iFileDrive ;
        CCAFParameters* iCAFParameters;
        
        
        
        
        
        TFourCC  iSinkFourCC ;
        TFourCC  iSourceFourCC ;
        
        // Queue of buffers from controller
        TSglQue<CSinkQueueItem>* iSinkQueue;
        //MDataSink* iConsumer;
        // Sink item Counter
        TInt iSnkItemsCount;
        // Observer bit rate in bps
        //Keeps track of the read position within the file. Needed in order to enable passing of the
        //read position to the CReadRequest to manage the LastBuffer flag.
        //NB: Continuous reads once EOF has been reached will result in iPosition going > iDLFileSize,
        //but this causes no ill effects
        TUint iSnkBytes;
        
        TInt iDownloadSize;
        
        // Transfer bit rate in bps
        TUint   iTransferRate;  
        
        TUint   iObserverBitRate;
        
        // Buffering size in bytes
        TUint   iBufferedDataSize;
        
        TInt iDLFileSize; //holds the cached file size
        TMMFMessage* iMessage;
        
        MMultimediaDataSourceObserver* iObserver;
        RPointerArray<CReadWriteRequest> iRequests;
        
        MAsyncEventHandler *iEventHandler;
        
        RPointerArray<CTransferBufferCopy> iTransferBufferCopies;
        TBool   isDownloadComplete;
        
        TBool iReOpenCAF;
        
        TInt iFileSize;

        // Pointer to the Preset Repository
		CRepository* iAudioConfigRepository;
        TBufferingConfig iPrimaryBufferingConfig;
        TBufferingConfig iSecondaryBufferingConfig;
        TUint iInitialBufferingPercentage;
        TInt iReadRequestPending;
        MDataSource* iParentDataSource;
        HBufC8* iInitData;           
    };
    
#endif
