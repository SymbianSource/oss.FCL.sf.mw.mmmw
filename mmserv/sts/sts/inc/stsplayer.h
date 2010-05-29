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

#include <mdaaudiosampleplayer.h>
#include <systemtoneservice.h>

class MProEngEngine;
class MProEngProfile;

class MStsPlayerObserver
    {
public:
    virtual void PlayComplete(unsigned int aContext) = 0;
    };

class CStsPlayer : private MMdaAudioPlayerCallback
    {
public:

    static CStsPlayer* CreateTonePlayer(MStsPlayerObserver& aObserver,
            CSystemToneService::TToneType aTone, unsigned int aContext);
    static CStsPlayer* CreateAlarmPlayer(MStsPlayerObserver& aObserver,
            CSystemToneService::TAlarmType aAlarm, unsigned int aContext);
    virtual ~CStsPlayer();
    void Play();
    void Stop();


protected:
    
    void LoadActiveProfileSettingsL();
    void SetToneSettings(CSystemToneService::TToneType aTone);
    void SetAlarmSettings(CSystemToneService::TAlarmType aAlarm);
    CStsPlayer(MStsPlayerObserver& aObserver, const TDesC& aFileName,
    int aRepeatNumberOfTimes, unsigned int aContext);
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

    MProEngEngine* iEngine;
    MProEngProfile* iProfile;

    TInt iVolume;
    TBool iWarningToneEnabled;
    TUint iAudioPreference;
    TUint iAudioPriority;

    };

#endif // STSPLAYER_H_
