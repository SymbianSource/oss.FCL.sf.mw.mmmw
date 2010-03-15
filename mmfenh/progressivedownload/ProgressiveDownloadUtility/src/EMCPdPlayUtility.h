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


#ifndef __CAUDIOPDPLAYUTILITY_H__
#define __CAUDIOPDPLAYUTILITY_H__

#include <e32std.h>
#include <e32base.h>
//#include <mdaaudiosampleplayer.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include "mmfclientutility.h"

#include <mmf/common/mmcaf.h>

#include <downloadmgrclient.h>
#include "MAudioPdPlayUtility.h"
#include "MProgressiveDownloadUtility.h"


class CAudioPdProperties;

/**
Concrete implementation of the CAudioPdPlayUtility API.
@see CAudioPdPlayUtility
*/

class CAudioPdPlayUtility: public CBase,
                           public MAudioPdPlayUtility
                           //public MAudioPdPlayUtilityCallback
	{

public:



	static CAudioPdPlayUtility* NewL(MAudioPdPlayUtilityCallback& aCallback);

	~CAudioPdPlayUtility();

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


	void OpenFileL(const TDesC& aFileName);


	void OpenFileL(const RFile& aFile);

	void DlCompleteOpenFileL(const TDesC& aFileName);

	//Source CustomCommands
	TInt GetFilePosition(TInt& aFilePosition);
	TInt SetFileSize(TInt aFileSize);
	TInt SetBytesDownloaded(TInt aBytesDownloaded, TBool aDownloadComplete);

	TInt CheckAudioPlayerState();


protected:

private:


	CAudioPdPlayUtility();
	void ConstructL();




    CAudioPdProperties* iProperties;


	};

#endif
