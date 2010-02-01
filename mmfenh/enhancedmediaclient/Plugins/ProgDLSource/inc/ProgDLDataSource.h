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
* Description:  Header of ProgDLSource.
*
*/


#ifndef __PROGDLDATASOURCE_H_
#define __PROGDLDATASOURCE_H_


#include <mmf/server/mmfdatasource.h> 
#include <MultimediaDataSource.h> 
#include <DataBufferSource.h> 

class CSinkQueueItem;
using multimedia::TBufferingConfig;
/**
@publishedAll
@released

Provides the filename and path of file. 

Used by CProgDLMultimediaSource::ConstructL().
*/
class TMMFFileParams
    {
    public:
    /**
    Filename and path of file. 
        */
        TFileName iPath ; // Filename and path of file.
    private:
    /**
    This member is internal and not intended for use.
        */
        TInt iReserved1;
    };

    /**
    @internalTechnology	
*/
const TUid KFileHandleUid = {0x101FFA19};
const TUid KMMFileSourceUid = {0x101F7DA9};
const TUid KMMFileHandleSourceUid = {0x101F7DAA};

/**
@publishedAll
@released

  Provides a pointer to the file handle 
  
    Used by CProgDLMultimediaSource::ConstructL().
*/
class TMMFFileHandleParams
    {
    public:
        TMMFFileHandleParams(RFile* aFile) : iUid(KFileHandleUid), iFile(aFile) {}
        TMMFFileHandleParams() : iUid(KFileHandleUid) {}
        
        TUid iUid;
        /**
        Handle of file 
        */
        RFile* iFile;
    private:
    /**
    This member is internal and not intended for use.
        */
        TInt iReserved1;
    };




    /**
    @publishedAll
    @released
*/
typedef TPckgBuf<TMMFFileParams>  TMMFFileConfig ;

/**
@publishedAll
@released
*/
typedef TPckgBuf<TMMFFileHandleParams>  TMMFFileHandleConfig ;


/**
@publishedAll
@released

  This details how big the iTransferBufferCopies array of CProgDLMultimediaSource may
  become before we start looking to free entries.
*/
const TInt KAcceptableTransferBufferCopiesSize = 5;

/**
@publishedAll
@released

  This details how big the iTransferBufferCopies array of CProgDLMultimediaSource may
  become before we start looking to free entries.
*/
const TInt KMaximumTransferBufferCopiesSize = 100;

class CProgDLMultimediaSource;

class CProgDLSource : public MDataSource
    {
    public:
        static CProgDLSource* NewL( TUid aType );
		~CProgDLSource(void);        
        // From MDataSource begins
        TFourCC SourceDataTypeCode(TMediaId aMediaId) ;
        void FillBufferL( CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId ) ;
        void BufferEmptiedL( CMMFBuffer* aBuffer ) ;
        TBool CanCreateSourceBuffer() ;
        CMMFBuffer* CreateSourceBufferL( TMediaId aMediaId, TBool &aReference ) ;
        TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler) ;
        void SourceThreadLogoff();
        void SourcePrimeL();
        void SourcePlayL();
        void SourceStopL();
        void SourceCustomCommand(TMMFMessage& aMessage);
        void ConstructSourceL(const TDesC8 &aInitData);
        TUid DataSourceType() const;
        TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId /*aMediaId*/ );
         // From MDataSource ends
        
        void SetMultimediaSource(CProgDLMultimediaSource& aMultimediaSource);
        TDesC8& GetInitData();
    private:
        CProgDLSource(TUid aType);
        
        void ConstructL (void);
    private:
        //Data
        CProgDLMultimediaSource* iMultiMediaSource; 
        HBufC8* iInitData;       
 
    }; 
    
#endif
