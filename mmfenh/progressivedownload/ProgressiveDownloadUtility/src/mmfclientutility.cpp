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
* Description:  Progressive Download Utility  Audio, MIDI and Video client utility functions.
*
*/


#include <e32std.h>
#include <f32file.h>
#include <bautils.h>
#include <caf/content.h>
#include <caf/data.h>
using namespace ContentAccess;

#include "mmfclientutility.h"
#include <mmf/common/mmfpaniccodes.h>

const TInt KMaxMimeLength = 256;
const TInt KMaxHeaderSize = 256;
#ifdef __WINDOWS_MEDIA
_LIT8(KWMAMimeType,"audio/x-ms-wma");
#endif

void CUPanic(TInt aCUPanicCode)
	{
	_LIT(KMMFMediaClientUtilityPaanicCategory, "MMFClientUtility");
	User::Panic(KMMFMediaClientUtilityPaanicCategory, aCUPanicCode);
	}

/**
 * @internalComponent
 */
 TUid CMMFClientUtility::ConvertMdaFormatUidToECOMWrite(TUid aMdaFormatUid)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::ConvertMdaFormatUidToECOMWrite\n"));
    #endif
	TUid ECOMUid = KNullUid;
	if (aMdaFormatUid == KUidMdaClipFormatWav)
		ECOMUid = TUid::Uid(KMmfUidFormatWAVWrite);
	else if (aMdaFormatUid == KUidMdaClipFormatAu)
		ECOMUid = TUid::Uid(KMmfUidFormatAUWrite);
	else if (aMdaFormatUid == KUidMdaClipFormatRawAudio)
		ECOMUid = TUid::Uid(KMmfUidFormatRAWWrite);
	else if (aMdaFormatUid == KUidMdaClipFormatRawAmr)
		ECOMUid = TUid::Uid(KAdvancedUidFormatAMRWrite);

	return ECOMUid;
	}

/**
 * @internalComponent
 */
 TUid CMMFClientUtility::ConvertMdaFormatUidToECOMRead(TUid aMdaFormatUid)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::ConvertMdaFormatUidToECOMRead\n"));
    #endif
	TUid ECOMUid = KNullUid;
	if (aMdaFormatUid == KUidMdaClipFormatWav)
		ECOMUid = TUid::Uid(KMmfUidFormatWAVRead);
	else if (aMdaFormatUid == KUidMdaClipFormatAu)
		ECOMUid = TUid::Uid(KMmfUidFormatAURead);
	else if (aMdaFormatUid == KUidMdaClipFormatRawAudio)
		ECOMUid = TUid::Uid(KMmfUidFormatRAWRead);
	else if (aMdaFormatUid == KUidMdaClipFormatRawAmr)
		ECOMUid = TUid::Uid(KAdvancedUidFormatAMRRead);

	return ECOMUid;
	}

/**
 * @internalComponent
 */
 TInt CMMFClientUtility::GetFileHeaderData(const TDesC& aFileName, TDes8& aHeaderData, TInt aMaxLength)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::GetFileHeaderData***\n"));
    #endif
	RFs fsSession;
	RFile file;
	TInt error = KErrNone;
	TInt size = 0;
	if ((error = fsSession.Connect()) == KErrNone)
		{
		//rj  if ((error = file.Open(fsSession, aFileName, EFileShareReadersOnly)) == KErrNone)
        if ((error = file.Open(fsSession, aFileName, EFileShareAny)) == KErrNone)
			{


			if ((error = file.Size(size)) == KErrNone)
				{
				if (size > 0)
					{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::GetFileHeaderData actual size= %d  \n"),size);
    #endif
					if (size > aMaxLength)
						size = aMaxLength;

					error = file.Read(aHeaderData, size);
					}
				}
			file.Close();
			}
		fsSession.Close();


		}

	#if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::GetFileHeaderData err %d  header %d\n"),error,size);
    #endif

	return error;
	}

/**
 * @internalComponent
 */
 TFourCC CMMFClientUtility::ConvertMdaCodecToFourCC(TMdaPackage& aCodec)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::ConvertMdaCodecToFourCC\n"));
    #endif
	TFourCC dataType = KMMFFourCCCodeNULL;
	switch (aCodec.Uid().iUid)
		{
		case KUidMdaWavPcmCodecDefine:
			{
			TMdaPcmWavCodec* pcmWavCodec = (TMdaPcmWavCodec*)&aCodec;
			if (pcmWavCodec->iBits == TMdaPcmWavCodec::E8BitPcm)
				dataType = KMMFFourCCCodePCMU8; //8 bit PCM
			else
				dataType = KMMFFourCCCodePCM16; //16 bit PCM
			break;
			}
		case KUidMdaAu8PcmCodecDefine:
			dataType = KMMFFourCCCodePCM8;
			break;
		case KUidMdaAuCodecDefine:
		case KUidMdaAu16PcmCodecDefine:
			dataType = KMMFFourCCCodePCM16B;
			break;

		case KUidMdaAuMulawCodecDefine:
		case KUidMdaWavMulawCodecDefine:
		case KUidMdaRawAudioMulawCodecDefine:  //uLAW
			dataType = KMMFFourCCCodeMuLAW;
			break;
		case KUidMdaAuAlawCodecDefine:
		case KUidMdaWavAlawCodecDefine:
		case KUidMdaRawAudioAlawCodecDefine:	 //ALAW
			dataType = KMMFFourCCCodeALAW;
			break;
		case KUidMdaRawAudioS8PcmCodecDefine:	 //  P8
			dataType = KMMFFourCCCodePCM8;
			break;
		case KUidMdaRawAudioU8PcmCodecDefine:	  // PU8
			dataType = KMMFFourCCCodePCMU8;
			break;
		case KUidMdaRawAudioSL16PcmCodecDefine: // P16
			dataType = KMMFFourCCCodePCM16;
			break;
		case KUidMdaRawAudioSB16PcmCodecDefine: //P16B
			dataType = KMMFFourCCCodePCM16B;
			break;
		case KUidMdaRawAudioUL16PcmCodecDefine: //PU16
			dataType = KMMFFourCCCodePCMU16;
			break;
		case KUidMdaRawAudioUB16PcmCodecDefine: //PU6B
			dataType = KMMFFourCCCodePCMU16B;
			break;
		case KUidMdaGsmWavCodecDefine: //GSM6
			dataType = KMMFFourCCCodeGSM610;
			break;
		case KUidMdaWavImaAdpcmCodecDefine:
			dataType = KMMFFourCCCodeIMAD;
			break;
		case KUidMdaRawAmrCodecDefine:
			dataType = KMMFFourCCCodeAMR;
			break;
		default:	// Not a Uid we recognise
			dataType = KMMFFourCCCodeNULL;
			break;
		}
	return dataType;
	}


CMMFUtilityFileInfo* CMMFUtilityFileInfo::NewL(TMMSource& aSource)
	{
	CMMFUtilityFileInfo* self = CMMFUtilityFileInfo::NewLC(aSource);
	CleanupStack::Pop(self);
	return self;
	}


CMMFUtilityFileInfo* CMMFUtilityFileInfo::NewLC(TMMSource& aSource)
	{
	CMMFUtilityFileInfo* self = new (ELeave) CMMFUtilityFileInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aSource);
	return self;
	}


void CMMFUtilityFileInfo::ConstructL(const TMMSource& aSource)
	{
	if (aSource.SourceType()==KUidMMFileSource)
		{
		const TMMFileSource& fileSource = static_cast<const TMMFileSource&>(aSource);
		iData = CData::NewL(TVirtualPathPtr(fileSource.Name(), fileSource.UniqueId()),
							EContentShareReadWrite/*EContentShareReadOnly*/);
		}

	if (aSource.SourceType()==KUidMMFileHandleSource)
		{
		const TMMFileHandleSource& fileHandleSource = static_cast<const TMMFileHandleSource&>(aSource);
		iData = CData::NewL(fileHandleSource.Handle(), fileHandleSource.UniqueId());
		}

	TInt err = iData->SetProperty(EAgentPropertyAgentUI, aSource.IsUIEnabled());

	if (err != KErrNone && err != KErrCANotSupported)
		{
		// KErrCANotSupported isn't a problem for us so eat the error code.
		User::Leave(err);
		}

	err = iData->EvaluateIntent(aSource.Intent());
	User::LeaveIfError(err);
	}

TInt CMMFUtilityFileInfo::EvaluateIntent(TIntent aIntent)
	{
	return iData->EvaluateIntent(aIntent);
	}
/**
 * @internalComponent
 */
TBool CMMFUtilityFileInfo::GetFileMimeTypeL(TDes8& aMimeType)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFUtilityFileInfo::GetFileMimeTypeL\n"));
    #endif
	return iData->GetMimeTypeL(aMimeType);
	}

/**
 * @internalComponent
 */
void CMMFUtilityFileInfo::GetFileHeaderDataL(TDes8& aHeaderData, TInt aMaxLength)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFUtilityFileInfo::GetFileHeaderDataL\n"));
    #endif
	TInt size = 0;
	iData->DataSizeL(size);
	if (size > 0)
		{
		if (size > aMaxLength)
			size = aMaxLength;
		TInt pos = 0;
		User::LeaveIfError(iData->Seek(ESeekStart, pos));
		User::LeaveIfError(iData->Read(aHeaderData, size));
		}
	}

/**
 * @internalComponent
 */
 HBufC8* CMMFClientUtility::GetFileExtensionL(const TDesC& aFileName)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::GetFileExtensionL\n"));
    #endif
	TParse fileName;
	fileName.Set(aFileName,NULL,NULL);
	HBufC8* fileSuffix = NULL;
	if(fileName.ExtPresent())
		{
		TPtrC fileSuffixPtr(fileName.Ext());
		fileSuffix = HBufC8::NewL(fileSuffixPtr.Length());
		fileSuffix->Des().Copy(fileSuffixPtr);
		}
	else
		{
		fileSuffix = KNullDesC8().AllocL();
		}
	return fileSuffix;
	}


/**
 * @internalComponent
 */
CMMFUtilityFileInfo::~CMMFUtilityFileInfo()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::~CMMFUtilityFileInfo\n"));
    #endif
	delete iData;
	}

/*
 * @internalComponent
 *
 * Returns an integer rating indicating how well the supplied format matches
 * the header data and file extension supplied.
 * 3 brownie points awarded for data & suffix match.
 * 2 brownie points awarded for data match alone.
 * 1 brownie point awarded for suffix match alone.
 */
TInt CMMFClientUtility::GetBestMatchL(const CMMFFormatImplementationInformation* format, const TDesC8& aHeaderData, const TDesC8& aFileExtension)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFClientUtility::GetBestMatchL\n"));
    #endif
	TInt browniePoints = 0;

	if (aHeaderData.Length() > 0) // Non empty file
		{
		if (aFileExtension.Length() > 0) // With a file extension
			{
			if (format->SupportsHeaderDataL(aHeaderData) &&
				format->SupportsFileExtension(aFileExtension))
				{
				browniePoints = 3;
				}
			else if (format->SupportsHeaderDataL(aHeaderData))
				{
				browniePoints = 2;
				}
			else
				{
				// See if this format has any 'empty' match data or no match data, indicating
				// that this format will match extension alone even if data present.
				// (A format may have more than one match data string.)
				const CDesC8Array& supportedHeaderData = format->SupportedHeaderData();

				if (supportedHeaderData.Count() == 0)
					{
					// No header data indicated.
					if (format->SupportsFileExtension(aFileExtension))
						{
							browniePoints = 1;
						}
					}
				else
					{
					for (register TInt i = 0; i < supportedHeaderData.Count(); i++)
						{
						if ((supportedHeaderData[i].Length() == 0) &&
							format->SupportsFileExtension(aFileExtension))
							{
							browniePoints = 1;
							}
						}
					}
				}
			}
		else
			{
			// No file suffix, so must match header data alone.
			if (format->SupportsHeaderDataL(aHeaderData))
				{
				browniePoints = 2;
				}
			}
		}
	else // Empty File
		{
		// We have no choice but to match extension, if there is one.
		if ((aFileExtension.Length() > 0) && format->SupportsFileExtension(aFileExtension))
			{
			browniePoints = 1;
			}
		}

	return browniePoints;
}

/**
 * @internalComponent
 *
 * This function parses all the play & record formats in the given list of controllers,
 * looking for controllers & formats that best match the requirements.
 * Play controllers will be returned before record controllers, and
 * in cases of equal priority between formats, ECom order will be maintained.
 *
 * @param	"aControllers"
 *			A reference to a user supplied list of controllers retrieved from ECom.
 *			This list may be have been filtered for audio/video/play/record.
 * @param	"aHeaderDataPlayback"
 *			A descriptor reference containing the file's header data.
 *          for matching against a controller's play formats. May be KNullDesC8
 * @param	"aFileExtensionPlayback"
 *			A descriptor reference containing the filename's extension.
 *          for matching against a controller's play formats. May be KNullDesC8
 * @param	"aHeaderDataRecord"
 *			A descriptor reference containing the file's header data.
 *          for matching against a controller's record formats. May be KNullDesC8
 * @param	"aFileExtensionRecord"
 *			A descriptor reference containing the filename's extension.
 *          for matching against a controller's record formats. May be KNullDesC8
 * @param	"aPrioritisedControllers"
 *			A reference to a user supplied list through which the list of
 *			prioritised controllers will be returned.
 * @since	7.0s
 * @lib	"MediaClientUtility.lib"
 */
void CMMFClientUtility::PrioritiseControllersL(
	const RMMFControllerImplInfoArray& aControllers,
	const TDesC8& aHeaderDataPlayback,
	const TDesC8& aFileExtensionPlayback,
	const TDesC8& aHeaderDataRecord,
	const TDesC8& aFileExtensionRecord,
	RMMFControllerImplInfoArray& prioritisedControllers)
	{
	RMMFControllerImplInfoArray fullMatchControllers; // data AND suffix
	CleanupClosePushL(fullMatchControllers);
	RMMFControllerImplInfoArray partMatchControllers; // data OR suffix
	CleanupClosePushL(partMatchControllers);

	TBool checkingPlaybackFormats = EFalse;
	TBool checkingRecordFormats = EFalse;

	if (aHeaderDataPlayback != KNullDesC8 || aFileExtensionPlayback != KNullDesC8)
		checkingPlaybackFormats = ETrue;
	if (aHeaderDataRecord != KNullDesC8 || aFileExtensionRecord != KNullDesC8)
		checkingRecordFormats = ETrue;

	// Examine each format for each controller. We only want to know at this stage
	// if the controller has suitable formats, so as soon as we know it has, we can
	// add it to out list, ranked by how well it matched.
	for (register TInt i = 0; i < aControllers.Count(); i++)
		{
		const CMMFControllerImplementationInformation* controller = aControllers[i];
		TInt savedBrowniePointsPlayback = 0;
		TInt savedBrowniePointsRecord = 0;

		if (checkingPlaybackFormats)
			{
			for (register TInt p = 0; p < controller->PlayFormats().Count(); p++)
				{
				const CMMFFormatImplementationInformation* format = controller->PlayFormats()[p];

				TInt browniePoints = GetBestMatchL(format, aHeaderDataPlayback, aFileExtensionPlayback);

				if (browniePoints >= savedBrowniePointsPlayback)
					savedBrowniePointsPlayback = browniePoints;
				}
			}

		if (checkingRecordFormats)
			{
			for (register TInt r = 0; r < controller->RecordFormats().Count(); r++)
				{
				const CMMFFormatImplementationInformation* format = controller->RecordFormats()[r];

				TInt browniePoints = GetBestMatchL(format, aHeaderDataRecord, aFileExtensionRecord);

				if (browniePoints >= savedBrowniePointsRecord)
					savedBrowniePointsRecord = browniePoints;
				}
			}

		TInt savedBrowniePoints = 0;
		// if we're checking both playback & record formats
		// make sure we've found both
		if (checkingPlaybackFormats && checkingRecordFormats)
			{
			savedBrowniePoints = Min(savedBrowniePointsPlayback, savedBrowniePointsRecord);
			}
		else if (checkingPlaybackFormats)
			{
			savedBrowniePoints = savedBrowniePointsPlayback;
			}
		else if (checkingRecordFormats)
			{
			savedBrowniePoints = savedBrowniePointsRecord;
			}

		// Checked all formats for this controller, now count our brownie points.
		switch (savedBrowniePoints)
			{
			case 3:
				User::LeaveIfError(fullMatchControllers.Append(controller));
				break;
			case 2:
				User::LeaveIfError(partMatchControllers.Insert(controller, 0));
				break;
			case 1:
				User::LeaveIfError(partMatchControllers.Append(controller));
				break;
			default:
				break;
			}
		}

	// The better the controller matches, the earlier it will be in the final list.
	for (register TInt x = 0; x < fullMatchControllers.Count(); x++)
		{
		if (prioritisedControllers.Find(fullMatchControllers[x]) == KErrNotFound)
			{
			User::LeaveIfError(prioritisedControllers.Append(fullMatchControllers[x]));
			}
		}

	for (register TInt y = 0; y < partMatchControllers.Count(); y++)
		{
		if (prioritisedControllers.Find(partMatchControllers[y]) == KErrNotFound)
			{
			User::LeaveIfError(prioritisedControllers.Append(partMatchControllers[y]));
			}
		}

	CleanupStack::PopAndDestroy(2, &fullMatchControllers); // fullMatchControllers, partMatchControllers
	}

/**
 * @internalComponent
 */
 CMMFMdaObjectStateChangeObserverCallback* CMMFMdaObjectStateChangeObserverCallback::NewL(MMdaObjectStateChangeObserver& aCallback)
	{
	return new(ELeave) CMMFMdaObjectStateChangeObserverCallback(aCallback);
	}

/**
 * @internalComponent
 */
CMMFMdaObjectStateChangeObserverCallback::~CMMFMdaObjectStateChangeObserverCallback()
	{
	Cancel();
	}

/**
 * @internalComponent
 */
 void CMMFMdaObjectStateChangeObserverCallback::CallBack(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	iObject = aObject;
	iPreviousState = aPreviousState;
	iCurrentState = aCurrentState;
	iErrorCode = aErrorCode;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

CMMFMdaObjectStateChangeObserverCallback::CMMFMdaObjectStateChangeObserverCallback(MMdaObjectStateChangeObserver& aCallback) :
	CActive(CActive::EPriorityHigh),
	iCallback(aCallback)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFMdaObjectStateChangeObserverCallback::CMMFMdaObjectStateChangeObserverCallback\n"));
    #endif
	CActiveScheduler::Add(this);
	}

void CMMFMdaObjectStateChangeObserverCallback::RunL()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFMdaObjectStateChangeObserverCallback::RunL\n"));
    #endif
	iCallback.MoscoStateChangeEvent(iObject, iPreviousState, iCurrentState, iErrorCode);
	}

void CMMFMdaObjectStateChangeObserverCallback::DoCancel()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFMdaObjectStateChangeObserverCallback::DoCancel\n"));
    #endif
	//nothing to cancel
	}


//****************************************
// CMMFFindAndOpenController
//****************************************

/**
 * Factory function to create a CMMFFindAndOpenController class
 *
 * @internalComponent
 */
 CMMFFindAndOpenController* CMMFFindAndOpenController::NewL(MMMFFindAndOpenControllerObserver& aObserver)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::NewL\n"));
    #endif
	CMMFFindAndOpenController* self = new(ELeave) CMMFFindAndOpenController(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMFFindAndOpenController::~CMMFFindAndOpenController()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::~CMMFFindAndOpenController\n"));
    #endif
	Cancel();

	// delete temporary variables
	Close();

	// this should cancel the AO
	delete iAddDataSourceSinkAsync;

	delete iPrimaryConfig;
	delete iSecondaryConfig;
	}

/**
 * Function to free up memory after a successful open has completed
 * Useful to allow a alloc testing to work.
 * Must not be called if ReOpen() is to be called
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::Close()
	{

	Cancel();

	if(iAddDataSourceSinkAsync)
		{
		iAddDataSourceSinkAsync->Cancel();
		}

	if (iPrimaryConfig)
		iPrimaryConfig->Close();
	if (iSecondaryConfig)
		iSecondaryConfig->Close();

	iPrioritisedControllers.Close();
	iControllers.ResetAndDestroy();
	iControllers.Close();

	iFileName.SetLength(0);
	iFileNameSecondary.SetLength(0);

	delete iUrl;
	iUrl = NULL;

	delete iMimeType;
	iMimeType = NULL;

	delete iUniqueId;
	iUniqueId = NULL;

	iFileHandle.Close();
	iUseFileHandle = EFalse;
	}

CMMFFindAndOpenController::CMMFFindAndOpenController(MMMFFindAndOpenControllerObserver& aObserver) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iDescriptor(NULL, 0)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::CMMFFindAndOpenController\n"));
    #endif
	CActiveScheduler::Add(this);
	}

void CMMFFindAndOpenController::ConstructL()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::ConstructL\n"));
    #endif
	iAddDataSourceSinkAsync = CMMFAddDataSourceSinkAsync::NewL(*this);
	iPrimaryConfig = new (ELeave)  CConfig();
	iSecondaryConfig =  new (ELeave) CConfig;
	iCurrentConfig =  iPrimaryConfig;
	}

void CMMFFindAndOpenController::RunL()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::RunL\n"));
    #endif
	Process();
	}

void CMMFFindAndOpenController::DoCancel()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::DoCancel\n"));
    #endif
	iAddDataSourceSinkAsync->Cancel();
	}

/**
 * Defines the media ID & priority to be used when opening a controller
 * Normally called once only after class has been constructed
 *
 * @param	aMediaId
 *			the media ID to use when searching for a controller
 *			e.g. KUidMediaTypeAudio
 * @param	aPrioritySettings
 *			the priority settings to use when opening a controller
 * @param	aMediaIdMatchType
 *			Defines the type of media id match to be performed on the plugins
 *			returned from the ECOM registry.
 * @leave	can leave with KErrNoMemory

 * @internalComponent
 */
 void CMMFFindAndOpenController::Configure(
	TUid aMediaId,
	TMMFPrioritySettings aPrioritySettings,
	CMMFPluginSelectionParameters::TMediaIdMatchType aMediaIdMatchType)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::Configure\n"));
    #endif
	iPrioritySettings = aPrioritySettings;

	iMediaIdMatchType = aMediaIdMatchType;

	iMediaId = aMediaId;
	}

void CMMFFindAndOpenController::ConfigureController(
	CConfig& config,
	RMMFController& aController,
	CMMFControllerEventMonitor& aEventMonitor,
	TControllerMode aControllerMode)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::ConfigureController\n"));
    #endif
	config.iController = &aController;
	config.iEventMonitor = &aEventMonitor;
	config.iControllerMode = aControllerMode;
	}

/**
 * Configures the primary controller
 *
 * @param	aController
 *			a reference to the client controller object to use
 * @param	aEventMonitor
 *			a reference to an event monitor object for receiving
 *			events from the controller
 * @param	aControllerMode
 *			indicates whether this controller is to be used for recording
 *          or playback
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::ConfigureController(
	RMMFController& aController,
	CMMFControllerEventMonitor& aEventMonitor,
	TControllerMode aControllerMode)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::ConfigureController2\n"));
    #endif
	ConfigureController(
		*iPrimaryConfig,
		aController,
		aEventMonitor,
		aControllerMode);
	}

/**
 * Configures the secondary controller
 *
 * This is only needed for the audio recorder utility which opens
 * one controller for playback and another for recording
 *
 * @param	aController
 *			a reference to the client controller object to use
 * @param	aEventMonitor
 *			a reference to an event monitor object for receiving
 *			events from the controller
 * @param	aControllerMode
 *			indicates whether this controller is to be used for recording
 *          or playback or converting
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::ConfigureSecondaryController(
	RMMFController& aController,
	CMMFControllerEventMonitor& aEventMonitor,
	TControllerMode aControllerMode)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::ConfigureSecondaryController\n"));
    #endif
	ConfigureController(
		*iSecondaryConfig,
		aController,
		aEventMonitor,
		aControllerMode);
	}

void CMMFFindAndOpenController::Init()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::Init\n"));
    #endif
	// This should be called prior to opening, so reset the error
	iError = KErrNone;
	iSourceSinkConfigured = EFalse;
	iControllerCount = 0;
	}

void CMMFFindAndOpenController::ConfigureSourceSink(
	CConfig& config,
	TSourceSink aSource,
	TSourceSink aSink)
	{
	TInt err;
	TRAP(err, config.iSource = CreateSourceSinkL(aSource));
	if (err != KErrNone)
		{
		iError = err;
		return;
		}

	TRAP(err, config.iSink = CreateSourceSinkL(aSink));
	if (err != KErrNone)
		{
		iError = err;
		return;
		}
	}


void CMMFFindAndOpenController::ConfigureSourceSink(
	CConfig& config,
	const TMMSource& aSource,
	TSourceSink aSink)
	{
	TInt err;
	TRAP(err, config.iSource = CreateSourceSinkL(aSource));
	if (err != KErrNone)
		{
		iError = err;
		return;
		}

	TRAP(err, config.iSink = CreateSourceSinkL(aSink));
	if (err != KErrNone)
		{
		iError = err;
		return;
		}
	}



/**
 * Configure the primary controller's source and sink
 * The descriptors passed to this function are copied so they do not need to be persistent.
 * To simplify the API, any errors that occur are reported back asynchronously following
 * a subsequent call to OpenByXXX()
 *
 * @param	aSourceUid
 *			the UID of the data source
 * @param	aSourceData
 *			a reference to a descriptor used to configure the source
 * @param	aSinkUid
 *			the UID of the data sink
 * @param	aSinkData
 *			a reference to a descriptor used to configure the sink
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::ConfigureSourceSink(
	TSourceSink aSource,
	TSourceSink aSink)
	{

	CConfig* config = NULL;

	Init();
	config = iPrimaryConfig;


	// must have already called ConfigureController()
	__ASSERT_ALWAYS(config->iController != NULL, CUPanic(EMMFMediaClientUtilityBadState));

	ConfigureSourceSink(
		*config,
		aSource,
		aSink);
	iCurrentConfig = config;

	iSourceSinkConfigured = ETrue;
	}


/**
 * Configure the primary controller's source and sink
 * The descriptors passed to this function are copied so they do not need to be persistent.
 * To simplify the API, any errors that occur are reported back asynchronously following
 * a subsequent call to OpenByXXX()
 *
 * @param	aSourceUid
 *			the UID of the data source
 * @param	aSourceData
 *			a reference to a descriptor used to configure the source
 * @param	aSinkUid
 *			the UID of the data sink
 * @param	aSinkData
 *			a reference to a descriptor used to configure the sink
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::ConfigureSecondarySourceSink(
	TSourceSink aSource,
	TSourceSink aSink)
	{
	if (iError != KErrNone)
		{
		// if there was an error configuring the primary source/sink, do not try the secondary one
		// Don't return the error, since the stored error will be returned by the OpenBy... method
		return;
		}

	CConfig* config = NULL;

	config = iSecondaryConfig;

	// must have already configured the primary controller
	__ASSERT_ALWAYS(iSourceSinkConfigured, CUPanic(EMMFMediaClientUtilityBadState));
	config = iSecondaryConfig;

	// must have already called ConfigureController()
	__ASSERT_ALWAYS(config->iController != NULL, CUPanic(EMMFMediaClientUtilityBadState));

	ConfigureSourceSink(
		*config,
		aSource,
		aSink);
	iCurrentConfig = config;

	iSourceSinkConfigured = ETrue;
	}




 void CMMFFindAndOpenController::ConfigureSourceSink(
	const TMMSource& aSource,
	TSourceSink aSink)
	{
	Init();
	CConfig* config = iPrimaryConfig;

	// must have already called ConfigureController()
	__ASSERT_ALWAYS(config->iController != NULL, CUPanic(EMMFMediaClientUtilityBadState));

	ConfigureSourceSink(
		*config,
		aSource,
		aSink);
	iCurrentConfig = config;

	iSourceSinkConfigured = ETrue;
	}




/**
 * Opens a controller using the supplied controller UID
 * and adds the source & sink
 * Completion is indicated asynchonously by a call to MfaocComplete()
 *
 * @param	aControllerUid
 *			the UID of the primary controller
 * @param	aControllerUid
 *			the UID of the secondary controller
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::OpenByControllerUid(
		TUid aControllerUid,
		TUid aSecondaryControllerUid)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::OpenByControllerUid\n"));
    #endif
	// must have already called ConfigureSourceSink()
	__ASSERT_ALWAYS(iSourceSinkConfigured, CUPanic(EMMFMediaClientUtilityBadState));

	// Have there been any errors so far ?
	if (iError != KErrNone)
		{
	    SchedSendError();
		return;
		}

	if (iCurrentConfig == iPrimaryConfig)
		{
		// only do this for the playback controller
		TRAP(iError, iCurrentConfig->iSource->EvaluateIntentL())

		if (iError != KErrNone)
			{
	    	SchedSendError();
			return;
			}
		}

	iPrimaryConfig->iControllerUid = aControllerUid;
	if (iCurrentConfig == iSecondaryConfig)
		{
		if (aSecondaryControllerUid == KNullUid)
			iSecondaryConfig->iControllerUid = aControllerUid;
		else
			iSecondaryConfig->iControllerUid = aSecondaryControllerUid;
		}

	iMode = EOpenByControllerUid;
	iControllerImplInfo = NULL;
	iState = EOpenController;
	KickState();
	}

/**
 * Opens a controller using the supplied file name
 * and adds the source & sink
 * A copy is made of the filename or file handle so that it need not be persistent
 * Completion is indicated asynchonously by a call to MfaocComplete()
 *
 * @param	aSource
 *			a reference to a TFileSource object to be used when searching
 *          for a controller
 * @param	aFileNameSecondary
 *			a reference to the seconday filename to be used when searching
 *          for a controller. This need only be supplied when converting
 *			between two files.
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::OpenByFileSource(const TMMSource& aSource, const TDesC& aFileNameSecondary)
	{
	// must have already called ConfigureSourceSink()
	__ASSERT_ALWAYS(iSourceSinkConfigured, CUPanic(EMMFMediaClientUtilityBadState));

	TInt err;
	// Have there been any errors so far ?
	if (iError != KErrNone)
		{
		SchedSendError();
		return;
		}

	if (aSource.SourceType()==KUidMMFileSource)
		{
		const TMMFileSource& fileSource = static_cast<const TMMFileSource&>(aSource);
		iFileName = fileSource.Name();
		}

	if (aSource.SourceType()==KUidMMFileHandleSource)
		{
		const TMMFileHandleSource& fileHandleSource = static_cast<const TMMFileHandleSource&>(aSource);
		err = iFileHandle.Duplicate(fileHandleSource.Handle());


		if (err != KErrNone)
			{
			SchedSendError(err);
			return;
			}

		iFileHandle.Name(iFileName); //ignore error return since we'll just do without the filename if not available


		iUseFileHandle = ETrue;
		}

	TRAP(err, iUniqueId = aSource.UniqueId().AllocL());
	iIntent = aSource.Intent();
	if (err != KErrNone)
		{
		SchedSendError(err);
		return;
		}


	// take a copy of the secondary file name
	iFileNameSecondary = aFileNameSecondary;

	iMode = EOpenByFileName;
	iState = EBuildControllerList;
	KickState();
	}

/**
 * Opens a controller using the supplied format UID
 * and adds the source & sink
 * Completion is indicated asynchonously by a call to MfaocComplete()
 *
 * @param	aFormatUid
 *			the UID of a format that must be supported by the controller
 * @param	aFormatUidSecondary
 *			the UID of a secondary format that must be supported by the controller
 *			This need only be supplied when converting between two differnet formats.
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::OpenByFormatUid(TUid aFormatUid, TUid aFormatUidSecondary)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::OpenByFormatUid\n"));
    #endif
	// must have already called ConfigureSourceSink()
	__ASSERT_ALWAYS(iSourceSinkConfigured, CUPanic(EMMFMediaClientUtilityBadState));

	// Have there been any errors so far ?
	if (iError != KErrNone)
		{
		SchedSendError();
		return;
		}

	iFormatUid = aFormatUid;
	iFormatUidSecondary = aFormatUidSecondary;

	iMode = EOpenByFormatUid;
	iState = EBuildControllerList;
	KickState();
	}

/**
 * Opens a controller using the supplied descriptor
 * and adds the source & sink
 * Completion is indicated asynchonously by a call to MfaocComplete()
 *
 * @param	aDescriptor
 *			a reference to the descriptor to be used when searching
 *          for a controller
 *
 * @internalComponent
 */
 void  CMMFFindAndOpenController::OpenByDescriptor(const TDesC8& aDescriptor)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::OpenByDescriptor\n"));
    #endif
	// must have already called ConfigureSourceSink()
	__ASSERT_ALWAYS(iSourceSinkConfigured, CUPanic(EMMFMediaClientUtilityBadState));

	// Have there been any errors so far ?
	if (iError != KErrNone)
		{
		SchedSendError();
		return;
		}

	// take a copy of the descriptor
	TUint8* desBufferPtr = const_cast<TUint8*> (aDescriptor.Ptr());
	iDescriptor.Set( desBufferPtr,aDescriptor.Length(),aDescriptor.Length());

	iMode = EOpenByDescriptor;
	iState = EBuildControllerList;
	KickState();
	}

/**
 * Opens a controller using the supplied URL
 * and adds the source & sink
 * Completion is indicated asynchonously by a call to MfaocComplete()
 *
 * @param	aUrl
 *			a reference to the URL to be used when searching for a controller
 * @param	aIapId
 *          the IAP ID to be used when searching for a controller
 * @param	aMimeType
 *          the MIME type of the data to be used when searching for a controller
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::OpenByUrl(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::OpenByUrl\n"));
    #endif
	// must have already called ConfigureSourceSink()
	__ASSERT_ALWAYS(iSourceSinkConfigured, CUPanic(EMMFMediaClientUtilityBadState));

	// Have there been any errors so far ?
	if (iError != KErrNone)
		{
		SchedSendError();
		return;
		}

	// take a copy of the Url
	delete iUrl;
	iUrl = NULL;
	iUrl = aUrl.Alloc();
	if (iUrl == NULL)
		{
		SchedSendError(KErrNoMemory);
		return;
		}

	// take a copy of the IapId
	iIapId = aIapId;

	// take a copy of the mime type
	delete iMimeType;
	iMimeType = NULL;
	iMimeType = aMimeType.Alloc();
	if (iMimeType == NULL)
		{
		SchedSendError(KErrNoMemory);
		return;
		}

	iMode = EOpenByUrl;
	iState = EBuildControllerList;
	KickState();
	}

/**
 * Static function to return a TMMFFileConfig object
 * suitable for passing to ConfigureSourceSink()
 *
 * @param	aFileName
 *          the filename to use
 *
 * @internalComponent
 */
 TMMFFileConfig CMMFFindAndOpenController::GetConfigFile(const TDesC& aFileName)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::GetConfigFile\n"));
    #endif
	TMMFFileConfig sourceSinkData;
	sourceSinkData().iPath = aFileName;
	return sourceSinkData;
	}

/**
 * Static function to return a TMMFDescriptorConfig object
 * suitable for passing to ConfigureSourceSink()
 *
 * @param	aFileName
 *          the filename to use
 *
 * @internalComponent
 */
 TMMFDescriptorConfig CMMFFindAndOpenController::GetConfigDescriptor(const TDesC8& aDescriptor)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::GetConfigDescriptor\n"));
    #endif
	TMMFDescriptorConfig sourceSinkData;
	sourceSinkData().iDes = (TAny*)&aDescriptor;
	sourceSinkData().iDesThreadId = RThread().Id();
	return sourceSinkData;
	}

/**
 * Static function to create a CBufFlat object
 * suitable for passing to ConfigureSourceSink()
 *
 * @param	aUrlCfgBuffer
 *          the reference to a caller-supplied pointer used to create
 *			a CBufFlat object. The caller is responsible for deletion.
 * @param	aUrl
 *			a reference to the URL to be used
 * @param	aIapId
 *          the IAP ID to be used
 * @return	can return KErrNone or KErrNoMemory
 *
 * @internalComponent
 */
 TInt CMMFFindAndOpenController::GetConfigUrl(CBufFlat*& aUrlCfgBuffer, const TDesC& aUrl, TInt aIapId)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::GetConfigDescriptor\n"));
    #endif
	TInt error;
	delete aUrlCfgBuffer;
	aUrlCfgBuffer = NULL;

	CMMFUrlParams* urlCfg = NULL;
	TRAP(error, urlCfg = CMMFUrlParams::NewL(aUrl,aIapId));
	if (error != KErrNone)
		return error;

	TRAP(error,
		aUrlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();
		CleanupStack::Pop(aUrlCfgBuffer);
		);

	delete urlCfg;

	return error;
	}

/**
 * ReOpens the previously opened primary controller
 *
 * @internalComponent
 */
 void CMMFFindAndOpenController::ReOpen()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::ReOpen\n"));
    #endif
	// should already have a valid controller uid so just open it
	iControllerImplInfo = NULL;
	iState = EOpenController;
	KickState();
	}

void CMMFFindAndOpenController::OpenPrimaryController(void)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::OpenPrimaryController\n"));
    #endif
	iCurrentConfig = iPrimaryConfig;
	switch(iMode)
		{
		case EOpenByFileName:
		case EOpenByFormatUid:
		case EOpenByDescriptor:
		case EOpenByUrl:
			iState = EBuildControllerList;
			break;
		case EOpenByControllerUid:
			iControllerImplInfo = NULL;
			iState = EOpenController;
			break;
		}
	KickState();
	}

void CMMFFindAndOpenController::KickState()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::KickState\n"));
    #endif
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CMMFFindAndOpenController::CloseController()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::CloseController\n"));
    #endif
	if (iCurrentConfig->iEventMonitor)
		iCurrentConfig->iEventMonitor->Cancel();
	iCurrentConfig->iController->Close();
	}

void CMMFFindAndOpenController::Process()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::Process\n"));
    #endif
	switch(iState)
		{
		case EBuildControllerList:
			switch(iMode)
				{
				case EOpenByFileName:
					TRAP(iError, BuildControllerListFileNameL());
					break;
				case EOpenByDescriptor:
					TRAP(iError, BuildControllerListDescriptorL());
					break;
				case EOpenByUrl:
					TRAP(iError, BuildControllerListUrlL());
					break;
				case EOpenByFormatUid:
					TRAP(iError, BuildControllerListFormatL());
					break;
				default:
					CUPanic(EMMFMediaClientUtilityBadState);
				}

			if (iError != KErrNone)
				{
				iState = EIdle;
				SendError();
				break;
				}

			// try the first controller
			iControllerIndex = -1;
			TryNextController();
			break;

		case EOpenController:
			// Make sure any existing controller is closed.
			CloseController();

			// Open the controller
			if (iControllerImplInfo)
				{
				iError = iCurrentConfig->iController->Open(*iControllerImplInfo, iPrioritySettings);
				}
			else
				{
				iError = iCurrentConfig->iController->Open(iCurrentConfig->iControllerUid, iPrioritySettings);
				}

			if (iError)
				{
				TryNextController();
				}
			else
				{

				iCurrentConfig->iEventMonitor->Start();

				if (iCurrentConfig == iSecondaryConfig)
					{
					iState = EAddSource;
					KickState();
					}
				else
					{
					iState = EAddSink;
					KickState();
					}
				}
			break;

		case EAddSource:
			{
			iState = EWaitingForSource;
			const CMMSourceSink* source = iCurrentConfig->iSource;
			iAddDataSourceSinkAsync->AddDataSource(*iCurrentConfig->iController,
												   source->SourceSinkUid(),
												   source->SourceSinkData());
			}
			break;

		case EAddSink:
			{
			iState = EWaitingForSink;
			const CMMSourceSink* sink = iCurrentConfig->iSink;
			iAddDataSourceSinkAsync->AddDataSink(*iCurrentConfig->iController,
												 sink->SourceSinkUid(),
												 sink->SourceSinkData());
			}
			break;

		case EWaitingForSource:
			break;

		case EWaitingForSink:
			break;

		case ESendError:
			SendError();
			iState = EIdle;
			break;

		case EIdle:
		default:
			break;
		}
	}

void CMMFFindAndOpenController::TryNextController()
	{

    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::TryNextController\n"));
    #endif
	// If an error occurred close the controller.
	if (iError != KErrNone)
		CloseController();

	// take the first available exit if we're out of memory
	if (iError == KErrNoMemory)
		{
		SendError();
		return;
		}

	if (iMode == EOpenByControllerUid || ++iControllerIndex >= iControllerCount)
		{
		SendError(KErrNotSupported);		// KErrNotSupported
		return;
		}

	if (iMode == EOpenByFileName || iMode == EOpenByFormatUid)
		{
		iControllerImplInfo = iPrioritisedControllers[iControllerIndex];
		}
	else	//if (iMode == EOpenByDescriptor || iMode == EOpenByUrl)
		{
		iControllerImplInfo = iControllers[iControllerIndex];
		}

        // This Flag is defined so that if the Helix Controller Supports
        // the playback of Local Media for WMA, then the ProgDL still
        // goes through the Old WMA Controller( AdvancedAudioController)
        // We are launching the Old WMA Controller using the UID.

#ifdef __WINDOWS_MEDIA
    HBufC8* mimeType = HBufC8::NewLC(KMaxMimeLength);
    TPtr8 mimeTypePtr = mimeType->Des();
    mimeTypePtr.Copy(KWMAMimeType());

    TBool IsSupported = EFalse;
    const RMMFFormatImplInfoArray& playFormatInfo = iControllerImplInfo->PlayFormats();

	for (TInt p = 0; p < iControllerImplInfo->PlayFormats().Count(); p++)
		{
		const CMMFFormatImplementationInformation* format = iControllerImplInfo->PlayFormats()[p];
        IsSupported = format->SupportsMimeType(*mimeType);
        if(IsSupported)
            break;
        }

    if(IsSupported)
        {
        iControllerImplInfo = NULL;
        iCurrentConfig->iControllerUid = TUid::Uid(0x10207A9B);
        }
    else
#endif
        {
    	iCurrentConfig->iControllerUid = iControllerImplInfo->Uid();
        }

#ifdef __WINDOWS_MEDIA
	CleanupStack::PopAndDestroy(mimeType);
#endif
	iState = EOpenController;
	KickState();
	}

void CMMFFindAndOpenController::MadssaoAddDataSourceSinkAsyncComplete(TInt aError, const TMMFMessageDestination& aHandle)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::MadssaoAddDataSourceSinkAsyncComplete\n"));
    #endif
	iError = aError;

	// take the first available exit if we're out of memory
	// or we've been cancelled
	if (iError == KErrNoMemory || iError == KErrCancel)
		{
		SendError();
		return;
		}

	// failed to add source or sink - try the next controller
	if (aError != KErrNone)
		{
		TryNextController();
		return;
		}

	if (iState == EWaitingForSource)
		{
		iSourceHandle = aHandle;
		if (iCurrentConfig == iSecondaryConfig)
			{
			iState = EAddSink;
			}
		else	// completed ok !
			{
			iState = EIdle;
			iError = KErrNone;
			SendError();
			return;
			}
		}
	else if (iState == EWaitingForSink)
		{
		iSinkHandle = aHandle;
		if (iCurrentConfig == iSecondaryConfig)	// completed ok !
			{
			iState = EIdle;
			iError = KErrNone;
			SendError();
			return;
			}
		else
			{
			iState = EAddSource;
			}
		}

	KickState();
	}

void CMMFFindAndOpenController::SendError(TInt aError)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::SendError err= %d\n"),aError);
    #endif
	if (iError == KErrNone)
		iError = aError;

	iObserver.MfaocComplete(iError, iCurrentConfig->iController, iCurrentConfig->iControllerUid, &iSourceHandle, &iSinkHandle);

	// if we've just attempted to open the Secondary controller,
	// try to open the Primary controller
	if (iCurrentConfig == iSecondaryConfig)
		{
		if (iError == KErrNone)
			OpenPrimaryController();
		}

	// if we failed to open, may as well free up some memory
	// if open succeeded we need to preserve state in case of a re-open
	if (iError != KErrNone)
		Close();
	}

void CMMFFindAndOpenController::SchedSendError(TInt aError)
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::SchedSendError\n"));
    #endif
	if (aError != KErrNone)
		iError = aError;
	iState = ESendError;
	KickState();
	}

void CMMFFindAndOpenController::BuildControllerListFileNameL()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::BuildControllerListFileNameL\n"));
    #endif
	// Retrieve a list of possible controllers from ECOM
	// If we don't have a match, leave with unsupported

	iControllers.ResetAndDestroy();
	iPrioritisedControllers.Reset();

	TControllerMode mode = iCurrentConfig->iControllerMode;

	// if we're playing, try to get the MIME type from the Content Access
	// Framework (CAF) & use that to select a controller - if that fails,
	// try to select a controller based on the header data/file extension

	CMMFUtilityFileInfo* fileInfo = NULL;

	TInt error;

	//If the current CMMSourceSink is a CMMFileSourceSink
	// Using the previous version we'd get KErrCANoPermission when calling EvaluateIntent in the
	// CMMFUtilityFileInfo ConstructL as the intent == EUnknown, so now pass the intent as a parameter
	// to TMMFileHandleSource and....
	if (iUseFileHandle)
		{
		if (iUniqueId != NULL)
			{
			TMMFileHandleSource fileHandleSource(iFileHandle, (*iUniqueId), iIntent);
			TRAP(error, fileInfo = CMMFUtilityFileInfo::NewL(fileHandleSource));
			}
		else
			{
			TMMFileHandleSource fileHandleSource(iFileHandle);
			TRAP(error, fileInfo = CMMFUtilityFileInfo::NewL(fileHandleSource));
			}
		}
	else
		{
		if (iUniqueId != NULL)
			{
			TMMFileSource fileSource(iFileName, (*iUniqueId), iIntent);
			TRAP(error, fileInfo = CMMFUtilityFileInfo::NewL(fileSource));
			}
		else
			{
			TMMFileSource fileSource(iFileName);
			TRAP(error, fileInfo = CMMFUtilityFileInfo::NewL(fileSource));
			}
		}

	if (fileInfo != NULL)
		{
		CleanupDeletePushL(fileInfo);
		}

	if (error != KErrNone)
		{
		// if playback mode, leave for any error
		// if record mode, allow KErrNotFound
		if (mode == EPlayback || (mode != EPlayback && error != KErrNotFound))
			{
			User::Leave(error);
			}
		}

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(iMediaId));

	cSelect->SetMediaIdsL(mediaIds, iMediaIdMatchType);


	if (mode == EPlayback)
		{
		ASSERT(fileInfo!=NULL);
		TBuf8<KMaxMimeLength> mimeType;
		TBool mimeTypeKnown = fileInfo->GetFileMimeTypeL(mimeType);
		if (mimeTypeKnown)
			{
			CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();
			fSelect->SetMatchToMimeTypeL(mimeType);
			cSelect->SetRequiredPlayFormatSupportL(*fSelect);
			cSelect->ListImplementationsL(iControllers);
			CleanupStack::PopAndDestroy(fSelect);
			}


		// copy to the iPrioritisedControllers array - this is a NOOP if the
		// MIME type is not known since iControllers will be empty
		ASSERT(mimeTypeKnown || iControllers.Count() == 0);
		for (TInt controllerIndex=0; controllerIndex < iControllers.Count(); controllerIndex++)
			User::LeaveIfError(iPrioritisedControllers.Append(iControllers[controllerIndex]));

		iControllerCount = iPrioritisedControllers.Count();
		if (iControllerCount > 0)
			{
			// Clean up
			// cSelect, mediaIds,
			CleanupStack::PopAndDestroy(2, cSelect);
			if (fileInfo != NULL)
				{
				CleanupStack::PopAndDestroy(fileInfo);
				}
			return;
			}
		}

	// Retrieve header data first. If file doesn't exist, its ok.
	HBufC8* headerData = HBufC8::NewLC(KMaxHeaderSize);
	TPtr8 headerDataPtr = headerData->Des();
	if (fileInfo)
		{
		fileInfo->GetFileHeaderDataL(headerDataPtr, KMaxHeaderSize);
		}

	// Get the filename's suffix
	HBufC8* fileSuffix = CMMFClientUtility::GetFileExtensionL(iFileName);

	CleanupStack::PushL(fileSuffix);
	TPtr8 fileSuffixPtr = fileSuffix->Des();

	// Get the secondary filename's header data (for convert)
	HBufC8* headerDataSecondary = HBufC8::NewLC(KMaxHeaderSize);
	TPtr8 headerDataPtrSecondary = headerDataSecondary->Des();
	if (iFileNameSecondary.Length() > 0 && fileInfo)
		{
		fileInfo->GetFileHeaderDataL(headerDataPtrSecondary, KMaxHeaderSize);
		}

	// Get the secondary filename's suffix
	HBufC8* fileSuffixSecondary = CMMFClientUtility::GetFileExtensionL(iFileNameSecondary);
	CleanupStack::PushL(fileSuffixSecondary);
	TPtr8 fileSuffixPtrSecondary = fileSuffixSecondary->Des();


	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	if (mode == EPlayback || mode == EConvert)
		cSelect->SetRequiredPlayFormatSupportL(*fSelect);
	if (mode == ERecord || mode == EConvert)
		cSelect->SetRequiredRecordFormatSupportL(*fSelect);

	cSelect->ListImplementationsL(iControllers);

	if (iControllers.Count()==0)
		User::Leave(KErrNotSupported);

	if (mode == ERecord)
		{
		CMMFClientUtility::PrioritiseControllersL(
			iControllers,
			headerDataPtrSecondary,
			fileSuffixPtrSecondary,
			headerDataPtr,
			fileSuffixPtr,
			iPrioritisedControllers);
		}
	else
		{
		CMMFClientUtility::PrioritiseControllersL(
			iControllers,
			headerDataPtr,
			fileSuffixPtr,
			headerDataPtrSecondary,
			fileSuffixPtrSecondary,
			iPrioritisedControllers);
		}

	iControllerCount = iPrioritisedControllers.Count();
	if (iControllerCount == 0)
		User::Leave(KErrNotSupported);

	// Clean up
	// cSelect, mediaIds,
	// headerData, fileSuffix, headerDataSecondary, fileSuffixSecondary,
	// fSelect
	CleanupStack::PopAndDestroy(7, cSelect);
	if (fileInfo != NULL)
		{
		CleanupStack::PopAndDestroy(fileInfo);
		}
	}

void CMMFFindAndOpenController::BuildControllerListDescriptorL()
	{
	// Retrieve a list of possible controllers from ECOM
	// If we don't have a match, leave with unsupported

	iControllers.ResetAndDestroy();

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();


	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(iMediaId));

	cSelect->SetMediaIdsL(mediaIds, iMediaIdMatchType);

	TPtrC8 header = iDescriptor.Left(KMaxHeaderSize);
	fSelect->SetMatchToHeaderDataL(header);


	TControllerMode mode = iCurrentConfig->iControllerMode;
	if (mode == EPlayback || mode == EConvert)
		cSelect->SetRequiredPlayFormatSupportL(*fSelect);
	if (mode == ERecord || mode == EConvert)
		cSelect->SetRequiredRecordFormatSupportL(*fSelect);

	cSelect->ListImplementationsL(iControllers);

	iControllerCount = iControllers.Count();
	if (iControllerCount == 0)
		User::Leave(KErrNotSupported);

	// Clean up
	// cSelect, fSelect, mediaIds
	CleanupStack::PopAndDestroy(3, cSelect);
	}

void CMMFFindAndOpenController::BuildControllerListUrlL()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::BuildControllerListUrlL"));
    #endif
	// Retrieve a list of possible controllers from ECOM
	// If we don't have a match, leave with unsupported

	iControllers.ResetAndDestroy();

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(iMediaId));

	cSelect->SetMediaIdsL(mediaIds, iMediaIdMatchType);


 	if (*iMimeType != KNullDesC8)
		{
		fSelect->SetMatchToMimeTypeL(*iMimeType);//We match to mime type
		}
	else
		fSelect->SetMatchToUriL(*iUrl);


	TControllerMode mode = iCurrentConfig->iControllerMode;
	if (mode == EPlayback || mode == EConvert)
		cSelect->SetRequiredPlayFormatSupportL(*fSelect);
	if (mode == ERecord || mode == EConvert)
		cSelect->SetRequiredRecordFormatSupportL(*fSelect);

	cSelect->ListImplementationsL(iControllers);

	iControllerCount = iControllers.Count();
	if (iControllerCount == 0)
		User::Leave(KErrNotSupported);

	// Clean up
	// cSelect, fSelect, mediaIds
	CleanupStack::PopAndDestroy(3, cSelect);
	}

void CMMFFindAndOpenController::BuildControllerListFormatL()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::BuildControllerListFormatL"));
    #endif
	// Retrieve a list of possible controllers from ECOM
	// If we don't have a match, leave with unsupported

	iControllers.ResetAndDestroy();
	iPrioritisedControllers.Reset();

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();

	// Select the media IDs to allow
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	User::LeaveIfError(mediaIds.Append(iMediaId));

	cSelect->SetMediaIdsL(mediaIds, iMediaIdMatchType);

	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	TControllerMode mode = iCurrentConfig->iControllerMode;
	if (mode == EPlayback || mode == EConvert)
		cSelect->SetRequiredPlayFormatSupportL(*fSelect);
	if (mode == ERecord || mode == EConvert)
		cSelect->SetRequiredRecordFormatSupportL(*fSelect);

	//Obtain a list of the controllers
	cSelect->ListImplementationsL(iControllers);

	CleanupStack::PopAndDestroy(3, cSelect); // cSelect, mediaIds, fSelect

	iControllerCount = iControllers.Count();
	if (iControllerCount == 0)
		User::Leave(KErrNotSupported);

	TUid formatUidPrimary;
	TUid formatUidSecondary;
	if (mode == ERecord)
		{
		formatUidSecondary = iFormatUid;
		formatUidPrimary = iFormatUidSecondary;
		}
	else
		{
		formatUidPrimary = iFormatUid;
		formatUidSecondary = iFormatUidSecondary;
		}

	for (TInt controllerIndex=0; controllerIndex < iControllers.Count(); controllerIndex++)
		{
		const RMMFFormatImplInfoArray& recFormatInfo = iControllers[controllerIndex]->RecordFormats();
		const RMMFFormatImplInfoArray& playFormatInfo = iControllers[controllerIndex]->PlayFormats();

		TBool playFormatMatched = EFalse;
		TBool recordFormatMatched = EFalse;

		if (formatUidPrimary == KNullUid)
			{
			playFormatMatched = ETrue;
			}
		else
			{
			for(TInt playFormatIndex =0; playFormatIndex < playFormatInfo.Count(); playFormatIndex++)
				{
				if(playFormatInfo[playFormatIndex]->Uid() == formatUidPrimary)
					{
					playFormatMatched = ETrue;
					break;
					}
				}
			}

		if (formatUidSecondary == KNullUid)
			{
			recordFormatMatched = ETrue;
			}
		else
			{
			for (TInt recFormatIndex =0; recFormatIndex < recFormatInfo.Count(); recFormatIndex++)
				{
				if (recFormatInfo[recFormatIndex]->Uid() == formatUidSecondary)
					{
					recordFormatMatched = ETrue;
					break;
					}
				}
			}

		if (playFormatMatched && recordFormatMatched)
			User::LeaveIfError(iPrioritisedControllers.Append(iControllers[controllerIndex]));
		}

	iControllerCount = iPrioritisedControllers.Count();
	if (iControllerCount == 0)
		User::Leave(KErrNotSupported);
	}

CMMSourceSink* CMMFFindAndOpenController::CreateSourceSinkL(const TSourceSink& aParams)
	{
	if (aParams.iUseFileHandle)
		{
		return CMMFileSourceSink::NewL(aParams.iUid, aParams.iFileHandle);
		}
	return CMMSourceSink::NewL(aParams.iUid, aParams.iConfigData);
	}


CMMSourceSink* CMMFFindAndOpenController::CreateSourceSinkL(const TMMSource& aSource)
	{
	if (!(aSource.SourceType()==KUidMMFileSource ||
		aSource.SourceType()==KUidMMFileHandleSource))
		User::Leave(KErrNotSupported);

	return CMMFileSourceSink::NewL(KUidMmfFileSource, aSource);
	}



 CMMFFindAndOpenController::TSourceSink::TSourceSink(TUid aUid, const TDesC8& aConfigData)
	: iConfigData(aConfigData)
	{
	iUid = aUid;

	iUseFileHandle = EFalse;
	}

 CMMFFindAndOpenController::TSourceSink::TSourceSink(TUid aUid, const RFile& aFile)
	: iConfigData(KNullDesC8)
	{
	iUid = aUid;

	iFileHandle = aFile;
	iUseFileHandle = ETrue;
	}

CMMFFindAndOpenController::CConfig::CConfig()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::CConfig"));
    #endif
	}

void CMMFFindAndOpenController::CConfig::Close()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::Close"));
    #endif
	delete iSource;
	iSource = NULL;
	delete iSink;
	iSink = NULL;
	}
CMMFFindAndOpenController::CConfig::~CConfig()
	{
    #if _DEBUG
      RDebug::Print(_L("CMMFFindAndOpenController::~CConfig"));
    #endif
	Close();
	}

 CMMSourceSink* CMMSourceSink::NewLC(TUid aUid, const TDesC8& aDescriptor)
	{
	CMMSourceSink* self = new (ELeave) CMMSourceSink(aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aDescriptor);
	return self;
	}

 CMMSourceSink* CMMSourceSink::NewL(TUid aUid, const TDesC8& aDescriptor)
	{
	CMMSourceSink* sourcesink = CMMSourceSink::NewLC(aUid, aDescriptor);
	CleanupStack::Pop(sourcesink);
	return sourcesink;
	}

CMMSourceSink::CMMSourceSink(TUid aUid)
	: iUid(aUid)
	{
	}

CMMSourceSink::~CMMSourceSink()
	{
	delete iBuf;
	}

void CMMSourceSink::ConstructL(const TDesC8& aDescriptor)
	{
	iBuf = aDescriptor.AllocL();
	}

TUid CMMSourceSink::SourceSinkUid() const
	{
	return iUid;
	}

const TDesC8& CMMSourceSink::SourceSinkData() const
	{
	return *iBuf;
	}

 CMMFileSourceSink* CMMFileSourceSink::NewLC(TUid aUid, const RFile& aFile)
	{
	CMMFileSourceSink* self = new (ELeave) CMMFileSourceSink(aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	return self;
	}

 CMMFileSourceSink* CMMFileSourceSink::NewL(TUid aUid, const RFile& aFile)
	{
	CMMFileSourceSink* sourcesink = CMMFileSourceSink::NewLC(aUid, aFile);
	CleanupStack::Pop(sourcesink);
	return sourcesink;
	}

CMMFileSourceSink::CMMFileSourceSink(TUid aUid)
	: CMMSourceSink(aUid)
	{
	}

void CMMFileSourceSink::ConstructL(const RFile& aFile)
	{
	iHandle.Duplicate(aFile);
	iUsingFileHandle = ETrue;
	iFileName = HBufC::NewMaxL(KMaxFileName);
	TPtr fileNamePtr = iFileName->Des();
	iHandle.Name(fileNamePtr);
	DoCreateFileHandleSourceConfigDataL();
	}
const TInt KExpandSize = 100;

void CMMFileSourceSink::DoCreateFileHandleSourceConfigDataL()
	{
	CBufFlat* buf = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream;
	stream.Open(*buf);
	CleanupClosePushL(stream);

	TPckgBuf<RFile*> fileptr(&iHandle);
	stream.WriteInt32L(KMMFileHandleSourceUid.iUid);

	stream.WriteL(fileptr);

	TInt length = 0;
	if (iUniqueId != NULL)
		length = iUniqueId->Length();
	stream.WriteInt32L(length);
	if (length>0)
		stream.WriteL(*iUniqueId);

	stream.WriteInt32L(iEnableUI);

	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);

	iSourceSinkData = buf->Ptr(0).AllocL();

	CleanupStack::PopAndDestroy(buf);
	}

const TDesC8& CMMFileSourceSink::SourceSinkData() const
	{
	ASSERT(iSourceSinkData);
	return *iSourceSinkData;
	}

CMMFileSourceSink::~CMMFileSourceSink()
	{
	iHandle.Close();
	delete iFileName;
	delete iSourceSinkData;
	delete iUniqueId;
	}

 CMMFileSourceSink* CMMFileSourceSink::NewLC(TUid aUid, const TMMSource& aSource)
	{
	CMMFileSourceSink* self = new (ELeave) CMMFileSourceSink(aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aSource);
	return self;
	}

 CMMFileSourceSink* CMMFileSourceSink::NewL(TUid aUid, const TMMSource& aSource)
	{
	CMMFileSourceSink* sourcesink = CMMFileSourceSink::NewLC(aUid, aSource);
	CleanupStack::Pop(sourcesink);
	return sourcesink;
	}

void CMMFileSourceSink::ConstructL(const TMMSource& aSource)
	{
	iUniqueId = aSource.UniqueId().AllocL();
	iIntent = aSource.Intent();
	iEnableUI = aSource.IsUIEnabled();

	if (aSource.SourceType() == KUidMMFileSource)
		{
		const TMMFileSource& fileSource = static_cast<const TMMFileSource&>(aSource);
		iFileName = fileSource.Name().AllocL();

		DoCreateFileSourceConfigDataL();
		}
	else if (aSource.SourceType() == KUidMMFileHandleSource)
		{
		const TMMFileHandleSource& fileHandleSource = static_cast<const TMMFileHandleSource&>(aSource);
		iHandle.Duplicate(fileHandleSource.Handle());
		iUsingFileHandle = ETrue;
		iFileName = HBufC::NewMaxL(KMaxFileName);
		TPtr fileNamePtr = iFileName->Des();
		iHandle.Name(fileNamePtr);

		DoCreateFileHandleSourceConfigDataL();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

void CMMSourceSink::EvaluateIntentL()
	{
	}

void CMMFileSourceSink::EvaluateIntentL()
	{
	if (iUsingFileHandle)
		{
   		ContentAccess::CContent* Content = ContentAccess::CContent::NewLC(iHandle);
   		Content->OpenContentLC(iIntent, *iUniqueId);
   		CleanupStack::PopAndDestroy(2, Content);
		}
	else
		{
		ContentAccess::CContent* Content = ContentAccess::CContent::NewLC(*iFileName);
   		Content->OpenContentLC(iIntent, *iUniqueId);
   		CleanupStack::PopAndDestroy(2, Content);
		}
	}



 void CMMFileSourceSink::EvaluateIntentL(ContentAccess::TIntent aIntent)
	{
	if (iUsingFileHandle)
		{
   		ContentAccess::CContent* Content = ContentAccess::CContent::NewLC(iHandle);
   		Content->OpenContentLC(aIntent, *iUniqueId);
   		CleanupStack::PopAndDestroy(2, Content);
		}
	else
		{
		ContentAccess::CContent* Content = ContentAccess::CContent::NewLC(*iFileName);
   		Content->OpenContentLC(aIntent, *iUniqueId);
   		CleanupStack::PopAndDestroy(2, Content);
		}
	}

void CMMFileSourceSink::DoCreateFileSourceConfigDataL()
	{
	CBufFlat* buf = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream stream;
	stream.Open(*buf);
	CleanupClosePushL(stream);

	stream.WriteInt32L(KMMFileSourceUid.iUid);
	stream.WriteInt32L(iFileName->Length());
	stream.WriteL(*iFileName);
	TInt length = 0;
	if (iUniqueId != NULL)
		length = iUniqueId->Length();
	stream.WriteInt32L(length);
	if (length>0)
		stream.WriteL(*iUniqueId);

	stream.WriteInt32L(iEnableUI);

	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);

	iSourceSinkData = buf->Ptr(0).AllocL();

	CleanupStack::PopAndDestroy(buf);
	}



