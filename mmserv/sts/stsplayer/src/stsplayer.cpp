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
#include "stsplayer.h"
_LIT(KDefaultFile,"z:\\data\\sounds\\digital\\clock.aac");

/*static*/CStsPlayer* CStsPlayer::Create(MStsPlayerObserver& aObserver,
        CSystemToneService::TToneType aToneType, unsigned int aPlayToneContext)
    {
    CStsPlayer* self = 0;
    switch (aToneType)
        {
        case CSystemToneService::EClockAlarm:
            self = new CStsPlayer(aObserver, KDefaultFile, 10, aPlayToneContext);
            break;
        default:
            self = new CStsPlayer(aObserver, KDefaultFile, 0, aPlayToneContext);
            break;
        }
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

CStsPlayer::CStsPlayer(MStsPlayerObserver& aObserver, const TDesC& aFileName,
        int aRepeatNumberOfTimes, unsigned int aPlayToneContext) :
    iObserver(aObserver), iPlayer(0), iFileName(aFileName),
            iRepeatNumberOfTimes(aRepeatNumberOfTimes), iPlayToneContext(
                    aPlayToneContext)
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
    TRAP_IGNORE(iPlayer->OpenFileL(iFileName));
    }

void CStsPlayer::Stop()
    {
    iPlayer->Stop();
    }

void CStsPlayer::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    if (aError == KErrNone)
        {
        TTimeIntervalMicroSeconds delay = 0;
        iPlayer->SetRepeats(iRepeatNumberOfTimes, delay);
        iPlayer->Play();
        }
    else
        {
        //TODO: add trace
        }
    }

void CStsPlayer::MapcPlayComplete(TInt aError)
    {
    if (aError != KErrNone)
        {
        //TODO: add trace
        }
    iObserver.PlayToneComplete(iPlayToneContext);
    }
