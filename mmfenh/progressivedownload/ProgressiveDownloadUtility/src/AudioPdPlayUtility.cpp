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

#include <bautils.h>
#include <utf.h>
#include <mmfpaniccodes.h>
#include "AudioPdPlayUtility.h"
#include "AudioPdProperties.h"
#include "progressivedownloadsourceuids.hrh"

#include <AudioPreference.h>

//void Panic(TInt aPanicCode);

const TUid KUidProgressiveDlSource	= {KProgressiveDownloadSourceUid};

void Panic(TInt aPanicCode)
	{
	_LIT(KProgressiveDownloadUtility, "ProgressiveDownloadUtility");
	User::Panic(KProgressiveDownloadUtility, aPanicCode);
	}

CAudioPdPlayUtility* CAudioPdPlayUtility::NewL(MAudioPdPlayUtilityCallback& aCallback)
	{
    CAudioPdPlayUtility* self = new(ELeave) CAudioPdPlayUtility();
	CleanupStack::PushL(self);

 	self->iProperties = CAudioPdProperties::NewL(aCallback);
	CleanupStack::Pop(self);
	return self;
	}

CAudioPdPlayUtility::~CAudioPdPlayUtility()
	{
	delete iProperties;
	}

CAudioPdPlayUtility::CAudioPdPlayUtility()
	{

	}

void CAudioPdPlayUtility::ConstructL()
	{

	}

//For Download Status
void CAudioPdPlayUtility::HandleDownloadEventL(TUint aTransactionID,
                                               TBrCtlDownloadEvent aEvent,
                                               TUint aValue)
	{
    iProperties->HandleDownloadEventL(aTransactionID,aEvent,aValue);
	}


void CAudioPdPlayUtility::Play()
	{
    iProperties->Play();
	}
void CAudioPdPlayUtility::Stop()
	{
    iProperties->Stop();
	}
TInt CAudioPdPlayUtility::Pause()
	{
    return iProperties->Pause();
	}
void CAudioPdPlayUtility::SetVolume(TInt aVolume)
	{
    iProperties->SetVolume(aVolume);
	}
void CAudioPdPlayUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
    iProperties->SetRepeats(aRepeatNumberOfTimes,aTrailingSilence);
	}
void CAudioPdPlayUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
    iProperties->SetVolumeRamp(aRampDuration);
	}
const TTimeIntervalMicroSeconds& CAudioPdPlayUtility::Duration()
	{
	return iProperties->Duration();
	}
TInt CAudioPdPlayUtility:: MaxVolume()
	{
	return iProperties->MaxVolume();
	}
void CAudioPdPlayUtility::Close()
	{
    iProperties->Close();
	}
TInt CAudioPdPlayUtility::GetPosition(TTimeIntervalMicroSeconds& aPosition)
	{
    return iProperties->GetPosition(aPosition);
	}
void CAudioPdPlayUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
    iProperties->SetPosition(aPosition);
	}

TInt CAudioPdPlayUtility::SetPriority(TInt aPriority, TMdaPriorityPreference aPref)
	{
    return iProperties->SetPriority(aPriority,aPref);
	}
TInt CAudioPdPlayUtility::GetVolume(TInt& aVolume)
	{
    return iProperties->GetVolume(aVolume);
	}
TInt CAudioPdPlayUtility::GetNumberOfMetaDataEntries(TInt& aNumEntries)
	{
    return iProperties->GetNumberOfMetaDataEntries(aNumEntries);
	}


CMMFMetaDataEntry* CAudioPdPlayUtility::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	return iProperties->GetMetaDataEntryL(aMetaDataIndex);
	}

TInt CAudioPdPlayUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd)
	{
    return iProperties->SetPlayWindow(aStart,aEnd);
	}

TInt CAudioPdPlayUtility::ClearPlayWindow()
	{
    return iProperties->ClearPlayWindow();
	}
TInt CAudioPdPlayUtility::SetBalance(TInt aBalance)
	{
    return iProperties->SetBalance(aBalance);
	}
TInt CAudioPdPlayUtility::GetBalance(TInt& aBalance)
	{
    return iProperties->GetBalance(aBalance);
	}
TInt CAudioPdPlayUtility::GetBitRate(TUint& aBitRate)
	{
    return iProperties->GetBitRate(aBitRate);
	}


void CAudioPdPlayUtility::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback)
	{
    iProperties->RegisterForAudioLoadingNotification(aCallback);
	}

void CAudioPdPlayUtility::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
    iProperties->GetAudioLoadingProgressL(aPercentageProgress);
	}

const CMMFControllerImplementationInformation& CAudioPdPlayUtility::ControllerImplementationInformationL()
	{
	return iProperties->ControllerImplementationInformationL();
	}


TInt CAudioPdPlayUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
    return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
TInt CAudioPdPlayUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
    return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
void CAudioPdPlayUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
void CAudioPdPlayUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

void CAudioPdPlayUtility::OpenFileL(const TDesC& aFileName)
	{
	iProperties->OpenFileL(aFileName);
	}


void CAudioPdPlayUtility::OpenFileL(const RFile& aFile)
	{
	RFile& file = const_cast<RFile&>(aFile);
	iProperties->OpenFileL(file);
	}


void CAudioPdPlayUtility::OpenFileL(const TMMSource& aSource)
	{
	iProperties->OpenFileL(aSource);
	}

void CAudioPdPlayUtility::DlCompleteOpenFileL(const TDesC& aFileName)
	{
	iProperties->DlCompleteOpenFileL(aFileName);
	}

TInt CAudioPdPlayUtility::GetFilePosition(TInt& aFilePosition)
	{
	return iProperties->GetFilePosition(aFilePosition);
	}

TInt CAudioPdPlayUtility::SetFileSize(TInt aFileSize)
	{
	return iProperties->SetFileSize(aFileSize);
	}

TInt CAudioPdPlayUtility::SetBytesDownloaded(TInt aBytesDownloaded, TBool aDownloadComplete)
	{
	return iProperties->SetBytesDownloaded(aBytesDownloaded,aDownloadComplete);
	}

MMMFDRMCustomCommand* CAudioPdPlayUtility::GetDRMCustomCommand()
	{
	//ASSERT(iProperties);
	return iProperties->GetDRMCustomCommand();
	}


TInt CAudioPdPlayUtility::CheckAudioPlayerState()
	{
	return iProperties->CheckAudioPlayerState();
	}


//===============================================================
// CAudioPdProperties implementation begins
//
//===============================================================
CAudioPdProperties* CAudioPdProperties::NewL(MAudioPdPlayUtilityCallback& aCallback)
	{
	CAudioPdProperties* self = new(ELeave) CAudioPdProperties(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aCallback);
	CleanupStack::Pop(self);
	return self;
	}
CAudioPdProperties* CAudioPdProperties::NewLC(MAudioPdPlayUtilityCallback& /*aCallback*/)
	{
	return NULL;
	}

CAudioPdProperties::~CAudioPdProperties()
	{

	delete iControllerImplementationInformation;
	delete iAsyncCallBack;
	delete iRepeatTrailingSilenceTimer;
	delete iFindAndOpenController;
	delete iControllerEventMonitor;
	iMediaIds.Close();
	iController.Close();
	}

CAudioPdProperties::CAudioPdProperties(/*MMdaAudioPlayerCallback*/MAudioPdPlayUtilityCallback& aCallback):
	iCallback(aCallback),
	iAudioPlayDeviceCommands(iController),
	iAudioPlayControllerCommands(iController),
	iDRMCustomCommands(iController)
	{
	iState = EStopped;
	iPrioritySettings.iPriority = KAudioPriorityRealOnePlayer;//80
	iPrioritySettings.iPref =(TMdaPriorityPreference) KAudioPrefRealOneLocalPlayback;// 0x01420001;
	iPlayStart = TTimeIntervalMicroSeconds(0);
	iPlayEnd = TTimeIntervalMicroSeconds(0);
	iPlayWindowSet = ENone;
    iSourceHandle =  NULL;
	}

void CAudioPdProperties::ConstructL(MAudioPdPlayUtilityCallback& /*aCallback*/)
	{

	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	iRepeatTrailingSilenceTimer = CRepeatTrailingSilenceTimer::NewL(*this);
	iAsyncCallBack = CMMFMdaAudioPlayerCallBack::NewL(iCallback);
	User::LeaveIfError(iMediaIds.Append(KUidMediaTypeAudio));
	iFindAndOpenController = CMMFFindAndOpenController::NewL(*this);
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);
	iFindAndOpenController->ConfigureController(iController, *iControllerEventMonitor, CMMFFindAndOpenController::EPlayback);

	}

void CAudioPdProperties::MfaocComplete(
		TInt& aError,
		RMMFController* /*aController*/,
		TUid aControllerUid,
		TMMFMessageDestination* aSourceHandle,
		TMMFMessageDestination* /*aSinkHandle*/)
	{

	iSourceHandle = aSourceHandle;

	if (aError == KErrNone)
		{
		iControllerUid = aControllerUid;

		// Get the clip duration
		iDuration = TTimeIntervalMicroSeconds(0);
		aError = iController.GetDuration(iDuration);

		// If an error occurred in any of the above, close the controller.
		if (aError != KErrNone)
			{
			iControllerEventMonitor->Cancel();
			iController.Close();
			}
		}

	iAsyncCallBack->InitComplete(aError, iDuration);
	}

void CAudioPdProperties::HandleDownloadEventL(TUint aTransactionID,
                                              TBrCtlDownloadEvent aEvent,
                                              TUint aValue)
	{
	iCallback.HandleDownloadEventL(aTransactionID,aEvent,aValue);

	}


void CAudioPdProperties::OpenFileL(const TDesC& aFileName)
	{
	TMMFileSource filesource(aFileName, ContentAccess::KDefaultContentObject, ContentAccess::EPlay);
	OpenFileL(filesource);
	}


void CAudioPdProperties::OpenFileL(const RFile& aFile)
	{
	RFile& file = const_cast<RFile&>(aFile);
	TMMFileHandleSource filesource(file, KNullDesC, ContentAccess::EPlay);
	OpenFileL(filesource);
	}

void CAudioPdProperties::OpenFileL(const TMMSource& aSource)
	{
	// If iAsyncCallBack is already active, we're still in the process of notifying the client
	// that a previous request to Open...(...) has completed.
	if (iAsyncCallBack->IsActive())
		User::Leave(KErrNotReady);


	if (aSource.SourceType()==KUidMMFileHandleSource)
		{

		RFile& fileHandle = static_cast<const TMMFileHandleSource&>(aSource).Handle();

	    iFindAndOpenController->ConfigureSourceSink(
	       CMMFFindAndOpenController::TSourceSink(KUidProgressiveDlSource,fileHandle),
		   CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));

		}
	if (aSource.SourceType()==KUidMMFileSource)
		{
		const TDesC& fileName = static_cast<const TMMFileSource&>(aSource).Name();

	    iFindAndOpenController->ConfigureSourceSink(
	       CMMFFindAndOpenController::TSourceSink(KUidProgressiveDlSource,
	       CMMFFindAndOpenController::GetConfigFile(fileName)),
		   CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));

		}

	iFindAndOpenController->OpenByFileSource(aSource);
	}

void CAudioPdProperties::DlCompleteOpenFileL(const TDesC& aFileName)
	{
	if (iAsyncCallBack->IsActive())
		User::Leave(KErrNotReady);

	Close();

	TMMFileSource filesource(aFileName, ContentAccess::KDefaultContentObject, ContentAccess::EPlay);


	iFindAndOpenController->ConfigureSourceSink(
			TMMFileSource(aFileName, filesource.UniqueId(), filesource.Intent(), filesource.IsUIEnabled()),
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));

	iFindAndOpenController->OpenByFileSource(filesource);


	}


void CAudioPdProperties::Play()
	{
	// if we're already playing, call the client's callback with KErrNotReady.
	// This is what the controller would do if we allowed the Play()
	// to propagate down. Need to do it here too (for consistency)
	// in case we're in a trailing silence period.
    if (iState == EPlaying)
		{
		iAsyncCallBack->PlayComplete(KErrNotReady);
		return;
		}

	// cancel the repeat timer in case the client has called Play()
	// without waiting for the previous play to complete
	iRepeatTrailingSilenceTimer->Cancel();
	// Reset played count
	iNumberOfTimesPlayed = 0;

	DoPlay();
	}

void CAudioPdProperties::DoPlay()
	{
#if defined(__AUDIO_PROFILING)
	RDebug::ProfileStart(4);
#endif  // defined(__AUDIO_PROFILING)
    TInt err = KErrNone;
    if (iState != EPaused)
        {
		err = iController.Prime();

#if defined(__AUDIO_PROFILING)
	RDebug::ProfileEnd(4);
#endif  // defined(__AUDIO_PROFILING)

		// make sure we don't set the position outside the play window -
		// but allow it to remain unchanged if it's within the window
		if (iPlayWindowSet == ESet &&
			(iPosition < iPlayStart || iPosition >= iPlayEnd))
			iPosition = iPlayStart;

		if (err==KErrNone)
			err = iController.SetPosition(iPosition);
        }

	if (err==KErrNone)
		{
		if (iPlayWindowSet == ESet)
			err = iAudioPlayControllerCommands.SetPlaybackWindow(iPlayStart, iPlayEnd);
		else if (iPlayWindowSet == EClear)
			{
			err = iAudioPlayControllerCommands.DeletePlaybackWindow();
			iPlayWindowSet = ENone;	// assume window will stay cleared
			}
		}

	if (err==KErrNone)
		{
#if defined(__AUDIO_PROFILING)
		RDebug::ProfileStart(5);
#endif  // defined(__AUDIO_PROFILING)

		err = iController.Play();

#if defined(__AUDIO_PROFILING)
		RDebug::ProfileEnd(5);
#endif  // defined(__AUDIO_PROFILING)
		}

	if (err!=KErrNone)
		iAsyncCallBack->PlayComplete(err);
	else
		{
		iState = EPlaying;
		iCallback.Playing();
		}
	}


void CAudioPdProperties::Stop()
	{
	if (iState==EPlaying || iState==EPaused)
		{
		// cancel the repeat timer in case the client has called Stop()
		// during the trailing silence period
		iRepeatTrailingSilenceTimer->Cancel();

		iController.Stop();
		iPosition = iPlayStart;
		iState = EStopped;
		}

	}
TInt CAudioPdProperties::Pause()
	{
    TInt err = KErrNone;
	if (iState==EPlaying)
		{
		err = iController.Pause();
		if (!err || err==KErrNotReady)
			err = iController.GetPosition(iPosition);
		iState = EPaused;
		iCallback.Paused();
		}
	return err;
	}
void CAudioPdProperties::SetVolume(TInt aVolume)
	{
	TInt err = iAudioPlayDeviceCommands.SetVolume(aVolume);

	__ASSERT_ALWAYS(err==KErrNone, Panic(EMMFMediaClientBadArgument));
	}
void CAudioPdProperties::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	iNumberOfTimesToRepeat = aRepeatNumberOfTimes;
	iTrailingSilence = aTrailingSilence;
	}
void CAudioPdProperties::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	iAudioPlayDeviceCommands.SetVolumeRamp(aRampDuration);
	}
const TTimeIntervalMicroSeconds& CAudioPdProperties::Duration()
	{
	TInt err = iController.GetDuration(iDuration);
	if (err)
		{
		iDuration = 0;
		}
	return iDuration;
	}
TInt CAudioPdProperties:: MaxVolume()
	{
		TInt maxVolume = 0;
#ifdef _DEBUG
	TInt error =
#endif
		iAudioPlayDeviceCommands.GetMaxVolume(maxVolume);
	__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem));
	return maxVolume;
	}
void CAudioPdProperties::Close()
	{
	// Reset the audio player state.
	Stop();
	iControllerEventMonitor->Cancel();
	iController.Close();
	}
TInt CAudioPdProperties::GetPosition(TTimeIntervalMicroSeconds& aPosition)
	{
    TInt error = KErrNone;
//	if (iState==EPlaying)
   //For progressivedownload pause and resume I need to get the
   //last position even if not playing.
	error = iController.GetPosition(iPosition);

	aPosition = iPosition;
	return error;
	}
void CAudioPdProperties::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	// Clip the position if aPosition is greater than the duration
	// or if aPosition is negative.
	const TTimeIntervalMicroSeconds maxPosition(Duration());
	const TTimeIntervalMicroSeconds minPosition(0);

	if (aPosition > maxPosition)
		iPosition = maxPosition;
	else if (aPosition < minPosition)
		iPosition = minPosition;
	else
		iPosition = aPosition;

    if (iState==EPlaying || iState==EPaused || iState == EStopped)
		{
		iController.SetPosition(iPosition);
		}
//	else if (iState == EPaused)
//		{
//		Stop();	// We call stop so that DevSound's internal buffers are reset
//		}
	}

TInt CAudioPdProperties::SetPriority(TInt aPriority, TMdaPriorityPreference aPref)
	{
    iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);

	return iController.SetPrioritySettings(iPrioritySettings);
	}
TInt CAudioPdProperties::GetVolume(TInt& aVolume)
	{
    TInt error = iAudioPlayDeviceCommands.GetVolume(aVolume);
	return error;
	}
TInt CAudioPdProperties::GetNumberOfMetaDataEntries(TInt& aNumEntries)
	{
    TInt error = iController.GetNumberOfMetaDataEntries(aNumEntries);
	return error;
	}


CMMFMetaDataEntry* CAudioPdProperties::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	return iController.GetMetaDataEntryL(aMetaDataIndex);
	}

TInt CAudioPdProperties::SetPlayWindow(const TTimeIntervalMicroSeconds& aPlayStart,
								const TTimeIntervalMicroSeconds& aPlayEnd)
	{
    TInt error = KErrNone;

	if (aPlayStart >= TTimeIntervalMicroSeconds(0) &&
		aPlayStart < iDuration &&
			aPlayStart < aPlayEnd &&
			aPlayEnd <= iDuration )
		{
		iPlayStart = aPlayStart;
		iPlayEnd = aPlayEnd;
		iPlayWindowSet = ESet;

		if (iState==EPlaying)
			error = iAudioPlayControllerCommands.SetPlaybackWindow(aPlayStart, aPlayEnd);
		}
	else
		error = KErrArgument;

	return error;
	}

TInt CAudioPdProperties::ClearPlayWindow()
	{
    	// clear play window start - very important because this is assigned
	// to iPosition when we stop & is used to set the position on the next Play()
	iPosition = iPlayStart = iPlayEnd = TTimeIntervalMicroSeconds(0);

	iPlayWindowSet = EClear;
	TInt err = KErrNone;
	if (iState==EPlaying)
		err = iAudioPlayControllerCommands.DeletePlaybackWindow();
	return err;
	}
TInt CAudioPdProperties::SetBalance(TInt aBalance)
	{
    TInt err = iAudioPlayDeviceCommands.SetBalance(aBalance);
	return err;
	}
TInt CAudioPdProperties::GetBalance(TInt& aBalance)
	{
	TInt err = iAudioPlayDeviceCommands.GetBalance(aBalance);
	return err;
	}

TInt CAudioPdProperties::GetBitRate(TUint& aBitRate)
	{
	RMMFAudioControllerCustomCommands controller(iController);
	TInt err = controller.GetSourceBitRate(aBitRate);
	return err;
	}

void CAudioPdProperties::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aLoadingObserver)
	{
    iLoadingObserver = &aLoadingObserver;
	}

void CAudioPdProperties::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
    User::LeaveIfError(iAudioPlayControllerCommands.GetLoadingProgress(aPercentageProgress));
	}

const CMMFControllerImplementationInformation& CAudioPdProperties::ControllerImplementationInformationL()
	{
	if (!iControllerImplementationInformation)
		{
		if (iControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iControllerUid);
		}
	return *iControllerImplementationInformation;
	}

TInt CAudioPdProperties::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
    return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
TInt CAudioPdProperties::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
    return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
void CAudioPdProperties::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
void CAudioPdProperties::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
    iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

TInt CAudioPdProperties::GetFilePosition(TInt& aFilePosition)
	{
  	TInt err = KErrNone;
  	TPckgBuf<TInt> dataFrom;

	if(iSourceHandle != NULL)
        {
    	TMMFMessageDestinationPckg destPckg(*iSourceHandle);
	    err =CustomCommandSync(destPckg, EGETFILEPOSITION, KNullDesC8, KNullDesC8, dataFrom);
	    }
	else
	    {
	    err = KErrNotFound;
	    }

    if(err==KErrNone)
       aFilePosition = dataFrom();

	return err;
    }

TInt CAudioPdProperties::SetFileSize(TInt aFileSize)
	{
  	TInt err = KErrNone;
	if(iSourceHandle != NULL)
        {
        TMMFMessageDestinationPckg destPckg(*iSourceHandle);
        TPckgBuf<TInt> data1(aFileSize);
        err = CustomCommandSync(destPckg, ESETFILESIZE, data1, KNullDesC8);
        }
	else
	    {
        err = KErrNotFound;
	    }
	return err;
	}

TInt CAudioPdProperties::SetBytesDownloaded(TInt aBytesDownloaded, TBool /*aDownloadComplete*/)
	{
  	TInt err = KErrNone;
	if(iSourceHandle != NULL)
        {
        TMMFMessageDestinationPckg destPckg(*iSourceHandle);
    	TPckgBuf<TInt> data1(aBytesDownloaded);
        err = CustomCommandSync(destPckg, ESETBYTESDOWNLOADED, data1, KNullDesC8);
        }
	else
	    {
	    err = KErrNotFound;
	    }

    return err;
	}


MMMFDRMCustomCommand* CAudioPdProperties::GetDRMCustomCommand()
	{
	if (iDRMCustomCommands.IsSupported())
		{
		return static_cast<MMMFDRMCustomCommand*>(&iDRMCustomCommands);
		}
	else
		{
		return NULL;
		}
	}


TInt CAudioPdProperties::CheckAudioPlayerState()
	{
	return iState;
	}
void CAudioPdProperties::HandleEvent(const TMMFEvent& aEvent)
	{
	// handle loading started/complete messages first, as the later code does not explicitly check the event type
	if (aEvent.iEventType==KMMFEventCategoryAudioLoadingStarted)
		{
		if (iLoadingObserver)
			iLoadingObserver->MaloLoadingStarted();
		}
	else if (aEvent.iEventType==KMMFEventCategoryAudioLoadingComplete)
		{
		if (iLoadingObserver)
			iLoadingObserver->MaloLoadingComplete();
		}
	else if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete)
		{
		TInt oldState = iState;
		iPosition = iPlayStart;
		if (aEvent.iErrorCode == KErrNone)
			{
			//If we weren't playing, ignore the event.
			if(oldState == EPlaying)
				{
				//we finished playing the clip so repeat if required
				iNumberOfTimesPlayed++;
				if ((iNumberOfTimesPlayed>iNumberOfTimesToRepeat) && (iNumberOfTimesToRepeat!=KMdaRepeatForever))
					{
					//we've repeated enough times now
					iNumberOfTimesPlayed = 0;
					iState = EStopped;
					iCallback.MapcPlayComplete(KErrNone);
					}
				else
					{
					// We need to repeat the clip
					// we're truncating the trailingsilence here so maximum time is ~37mins
               		TTimeIntervalMicroSeconds32 silence = I64INT(iTrailingSilence.Int64());
					iRepeatTrailingSilenceTimer->After(silence);
					}
				}
			}
		else
			{ //aEvent.iErrorCode != KErrNone
			//if we weren't playing, don't advise Client.
			iState = EStopped;
			if(oldState == EPlaying)
				iCallback.MapcPlayComplete(aEvent.iErrorCode);
			}
		}

	// else we have an unexpected event that cannot be dealt with by the client.
	// We will simply ignore this.
	}


void CAudioPdProperties::RepeatTrailingSilenceTimerComplete()
	{
	// reset the position for subsequent plays
	iPosition = iPlayStart;
	DoPlay();
	}


CRepeatTrailingSilenceTimer* CRepeatTrailingSilenceTimer::NewL(MRepeatTrailingSilenceTimerObs& aObs)
	{
	CRepeatTrailingSilenceTimer* s = new(ELeave) CRepeatTrailingSilenceTimer(aObs);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

void CRepeatTrailingSilenceTimer::RunL()
	{
	iObs.RepeatTrailingSilenceTimerComplete();
	}

CRepeatTrailingSilenceTimer::CRepeatTrailingSilenceTimer(MRepeatTrailingSilenceTimerObs& aObs) :
	CTimer(EPriorityHigh),
	iObs(aObs)
	{
	CActiveScheduler::Add(this);
	}


CMMFMdaAudioPlayerCallBack* CMMFMdaAudioPlayerCallBack::NewL(MAudioPdPlayUtilityCallback& aCallback)
	{
	return new(ELeave) CMMFMdaAudioPlayerCallBack(aCallback);
	}

CMMFMdaAudioPlayerCallBack::CMMFMdaAudioPlayerCallBack(MAudioPdPlayUtilityCallback& aCallback) :
	CActive(CActive::EPriorityHigh), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

CMMFMdaAudioPlayerCallBack::~CMMFMdaAudioPlayerCallBack()
	{
	Cancel();
	}

void CMMFMdaAudioPlayerCallBack::InitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	iState = ECallbackInitComplete;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

void CMMFMdaAudioPlayerCallBack::PlayComplete(TInt aError)
	{
	iError = aError;
	iState = ECallbackPlayComplete;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}


void CMMFMdaAudioPlayerCallBack::PlayingCallback()
	{
	iState = ECallbackPlaying;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

void CMMFMdaAudioPlayerCallBack::PausedCallback()
	{
	iState = ECallbackPaused;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}


void CMMFMdaAudioPlayerCallBack::RunL()
	{
	switch (iState)
		{
		case ECallbackInitComplete:
			iCallback.MapcInitComplete(iError, iDuration);
			break;
		case ECallbackPlayComplete:
			iCallback.MapcPlayComplete(iError);
			break;
		case ECallbackPlaying:
			iCallback.Playing();
			break;
		case ECallbackPaused:
			iCallback.Paused();
			break;

		}
	}

void CMMFMdaAudioPlayerCallBack::DoCancel()
	{
	// Nothing to cancel
	}

