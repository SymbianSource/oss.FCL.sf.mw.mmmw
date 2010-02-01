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
#include "tmsutility.h"
#include "tmsclientsinkimpl.h"
#include "tmsclientsinkbodyimpl.h"

using namespace TMS;

TMSClientSinkImpl::TMSClientSinkImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

TMSClientSinkImpl::~TMSClientSinkImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

gint TMSClientSinkImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSClientSinkBody* bodyimpl(NULL);
    TRACE_PRN_FN_ENT;
    ret = TMSClientSinkBodyImpl::Create(bodyimpl);

    if (ret == TMS_RESULT_SUCCESS)
        {
        this->iBody = bodyimpl;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSClientSinkImpl::Create(TMSSink*& tmssink)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSClientSinkImpl *self = new TMSClientSinkImpl;

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
    tmssink = self;
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSClientSinkImpl::SetProxy(TMSCallProxy* aProxy, gpointer queuehandler)
    {
    gint ret = TMS_RESULT_SUCCESS;
    if (this->iBody)
        {
        ((TMSClientSinkBodyImpl*) this->iBody)->SetProxy(aProxy, queuehandler);
        }
    else
        {
        ret = TMS_RESULT_UNINITIALIZED_OBJECT;
        }
    return ret;
    }

// End of file
