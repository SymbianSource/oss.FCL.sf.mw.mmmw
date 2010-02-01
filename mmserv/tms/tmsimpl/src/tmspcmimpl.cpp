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
#include "tmspcmbodyimpl.h"
#include "tmspcmformatimpl.h"

using namespace TMS;

TMSPCMFormatImpl::TMSPCMFormatImpl()
    {
    }

TMSPCMFormatImpl::~TMSPCMFormatImpl()
    {
    }

gint TMSPCMFormatImpl::PostConstruct()
    {
    TRACE_PRN_FN_ENT;
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSPCMFormatBody* bodyimpl(NULL);

    ret = TMSPCMFormatBodyImpl::Create(bodyimpl);

    if (ret == TMS_RESULT_SUCCESS)
        {
        this->iBody = bodyimpl;
        this->iFormatBody = (TMSPCMFormatBodyImpl*) bodyimpl;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSPCMFormatImpl::Create(TMSFormat*& tmsformat)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSPCMFormatImpl *self = new TMSPCMFormatImpl;

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
    tmsformat = self;
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSPCMFormatImpl::SetProxy(TMSGlobalContext* context,
        gpointer queuehandler)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (this->iBody)
        {
        ((TMSPCMFormatBodyImpl*) this->iBody)->SetProxy(context, queuehandler);
        }
    else
        {
        ret = TMS_RESULT_UNINITIALIZED_OBJECT;
        }
    return ret;
    }

// End of file
