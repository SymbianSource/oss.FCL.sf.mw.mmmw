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

#include <MdaAudioSamplePlayer.h>
#include <systemtoneservice.h>

class MStsPlayerObserver
    {
public:
    virtual void PlayToneComplete(unsigned int aPlayToneContext) = 0;
    };

class CStsPlayer : private MMdaAudioPlayerCallback
    {
public:
    static CStsPlayer* Create(MStsPlayerObserver& aObserver,
            CSystemToneService::TToneType aToneType, unsigned int aPlayToneContext);
    virtual ~CStsPlayer();
    void Play();
    void Stop();

protected:
    CStsPlayer(MStsPlayerObserver& aObserver, const TDesC& aFileName,
            int aRepeatNumberOfTimes, unsigned int aPlayToneContext);
    bool Init();

private:
    void MapcInitComplete(TInt aError,
            const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);

    MStsPlayerObserver& iObserver;
    CMdaAudioPlayerUtility* iPlayer;
    TPtrC iFileName;
    int iRepeatNumberOfTimes;
    unsigned int iPlayToneContext;
    };

#endif // STSPLAYER_H_
