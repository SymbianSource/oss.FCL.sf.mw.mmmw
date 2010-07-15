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
* Description:  Implementation of ProgDLSource.
*
*/


#include <f32file.h>
#include <e32std.h>
#include <mmfdatabuffer.h>
#include <mmfutilities.h>
#include <mmf/common/mmfcontroller.h>
#include <mmfpaniccodes.h>
#include "ProgDLDataSource.h"
#include "ProgDLMultimediaSource.h"
#include "MmffilePriv.h"
#include "FileAccess.h"
#include "SinkQueueItem.h"
#include "ProgDLSourceCustomCommands.h"
#include <MultimediaDataSourceEvents.h>
#include "ProgDLSourceUid.h"
#include "AudioConfigurationPrivateCRKeys.h"

#ifdef _DEBUG
#define DEBPRN1(str)        RDebug::Print(str);
#define DEBPRN2(str, val1)   RDebug::Print(str, val1);
#define DEBPRN3(str, val1, val2)   RDebug::Print(str, val1, val2);
#define DEBPRN4(str, val1, val2, val3)   RDebug::Print(str, val1, val2, val3);
#define DEBPRN5(str, val1, val2, val3, val4)   RDebug::Print(str, val1, val2, val3, val4);
#else
#define DEBPRN1(str)
#define DEBPRN2(str, val1)
#define DEBPRN3(str, val1, val2)
#define DEBPRN4(str, val1, val2, val3)
#define DEBPRN5(str, val1, val2, val3, val4)
#endif // _DEBUG

const TUint KMinPlaybackDuration = 15; // 15 second

void Panic(TMMFFilePanicCode aPanicCode)
    {
    _LIT(KMMFFilePanicCategory, "MMFFile");
    User::Panic(KMMFFilePanicCategory, aPanicCode);
    }

    /**
    * Constructs a CTransferBufferCopy
    *
    * @return CTransferBufferCopy*
*/
CTransferBufferCopy* CTransferBufferCopy::NewL(TInt aMaxLength)
    {
    CTransferBufferCopy* self = new (ELeave) CTransferBufferCopy(aMaxLength);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

    /**
    * Second phase constructor for CTransferBufferCopy
    *
    * @return void
*/
void CTransferBufferCopy::ConstructL()
    {
    iBuffer = static_cast<TUint8*>(User::AllocL(iMaxLength));
    iBufferDes.Set(iBuffer,0,iMaxLength);
    }

/************************/
CProgDLSource* CProgDLSource::NewL(TUid aType )
    {
    DEBPRN1(_L("CProgDLSource::NewL"));
    CProgDLSource* self = new (ELeave) CProgDLSource(aType);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CProgDLSource::CProgDLSource(TUid aType)
: MDataSource(aType)
    {
    iInitData = NULL;
    }

CProgDLSource::~CProgDLSource(void)
    {
    delete iInitData;
    iInitData = NULL;        
    }

void CProgDLSource::ConstructL (void)
    {
    }

// From MDataSource begins
TUid CProgDLSource::DataSourceType() const
    {
    return KMmfProgDLSource;
    }


TFourCC CProgDLSource::SourceDataTypeCode(TMediaId /*aMediaId*/ )
    {
    TFourCC fourCC;        
    return iMultiMediaSource->GetDataTypeCode(fourCC);
    }

TInt CProgDLSource::SetSourceDataTypeCode(TFourCC aSourceFourCC,
                                                  TMediaId /*aMediaId*/ )
    {
    iMultiMediaSource->SetDataTypeCode(aSourceFourCC);
    return KErrNone;
    }

void CProgDLSource::FillBufferL(CMMFBuffer* aBuffer,
                                        MDataSink* aConsumer,
                                        TMediaId aMediaId )
    {
    iMultiMediaSource->FillBufferL(aBuffer,aConsumer,aMediaId);
    }

void CProgDLSource::BufferEmptiedL(CMMFBuffer* /*aBuffer*/ )
    {
    User::Leave(KErrUnknown);
    }

TBool CProgDLSource::CanCreateSourceBuffer()
    {
    return EFalse;
    }

CMMFBuffer* CProgDLSource::CreateSourceBufferL(TMediaId /*aMediaId*/,
                                                       TBool &/*aReference*/ )
    {
    return NULL;
    }

TInt CProgDLSource::SourceThreadLogon( MAsyncEventHandler& /*aEventHandler*/ )
    {
    return iMultiMediaSource->Open();
    }

void CProgDLSource::SourceThreadLogoff()
    {
    iMultiMediaSource->Close();
    }

void CProgDLSource::SourcePrimeL()
    {
    DEBPRN1(_L("CProgDLSource::SourcePrimeL"));
    User::LeaveIfError(iMultiMediaSource->Prime());
    }

void CProgDLSource::SourcePlayL()
    {
    DEBPRN1(_L("CProgDLSource::SourcePlayL"));
    User::LeaveIfError(iMultiMediaSource->Play());
    }

void CProgDLSource::SourceStopL()
    {
    DEBPRN1(_L("CProgDLSource::SourceStopL"));
    User::LeaveIfError(iMultiMediaSource->Stop());
    }


void CProgDLSource::ConstructSourceL(const TDesC8& aInitData )
    {
    if(iInitData)
        {
        delete iInitData;
        iInitData = NULL;        
        }
    iInitData = aInitData.AllocL();        
    }

void CProgDLSource::SourceCustomCommand(TMMFMessage& aMessage)
    {
    iMultiMediaSource->SourceCustomCommand(aMessage);        
    }

void CProgDLSource::SetMultimediaSource(CProgDLMultimediaSource& aMultimediaSource)
    {
    iMultiMediaSource = &aMultimediaSource;   
    }

TDesC8& CProgDLSource::GetInitData()
    {
    return *iInitData;    
    }
/****************************/


EXPORT_C CProgDLMultimediaSource* CProgDLMultimediaSource::NewL(MDataSource& aDataSource)
    {
    DEBPRN1(_L("CProgDLMultimediaSource::NewL"));
    CProgDLMultimediaSource* self = new (ELeave) CProgDLMultimediaSource(aDataSource);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

    /**
    Destructor.
*/
CProgDLMultimediaSource::~CProgDLMultimediaSource() 
    {
    DEBPRN1(_L("CProgDLMultimediaSource::~CProgDLMultimediaSource() "));    
    delete iInitData;
    delete iFile;
    EmptySinkQueue();
    iHandle.Close();
    iFsSession.Close();
    delete iFileName;
    delete iFileExt;
    delete iFilePath;
    delete iFileDrive;
    
    
    delete iCAFParameters;
    delete iSinkQueue;
    delete iAudioConfigRepository;
    
    if(iMessage)
        {
        if(!iMessage->IsCompleted())
            {
            iMessage->Complete(KErrDied);
            delete iMessage;
            iMessage = NULL;
            }
        }
    // Get rid of everything in RArray's & close them.
    iRequests.ResetAndDestroy();
    iTransferBufferCopies.ResetAndDestroy();
    DEBPRN1(_L("CProgDLMultimediaSource::~CProgDLMultimediaSource() exit"));   
    }

    /**
    Protected constructor.
    
      The default implementation is empty.
*/
CProgDLMultimediaSource::CProgDLMultimediaSource(MDataSource& aDataSource)
                                       : iSnkItemsCount(0),
                                       iSnkBytes(0),
                                       iDownloadSize(0),
                                       iTransferRate(0),
                                       //iConsumer(NULL),
                                       iBufferedDataSize(0),
                                       iDLFileSize(0),
                                       isDownloadComplete(EFalse),
                                       iFileSize(0),
                                       iParentDataSource(&aDataSource)
    {
    iMessage = NULL;
    iFile = NULL;
    iState = ECLOSED;
    iObserver = NULL;
    iReadRequestPending = 0;
    iReOpenCAF = EFalse;
    iObserverBitRate = 0;
    }

    /**
    Constructs an CProgDLMultimediaSource MDataSource.
    
      @return A pointer to the new CProgDLMultimediaSource data source.
*/
/*MDataSource* CProgDLMultimediaSource::NewSourceL() 
{
CProgDLMultimediaSource* self = new (ELeave) CProgDLMultimediaSource( KMmfProgDLSource ) ;
return STATIC_CAST( MDataSource*, self ) ;
    }*/


    /**
    Perform source construction dependant on the source construction
    initialisation data aInitData.
    
      @param  aInitData
      The TPckg<TMMFFileParams> descriptor package containing the file name and full path.
*/
void CProgDLMultimediaSource::ConstructL() 
    {
    CProgDLSource* progDLSource = static_cast<CProgDLSource*>(iParentDataSource);    
    iInitData = (progDLSource->GetInitData()).AllocL();
    progDLSource->SetMultimediaSource(*this);
    ConstructL(*iInitData, ESourceMode);
    iSinkQueue = new(ELeave) TSglQue<CSinkQueueItem>(_FOFF(CSinkQueueItem, iLink));
    }


    /**
    Protected constructor.
    
      Extracts the initialisation data provided by the calling functions: ConstructSourceL() and 
      ConstructSinkL(). Creates a file server session and sets up file name. If there is a file name and 
      it cannot be found this function leaves. If there is no file name the function leaves. Does not 
      attempt to open the file or check whether the file exists.
      
        If aInitData contains a TMMFFileHandleParams instead of TMMFFileParams, the source/sink is constructed from 
        the file handle provided by the caller
        
          @param  aInitData
          Initialisation data packaged in a TMMFFileParams or in a TMMFFileHandleParams (File Handle)
*/
void CProgDLMultimediaSource::ConstructL(const TDesC8& aInitData,TMMFileMode aFileMode)
    {
    User::LeaveIfError(iFsSession.Connect());
    // on IPCv2 we auto attach
    User::LeaveIfError(iFsSession.ShareAuto());
    
    User::LeaveIfError(iFsSession.ShareProtected());
    
    TBool fileInit = EFalse;
    HBufC* filename = NULL; 
    TBool filenamePushed = EFalse;
    
    iCAFParameters = new (ELeave) CCAFParameters;
    TBool drmContent = EFalse;
    RDesReadStream stream(aInitData);
    CleanupClosePushL(stream);

    iAudioConfigRepository = CRepository::NewL(KCRUidAudioConfiguration);

    // From Central Repository
    GetAudioConfiguration();
    
    TUid initUid;
    
    initUid = TUid::Uid(stream.ReadInt32L());
    
    if (initUid == KMMFileHandleSourceUid)
        {
        TPckgBuf<RFile*> fileptr;
        stream.ReadL(fileptr);
        
        iHandle.Duplicate(*fileptr());
        
        TInt length;
        length = stream.ReadInt32L();
        if (length>0)
            {
            iCAFParameters->iUniqueId = HBufC::NewL(length);
            TPtr16 ptr = iCAFParameters->iUniqueId->Des();
            stream.ReadL(ptr, length);
            }
        iFileHandle = ETrue;
        filename = HBufC::NewMaxL(KMaxFileName);
        TPtr ptr = filename->Des();
        iHandle.Name(ptr);
        fileInit = ETrue;
        drmContent = ETrue;
        
        iCAFParameters->iEnableUI = stream.ReadInt32L();
        }
    
    else if (initUid == KMMFileSourceUid)
        {
        TInt length;
        length = stream.ReadInt32L();
        filename = HBufC::NewMaxLC(length);
        TPtr ptr = filename->Des();
        stream.ReadL(ptr, length);
        
        length = stream.ReadInt32L();
        if (length>0)
            {
            iCAFParameters->iUniqueId = HBufC::NewMaxL(length);
            ptr.Set(iCAFParameters->iUniqueId->Des());
            stream.ReadL(ptr, length);
            }
        CleanupStack::Pop(filename);
        
        fileInit = ETrue;
        drmContent = ETrue;
        iFileHandle = EFalse; 
        iCAFParameters->iEnableUI = stream.ReadInt32L();
        }
    else
        {
        //		TODO If the UID is unknown we should reject, but  currently
        //		code also used for older calls that just supply filename.
        //		User::Leave(KErrNotSupported);
        }
    
    CleanupStack::PopAndDestroy(&stream);
    
    if (!fileInit && aInitData.Length() == sizeof(TMMFFileHandleParams))
        {
        TMMFFileHandleParams params;
        TPckgC<TMMFFileHandleParams> config(params);
        config.Set(aInitData);
        params = config();
        
        
        if (params.iUid == KFileHandleUid)
            {
            fileInit = ETrue;
            User::LeaveIfError(iHandle.Duplicate(*params.iFile));
            TInt pos = 0;
            // make sure the duplicate handle is at the start of the file - the usage of the file handle really requires this
            User::LeaveIfError(iHandle.Seek(ESeekStart, pos));
            iFileHandle = ETrue;
            filename = HBufC::NewMaxLC(KMaxFileName);
            filenamePushed = ETrue;
            TPtr ptr = filename->Des();
            User::LeaveIfError(iHandle.Name(ptr));
            }
        }
    
    if (!fileInit) // do old case as last resort
        {
        TMMFFileParams params;
        TPckgC<TMMFFileParams> config(params);
        config.Set(aInitData);
        params = config();
        
        filename = params.iPath.AllocL();
        fileInit = ETrue;
        }
    
    if (!filenamePushed)
        {
        // from now on it is assumed pushed.
        CleanupStack::PushL(filename);
        }
    
    TParse parser ;
    User::LeaveIfError(parser.Set(*filename, NULL, NULL));
    CleanupStack::PopAndDestroy(filename);
    if ( !( parser.NamePresent() ) && !( parser.ExtPresent() ) )
        User::Leave( KErrBadName ) ;
    
    iFullFileName.Copy( parser.FullName() ) ;	
    iFileName = parser.Name().AllocL() ;
    iFileExt = parser.Ext().AllocL() ;
    iFilePath = parser.Path().AllocL() ;
    iFileDrive = parser.Drive().AllocL() ;
    
    // in order to simulate old behaviour we are not passing error out
    // but will try to create Content again during PrimeL()
    if (fileInit && drmContent && aFileMode==ESourceMode)
        {
        TInt contentError;
        if (iFileHandle)
            {
            TRAP(contentError, 
                iFile = CContentFile::NewL(iHandle, UniqueId(), iCAFParameters->iEnableUI);
            );
            }
        else
            {
            // Open for read-only access
            TRAP(contentError,
                iFile = CContentFile::NewL(iFsSession, iFullFileName, UniqueId(), EFileShareAny, iCAFParameters->iEnableUI);
            );
            }
        }
    }
    
    
/**
@deprecated

Returns an RFile handle to the current file.

If there is no current file, one is created. If the file exists then it is opened with read access 
if it is read only, write access otherwise. If the file does not exist then it is opened with
write access.

@leave KErrNotReady
The file is not open.

@return A handle to the current file.
*/
RFile& CProgDLMultimediaSource::FileL()
    {
    if (!iFile)
        User::Leave(KErrNotReady);
    if (iFileHandle)
        return iHandle;
    else
        return iFile->FileL();
    }
            
/** 
Returns the file name of the current file.

  Note: This will give the wrong answer if the file is renamed!
  
    @return The FileName (without extension).
*/
const TDesC& CProgDLMultimediaSource::FileName() const
    {
    return *iFileName ;
    }
            
/**
Returns the extension of the current file.

Note: This will give the wrong answer if the file is renamed!

@return The File Extension.
*/
const TDesC& CProgDLMultimediaSource::Extension() const 
    {
    return *iFileExt ;
    }
            
/** 
Returns the path of the current file.

Note: This will give the wrong answer if the file is renamed!

@return The FilePath (without filename and extension)
*/
const TDesC& CProgDLMultimediaSource::FilePath() const 
    {
    return *iFilePath ;
    }

/** 
Returns the drive on which the current file is located.

Note: This will give the wrong answer if the file is renamed!

@return The FileDrive (drive letter only, without path, filename and extension).
*/
const TDesC& CProgDLMultimediaSource::FileDrive() const 
    {
    return *iFileDrive ;
    }

/** 
Returns the full name of the current file.

Note: This will give the wrong answer if the file is renamed!

@return The file name (full filename including drive letter, without path, filename and extension).
*/
const TFileName CProgDLMultimediaSource::FullName() const
    {
    return iFullFileName;
    }
            
/** 
Returns the uniqueID associated with this content. If no uniqueID has been provided, a null
descriptor will be provided

@return The UniqueID
*/
const TDesC& CProgDLMultimediaSource::UniqueId() const
    {
    if (iCAFParameters->iUniqueId)
        return *(iCAFParameters->iUniqueId);
    else
        return KNullDesC;
    }
            
/**
Obtains a CTransferBufferCopy from iTransferBufferCopies that is
at least as big as that required.

There is no need to put the pointer returned by this method onto the CleanupStack
as it will have already been placed into iTransferBufferCopies.

@param  aMaxLength
The size required.

@return A pointer to a valid CTransferBufferCopy.
*/
CTransferBufferCopy* CProgDLMultimediaSource::ObtainCopyOfTransferBufferL(TInt aMaxLength)
    {
    //find a free transfer buffer copy of the right size
    TInt firstFree = -1;
    CTransferBufferCopy* transBufCopyToUse = NULL;
    
    for(TInt cnt=0; cnt < iTransferBufferCopies.Count(); cnt++)
        {
        if(!iTransferBufferCopies[cnt]->InUse())
            {
            //record the first free entry, we may remove this
            //if entries in iTransferBufferCopies > KAcceptableTransferBufferCopiesSize
            if(firstFree == -1) 
                firstFree = cnt;
            
            if(iTransferBufferCopies[cnt]->MaxLength() >= aMaxLength)
                {
                transBufCopyToUse = iTransferBufferCopies[cnt];
                
                //Set the MaxLength. This will ensure that the copy acts the same as
                //the original Transfer buffer, eg. file server will throw KErrOverflow
                transBufCopyToUse->ReUse(aMaxLength);
                break;
                }
            }
        }
    
    //If we failed to find a suitable entry, we need to create a new one
    if(!transBufCopyToUse)
        {
        //Firstly, should we re-cycle an existing entry?
        //There must be entries in the array, a free entry must have been found,
        //the size of the array must be beyond the water mark where we want to start
        //cycling free entries.
        if((iTransferBufferCopies.Count() > 0) &&
            (firstFree != -1) &&
            (iTransferBufferCopies.Count() > KAcceptableTransferBufferCopiesSize))
            {
            delete iTransferBufferCopies[firstFree];
            iTransferBufferCopies.Remove(firstFree);
            
            transBufCopyToUse = CTransferBufferCopy::NewL(aMaxLength);
            CleanupStack::PushL(transBufCopyToUse);
            User::LeaveIfError(iTransferBufferCopies.Insert(transBufCopyToUse,firstFree));
            
            CleanupStack::Pop();
            }
        else
            {
    #ifdef _DEBUG
            if(iTransferBufferCopies.Count() > KMaximumTransferBufferCopiesSize)
                {
                User::Panic(_L("iTransferBufferCopies grew too large in CProgDLMultimediaSource"),KErrTooBig);
                }
    #endif
            
            transBufCopyToUse = CTransferBufferCopy::NewL(aMaxLength);
            CleanupStack::PushL(transBufCopyToUse);
            User::LeaveIfError(iTransferBufferCopies.Append(transBufCopyToUse));
            
            CleanupStack::Pop();
            }
        }
    
    return transBufCopyToUse;
    }
            
            
            
/** 
Loads aBuffer from iFile.

The file must already be open for reading. File read is asynchronous. CReadRequest is created to 
respond to completion.

@param  aBuffer
The buffer to be filled from the file.
@param  aConsumer
The data sink consumer of the buffer.
*/
void CProgDLMultimediaSource::FillBufferL( CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /*aMediaId*/ )
    {
    // Requires that iFile is open for read.
    // Reads data from iFile into aBuffer
    TInt status(KErrNone);
    DEBPRN2(_L("CProgDLMultimediaSource::FillBuffer[%x]]"), aBuffer );
    if ((!aConsumer) || (!aBuffer))
        User::Leave(KErrArgument);
    
    if (!iFile)
        User::Leave(KErrNotReady);
    
    AppendBufferToSinkQueue(aBuffer,NULL,aConsumer,EFalse);
    DEBPRN4(_L("CProgDLMultimediaSource::FillBuffer DLComplete[%d] ReOpenCAF[%d] RequestPending[%d]"), isDownloadComplete,iReOpenCAF,iReadRequestPending );
    if(isDownloadComplete && !iReOpenCAF) 
        {
        if(!iReadRequestPending)
            {
            ReOpenCAF();
            status = ServiceFillBuffer();
            if(status == KErrUnderflow)
                {
                StateChanged(EBUFFERING);
                }
            }
        }
    else
        {
        status = ServiceFillBuffer();
        
        if(status == KErrUnderflow)
            {
            StateChanged(EBUFFERING);
            }
        }        
    }
            
/**
Stores a request in an array.

CReadWriteRequests are stored in the array iRequests.
This function takes ownership and places the request in the array.
It also checks the array for completed requests and removes them.

@param  aRequest
The request to store.
*/
void CProgDLMultimediaSource::StoreRequestL( CReadWriteRequest* aRequest )
    {
    // add aRequest to iRequests
    User::LeaveIfError( iRequests.Append( aRequest ) ) ;
    
    // Clear out any completed requests
    for ( TInt ii = 0 ; ii < iRequests.Count() ; ii++ )
        {
        if (iRequests[ii]->Completed())
            {
            CReadWriteRequest* request = iRequests[ii];
            delete request;
            
            iRequests.Remove(ii);
            ii--;
            }
        }
    }


/**
Cancels outstanding requests.

CReadWriteRequests are stored in the array iRequests.
This function cancels any outstanding requests and removes them
from iRequests.
*/
void CProgDLMultimediaSource::CancelRequests()
    {
    // Clear out any completed requests
    for ( TInt ii = 0 ; ii < iRequests.Count() ; ii++ )
        {
        CReadWriteRequest* request = iRequests[ii];
        if(!request->Processing())
            {
            delete request;
            iRequests.Remove(ii);
            ii--;
            }
        }
    }

            
/**
Evaluates a given intent against the rights associated with the file.

The rights are not updated by this function call.

@param  aIntent
The intent to evaluate.

  @return An error code indicating if the function call was successful. KErrNone on success, otherwise
  another of the system-wide error codes.
*/
TInt CProgDLMultimediaSource::EvaluateIntent(ContentAccess::TIntent aIntent) const
    {
    if (!iFile)
        {
        return KErrNotReady;
        }
    
    return iFile->EvaluateIntent(aIntent);
    }
            
/**
Evaluates and executes a given intent against the rights associated with the file.

The rights object is updated after calling this function.

@param  aIntent
The intent to evaluate.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
another of the system-wide error codes.
*/
TInt CProgDLMultimediaSource::ExecuteIntent(ContentAccess::TIntent aIntent)
    {
    if (!iFile)
        {
        return KErrNotReady;
        }
    
    return iFile->ExecuteIntent(aIntent);
    }
            
/**
Returns whether the file is protected.

@return A boolean indicating if the file is protected. ETrue if the file is protected.
*/
TInt CProgDLMultimediaSource::GetDRMProtection(TBool& aIsProtected)
    {
    TInt err(KErrNone);	
    if (!iFile)
        {
        return KErrNotReady;
        }
    
    TRAP(err,aIsProtected = iFile->IsProtected());
    return err;
    }
            
TInt CProgDLMultimediaSource::SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
    {
    if (!iFile)
        {
        return KErrNotReady;
        }
    
    return iFile->SetAgentProperty(aProperty, aValue);
    }
            

TBool CReadWriteRequest::SourceType() 
    {
    return iSourceType ;
    }


/*
*	Returns the data member of CMMFDataBuffer or CMMFTransferBuffer (as TPtr8)
*  
*/
TDes8& CReadWriteRequest::BufferDes()
    {
    if(iTransferBufferCopy)
        return iTransferBufferCopy->Des();
    else
        {
        //reset iBufferDes in case iBuffer has changed...
        iBufferDes = &(STATIC_CAST(CMMFDataBuffer*, iBuffer)->Data());
        return *iBufferDes;
        }
    }

const TDesC8& CReadWriteRequest::BufferDesC()
    {
    if(iTransferBufferCopy)
        return iTransferBufferCopy->Des();
    else
        return BufferDes();
    }

CMMFBuffer* CReadWriteRequest::Buffer()
    {
    return iBuffer;
    }

TAny* CReadWriteRequest::GetSinkOrSource()
    {
    return iSinkOrSource;
    }

/*
*	Destructor.
*/
CReadWriteRequest::~CReadWriteRequest() 
    {
    Cancel();
    if(iTransferBufferCopy)
        iTransferBufferCopy->SetInUse(EFalse);
    }

/*
*	Allows owning class access to SetActive()
*/
void CReadWriteRequest::SetActive() 
    {
    CActive::SetActive() ;
    }
        
/*
*  For the moment at least...    Canceled requests may be deleted
*/
void CReadWriteRequest::DoCancel() 
    {
    iState = ECompleted;
    }
            
/*
*	Called when errors in RunL force Leave.  For the moment just mark the request deletable
*/
TInt CReadWriteRequest::RunError( TInt aError ) 
    {
    //RunL can leave.
    iState = ECompleted;
    iError = aError; //keep this error internally for now
    return KErrNone ;
    }
        
/*
*	On completion of read request call back to the MDataSink
*/
void CReadRequest::RunL() 
    {
    iState = EProcessing;
    //Copy the data from the normal buffer into the Transfer buffer
    if(iTransferBufferCopy)
        {
        //must specify the size here as the dest may be smaller than the source.
        TDes8& destDesc = STATIC_CAST(CMMFDataBuffer*, iBuffer)->Data();
        destDesc.Copy(iTransferBufferCopy->Des().Left(destDesc.MaxLength()));
    
        iTransferBufferCopy->SetInUse(EFalse);
        }

        STATIC_CAST(CProgDLMultimediaSource*,iParent)->ReadRequestStatus(STATIC_CAST(CReadWriteRequest*,this),iStatus);
    iState = ECompleted;
    }

        // From CMultimediaDataSource begins
TInt CProgDLMultimediaSource::SetObserver( MMultimediaDataSourceObserver& aObserver )
    {
    TInt status(KErrNone);
    iObserver = &aObserver;
    return status;
    }
        
TInt CProgDLMultimediaSource::GetObserver( MMultimediaDataSourceObserver*& aObserver )
    {
    TInt status(KErrNone);
    aObserver = iObserver;
    return status;
    }
        
void CProgDLMultimediaSource::Event( TUid aEvent )
    {
    if( aEvent == KMultimediaDataSourceEventBitRateChanged )
        {
        
        if(iObserver)
            {
                iObserver->GetBitRate( iObserverBitRate ); 
                if(iState == EBUFFERING)
                    CalculateBufferedDataSize();
            }
        }    
    
    }
        
TInt CProgDLMultimediaSource::SetDataTypeCode(TFourCC aSourceFourCC )
    {
    TInt status(KErrNone);
    iSourceFourCC = aSourceFourCC;
    return status;
    }
        
TInt CProgDLMultimediaSource::GetDataTypeCode(TFourCC& aSourceFourCC )
    {
    TInt status(KErrNone);
    aSourceFourCC = iSourceFourCC;
    return status;
    }

TInt CProgDLMultimediaSource::GetSize( TUint& aSize )
    {
    TInt err = KErrNone;
    if (iDLFileSize >=0)
    	{
    	aSize = iDLFileSize;
    	}
    else
    	{
    	aSize = 0;
    	}
    return err;
    }

TInt CProgDLMultimediaSource::Open()
    {
    TInt status(KErrNotReady);
    DEBPRN1(_L("CProgDLMultimediaSource::Open"));
    switch ( iState )
        {
        case ECLOSED:
            StateChanged(ESTOPPED);
            status = KErrNone;
            break;
        case ESTOPPED:
        case EPRIMED:
        case EEXECUTING:
        case EBUFFERING:
        default:
            DEBPRN2(_L("CProgDLMultimediaSource::Open[Illegal cmd on state[%d]]"), iState );
            break;
        };
    return status;
    }

TInt CProgDLMultimediaSource::Close()
    {
    TInt status(KErrNone);
    iHandle.Close();
    iFsSession.Close();

    StateChanged(ECLOSED);
    // Clear app buffers
    EmptySinkQueue();
    iSnkBytes = 0;
    // Clear observer buffers
    return status;
    }

TInt CProgDLMultimediaSource::Prime()
    {
    TInt status(KErrNotReady);
    DEBPRN1(_L("CProgDLMultimediaSource::Prime"));
    switch ( iState )
        {
        case ESTOPPED:
            
            // don't reopen file if already open
            if (!iFile)
                {
                if (iFileHandle)
                    {
                    TRAPD(err,iFile = CContentFile::NewL(iHandle, UniqueId(), iCAFParameters->iEnableUI));
                    if(err)
                        return err;
                    }
                else
                    {
                    // Open for read-only access
                    TRAPD(err,iFile = CContentFile::NewL(iFsSession, iFullFileName, UniqueId(), EFileShareAny, iCAFParameters->iEnableUI));
                    if(err)
                        return err;                    
                    }
                }
            
            if(iFile)
                {
                if(iFile->IsProtected())
                    iFile->Size(iFileSize);
                else
                    iFileSize = iDLFileSize;		          
                }												
            
            StateChanged(EPRIMED);
            status = KErrNone;
            break;
        case EPRIMED:
            status = KErrNone;
            break;
        case ECLOSED:
        case EEXECUTING:
        case EBUFFERING:
        default:
            DEBPRN2(_L("CProgDLMultimediaSource::Prime[Illegal cmd on state[%d]]"), iState );
            break;
        }
    return status;
    }

TInt CProgDLMultimediaSource::Play()
    {
    TInt status(KErrNotReady);
    DEBPRN1(_L("CProgDLMultimediaSource::Play"));
    switch ( iState )
        {
        case EPRIMED:
            CalculateBufferedDataSize();
            StateChanged(EEXECUTING);
            
            while ( !iSinkQueue->IsEmpty() )
                {
                status = ServiceFillBuffer();
                }
            
            if(status == KErrUnderflow)
                {
                StateChanged(EBUFFERING);
                }
            
            status = KErrNone;	
            break;
        case EEXECUTING:
        case EBUFFERING:
            status = KErrNone;
            // No op
            break;
        case ECLOSED:
        case ESTOPPED:
        default:
            DEBPRN2(_L("CProgDLMultimediaSource::Play[Illegal cmd on state[%d]]"), iState );
            break;
        };
    return status;
    }
        
TInt CProgDLMultimediaSource::Stop()
    {
    TInt status(KErrNotReady);
    DEBPRN1(_L("CProgDLMultimediaSource::Stop"));
    switch ( iState )
        {
        case EPRIMED:
        case EEXECUTING:
        case EBUFFERING:
            {
            TInt pos = 0;
            CancelRequests();
			// we should not delete the iFile, it causes the CAF to delete the rights if they are expired.

            // Since the requests will not be deleted if it is still inside RunL() (iState is EProcessing), 
            // iReadRequestPending should not be initialized to 0 always
            iReadRequestPending = iRequests.Count();
            //iDLFileSize = -1;
            iSnkBytes=pos;
            iBufferedDataSize = 0;
            EmptySinkQueue();
            StateChanged(ESTOPPED);
            status = KErrNone;
            }
            break;
        case ECLOSED:
        default:
            DEBPRN2(_L("CProgDLMultimediaSource::Stop[Illegal cmd on state[%d]]"), iState );
            break;
        };
    return status;
    }
    
    
TInt CProgDLMultimediaSource::FillBuffer( CMMFBuffer* aBuffer )
    {
    // Requires that iFile is open for read.
    // Reads data from iFile into aBuffer
    
    TInt status(KErrNone);
    DEBPRN2(_L("CProgDLMultimediaSource::FillBuffer[%x]]"), aBuffer );
    if (!aBuffer)
        return KErrArgument;
    
    if (!iFile)
        return KErrNotReady;
    
    AppendBufferToSinkQueue(aBuffer,iObserver,NULL,EFalse);
    
    DEBPRN4(_L("CProgDLMultimediaSource::FillBuffer DLComplete[%d] ReOpenCAF[%d] RequestPending[%d]"), isDownloadComplete,iReOpenCAF,iReadRequestPending );
    if(isDownloadComplete && !iReOpenCAF) 
        {
        if(!iReadRequestPending)
            {
            ReOpenCAF();                        
            status = ServiceFillBuffer();
            if(status == KErrUnderflow)
                {
                StateChanged(EBUFFERING);
                }
            }
        }
    else
        {
        status = ServiceFillBuffer();
        
        if(status == KErrUnderflow)
            {
            StateChanged(EBUFFERING);
            }
        }        
    return status; 	
    }

    
TAny* CProgDLMultimediaSource::CustomInterface( TUid /*aInterfaceUid*/ )
    {
    return NULL;
    }
    
TInt CProgDLMultimediaSource::AppendBufferToSinkQueue( CMMFBuffer* aBuffer,
                                                    MMultimediaDataSourceObserver* aObserver,
                                                    MDataSink* aConsumer,TBool aTop )  
    {
    TInt status(KErrNone);
    
    DEBPRN2(_L("CProgDLMultimediaSource::AppendBufferToSinkQueue() Buffer[%x]"), aBuffer );				    
    // Add observer buffer to queue
    CMMFDataBuffer* dest = static_cast<CMMFDataBuffer*>( aBuffer );
    TDes8& destBufferDes = dest->Data();
    
    CSinkQueueItem* request(NULL);
    
    TRAP( status, request = CSinkQueueItem::NewL( aBuffer, aObserver,aConsumer) );
    if ( status == KErrNone )
        {
        if(aTop)
            iSinkQueue->AddFirst(*request);
        else  
            iSinkQueue->AddLast(*request);
        
        iSnkItemsCount++;
        
        DEBPRN3(_L("CProgDLMultimediaSource::AppendBufferToQueue[ReqSize[%d]SnkItems[%d]]"), \
            aBuffer->RequestSize(), iSnkItemsCount );
        }
    return status;
    }
    
void CProgDLMultimediaSource::SourceCustomCommand(TMMFMessage& aMessage)
    {
    TInt err(KErrNone);
    switch ( aMessage.Function() )
        {
        
        case ESetUpdateFileName:
            {
            // Read size info
            TPckgBuf<TFileName> sizePckg;
            err = aMessage.ReadData1FromClient(sizePckg);
            if ( err == KErrNone )
                {
                iFullFileName = sizePckg();
                }
            aMessage.Complete(KErrNone);			            
            
            }   
            break;

        case EDownloadRate:
            {
            
            // Read size info
            TPckgBuf<TUint> sizePckg;
            err = aMessage.ReadData1FromClient(sizePckg);
            if ( err == KErrNone )
                {
                iTransferRate = sizePckg();
                }
            aMessage.Complete(KErrNone);			            
            DEBPRN2(_L("CProgDLMultimediaSource::SourceCustomCommand EDownloadRate[%u] "),iTransferRate);
            if(iState == EBUFFERING /*&& !iSinkQueue->IsEmpty()*/)
                { 
                CalculateBufferedDataSize();
                }
            }   
            break;
        case ESetDownloadState:
            {
            isDownloadComplete = ETrue;    
            if(iFile)
                iFile->Size(iFileSize);
            
            if(iFileSize != iDLFileSize)
                iDLFileSize = iFileSize;
            
            iDownloadSize =  iFileSize;
            iBufferedDataSize = 0;
            aMessage.Complete(KErrNone);
    
            if(iObserver)
                iObserver->Event(KMultimediaDataSourceEventRandomSeekingSupportChanged);	
            
            if(iState == EBUFFERING )
                {
                if((iDownloadSize - iSnkBytes) > iBufferedDataSize)
                    {
                    StateChanged(EEXECUTING);
                    while ( !iSinkQueue->IsEmpty() )
                        {
                        err = ServiceFillBuffer();
                        if(err == KErrUnderflow)
                            {
                            StateChanged(EBUFFERING);
                            break;
                            }
                        }
                    }
                }
            }
            break;
            
        case ESetDownloadSize:
            {
            // Read size info
            TPckgBuf<TInt> sizePckg;
            err = aMessage.ReadData1FromClient(sizePckg);
            if ( err == KErrNone )
                {
                iDownloadSize = sizePckg();
                }
            aMessage.Complete(KErrNone);			
            TInt size = 0;

            if(iFile)
                iFile->Size(size);
            
            DEBPRN4(_L("CProgDLMultimediaSource::SourceCustomCommand ********BufferedDataSize[%u] DownloadSize[%d] iSnkbytes[%d]"), iBufferedDataSize ,iDownloadSize, iSnkBytes ); 		            
            DEBPRN4(_L("CProgDLMultimediaSource::SourceCustomCommand ********iDLFileSize[%d] iFileSize[%d]ActualSize[%d]"), iDLFileSize,iFileSize,size); 		            
            if(iState == EBUFFERING )
                {
                if((iDownloadSize - iSnkBytes) > iBufferedDataSize)
                    {
                    StateChanged(EEXECUTING);
                    while ( !iSinkQueue->IsEmpty() )
                        {
                        err = ServiceFillBuffer();
                        if(err == KErrUnderflow)
                            {
                            StateChanged(EBUFFERING);
                            break;
                            }
                        }
                    }
                }			
            }
            break;
                
        case ESetActualSize:
            {
            // Read size info
            TPckgBuf<TInt> sizePckg;
            err = aMessage.ReadData1FromClient(sizePckg);
            if ( err == KErrNone )
                {
                iDLFileSize = sizePckg();
                }
            aMessage.Complete(KErrNone);
            
            if(iFile)
                {
                if(iFile->IsProtected())
                    iFile->Size(iFileSize);
                else
                    iFileSize = iDLFileSize;		          
                }
            DEBPRN3(_L("CProgDLMultimediaSource::SourceCustomCommand ActualSize********iDLFileSize[%d] iFileSize[%d]"), iDLFileSize,iFileSize); 		            
            
            if(iObserver)
                iObserver->Event(KMultimediaDataSourceObserverEventSourceSizeChanged);
            
            }
            break;   
                
        case EGetSourceState:
            {
            TPckgBuf<TState> statePckg;	
            aMessage.ReadData1FromClient(statePckg);
            DEBPRN3(_L("CProgDLMultimediaSource::SourceCustomCommand() Client State[%d] SourceState[%d]"), statePckg(), iState);
            
            if(iState != statePckg())
                {
                statePckg() = iState;	
                aMessage.WriteDataToClient(statePckg);
                aMessage.Complete(KErrNone);
                }
            else
                iMessage = new(ELeave) TMMFMessage(aMessage);
             }   
            break;   

        case EGetBitRate:
            {
            TPckgBuf<TUint> bitRatePckg;	 
            DEBPRN2(_L("CProgDLMultimediaSource::SourceCustomCommand() Client BitRateFromS[%d]"),iObserverBitRate);
            bitRatePckg() = iObserverBitRate ;	
            aMessage.WriteDataToClient(bitRatePckg);
            aMessage.Complete(KErrNone);
            }            
            break;   

        case EGetPercentageBuffered:
            {
            TPckgBuf<TUint> perBufPckg; 
            TUint perBuf = 0;
            if(iState == EBUFFERING && iBufferedDataSize != 0)
                {
                CalculateBufferedDataSize();                    
                if ( iBufferedDataSize == 0 ) 
                    {
                    perBuf = 0; 
                    }
                else
                    {
                    perBuf = (iDownloadSize - iSnkBytes) * 100 / iBufferedDataSize;
                    }
                }
            
            RDebug::Print(_L(" Percentage Buffered [%d]"),perBuf);
            perBufPckg() = perBuf;
            aMessage.WriteDataToClient(perBufPckg);
            aMessage.Complete(KErrNone);
            }
            break;   
                
        default:
            err = KErrArgument;
            break;
        }
    }        

TInt CProgDLMultimediaSource::ServiceFillBuffer()
    {
    
    DEBPRN3(_L("CProgDLMultimediaSource::ServiceFillBuffer() state[%d] Download Size[%d]"), iState ,iDownloadSize );
    
    TInt status(KErrNone);
    if ( iSinkQueue->IsEmpty() )
        return KErrNone;
    
    CSinkQueueItem* tempsnkItem = iSinkQueue->First();
    CMMFBuffer* tempbuffer = tempsnkItem->Buffer();
    
    TInt bufSize = tempbuffer->RequestSize();
    
    DEBPRN5(_L("CProgDLMultimediaSource::ServiceFillBuffer() ********BufferedDataSize[%u] DownloadSize[%d] iSnkbytes[%d] BufSize[%d]"), iBufferedDataSize ,iDownloadSize, iSnkBytes,bufSize ); 		            				
    if((((iDownloadSize - iSnkBytes) > bufSize) && 
        iState == EEXECUTING && 
        iDownloadSize > iBufferedDataSize && ((iReadRequestPending + 1)*bufSize < (iDownloadSize - iSnkBytes))) || (iFileSize == iDownloadSize))
        {
        iReadRequestPending++;
        CSinkQueueItem* snkItem = iSinkQueue->First();
        iSinkQueue->Remove(*snkItem);
        iSnkItemsCount--;
        CMMFBuffer* buffer = snkItem->Buffer();
        DEBPRN2(_L("CProgDLMultimediaSource::ServiceFillBuffer() Buffer[%x]"), buffer );				    
        MMultimediaDataSourceObserver* observer = snkItem->Observer();
        MDataSink* consumer = snkItem->Consumer();
        delete snkItem;
        
        if (CMMFBuffer::IsSupportedDataBuffer(buffer->Type()))
            {
            CTransferBufferCopy* transBufCopy = NULL;
            CReadRequest* request = NULL;
            
            TDes8& bufferDes = STATIC_CAST( CMMFDataBuffer*, buffer )->Data();
            
            TInt requestSize;
            if(buffer->RequestSize())
                requestSize = buffer->RequestSize();
            else
                requestSize = bufferDes.MaxLength();
            
            //check whether buffer is safe to send to file server
            //if not, eg for a transfer buffer, then it needs to be copied
            if (!CMMFBuffer::IsFileServerSafe(buffer->Type()))
                {
                //NB: failure in this method will NOT cause transBufCopy to leak as it will be 
                //inserted into iTransferBufferCopies by ObtainCopyOfTransferBufferL.
                TRAP(status,transBufCopy = ObtainCopyOfTransferBufferL(bufferDes.MaxLength()));
                
                if(consumer && !observer)
                    request = new(ELeave) CReadRequest(this,STATIC_CAST(TAny*, consumer), buffer, transBufCopy, iSnkBytes, iDownloadSize, ETrue);
                else
                    request = new(ELeave) CReadRequest(this,STATIC_CAST(TAny*, observer), buffer, transBufCopy, iSnkBytes, iDownloadSize, EFalse);					
                
                }
            else
                {
                if(consumer && !observer)
                    request = new(ELeave) CReadRequest(this,STATIC_CAST(TAny*, consumer), buffer, iSnkBytes, iDownloadSize, ETrue);
                else
                    request = new(ELeave) CReadRequest(this,STATIC_CAST(TAny*, observer), buffer, iSnkBytes, iDownloadSize, EFalse);					
                }
            
            CleanupStack::PushL( request );
            
            TRAP_IGNORE(StoreRequestL(request)); // transfers ownership
            CleanupStack::Pop() ; // request
            
            request->SetActive();
            
            //	iSnkBytes += requestSize;
            
            if ((iSnkBytes + request->Buffer()->RequestSize()) >= iDownloadSize && isDownloadComplete)
                {
    	              request->Buffer()->SetLastBuffer(ETrue);
                      DEBPRN1(_L("CProgDLMultimediaSource::ServiceFillBuffer() LastBuffer$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"));
                }
            
            iFile->Read(request->BufferDes(), requestSize, request->iStatus);
            
            }
        else // if (CMMFBuffer::IsSupportedDataBuffer(buffer->Type()))
            return KErrNotSupported;
        }
        /*else if(iDownloadSize == iSnkBytes)
        {
        Stop();
    }*/
    else
        {
        
        //StateChanged(EBUFFERING);
        return KErrUnderflow;
        }
    return status;
    }

TInt CProgDLMultimediaSource::EmptySinkQueue()
    {
    TInt status(KErrNone);
    // Empty sink queue
    CSinkQueueItem* snkItem;
    while ( !iSinkQueue->IsEmpty() )
        {
        snkItem = iSinkQueue->First();
        iSinkQueue->Remove(*snkItem);
        delete snkItem;
        }
    
    iSnkItemsCount = 0;
    return status;
    }

void CProgDLMultimediaSource::CalculateBufferedDataSize()
    {
    DEBPRN1(_L("CProgDLMultimediaSource::CalculateBufferedDataSize()"));
    iBufferedDataSize = 0;
    
    // Get the bit rate from observer
    if(iObserver)
        iObserver->GetBitRate( iObserverBitRate );
    
    // If we don't know observer bit rate
    if(iDownloadSize == iDLFileSize)
        {
        iBufferedDataSize = 0;
        return;
        }   

    if(iObserverBitRate == 0 && (iSinkQueue->First() != NULL))
        {
            if(iPrimaryBufferingConfig.iType == TBufferingConfig::FIXEDDURATION
            || iPrimaryBufferingConfig.iType == TBufferingConfig::DYNAMICDURATION)
                {
                    if(iSecondaryBufferingConfig.iType == TBufferingConfig::FIXEDDURATION
                    || iSecondaryBufferingConfig.iType == TBufferingConfig::DYNAMICDURATION)
                        {
                        iBufferedDataSize =  iDLFileSize * iInitialBufferingPercentage / 100;       
                        }
                    else
                        {
                        if(iDLFileSize < iSecondaryBufferingConfig.iAmount)
                            {
                            iBufferedDataSize = iDLFileSize;
                            }
                        else
                            {
                            iBufferedDataSize = iSecondaryBufferingConfig.iAmount;
                            }
                        }    
                }
            else
                {
                if(iDLFileSize < iPrimaryBufferingConfig.iAmount)
                    {
                    iBufferedDataSize = iDLFileSize;
                    }
                else
                    {
                    iBufferedDataSize = iPrimaryBufferingConfig.iAmount;
                    }
                }                
            
        }
     else
        {
            if(iPrimaryBufferingConfig.iType == TBufferingConfig::FIXEDDURATION
            || iPrimaryBufferingConfig.iType == TBufferingConfig::DYNAMICDURATION)
                {
                TUint constantC = KMinPlaybackDuration * iObserverBitRate / 8;

                // calculate time remaining
                //TInt64 timeRemaining = ( iDLFileSize - iSnkBytes ) * 8 / iObserverBitRate;
                TUint bytesRemaining = iDLFileSize - iSnkBytes;
                iBufferedDataSize = bytesRemaining < constantC ? bytesRemaining : constantC;

//                timeRemaining = timeRemaining < 0 ? 0 : timeRemaining;

                // calculate the value in a temporary variable
//                TInt64 bufferedDataSize = ((TInt64(iObserverBitRate)) - (TInt64(iTransferRate))) * timeRemaining / 8;
//                bufferedDataSize = bufferedDataSize < constantC ? bufferedDataSize : constantC;

//                iBufferedDataSize = bufferedDataSize > KMaxTUint ? KMaxTUint : I64LOW(bufferedDataSize);
                }
            else
                {
                iBufferedDataSize = iPrimaryBufferingConfig.iAmount;
                }                
        }   

    DEBPRN5(_L("CProgDLMultimediaSource::CalculateBufferdDataSize[%u] ObserverBitRate[%d] SnkBytes[%u] TransferRate[%d]"), iBufferedDataSize,iObserverBitRate,iSnkBytes,iTransferRate );
    }

void CProgDLMultimediaSource::StateChanged(TState newState)
    {
    DEBPRN3(_L("CProgDLMultimediaSource::StateChanged OLD[%d] NEW[%d]"),iState, newState);        
    if(iState != newState)
        {
        if(iMessage)
            {
            if(!iMessage->IsCompleted())
                {
                TPckgBuf<TState> statePckg(newState);	
                iMessage->WriteDataToClient(statePckg);
                iMessage->Complete(KErrNone);
                delete iMessage;
                iMessage = NULL;
                }
            }
        iState = newState;
        }
    }

TInt CProgDLMultimediaSource::ReadRequestStatus(CReadWriteRequest* aRequest, TRequestStatus& aStatus)
    {
    
    if(aStatus != KErrNone)
        {
        TMMFEvent event(KMMFErrorCategoryControllerGeneralError, aStatus.Int());
        //TODO: Need to send this error back to client. Currently removing this as iEventHandler is never set.
        //iEventHandler->SendEventToClient(event);
        return KErrNone;
        }
    else
        {
        
        DEBPRN4(_L("CProgDLMultimediaSource::ReadRequestStatus Buffer[%x] BufferSize[%d] RequestSize[%d]"),aRequest->Buffer(),aRequest->Buffer()->BufferSize(),aRequest->Buffer()->RequestSize());
        DEBPRN3(_L("CProgDLMultimediaSource::ReadRequestStatus Buffer[%x] LastBuffer[%d]"),aRequest->Buffer(),aRequest->Buffer()->LastBuffer());
        
        TBool lastBuffer = aRequest->Buffer()->LastBuffer();
        
        if((aRequest->Buffer()->BufferSize() != aRequest->Buffer()->RequestSize()) && !lastBuffer)
            {
            if(aRequest->SourceType())
                {
                MDataSink* sinkOrSource = REINTERPRET_CAST(MDataSink*,aRequest->GetSinkOrSource());
                TUint position = aRequest->Buffer()->Position();
                TUint newPos = position - aRequest->Buffer()->BufferSize();
                aRequest->Buffer()->SetPosition(newPos);
                AppendBufferToSinkQueue(aRequest->Buffer(),(MMultimediaDataSourceObserver*)NULL,sinkOrSource,ETrue);
                //iSnkBytes -= aRequest->Buffer()->BufferSize();
                iFile->Seek(ESeekStart,iSnkBytes);
                StateChanged(EBUFFERING);
                }
            else
                {
                MMultimediaDataSourceObserver* sinkOrSource = REINTERPRET_CAST(MMultimediaDataSourceObserver*,aRequest->GetSinkOrSource());
                TInt position = aRequest->Buffer()->Position();
                TInt newPos = position - aRequest->Buffer()->BufferSize();
                aRequest->Buffer()->SetPosition(newPos);
                AppendBufferToSinkQueue(aRequest->Buffer(),sinkOrSource,(MDataSink*)NULL,ETrue);
                //iSnkBytes -= aRequest->Buffer()->BufferSize();
                iFile->Seek(ESeekStart,iSnkBytes);
                StateChanged(EBUFFERING);
                }
            }
        else
            {	
            iSnkBytes += aRequest->Buffer()->BufferSize();
            
            
            if(aRequest->SourceType())
                    REINTERPRET_CAST(MDataSink*, aRequest->GetSinkOrSource())->BufferFilledL(aRequest->Buffer()) ; // callback to MDataSource/Sink
            else
                    REINTERPRET_CAST(MMultimediaDataSourceObserver*, aRequest->GetSinkOrSource())->BufferFilled(aRequest->Buffer()) ; // callback to MDataSource/Sink			
                
                
            }
            
        iReadRequestPending--;
        DEBPRN4(_L("CProgDLMultimediaSource::ReadRequestStatus iReadRequestPending[%d] isDownloadComplete[%d] iReOpenCAF[%d]"),iReadRequestPending,isDownloadComplete,iReOpenCAF);        
        if(!iReadRequestPending && isDownloadComplete && !iReOpenCAF)
            {
            ReOpenCAF();
            while ( !iSinkQueue->IsEmpty() )
                {
                TInt err = ServiceFillBuffer();
                if(err == KErrUnderflow)
                    {
                    StateChanged(EBUFFERING);
                    break;
                    }
                }
            }
        
        return KErrNone;		
        }
    }


TInt CProgDLMultimediaSource::ReOpenCAF()
    {
    TInt status(KErrNone);
    DEBPRN2(_L("CProgDLMultimediaSource::StateChanged ReOpenCAF[%d]"),iSnkBytes);     
    delete iFile;
    iFile = NULL;
    
    if (!iFile)
        {
        if (iFileHandle)
            {
            TRAPD(status,iFile = CContentFile::NewL(iHandle, UniqueId(), iCAFParameters->iEnableUI));
            if(status)
                return status;
            }
        else
            {
            // Open for read-only access
            TRAPD(status,iFile = CContentFile::NewL(iFsSession, iFullFileName, UniqueId(), EFileShareAny, iCAFParameters->iEnableUI));
            if(status)
                return status;                    
            }
            
         if(!status)
            iFile->Seek(ESeekCurrent,iSnkBytes);   
        }
    
    if(iFile)
        {
        if(iFile->IsProtected())
            {
            iFile->Size(iFileSize);
            iDownloadSize = iFileSize;
            iDLFileSize = iFileSize;
            
			if(iObserver)
				{
				iObserver->Event(KMultimediaDataSourceObserverEventSourceSizeChanged);
				}
            }
        else
            {
			iFileSize = iDLFileSize;		          
            }
        }												

    iReOpenCAF = ETrue;
    DEBPRN3(_L("CProgDLMultimediaSource::StateChanged Exit status[%d] iFileSize[%d]"),status,iFileSize);     
    return status;    
    }

void CProgDLMultimediaSource::GetAudioConfiguration()
    {
    TInt bufType = 0;
    TInt bufAmount = 0;
    TInt bufPer = 0;
    
    iAudioConfigRepository->Get(KPDLPrimaryBufferingType,bufType);
    iAudioConfigRepository->Get(KPDLPrimaryBufferingAmount,bufAmount);
    
    iPrimaryBufferingConfig.iType = TBufferingConfig::TBufferingType(bufType);
    iPrimaryBufferingConfig.iAmount = bufAmount;
    
    iAudioConfigRepository->Get(KPDLSecondaryBufferingType,bufType);
    iAudioConfigRepository->Get(KPDLSecondaryBufferingAmount,bufAmount);
    
    iSecondaryBufferingConfig.iType = TBufferingConfig::TBufferingType(bufType);
    iSecondaryBufferingConfig.iAmount = bufAmount;
    
    iAudioConfigRepository->Get(KPDLInitialBufferingPercentage,bufPer);
    iInitialBufferingPercentage = bufPer;
    }

TInt CProgDLMultimediaSource::Seek(TUint aPosInBytes)
    {
    DEBPRN2(_L("CProgDLMultimediaSource::Seek[%d]]"), aPosInBytes );
    TInt status(KErrNone);
    if(aPosInBytes <= iFileSize)
        {
        iSnkBytes = aPosInBytes;
        status = iFile->Seek(ESeekStart,iSnkBytes);
        }
    else
        {
        status = KErrNotReady;        
        }
    return status;
    }

TInt CProgDLMultimediaSource::GetSeekingSupport( TBool& aSeekSupport )
    {
    aSeekSupport = ETrue;
    return KErrNone;
    }

TInt CProgDLMultimediaSource::GetRandomSeekingSupport( TBool& aSeekSupport )
    {
    aSeekSupport = ETrue;
    return KErrNone;
    }
//End of File

