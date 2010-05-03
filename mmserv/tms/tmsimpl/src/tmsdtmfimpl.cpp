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

#include "tmsutility.h"
#include "tmsdtmfbodyimpl.h"
#include "tmsdtmfimpl.h"

using namespace TMS;

TMSDTMFImpl::TMSDTMFImpl(TMSStreamType streamtype)
    {
    TRACE_PRN_FN_ENT;
    iStreamType = streamtype;
    TRACE_PRN_FN_EXT;
    }

TMSDTMFImpl::~TMSDTMFImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

gint TMSDTMFImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSDTMFBody* bodyimpl(NULL);
    TRACE_PRN_FN_ENT;
    ret = TMSDTMFBodyImpl::Create(iStreamType, bodyimpl);

    if (ret == TMS_RESULT_SUCCESS)
        {
        this->iBody = bodyimpl;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

EXPORT_C gint TMSDTMFImpl::Create(TMSStreamType streamtype, TMSDTMF*& dtmf)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSDTMFImpl *self = new TMSDTMFImpl(streamtype);

    TRACE_PRN_FN_ENT;
    if (self)
        {
        ret = self->PostConstruct();
        if (ret != TMS_RESULT_SUCCESS)
            {
            delete self;
            self = NULL;
            }
        }
    if (self && ret == TMS_RESULT_SUCCESS)
        {
        dtmf = self;
        ret = self->SetParent(dtmf);
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

EXPORT_C gint TMSDTMFImpl::Delete(TMSDTMF*& dtmf)
    {
    gint ret(TMS_RESULT_SUCCESS);
    TRACE_PRN_FN_ENT;
    delete (static_cast<TMSDTMFImpl*>(dtmf));
    dtmf = NULL;
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSDTMFImpl::SetParent(TMSDTMF*& parent)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (this->iBody)
        {
        static_cast<TMSDTMFBodyImpl*>(this->iBody)->SetParent(parent);
        }
    else
        {
        ret = TMS_RESULT_UNINITIALIZED_OBJECT;
        }
    return ret;
    }

