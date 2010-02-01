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
#include "tmsg711bodyimpl.h"
#include "tmsg711formatimpl.h"

using namespace TMS;

TMSG711FormatImpl::TMSG711FormatImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

TMSG711FormatImpl::~TMSG711FormatImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

gint TMSG711FormatImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSG711FormatBody* bodyimpl(NULL);
    TRACE_PRN_FN_ENT;
    ret = TMSG711FormatBodyImpl::Create(bodyimpl);

    if (ret == TMS_RESULT_SUCCESS)
        {
        this->iBody = bodyimpl;
        this->iFormatBody = (TMSG711FormatBodyImpl*) bodyimpl;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSG711FormatImpl::Create(TMSFormat*& tmsformat)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSG711FormatImpl *self = new TMSG711FormatImpl;

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

gint TMSG711FormatImpl::SetProxy(TMSGlobalContext* context,
        gpointer queuehandler)
    {
    gint ret(TMS_RESULT_SUCCESS);
    if (this->iBody)
        {
        ((TMSG711FormatBodyImpl*) this->iBody)->SetProxy(context, queuehandler);
        }
    else
        {
        ret = TMS_RESULT_UNINITIALIZED_OBJECT;
        }
    return ret;
    }

// End of file
