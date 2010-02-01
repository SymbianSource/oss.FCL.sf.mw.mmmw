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
#include "tmsglobalgaineffectbodyimpl.h"
#include "tmsutility.h"
#include "tmsglobalgaineffectimpl.h"

using namespace TMS;

TMSGlobalGainEffectImpl::TMSGlobalGainEffectImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

TMSGlobalGainEffectImpl::~TMSGlobalGainEffectImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

gint TMSGlobalGainEffectImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSGlobalGainEffectBody* bodyimpl(NULL);
    TRACE_PRN_FN_ENT;
    ret = TMSGlobalGainEffectBodyImpl::Create(bodyimpl);

    if (ret == TMS_RESULT_SUCCESS)
        {
        this->iBody = bodyimpl;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSGlobalGainEffectImpl::Create(TMSEffect*& tmseffect)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSGlobalGainEffectImpl *self = new TMSGlobalGainEffectImpl;

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
        tmseffect = self;
        self->SetParentEffect(tmseffect);
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSGlobalGainEffectImpl::SetParentEffect(TMSEffect*& parenteffect)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (this->iBody)
        {
        ((TMSGlobalGainEffectBodyImpl*) this->iBody)->SetParentEffect(
                parenteffect);
        }
    else
        {
        ret = TMS_RESULT_UNINITIALIZED_OBJECT;
        }
    return ret;
    }

// End of file
