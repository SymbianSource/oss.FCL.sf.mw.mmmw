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


#include <f32file.h>
#include <e32std.h>
#include <mmfdatabuffer.h>
#include <mmfutilities.h>
#include <mmf/common/mmfcontroller.h>
#include <mmfpaniccodes.h>
//#include "mmffile.h"

#include "MmffilePriv.h"
#include "FileAccess.h"


#include "ProgressiveDownloadSource.h"
#include <implementationproxy.h>

const TUid KUidProgressiveDlSource	= {KProgressiveDownloadSourceUid};


void Panic(TMMFFilePanicCode aPanicCode)
	{
 	_LIT(KMMFFilePanicCategory, "CProgressiveDownloadSource");
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

/**
Destructor.
*/
CProgressiveDownloadSource::~CProgressiveDownloadSource()
	{
	delete iFile;

	iHandle.Close();
	iFsSession.Close();
	delete iFileName;
	delete iFileExt;
	delete iFilePath;
	delete iFileDrive;
	delete iMmfFileEventHandler;
	delete iUniqueId;

	// Get rid of everything in RArray's & close them.
	iRequests.ResetAndDestroy();
	iTransferBufferCopies.ResetAndDestroy();
	}

/**
Protected constructor.

The default implementation is empty.
*/
CProgressiveDownloadSource::CProgressiveDownloadSource() : CMMFClip(KUidProgressiveDlSource/*KUidMmfFileSource*/, KUidMmfFileSink ), iFileSize(-1)
	{
	iSinkNotStopped = EFalse;
	}

/**
Constructs an CProgressiveDownloadSource MDataSource.

@return A pointer to the new CProgressiveDownloadSource data source.
*/
MDataSource* CProgressiveDownloadSource::NewSourceL()
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::NewSourceL"));
    #endif
	CProgressiveDownloadSource* self = new (ELeave) CProgressiveDownloadSource ;
	return STATIC_CAST( MDataSource*, self ) ;
	}

/**
Constructs a CProgressiveDownloadSource MDataSink

@return A pointer to the new CProgressiveDownloadSource data sink.
*/
MDataSink* CProgressiveDownloadSource::NewSinkL()
	{
	CProgressiveDownloadSource* self = new (ELeave) CProgressiveDownloadSource ;
	return STATIC_CAST( MDataSink*, self ) ;
	}

/**
Perform source construction dependant on the source construction
initialisation data aInitData.

@param  aInitData
        The TPckg<TMMFFileParams> descriptor package containing the file name and full path.
*/
void CProgressiveDownloadSource::ConstructSourceL(const TDesC8& aInitData )
	{
	ConstructL(aInitData, ESourceMode);
	}

/**
Performs sink construction dependant on the sink construction
initialisation data aInitData.

@param  aInitData
        The TPckg<TMMFFileParams> descriptor package containing the file name and full path.
*/
void CProgressiveDownloadSource::ConstructSinkL(const TDesC8& aInitData)
	{
	ConstructL(aInitData, ESinkMode);
	}

/**
Protected constructor.

Extracts the initialisation data provided by the calling functions: ConstructSourceL() and
ConstructSourceL(). Creates a file server session and sets up file name. If there is a file name and
it cannot be found this function leaves. If there is no file name the function leaves. Does not
attempt to open the file or check whether the file exists.

@param  aInitData
        Initialisation data packaged in a TMMFFileParams.
*/
void CProgressiveDownloadSource::ConstructL(const TDesC8& aInitData,TMMFileMode aFileMode)
	{
	User::LeaveIfError(iFsSession.Connect());
#ifdef __IPC_V2_PRESENT__
	// on IPCv2 we auto attach
	User::LeaveIfError(iFsSession.ShareAuto());
#else
	// on IPCv1
	we use explicit - more efficient
	User::LeaveIfError(iFsSession.Share(RSessionBase::EExplicitAttach));
#endif


	TBool fileInit = EFalse;
	HBufC* filename = NULL;
	TBool filenamePushed = EFalse;


	TBool drmContent = EFalse;
	RDesReadStream stream(aInitData);
	CleanupClosePushL(stream);

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
			iUniqueId = HBufC::NewL(length);
			TPtr16 ptr = iUniqueId->Des();
			stream.ReadL(ptr, length);
			}
		iFileHandle = ETrue;
		filename = HBufC::NewMaxL(KMaxFileName);
		TPtr ptr = filename->Des();
		iHandle.Name(ptr);
		fileInit = ETrue;
		drmContent = ETrue;
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
			iUniqueId = HBufC::NewMaxL(length);
			ptr.Set(iUniqueId->Des());
			stream.ReadL(ptr, length);
			}
		CleanupStack::Pop(filename);

		fileInit = ETrue;
		drmContent = ETrue;
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
				iFile = CContentFile::NewL(iHandle, UniqueId());
				);
			}
		else
			{
			// Open for read-only access
			//rj progressive download needs shared access
			TRAP(contentError,
				iFile = CContentFile::NewL(iFsSession, iFullFileName, UniqueId(), EFileShareAny);
			);
			}
		iFileOpen = (contentError==KErrNone);
		}

	(void)(aFileMode=ESourceMode); // prevent from compiler warning

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
RFile& CProgressiveDownloadSource::FileL()
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::FileL"));
    #endif
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
const TDesC& CProgressiveDownloadSource::FileName() const
	{
	return *iFileName ;
	}

/**
Returns the extension of the current file.

Note: This will give the wrong answer if the file is renamed!

@return The File Extension.
*/
const TDesC& CProgressiveDownloadSource::Extension() const
	{
	return *iFileExt ;
	}

/**
Returns the path of the current file.

Note: This will give the wrong answer if the file is renamed!

@return The FilePath (without filename and extension)
*/
const TDesC& CProgressiveDownloadSource::FilePath() const
	{
	return *iFilePath ;
	}

/**
Returns the drive on which the current file is located.

Note: This will give the wrong answer if the file is renamed!

@return The FileDrive (drive letter only, without path, filename and extension).
*/
const TDesC& CProgressiveDownloadSource::FileDrive() const
	{
	return *iFileDrive ;
	}

/**
Returns the full name of the current file.

Note: This will give the wrong answer if the file is renamed!

@return The file name (full filename including drive letter, without path, filename and extension).
*/
const TFileName CProgressiveDownloadSource::FullName() const
	{
	return iFullFileName;
	}


/**
Returns the uniqueID associated with this content. If no uniqueID has been provided, a null
descriptor will be provided

@return The UniqueID
*/
const TDesC& CProgressiveDownloadSource::UniqueId() const
	{
	if (iUniqueId)
		return *iUniqueId;
	else
		return KNullDesC;
	}



/**
Deletes the file.

Closes the currently open file, then deletes it. If the file source is accessing a file handle,
the file is truncated to 0 bytes instead.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CProgressiveDownloadSource::Delete()
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::Delete"));
    #endif
	if (!iFileHandle)
		{
		delete iFile;
		iFile = NULL;
		iFileSize=-1;
		iPosition=0;

		return iFsSession.Delete(iFullFileName);
		}
	else
		{
		iFileSize=-1;
		iPosition=0;

		return iFile->SetSize(0);
		}

	}

/**
Sets the file size.

@param  aSize
        The size of the file.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CProgressiveDownloadSource::SetSize(TInt aSize)
	{
	#if _DEBUG
	  RDebug::Print(_L("[%x]CProgressiveDownloadSource::SetSize to %d"),this,aSize);
    #endif

    iFix = aSize;

	if ( !iFile )
		return KErrNotReady;

	TInt err =  iFile->SetSize(aSize);
	if(err == KErrNone)
		iFileSize = aSize;
	else
		iFileSize = -1;

	return err;
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
CTransferBufferCopy* CProgressiveDownloadSource::ObtainCopyOfTransferBufferL(TInt aMaxLength)
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
				User::Panic(_L("iTransferBufferCopies grew too large in CProgressiveDownloadSource"),KErrTooBig);
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
void CProgressiveDownloadSource::FillBufferL( CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /*aMediaId*/ )
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::FillBufferL"));
    #endif
	// Requires that iFile is open for read.
	// Reads data from iFile into aBuffer
	if ((aConsumer == NULL) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (!iFile || (iMmfFileEventHandler == NULL))
		User::Leave(KErrNotReady);

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		CTransferBufferCopy* transBufCopy = NULL;
		CReadRequest* request = NULL;

		TDes8& aBufferDes = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data();

		TInt requestSize;
		if(aBuffer->RequestSize())
			requestSize = aBuffer->RequestSize();
		else
			requestSize = aBufferDes.MaxLength();

		//check whether buffer is safe to send to file server
		//if not, eg for a transfer buffer, then it needs to be copied
		if (!CMMFBuffer::IsFileServerSafe(aBuffer->Type()))
			{
			//NB: failure in this method will NOT cause transBufCopy to leak as it will be
			//inserted into iTransferBufferCopies by ObtainCopyOfTransferBufferL.
			transBufCopy = ObtainCopyOfTransferBufferL(aBufferDes.MaxLength());
			request = new(ELeave) CReadRequest(STATIC_CAST(TAny*, aConsumer), aBuffer, transBufCopy, iPosition, Size(),iBytesDownloaded, iMmfFileEventHandler);
			}
		else
			{
			request = new(ELeave) CReadRequest(STATIC_CAST(TAny*, aConsumer), aBuffer, iPosition, Size(),iBytesDownloaded, iMmfFileEventHandler);
			}

		CleanupStack::PushL( request );

		StoreRequestL(request); // transfers ownership
		CleanupStack::Pop() ; // request

		iFile->Read(request->BufferDes(), requestSize, request->iStatus);
		iPosition += requestSize;

		if (iPosition >= iFileSize)
			{
			aBuffer->SetLastBuffer(ETrue);
			}

		request->SetActive();
		}
	else // if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		User::Leave( KErrNotSupported ) ;
	}

/**
Empties aBuffer into iFile. The file must be already open for writing.

@param  aBuffer
        The buffer to be written to the file.
@param  aSupplier
        The data source supplier of the buffer.
*/
void CProgressiveDownloadSource::EmptyBufferL( CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/ )
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::EmptyBufferL"));
    #endif
	// Requires that iFile is open for write.
	// Writes data from iFile into aBuffer
	if ((aSupplier == NULL) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (!iFile || (iMmfFileEventHandler == NULL))
		User::Leave(KErrNotReady);

	CTransferBufferCopy* transBufCopy = NULL;

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		CWriteRequest* request = NULL;
		TDes8& aBufferDes = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data();

		//check whether buffer is safe to send to file server
		//if not, eg for a transfer buffer, then it needs to be copied
		if (!CMMFBuffer::IsFileServerSafe(aBuffer->Type()))
			{
			//Obtain a normal buffer to send to the file server
			//NB: failure in this method will NOT cause transBufCopy to leak as it will be
			//inserted into iTransferBufferCopies by ObtainCopyOfTransferBufferL.
			transBufCopy = ObtainCopyOfTransferBufferL(aBufferDes.MaxLength());

			//Copy the data into the buffer we will send to the file server
			transBufCopy->Des().Copy(aBufferDes);

			request = new(ELeave) CWriteRequest(STATIC_CAST(TAny*, aSupplier), aBuffer, transBufCopy, iMmfFileEventHandler);
			}
		else
			{
			request = new(ELeave) CWriteRequest(STATIC_CAST(TAny*, aSupplier), aBuffer, iMmfFileEventHandler);
			}

		CleanupStack::PushL( request );

		StoreRequestL(request);  // transfers ownership
		CleanupStack::Pop(); // request

		iFile->Write(request->BufferDes(), request->BufferDes().Length(), request->iStatus);
		request->SetActive();
		}
	else  // if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		User::Leave( KErrNotSupported ) ;
		}
	}

/**
Loads aLength number of bytes into aBuffer from specified point in iFile.

@param  aLength
        The number of bytes to be read into buffer.
@param  aBuffer
        The buffer to be filled from the file.
@param  aPosition
        The offset into the file at which to start reading.
@param  aConsumer
        The data sink consumer of the buffer.
*/
void CProgressiveDownloadSource::ReadBufferL(TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer)
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::ReadBufferL Async"));
    #endif
	// Requires that iFile is open for read.
	// Reads data from iFile into aBuffer
	if ((aLength < 0) || (aPosition<0) || (aConsumer == NULL) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (!iFile || (iMmfFileEventHandler == NULL))
		User::Leave(KErrNotReady);

	CTransferBufferCopy* transBufCopy = NULL;

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		CReadRequest* request = NULL;
		TDes8& aBufferDes = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data();

		//check whether buffer is safe to send to file server
		//if not, eg for a transfer buffer, then it needs to be copied
		if (!CMMFBuffer::IsFileServerSafe(aBuffer->Type()))
			{
			//Obtain a normal buffer to send to the file server
			//NB: failure in this method will NOT cause transBufCopy to leak as it will be
			//inserted into iTransferBufferCopies by ObtainCopyOfTransferBufferL.
			transBufCopy = ObtainCopyOfTransferBufferL(aBufferDes.MaxLength());

			request = new(ELeave) CReadRequest(STATIC_CAST(TAny*, aConsumer), aBuffer, transBufCopy, aPosition, Size(), iBytesDownloaded,iMmfFileEventHandler);
			}
		else
			{
			request = new(ELeave) CReadRequest(STATIC_CAST(TAny*, aConsumer), aBuffer, aPosition, Size(), iBytesDownloaded,iMmfFileEventHandler);
			}

		CleanupStack::PushL( request );

		StoreRequestL(request) ;  //transfers ownership
		CleanupStack::Pop() ; //request



		TInt err = iFile->Seek(ESeekStart, aPosition);
		if (err==KErrNone)
			iFile->Read(request->BufferDes(), aLength, request->iStatus);
		else
			{
			TRequestStatus* status = &request->iStatus;
			User::RequestComplete(status, err);
			}

		//rj wait until runl is completed
	//	iPosition = aPosition + aLength;
       iPosition = aPosition;

	// rj 	if (iPosition >= iFileSize)
	//		{
	//		aBuffer->SetLastBuffer(ETrue);
	//		}

		request->SetActive();
		}
	else // if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		User::Leave( KErrNotSupported ) ;
	}


/**
Loads aBuffer from specified point in iFile.

The file must already be open for reading.

@param  aBuffer
        The buffer to be filled from the file.
@param  aPosition
        The offset into file at which to start reading.
@param  aConsumer
        The data sink consumer of the buffer.
*/
void CProgressiveDownloadSource::ReadBufferL(CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer)
	{
    #if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::ReadBufferL Async"));
    #endif

	// Requires that iFile is open for read.
	// Reads data from iFile into aBuffer
	if ((aPosition<0) || (aConsumer == NULL) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TInt requestSize;
		if(aBuffer->RequestSize())
			requestSize = aBuffer->RequestSize();
		else
			requestSize = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data().MaxLength();

		ReadBufferL(requestSize, aBuffer, aPosition, aConsumer);
		}
	else // if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		User::Leave(KErrNotSupported);
	}


/**
Loads aBuffer from specified point in iFile.  Note that this is a synchronous read.

@param  aBuffer
        The buffer to be filled from the file.
@param  aPosition
        The offset into file at which to start reading.
*/
void CProgressiveDownloadSource::ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition)
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::ReadBufferL Sync"));
    #endif
	// Requires that iFile is open for read.
	// Reads data from iFile into aBuffer
	if ((aPosition<0) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (!iFile)
		User::Leave(KErrNotReady);

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& aBufferDes = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data();

		TInt requestSize;
		if(aBuffer->RequestSize())
			requestSize = aBuffer->RequestSize();
		else
			requestSize = aBufferDes.MaxLength();

		//check whether buffer is safe to send to file server
		//if not, eg for a transfer buffer, then it needs to be copied
		if (!CMMFBuffer::IsFileServerSafe(aBuffer->Type()))
			{
			//NB: failure in this method will NOT cause transBufCopy to leak as it will be
			//inserted into iTransferBufferCopies by ObtainCopyOfTransferBufferL.
			CTransferBufferCopy* transBufCopy = ObtainCopyOfTransferBufferL(aBufferDes.MaxLength());

			User::LeaveIfError(iFile->Seek(ESeekStart, aPosition));
			User::LeaveIfError(iFile->Read(transBufCopy->Des(), requestSize));
			aBufferDes.Copy(transBufCopy->Des().Left(aBufferDes.MaxLength()));
			}
		else
			{
			User::LeaveIfError(iFile->Seek(ESeekStart, aPosition));
			User::LeaveIfError(iFile->Read(aBufferDes, requestSize));
			}

		iPosition = aPosition + aBufferDes.Length();

		//check if the buffer is the last buffer and if so set the last buffer flag on the CMMFDataBuffer
		//NB: setting last buffer is the done by the formatter, but this is a hang over to account for
		//existing formatters that may fail if this is removed.
		if (aBufferDes.Length() < requestSize)
			aBuffer->SetLastBuffer(ETrue);
		}
	else  // if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		User::Leave(KErrNotSupported);
	}

/**
Empties aLength bytes from aBuffer into iFile at specified location.

@param  aLength
        The number of bytes to be emptied from buffer.
@param  aBuffer
        The data buffer containing bytes to be written.
@param  aPosition
        The offset into file at which to start writing.
@param  aSupplier
        The data source to be notified when the write has been completed.

@leave  KErrNotReady
        SinkPrimeL() and SinkThreadLogon() have not been called.
@leave  KErrArgument
        aLength<0 or aPosition<0 or aSupplier is NULL.
@leave  KErrNotSupported
        aBuffer is not a supported CMMFDataBuffer
*/
void CProgressiveDownloadSource::WriteBufferL(TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier)
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::WriteBufferL Async"));
    #endif

	if ((aLength<0) || (aPosition<0) || (aSupplier == NULL) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (!iFile || (iMmfFileEventHandler == NULL))
		User::Leave(KErrNotReady);

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		CWriteRequest* request = NULL;
		TDes8& aBufferDes = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data();

		//check whether buffer is safe to send to file server
		//if not, eg for a transfer buffer, then it needs to be copied
		if (!CMMFBuffer::IsFileServerSafe(aBuffer->Type()))
			{
			//NB: failure in this method will NOT cause transBufCopy to leak as it will be
			//inserted into iTransferBufferCopies by ObtainCopyOfTransferBufferL.
			CTransferBufferCopy* transBufCopy = ObtainCopyOfTransferBufferL(aBufferDes.MaxLength());

			transBufCopy->Des().Copy(aBufferDes);

			request = new(ELeave) CWriteRequest(STATIC_CAST(TAny*, aSupplier), aBuffer, transBufCopy, iMmfFileEventHandler);
			}
		else
			{
			request = new(ELeave) CWriteRequest(STATIC_CAST(TAny*, aSupplier), aBuffer, iMmfFileEventHandler);
			}

		CleanupStack::PushL( request );

		StoreRequestL(request);  // transfers ownership
		CleanupStack::Pop(); // request

		iFile->Seek(ESeekStart, aPosition);
		iFile->Write(request->BufferDes(), aLength, request->iStatus);
		//iFileSize = -1; //reset cached size

		request->SetActive();
		}
	else // if (!CMMFBuffer::IsFileServerSafe(aBuffer->Type()))
		{
		//write bitmap to file
		User::Leave(KErrNotSupported);
		}
	}

/**
Empties aBuffer into iFile at the specified location.

@param  aBuffer
        The data buffer containing bytes to be written.
@param  aPosition
        The offset into file at which to start writing.
@param  aSupplier
        The data source to be notified when the write has been completed.

@leave  KErrNotReady
        SinkPrimeL() and SinkThreadLogon() have not been called.
@leave  KErrArgument
        aSupplier is NULL.
@leave  KErrNotSupported
        The aBuffer is not of type KMMFDataBuffer.
*/
void CProgressiveDownloadSource::WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier)
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::WriteBufferL Async"));
    #endif
	// Requires that iFile is open for write.
	// Writes data from iFile into aBuffer
	if ((aPosition<0) || (aSupplier == NULL) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TUint requestSize = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data().Length();

		WriteBufferL(requestSize, aBuffer, aPosition, aSupplier);
		}
	else  // if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		//write bitmap to file
		User::Leave( KErrNotSupported ) ;
		}
	}

/**
Empties aBuffer into iFile at specified location.  Note that this is a synchronous write.

@param  aBuffer
        The data buffer containing bytes to be written.
@param  aPosition
        The offset into file at which to start writing.

@return The error code from RFile.
*/
void CProgressiveDownloadSource::WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition )
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::WriteBufferL Sync"));
    #endif
	if ((aPosition<0) || (aBuffer == NULL))
		User::Leave(KErrArgument);

	if (!iFile)
		User::Leave(KErrNotReady);

	TInt err(KErrNone) ;

	//check whether buffer is safe to send to file server
	//if not, eg for a transfer buffer, then it needs to be copied
	if ((!CMMFBuffer::IsFileServerSafe(aBuffer->Type()))
		&& (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type())))
		{
		TDes8& aBufferDes = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data();

		//NB: failure in this method will NOT cause transBufCopy to leak as it will be
		//inserted into iTransferBufferCopies by ObtainCopyOfTransferBufferL.
		CTransferBufferCopy* transBufCopy = ObtainCopyOfTransferBufferL(aBufferDes.MaxLength());

		transBufCopy->Des().Copy(aBufferDes);
		err = iFile->Seek(ESeekStart, aPosition);
		if (err==KErrNone)
			err = iFile->Write(transBufCopy->Des(),transBufCopy->Des().Length());
		//iFileSize = -1; //reset cached size
		}
	else if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& aBufferDes = STATIC_CAST( CMMFDataBuffer*, aBuffer )->Data();

		err = iFile->Seek(ESeekStart, aPosition);
		if (err==KErrNone)
			err = iFile->Write(aBufferDes, aBufferDes.Length());
		//iFileSize = -1; //reset cached size
		}
	else // if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		User::Leave(KErrNotSupported);
		}

	User::LeaveIfError(err);
	}

/**
Gets the number of free bytes in the device's file system.

@return The number of free bytes.
*/
TInt64 CProgressiveDownloadSource::BytesFree()
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::BytesFree"));
    #endif

	TVolumeInfo volInfo;
	if (iFsSession.Volume(volInfo) == KErrNone)
		return volInfo.iFree;
	return TInt64(0);
	}

/**
Returns the size of the file in bytes.

Note: This is not the maximum length.

@return The size of the file in bytes.
*/
TInt CProgressiveDownloadSource::Size()
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::size iFileSize %d"),iFileSize);
	  RDebug::Print(_L("CProgressiveDownloadSource::size iFixSize %d"),iFix);
    #endif

  	TInt size = 0;
	TInt err = KErrNone;
	TBool fileOpened = EFalse;


	 if(iFix != 0)   //rj
	 	{
	 	iFileSize = iFix;
	 	return iFix;
	 	}


	if(iFileSize != -1)
		return iFileSize;

	if (!iFile)
		{
		// Open the file.
		TRAP(err, SourcePrimeL());
		if (iFile)
			fileOpened = ETrue;
		}
	if (err == KErrNone && iFile)
		err = iFile->Size(size);
	if (err)
		{
		size = 0;
	//	iFileSize = -1; //reset cached size
		}
	else
		iFileSize = size; //cache the filesize

	if (fileOpened)
		TRAP_IGNORE(SourceStopL());	// Close the file

	return size;

	}

/**
Source thread logon.

Shares fsSession between threads

@param  aEventHandler
        This is an MAsyncEventHandler to handle asynchronous events that occur during the
        transfer of multimedia data.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CProgressiveDownloadSource::SourceThreadLogon(MAsyncEventHandler& aEventHandler)
	{
	iEventHandler = &aEventHandler;
	if(!iMmfFileEventHandler)
		{
		iMmfFileEventHandler = new CMMFFileAsyncEventHandler(this);
		if(!iMmfFileEventHandler)
			return KErrNoMemory;
		}
#ifdef __IPC_V2_PRESENT__
	return KErrNone; // nothing to do
#else
	return iFsSession.Attach();
#endif // __HIDE_IPC_V1__
	}

/**
Logs off source thread.
*/
void CProgressiveDownloadSource::SourceThreadLogoff()
	{
	delete iMmfFileEventHandler;
	iMmfFileEventHandler = NULL;
	iEventHandler = NULL;
	}


/**
Sink thread logon.

Shares fsSession between threads.

@param  aEventHandler
        This is an MAsyncEventHandler to handle asynchronous events that occur during the
        transfer of multimedia data.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CProgressiveDownloadSource::SinkThreadLogon(MAsyncEventHandler& aEventHandler)
	{
	iEventHandler = &aEventHandler;
	if(!iMmfFileEventHandler)
		{
		iMmfFileEventHandler = new CMMFFileAsyncEventHandler(this);
		if(!iMmfFileEventHandler)
			return KErrNoMemory;
		}
#ifdef __IPC_V2_PRESENT__
	return KErrNone;
#else
	return iFsSession.Attach();
#endif // __HIDE_IPC_V1__
	}

/**
Sink thread log off.
*/
void CProgressiveDownloadSource::SinkThreadLogoff()
	{
	delete iMmfFileEventHandler;
	iMmfFileEventHandler = NULL;
	iEventHandler = NULL;
	}

/**
Stores a request in an array.

CReadWriteRequests are stored in the array iRequests.
This function takes ownership and places the request in the array.
It also checks the array for completed requests and removes them.

@param  aRequest
        The request to store.
*/
void CProgressiveDownloadSource::StoreRequestL( CReadWriteRequest* aRequest )
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
void CProgressiveDownloadSource::CancelRequests()
	{
	// Clear out any completed requests
	for ( TInt ii = 0 ; ii < iRequests.Count() ; ii++ )
		{
		CReadWriteRequest* request = iRequests[ii];
		delete request;
		iRequests.Remove(ii);
		ii--;
		}
	}



/**
Returns the data type as a fourCC code of CProgressiveDownloadSource as a data source.

@return The data type fourCC code.
*/
TFourCC CProgressiveDownloadSource::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return  iSourceFourCC ;
	}

/**
Returns the data type as a fourCC code of CProgressiveDownloadSource as a data sink.

@return The data type fourCC code
*/
TFourCC CProgressiveDownloadSource::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	return  iSinkFourCC ;
	}


/**
CProgressiveDownloadSource as a source is always passive so this function is not supported.

@param  aBuffer
        The emptied buffer.
*/
void CProgressiveDownloadSource::BufferEmptiedL(CMMFBuffer* /* aBuffer */)
	{
	Panic(EMMFFilePanicBufferEmptiedLNotSupported);
	}

/**
Tests whether a source buffer can be created.

@return	A boolean indicating if if CProgressiveDownloadSource can create its own buffer. EFalse if CProgressiveDownloadSource cannot
        create it's own buffer.
*/
TBool CProgressiveDownloadSource::CanCreateSourceBuffer()
	{
	return EFalse;
	}

/**
Creates a source buffer.

@param  aMediaId
        The Media ID.
@param  aReference
        A boolean indicating if MDataSource owns the buffer. ETrue if it does, EFalse if the caller
        owns the buffer.

@return	NULL as a CProgressiveDownloadSource cannot create it's own buffer
*/
CMMFBuffer* CProgressiveDownloadSource::CreateSourceBufferL( TMediaId /*aMediaId*/ , TBool& /*aReference*/)
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::CreateSourceBufferL"));
    #endif
	User::Leave(KErrNotSupported);
	return NULL ;
	}

/**
CProgressiveDownloadSource as a sink is always passive so this function is not supported.

@param  aBuffer
        The buffer.
*/
void CProgressiveDownloadSource::BufferFilledL(CMMFBuffer* /* aBuffer */)
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::BufferFilledL"));
    #endif
	Panic(EMMFFilePanicBufferFilledLNotSupported);
	}

/**
Tests whether a sink buffer can be created.

@return	A boolean indicating if the sink buffer can be created. EFalse if CProgressiveDownloadSource cannot create
        it's own buffer
*/
TBool CProgressiveDownloadSource::CanCreateSinkBuffer()
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::CanCreateSinkBuffer"));
    #endif
	return EFalse ;
	}

/**
Creates a sink buffer.

@param  aMediaId
        The Media ID.
@param  aReference
        A boolean indicating if MDataSource owns the buffer. ETrue if MDataSource owns the buffer,
        EFalse if the caller owns the buffer.

@return	NULL as a CProgressiveDownloadSource cannot create it's own buffer
*/
CMMFBuffer* CProgressiveDownloadSource::CreateSinkBufferL(TMediaId /*aMediaId*/ , TBool& /*aReference*/)
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::CreateSinkBufferL"));
    #endif
	User::Leave(KErrNotSupported);
	return NULL ;
	}


/**
Primes the source.

When used as a source, the file prime opens the file as read only.
*/
void CProgressiveDownloadSource::SourcePrimeL()
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::SourcePrimeL"));
    #endif

	// don't reopen file if already open
	if (!iFile)
		{

		if (iFileHandle)
			{
			iFile = CContentFile::NewL(iHandle, UniqueId());
			}
		else
			{
			// rj Open for progressive download need to have shared access
			iFile = CContentFile::NewL(iFsSession, iFullFileName, UniqueId(), EFileShareAny);

			}

		}
	iFileOpen = ETrue;
	}

/**
Primes the sink.

When used as a sink, the file prime opens the file for read/write access.
*/
void CProgressiveDownloadSource::SinkPrimeL()
	{
	// don't reopen file if already open
	if (!iFile)
		{

		if (iFileHandle)
			iFile = CF32File::NewL(iHandle);
		else
			iFile = CF32File::NewL(iFsSession, iFullFileName, EFileRead | EFileWrite);

		iSinkNotStopped = ETrue;
		}
	iFileOpen = ETrue;
	}

/**
Stops the file source. When stopping close the file.
*/
void CProgressiveDownloadSource::SourceStopL()
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::SourceStopL"));
    #endif
	CancelRequests();

    if(iFile == NULL) //rj
       return;

	iFileOpen = EFalse;
	TInt pos = 0;
	if (!iFileHandle && !iFile->IsProtected())
		{
		delete iFile;
		iFile = NULL;
	//	iFileSize = -1;
		}
	else
		{
		User::LeaveIfError(iFile->Seek(ESeekStart, pos));

		}

	iPosition=pos;
	}

/**
Stops the file sink.

When stopping close the file.
*/
void CProgressiveDownloadSource::SinkStopL()
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::SinkStopL"));
    #endif

	CancelRequests();
	iFileOpen = EFalse;
	if (!iFileHandle)
		{
		iSinkNotStopped = EFalse;
		delete iFile;
		iFile = NULL;
		}
	else
		{
		TInt pos = 0;
		User::LeaveIfError(iFile->Seek(ESeekStart, pos));
		}
	//iFileSize = -1;
	iPosition=0;
	}

/**
Returns a boolean indicating if the sink has been stopped.

@return A boolean indicating if the sink has stopped.
 */
TBool CProgressiveDownloadSource::SinkStopped()
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::SinkStopped"));
    #endif
	if(iSinkNotStopped == EFalse)
		return ETrue;
	else
		return EFalse;
	}

/**
Evaluates a given intent against the rights associated with the file.

The rights are not updated by this function call.

@param  aIntent
        The intent to evaluate.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CProgressiveDownloadSource::EvaluateIntent(ContentAccess::TIntent aIntent) const
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::EvaluateIntent"));
    #endif
	if (iFile==NULL)
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
TInt CProgressiveDownloadSource::ExecuteIntent(ContentAccess::TIntent aIntent)
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::ExecuteIntent"));
    #endif
	if (!iFile)
		return KErrNotReady;

	return iFile->ExecuteIntent(aIntent);
	}

/**
Returns whether the file is protected.

@return A boolean indicating if the file is protected. ETrue if the file is protected.
*/
TBool CProgressiveDownloadSource::IsProtectedL() const
	{

	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::IsProtectedL"));
    #endif
	if (!iFile)
		User::Leave(KErrNotReady);

	return iFile->IsProtected();
	}





TInt CProgressiveDownloadSource::SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
	{
	if (iFile==NULL)
		{
		return KErrNotReady;
		}

	return iFile->SetAgentProperty(aProperty, aValue);
	}

void CProgressiveDownloadSource::SourceCustomCommand(TMMFMessage& aMessage)
	{
    #if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadSource::SourceCustomCommand"));
    #endif

    TInt err= KErrNone;
	switch(aMessage.Function())
		  {
		  case EGETFILEPOSITION:
			  {
			  //get iPosition;
			   #if _DEBUG
	           RDebug::Print(_L("CProgressiveDownloadSource::SrcCustomCommand file position %d"),iPosition);
               #endif
              TPckgBuf<TInt> positionPckg(iPosition);
			  err = aMessage.WriteDataToClient(positionPckg);

			  break;
			  }
          case ESETFILESIZE:
			  {
			  //set file size;
			  TPckgBuf<TInt> fileSizePckg;
			  err = aMessage.ReadData1FromClient(fileSizePckg);
			  if((err==KErrNone) && (fileSizePckg()>0))
				  SetSize(fileSizePckg());

			  break;

			  }
		 case ESETBYTESDOWNLOADED:
		 	  {
		      TPckgBuf<TInt> bytesDownloadedPckg;
		      err = aMessage.ReadData1FromClient(bytesDownloadedPckg);

		      if((err==KErrNone) &&(bytesDownloadedPckg()>0))
		          iBytesDownloaded = bytesDownloadedPckg();
		      break;
		 	  }
		  default:
			  err = KErrNotSupported;
			  break;
		  }

	aMessage.Complete(err);

	}

/*
 *	Returns ETrue if the request can safely be deleted.
 */
TBool CReadWriteRequest::Completed()
	{
	return iCompleted ;
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
	iCompleted = ETrue ;
	}

/*
 *	Called when errors in RunL force Leave.  For the moment just mark the request deletable
 */
TInt CReadWriteRequest::RunError( TInt aError )
	{
	//RunL can leave.
	iCompleted = ETrue ;
	iError = aError; //keep this error internally for now
	return KErrNone ;
	}

/*
 *	On completion of read request call back to the MDataSink
 */
void CReadRequest::RunL()
	{

	#if _DEBUG
	   RDebug::Print(_L("CReadRequest::RunL file byte position %d"),iPosition);
	   RDebug::Print(_L("CReadRequest::RunL buffer length %d"),BufferDes().Length());
	#endif

	if (iStatus != KErrNone)
		{
		TMMFEvent event(KMMFErrorCategoryControllerGeneralError, iStatus.Int());
		iEventHandler->SendEventToClient(event);
		}
	else
		{
		//Copy the data from the normal buffer into the Transfer buffer
		if(iTransferBufferCopy)
			{
			//must specify the size here as the dest may be smaller than the source.
			TDes8& destDesc = STATIC_CAST(CMMFDataBuffer*, iBuffer)->Data();
			destDesc.Copy(iTransferBufferCopy->Des().Left(destDesc.MaxLength()));

			iTransferBufferCopy->SetInUse(EFalse);
			}

	   #if _DEBUG
	      RDebug::Print(_L("CReadRequest::RunL bytes downloaded %d"),iBytesDownloaded);
	   #endif
	   if(iBytesDownloaded > 0 &&
	      iBytesDownloaded <= iPosition + BufferDes().Length() &&
	      iBytesDownloaded < iFileSize)
		 {
	   	 #if _DEBUG
	       RDebug::Print(_L("CReadRequest::RunL - OUT OF DATA"));
	   	 #endif

	     STATIC_CAST(CMMFDataBuffer*, iBuffer)->Data().SetLength(0);

		 }
	    else
	     {
	     iPosition = iPosition + BufferDes().Length();
	     }

		//has all the files data been read
		if(iPosition >= iFileSize)
			{
        #if _DEBUG
           RDebug::Print(_L("CReadRequest::RunL filesize %d"),iFileSize);
		   RDebug::Print(_L("CReadRequest::RunL file position %d"),iPosition);
		   RDebug::Print(_L("CReadRequest::RunL setlastbuffer "));
        #endif

			iBuffer->SetLastBuffer(ETrue);
			}

		REINTERPRET_CAST(MDataSink*, iSinkOrSource)->BufferFilledL(iBuffer) ; // callback to MDataSource/Sink
		}

	iCompleted = ETrue ;
	}

/*
 *  On completion of write request call back to the MDataSource
 */
void CWriteRequest::RunL()
	{
	if(iTransferBufferCopy)
		iTransferBufferCopy->SetInUse(EFalse);

	if (iStatus != KErrNone)
		{
		TMMFEvent event(KMMFErrorCategoryControllerGeneralError, iStatus.Int());
		iEventHandler->SendEventToClient(event);
		}
	else
		REINTERPRET_CAST(MDataSource*, iSinkOrSource)->BufferEmptiedL(iBuffer) ; // callback to MDataSource/Sink

	iCompleted = ETrue ;
	}

CProgressiveDownloadSource::CMMFFileAsyncEventHandler::CMMFFileAsyncEventHandler(CProgressiveDownloadSource* aParent)
	{
	iParent = aParent;
	}

CProgressiveDownloadSource::CMMFFileAsyncEventHandler::~CMMFFileAsyncEventHandler()
	{
	}

TInt CProgressiveDownloadSource::CMMFFileAsyncEventHandler::SendEventToClient(const TMMFEvent& aEvent)
	{

#if _DEBUG
     RDebug::Print(_L("CProgressiveDownloadSource::CMMFFileAsyncEventHandler::SendEventToClient err=%d"),aEvent.iErrorCode);
#endif

	if(aEvent.iErrorCode == KErrNotReady)//i.e. MMC removed while recording
		TRAP_IGNORE(iParent->SinkStopL());
	return iParent->iEventHandler->SendEventToClient(aEvent);
	}



// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KProgressiveDownloadSourceUid,CProgressiveDownloadSource::NewSourceL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
