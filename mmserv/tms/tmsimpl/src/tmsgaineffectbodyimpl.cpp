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
#include "tmscallproxy.h"
#include "tmsqueuehandler.h"
#include "tmsgaineffectbodyimpl.h"

using namespace TMS;

TMSGainEffectBodyImpl::TMSGainEffectBodyImpl() :
    iObserver(NULL),
    iProxy(NULL),
    iParentEffect(NULL)
    {
    }

TMSGainEffectBodyImpl::~TMSGainEffectBodyImpl()
    {
    }

gint TMSGainEffectBodyImpl::Create(TMSGainEffectBody*& bodyimpl)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSGainEffectBodyImpl* self = new TMSGainEffectBodyImpl;
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

gint TMSGainEffectBodyImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_SUCCESS);
    return ret;
    }

gint TMSGainEffectBodyImpl::AddObserver(TMSEffectObserver& obsrvr,
        gpointer user_data)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (!iObserver)
        {
        iObserver = &obsrvr;
        iUserData = user_data;
        }
    else
        {
        ret = TMS_RESULT_ALREADY_EXIST;
        }
    return ret;
    }

/**
 * Remove a stream observer from this stream.
 *
 * This function can be called at any time. It is recommended to remove
 * observer after calling Deinit() on stream. Else observer may receive
 * a callback that is alread dispatched.
 *
 * @param  obsrvr
 *      The listener to remove.
 *
 * @return
 *      TMS_RESULT_SUCCESS if the obsrvr is removed successfully from list.
 *      TMS_RESULT_DOES_NOT_EXIST if obsrvr is not already in the list.
 *
 */
gint TMSGainEffectBodyImpl::RemoveObserver(TMSEffectObserver& obsrvr)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (&obsrvr == iObserver)
        {
        iObserver = NULL;
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGainEffectBodyImpl::GetLevel(guint& level)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (iProxy)
        {
        ret = iProxy->GetGain(level);
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGainEffectBodyImpl::SetLevel(const guint level)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (iProxy)
        {
        ret = iProxy->SetGain(level);
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGainEffectBodyImpl::GetMaxLevel(guint& level)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (iProxy)
        {
        ret = iProxy->GetMaxGain(level);
        }
    else
        {
        ret = TMS_RESULT_DOES_NOT_EXIST;
        }
    return ret;
    }

gint TMSGainEffectBodyImpl::GetType(TMSEffectType& effecttype)
    {
    gint ret(TMS_RESULT_SUCCESS);
    effecttype = TMS_EFFECT_GAIN;
    return ret;
    }

gint TMSGainEffectBodyImpl::SetParentEffect(TMSEffect*& parenteffect)
    {
    gint ret(TMS_RESULT_SUCCESS);
    iParentEffect = NULL;
    iParentEffect = parenteffect;
    return ret;
    }

void TMSGainEffectBodyImpl::SetProxy(TMSCallProxy* aProxy,
        gpointer queuehandler)
    {
    iProxy = aProxy;
    ((TMSQueueHandler*) queuehandler)->AddObserver(*this, TMS_EFFECT_GAIN);
    }

void TMSGainEffectBodyImpl::QueueEvent(TInt aEventType, TInt aError,
        void* /*user_data*/)
    {
    TMSSignalEvent event;

    event.type = TMS_EVENT_EFFECT_GAIN_CHANGED;
    event.reason = aError;

    switch (aEventType)
        {
        case TMS_EVENT_EFFECT_GAIN_CHANGED:
            {
            if (iObserver && iParentEffect)
                {
                iObserver->EffectsEvent(iParentEffect, event);
                }
            }
            break;
        default:
            break;
        }
    }

// End of file
