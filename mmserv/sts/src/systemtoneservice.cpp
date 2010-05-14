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
 * This file is the direct implementation of the System Tone Service
 * API.  This acts as a facade to the actual implementation logic
 * which is in the CStsImplementation class.
 */

//  Include Files  
#include <systemtoneservice.h>	// CSystemToneService
#include "stsimplementation.h"

//  Member Functions
EXPORT_C /*static*/CSystemToneService* CSystemToneService::Create()
    {
    CSystemToneService* self = 0;
    CStsImplementation* implementation = CStsImplementation::Create();
    if (implementation != 0)
        {
        self = new CSystemToneService(*implementation);
        if (self == 0)
            {
            delete implementation;
            }
        }
    return self;
    }

EXPORT_C /*static*/void CSystemToneService::Delete(
        CSystemToneService* aSystemToneService)
    {
    delete &(aSystemToneService->iImplementation);
    delete aSystemToneService;
    }

CSystemToneService::CSystemToneService(CStsImplementation& aImplementation) :
    iImplementation(aImplementation)
    {
    }

CSystemToneService::~CSystemToneService()
    {
    }

EXPORT_C void CSystemToneService::PlayTone(TToneType aTone)
    {
    iImplementation.PlayTone(aTone);
    }

EXPORT_C void CSystemToneService::PlayAlarm(TAlarmType aAlarm,
        unsigned int& aAlarmContext, MStsPlayAlarmObserver& aObserver)
    {
    iImplementation.PlayAlarm(aAlarm, aAlarmContext, aObserver);
    }

EXPORT_C void CSystemToneService::StopAlarm(unsigned int aAlarmContext)
    {
    iImplementation.StopAlarm(aAlarmContext);
    }
