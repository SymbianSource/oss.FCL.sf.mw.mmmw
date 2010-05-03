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
 * This file provides the implementation of System Tone Service used
 * by the STS Server.
 */

#include "sts.h"

/*static*/CSts* CSts::Create()
    {
    CSts* self = new CSts();
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

/*static*/void CSts::Delete(CSts* aSts)
    {
    delete aSts;
    }

CSts::CSts() :
    iNextContext(1)
    {
    }

bool CSts::Init()
    {
    return true;
    }

CSts::~CSts()
    {
    CleanUpPlayers();
    }

void CSts::PlayTone(CSystemToneService::TToneType aToneType,
        unsigned int& aPlayToneContext)
    {
    CStsPlayer* player = CStsPlayer::Create(*this, aToneType, iNextContext);
    if (player != 0)
        {
        iMap[iNextContext] = player;
        aPlayToneContext = iNextContext;
        iNextContext++;
        player->Play();
        }
    else
        {
        aPlayToneContext = 0;
        }
    }

void CSts::StopTone(unsigned int aPlayToneContext)
    {
    CStsPlayer* player = iMap[aPlayToneContext];
    if (player)
        {
        player->Stop();
        PlayToneComplete(aPlayToneContext);
        }
    }

void CSts::CleanUpPlayers()
    {
    for (TPlayerMap::iterator i = iMap.begin(); i != iMap.end(); i++)
        {
        StopTone(i->first);
        }
    }

void CSts::PlayToneComplete(unsigned int aPlayToneContext)
    {
    CStsPlayer* player = iMap[aPlayToneContext];
    iMap[aPlayToneContext] = 0;
    if (player)
        {
        delete player;
        }
    }
