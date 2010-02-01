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


#ifndef __AUDIOPDPLAYPROPERTIES_H__
#define __AUDIOPDPLAYPROPERTIES_H__



#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "MAudioPlayUtility.h"
#include "AudioPdPlayUtility.h"


#include "MmfClientUtility.h"

#include <mmf/common/mmcaf.h>

static const TUid KUidMmfAudioController = {KMmfUidControllerAudio};

/**
Mixin class to allow notification that the timed silence has finished.
*/
class MRepeatTrailingSilenceTimerObs
	{
public:
	virtual void RepeatTrailingSilenceTimerComplete() = 0;
	};

/**
CTimer-based active object that waits the requested time before notifying its observer.
*/
class CRepeatTrailingSilenceTimer : public CTimer
	{
public:
	static CRepeatTrailingSilenceTimer* NewL(MRepeatTrailingSilenceTimerObs& aObs);
	void RunL();
private:
	CRepeatTrailingSilenceTimer(MRepeatTrailingSilenceTimerObs& aObs);
private:
	MRepeatTrailingSilenceTimerObs& iObs;
	};

/**
Active object utility class to allow the callback to be called asynchronously.
This should help prevent re-entrant code in clients of the mediaframework.
*/
class CMMFMdaAudioPlayerCallBack : public CActive
	{
public:
	enum TCallbackState {
		ECallbackInitComplete,
		ECallbackPlayComplete,
		ECallbackPlaying,
		ECallbackPaused
		};


public:
	static CMMFMdaAudioPlayerCallBack* NewL(MAudioPdPlayUtilityCallback& aCallback);
	~CMMFMdaAudioPlayerCallBack();
	void InitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void PlayComplete(TInt aError);
	void PlayingCallback();
	void PausedCallback();
private:
	CMMFMdaAudioPlayerCallBack(MAudioPdPlayUtilityCallback& aCallback);
	void RunL();
	void DoCancel();
private:
	MAudioPdPlayUtilityCallback& iCallback;
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;
	TCallbackState iState;
	};



/**
Concrete implementation of the CAudioPdPlayUtility API.
@see CAudioPdPlayUtility
*/

class CAudioPdProperties: public CBase,
                         // public MAudioPdPlayUtility //may not be necessary
						 public MMMFControllerEventMonitorObserver,
					     public MRepeatTrailingSilenceTimerObs,
						 public MMMFFindAndOpenControllerObserver
	{

public:

	friend class CAudioPdPlayUtility;


		enum TMMFAudioPlayerState
		{
		EStopped,
		EOpening,
		EPaused,
		EPlaying
		};

	static CAudioPdProperties* NewL(MAudioPdPlayUtilityCallback& aCallback);
	static CAudioPdProperties* NewLC(MAudioPdPlayUtilityCallback& aCallback);

	~CAudioPdProperties();


   //For Download Status
    void HandleDownloadEventL(TUint aTransactionID,
                              TBrCtlDownloadEvent aEvent,
                              TUint aValue);


	 //For Audio control
	void Play();
	void Stop();
	TInt Pause();
	void SetVolume(TInt aVolume);
	void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	const TTimeIntervalMicroSeconds& Duration();
	TInt MaxVolume();
	void Close();
	TInt GetPosition(TTimeIntervalMicroSeconds& aPosition);
	void SetPosition(const TTimeIntervalMicroSeconds& aPosition);

	TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);
	TInt GetVolume(TInt& aVolume);
	TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);
	CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);
	TInt SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd);
	TInt ClearPlayWindow();
	TInt SetBalance(TInt aBalance = KMMFBalanceCenter);
	TInt GetBalance(TInt& aBalance);
	TInt GetBitRate(TUint& aBitRate);

	void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);
	void GetAudioLoadingProgressL(TInt& aPercentageProgress);
	const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);
	void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);


	MMMFDRMCustomCommand* GetDRMCustomCommand();


	// from MMMFControllerEventMonitorObserver
	virtual void HandleEvent(const TMMFEvent& aEvent);
	// from MRepeatTrailingSilenceTimerObs
	virtual void RepeatTrailingSilenceTimerComplete();

	// from MMMFFindAndOpenControllerObserver
	virtual void MfaocComplete(
		TInt& aError,
		RMMFController* aController,
		TUid aControllerUid,
		TMMFMessageDestination* aSourceHandle,
		TMMFMessageDestination* aSinkHandle);


	void OpenFileL(const TDesC& aFileName);


	void OpenFileL(const RFile& aFile);

	void OpenFileL(const TMMSource& aSource);


    void DlCompleteOpenFileL(const TDesC& aFileName);

	TInt GetFilePosition(TInt& aFilePosition);
	TInt SetFileSize(TInt aFileSize);
	TInt SetBytesDownloaded(TInt aBytesDownloaded, TBool aDownloadComplete);

	TInt CheckAudioPlayerState();

    void DoPlay();

protected:

private:
	CAudioPdProperties(/*MMdaAudioPlayerCallback*/MAudioPdPlayUtilityCallback& aCallback);
	void ConstructL(MAudioPdPlayUtilityCallback& aCallback);


	MAudioPdPlayUtilityCallback* iAudioPdPlayCallback;

	// last play window command
	enum TPlayWindowCommand
		{
		ENone,
		ESet,
		EClear
		};

	/*MMdaAudioPlayerCallback*/MAudioPdPlayUtilityCallback& iCallback;
	CMMFMdaAudioPlayerCallBack* iAsyncCallBack;
	MAudioLoadingObserver* iLoadingObserver;
	RMMFController iController;
	CMMFControllerEventMonitor* iControllerEventMonitor;
	TMMFAudioPlayerState iState;
	TTimeIntervalMicroSeconds iDuration; // Needed because of api "Duration()" that returns a reference
	TMMFPrioritySettings iPrioritySettings;

	TInt iNumberOfTimesPlayed;
	TInt iNumberOfTimesToRepeat;
	TTimeIntervalMicroSeconds iTrailingSilence;
	CRepeatTrailingSilenceTimer* iRepeatTrailingSilenceTimer;

	// Source and sink handle info
	TMMFMessageDestination* iSourceHandle;
	TMMFMessageDestination iSinkHandle;

	// Custom command handlers
	RMMFAudioPlayDeviceCustomCommands iAudioPlayDeviceCommands;
	RMMFAudioPlayControllerCustomCommands iAudioPlayControllerCommands;


	RMMFDRMCustomCommands iDRMCustomCommands;


	// Current playback time so we can resume from where we were stopped
	TTimeIntervalMicroSeconds iPosition;

	// Play window start and end times and whether it has been set
	TTimeIntervalMicroSeconds iPlayStart;
	TTimeIntervalMicroSeconds iPlayEnd;
	TPlayWindowCommand iPlayWindowSet;
	RArray<TUid> iMediaIds;

	CMMFControllerImplementationInformation* iControllerImplementationInformation;
	TUid iControllerUid;

	// utility class to find and open a suitable controller asynchronously
	CMMFFindAndOpenController* iFindAndOpenController;


	};

#endif
