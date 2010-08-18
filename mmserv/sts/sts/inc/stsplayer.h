/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 * This file defines the interface for creating, playing,
 * stopping, and deleting an MMF player for a tone type.
 */
#ifndef STSPLAYER_H_
#define STSPLAYER_H_


#include <systemtoneservice.h>
#include <mdaaudiosampleplayer.h>


class MStsPlayerObserver
    {
public:
    virtual void PlayComplete(unsigned int aContext) = 0;
    };

class CStsPlayer : private MMdaAudioPlayerCallback
    {
public:

    static CStsPlayer* CreateTonePlayer(MStsPlayerObserver& aObserver,
            CSystemToneService::TToneType aTone, unsigned int aContext, 
           	const TDesC& aFileName, TInt aVolume, 
           	TUint aAudioPreference, TUint  aAudioPriority);
    static CStsPlayer* CreateAlarmPlayer(MStsPlayerObserver& aObserver,
            CSystemToneService::TAlarmType aAlarm, unsigned int aContext,
            const TDesC& aFileName, TInt aVolume,
    				TUint aAudioPreference, TUint  aAudioPriority);	
    virtual ~CStsPlayer();
    void Play();
    void Stop();
    
    
protected:
        
    CStsPlayer(MStsPlayerObserver& aObserver, const TDesC& aFileName,
    int aRepeatNumberOfTimes, unsigned int aContext,
    TInt aVolume, TUint aAudioPreference, TUint aAudioPriority);
    bool Init();

private:
    void MapcInitComplete(TInt aError,
    const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);

    MStsPlayerObserver& iObserver;
    CMdaAudioPlayerUtility* iPlayer;
    TPtrC iFileName;
    int iRepeatNumberOfTimes;
    unsigned int iContext;

	TInt  iVolume;
    TBool iWarningToneEnabled;
    TUint iAudioPreference;
    TUint iAudioPriority;
		
    };

#endif // STSPLAYER_H_
