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
#include "tmscallbodyimpl.h"

using namespace TMS;

TMSCallBodyImpl::TMSCallBodyImpl()
    {
    }

TMSCallBodyImpl::~TMSCallBodyImpl()
    {
    }

gint TMSCallBodyImpl::Create(TMSCallBody*& bodyimpl)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSCallBodyImpl* self = new TMSCallBodyImpl();
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

gint TMSCallBodyImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_SUCCESS);
    return ret;
    }

TMSCallType TMSCallBodyImpl::GetCallType()
    {
    TMSCallType ctype(-1);
    return ctype;
    }

gint TMSCallBodyImpl::GetCallContextId(guint& /*ctxid*/)
    {
    gint ret(TMS_RESULT_SUCCESS);
    return ret;
    }

gint TMSCallBodyImpl::CreateStream(TMSStreamType /*type*/, TMSStream*& /*strm*/)
    {
    gint ret(TMS_RESULT_SUCCESS);
    return ret;
    }

gint TMSCallBodyImpl::DeleteStream(TMSStream*& /*strm*/)
    {
    gint ret(TMS_RESULT_SUCCESS);
    return ret;
    }

// End of file
