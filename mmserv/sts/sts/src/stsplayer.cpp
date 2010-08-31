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
 * This file provides the implementation for creating and deleting a
 * an MMF based player for playing and stopping a tone playback.
 */

//  Include Files  
#include "stsplayer.h"
#include <AudioPreference.h>
#include <systemtoneservice.h>
#include "stsplayersettings.h"
#include "stssettingsmanager.h"

/*static*/CStsPlayer* CStsPlayer::Create(MStsPlayerObserver& aObserver,
        CStsSettingsManager& aSettingsManager, unsigned int aContext,
        CSystemToneService::TAlarmType aAlarm)
    {
    CStsPlayer* self = 0;
    CStsSettingsManager::MPlayerSettings& playerSettings =
            aSettingsManager.GetPlayerSettings(aAlarm);
    TUint audioPreference;
    TUint audioPriority;
    aSettingsManager.GetAudioPriorityPref(aAlarm, audioPriority,
            audioPreference);
    self = new CStsPlayer(aObserver, playerSettings, aContext,
            audioPreference, audioPriority);
    if (self != 0)
        {
        bool successful = self->Init();
        if (!successful)
            {
            delete self;
            self = 0;
            }
        }
    return self;
    }

/*static*/CStsPlayer* CStsPlayer::Create(MStsPlayerObserver& aObserver,
        CStsSettingsManager& aSettingsManager, unsigned int aContext,
        CSystemToneService::TToneType aTone)
    {
    CStsPlayer* self = 0;
    CStsSettingsManager::MPlayerSettings& playerSettings =
            aSettingsManager.GetPlayerSettings(aTone);
    TUint audioPreference;
    TUint audioPriority;
    aSettingsManager.GetAudioPriorityPref(aTone, audioPriority,
            audioPreference);
    self = new CStsPlayer(aObserver, playerSettings, aContext,
            audioPreference, audioPriority);
    if (self != 0)
        {
        bool successful = self->Init();
        if (!successful)
            {
            delete self;
            self = 0;
            }
        }
    return self;
    }

CStsPlayer::CStsPlayer(MStsPlayerObserver& aObserver,
        CStsSettingsManager::MPlayerSettings& aPlayerSettings,
        unsigned int aContext, TUint aAudioPreference, TUint aAudioPriority) :
    iObserver(aObserver), iPlayerSettings(aPlayerSettings),
            iContext(aContext), iAudioPreference(aAudioPreference),
            iAudioPriority(aAudioPriority), iPlayer(0)
    {
    }

bool CStsPlayer::Init()
    {
    TRAPD(result, iPlayer = CMdaAudioPlayerUtility::NewL(*this));
    return result == KErrNone;
    }

CStsPlayer::~CStsPlayer()
    {
    delete iPlayer;
    }

void CStsPlayer::Play()
    {
    // Play the tone
    TRAPD( err, iPlayer->OpenFileL(iPlayerSettings.GetFileName()) );
    // If there is an error, indicate that the playback is complete. 
    if (err)
        {
        //TODO: Add trace here
        iObserver.PlayComplete(iContext);
        }
    }

void CStsPlayer::Stop()
    {
    iPlayer->Stop();
    }

void CStsPlayer::MapcInitComplete(TInt aError,
        const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    if (aError == KErrNone)
        {
        TInt64 volumex100 = iPlayer->MaxVolume()
                * iPlayerSettings.GetVolumePercentage();
        TInt volume = volumex100 / 100;
        // Set PriorityPref
        iPlayer->SetPriority(iAudioPriority, iAudioPreference);
        iPlayer->SetRepeats(iPlayerSettings.GetNumberOfRepeats(),
                iPlayerSettings.GetRepeatDelay());
        iPlayer->SetVolume(volume);
        iPlayer->SetVolumeRamp(iPlayerSettings.GetVolumeRamp());
        iPlayer->Play();
        }
    else
        {
        //TODO: add trace
        // Since there is an error, indicate that the playback is complete
        RDebug::Print(_L("InitCompleteReturned"), aError);
        iObserver.PlayComplete(iContext);
        }
    }

void CStsPlayer::MapcPlayComplete(TInt aError)
    {
    if (aError != KErrNone)
        {
        //TODO: add trace
        }
    iObserver.PlayComplete(iContext);
    }
