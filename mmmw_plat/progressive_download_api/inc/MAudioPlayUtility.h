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
* Description:  Interface definition for the ProgressiveDownloadUtility functions.
*
*/



#ifndef __MAUDIOPLAYUTILITY_H__
#define __MAUDIOPLAYUTILITY_H__

#include <e32std.h>
#include <e32base.h>
#include <mdaaudiosampleplayer.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>

#include <bldvariant.hrh>

class MAudioPlayUtility
	{
	public:
      
	 virtual void Play()=0;
	 virtual void Stop()=0;
	 virtual TInt Pause()=0;
	 virtual void SetVolume(TInt aVolume)=0;
	 virtual void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)=0;
	 virtual void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)=0;
	 virtual const TTimeIntervalMicroSeconds& Duration()=0;
	 virtual TInt MaxVolume()=0;	
	 virtual void Close()=0;
	 virtual TInt GetPosition(TTimeIntervalMicroSeconds& aPosition)=0;
	 virtual void SetPosition(const TTimeIntervalMicroSeconds& aPosition)=0;

	 virtual TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref)=0;
	 virtual TInt GetVolume(TInt& aVolume)=0;
	 virtual TInt GetNumberOfMetaDataEntries(TInt& aNumEntries)=0;
	 virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex)=0;
	 virtual TInt SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd)=0;
	 virtual TInt ClearPlayWindow()=0;
	 virtual TInt SetBalance(TInt aBalance = KMMFBalanceCenter)=0;
	 virtual TInt GetBalance(TInt& aBalance)=0;
	 virtual TInt GetBitRate(TUint& aBitRate)=0;
	
	 virtual void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback)=0;
	 virtual void GetAudioLoadingProgressL(TInt& aPercentageProgress)=0;
	 virtual const CMMFControllerImplementationInformation& ControllerImplementationInformationL()=0;
	
	 virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)=0;
	 virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)=0;
	 virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)=0;
	 virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)=0;


	 virtual MMMFDRMCustomCommand* GetDRMCustomCommand()=0;


	};
	
#endif
