/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Telephony Multimedia Service
 *
 */

#include <tms.h>
#include <tmseffectobsrvr.h>
#include "tmsproxy.h"
#include "tmsutility.h"
#include "tmsglobalvoleffectbodyimpl.h"

using namespace TMS;

TMSGlobalVolEffectBodyImpl::TMSGlobalVolEffectBodyImpl() :
    iObserver(NULL),
    iProxy(NULL),
    iGlobalEffect(NULL)
    {
    }

TMSGlobalVolEffectBodyImpl::~TMSGlobalVolEffectBodyImpl()
    {
    if (iProxy)
        {
        iProxy->Close();
        delete iProxy;
        iProxy = NULL;
        }
    iObserver = NULL;
    iGlobalEffect = NULL;
    iUserData = NULL;
    }

gint TMSGlobalVolEffectBodyImpl::Create(TMSGlobalVolEffectBody*& bodyimpl)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSGlobalVolEffectBodyImpl* self = new TMSGlobalVolEffectBodyImpl;
    if (self)
        {
        ret = self->PostConstruct();
        if (ret != TMS_RESULT_SUCCESS)
            {
            delete self;
            self = NULL;
            }
        }
    bodyimpl = self;
    return ret;
    }

gint TMSGlobalVolEffectBodyImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_SUCCESS);
    iClientId = 1;
    iProxy = new TMSProxy;
    if (iProxy)
        {
        if (iProxy->Connect() != TMS_RESULT_SUCCESS)
            {
            delete iProxy;
            iProxy = NULL;
            ret = TMS_RESULT_FATAL_ERROR;
            }
        }
    else
        {
        ret = TMS_RESULT_UNINITIALIZED_OBJECT;
        }
    return ret;
    }

gint TMSGlobalVolEffectBodyImpl::AddObserver(TMSEffectObserver& obsrvr,
        gpointer /*user_data*/)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (!iObserver)
        {
        iObserver = &obsrvr;
        //iUserData = user_data;
        if (iProxy)
            {
            ret = iProxy->SetMsgQueueNotifier(EMsgQueueGlobalVolumeType,
                    iObserver, iGlobalEffect, iClientId);
            if (ret == TMS_RESULT_SUCCESS)
                {
                ret = iProxy->StartGlobalEffectNotifier();
                }
            }
        else
            {
            ret = TMS_RESULT_DOES_NOT_EXIST;
            }
        }
    else
        {
        ret = TMS_RESULT_ALREADY_EXIST;
        }
    return ret;
    }

gint TMSGlobalVolEffectBodyImpl::RemoveObserver(TMSEffectObserver& obsrvr)
    {
    gint ret(TMS_RESULT_SUCCESS);

    if (iProxy && (&obsrvr == iObserver))
        {
        iProxy->RemoveMsgQueueNotifier(EMsgQueueGlobalVolumeType, iObserver);
        iObserver = NULL;
        //ret = iProxy->StopEffectNotifier(obsrvr);
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGlobalVolEffectBodyImpl::GetLevel(guint& level)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (iProxy)
        {
        ret = iProxy->GetLevel(level);
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGlobalVolEffectBodyImpl::SetLevel(const guint level)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (iProxy)
        {
        ret = iProxy->SetLevel(level);
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGlobalVolEffectBodyImpl::GetMaxLevel(guint& level)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (iProxy)
        {
        ret = iProxy->GetMaxLevel(level);
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGlobalVolEffectBodyImpl::GetType(TMSEffectType& effecttype)
    {
    gint ret(TMS_RESULT_SUCCESS);
    effecttype = TMS_EFFECT_GLOBAL_VOL;
    return ret;
    }

void TMSGlobalVolEffectBodyImpl::SetParentEffect(TMSEffect*& parenteffect)
    {
    iGlobalEffect = NULL;
    iGlobalEffect = parenteffect;
    }

// End of file
