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

class CSts::CPlayerNode
    {
public:
    CPlayerNode(CStsPlayer* aPlayer);
    CPlayerNode(CStsPlayer* aPlayer, MStsPlayAlarmObserver& aObserver);
    ~CPlayerNode();
    bool HasObserver();
    CStsPlayer* Player();
    MStsPlayAlarmObserver& Observer();
private:
    CStsPlayer* iPlayer;
    MStsPlayAlarmObserver* iObserver;
    };

CSts::CPlayerNode::CPlayerNode(CStsPlayer* aPlayer) :
    iPlayer(aPlayer), iObserver(0)
    {
    }

CSts::CPlayerNode::CPlayerNode(CStsPlayer* aPlayer,
        MStsPlayAlarmObserver& aObserver) :
    iPlayer(aPlayer), iObserver(&aObserver)
    {
    }

CSts::CPlayerNode::~CPlayerNode()
    {
    delete iPlayer;
    }

bool CSts::CPlayerNode::HasObserver()
    {
    return iObserver != 0;
    }

CStsPlayer* CSts::CPlayerNode::Player()
    {
    return iPlayer;
    }

MStsPlayAlarmObserver& CSts::CPlayerNode::Observer()
    {
    return *iObserver;
    }

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

void CSts::PlayTone(CSystemToneService::TToneType aTone)
    {
    CStsPlayer* player = CStsPlayer::CreateTonePlayer(*this, aTone,
            iNextContext);
    if (player != 0)
        {
        iPlayerMap[iNextContext] = new CPlayerNode(player);
        iNextContext++;
        if (iNextContext == 0)
            iNextContext++;
        player->Play();
        }
    else
        {
        //TODO: Add trace here
        }
    }

void CSts::PlayAlarm(CSystemToneService::TAlarmType aAlarm,
        unsigned int& aAlarmContext, MStsPlayAlarmObserver& aObserver)
    {
    CStsPlayer* player = CStsPlayer::CreateAlarmPlayer(*this, aAlarm,
            iNextContext);
    if (player != 0)
        {
        iPlayerMap[iNextContext] = new CPlayerNode(player, aObserver);
        aAlarmContext = iNextContext;
        iNextContext++;
        if (iNextContext == 0)
            iNextContext++;
        player->Play();
        }
    else
        {
        //TODO: Add trace here
        aAlarmContext = 0;
        }
    }

void CSts::StopAlarm(unsigned int aAlarmContext)
    {
    CPlayerNode* node = iPlayerMap[aAlarmContext];
    iPlayerMap.erase(aAlarmContext);
    if (node)
        {
        node->Player()->Stop();
        delete node;
        }
    else
        {
        //TODO: Add trace here
        }
    }

void CSts::CleanUpPlayers()
    {
        while (!iPlayerMap.empty())
            {
            //TODO: Add trace here
            StopAlarm(iPlayerMap.begin()->first);
            }
    }

void CSts::PlayComplete(unsigned int aContext)
    {
    CPlayerNode* node = iPlayerMap[aContext];
    iPlayerMap.erase(aContext);
    if (node)
        {
        CStsPlayer* player = node->Player();
        if (node->HasObserver())
            {
            node->Observer().PlayAlarmComplete(aContext);
            }
        delete node;
        }
    else
        {
        // TODO: log unexpected error
        }
    }
