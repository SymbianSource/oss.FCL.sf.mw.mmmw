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
#include <tmsclientsinkobsrvr.h>
#include "tmscallproxy.h"
#include "tmsmembuffer.h"
#include "tmsqueuehandler.h"
#include "tmsclientsinkbodyimpl.h"

using namespace TMS;

TMSClientSinkBodyImpl::TMSClientSinkBodyImpl() :
    iObserver(NULL),
    iProxy(NULL)
    {
    }

TMSClientSinkBodyImpl::~TMSClientSinkBodyImpl()
    {
    }

gint TMSClientSinkBodyImpl::Create(TMSClientSinkBody*& bodyimpl)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSClientSinkBodyImpl* self = new TMSClientSinkBodyImpl;
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

gint TMSClientSinkBodyImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_SUCCESS);
    return ret;
    }

gint TMSClientSinkBodyImpl::AddObserver(TMSClientSinkObserver& obsrvr,
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

gint TMSClientSinkBodyImpl::RemoveObserver(TMSClientSinkObserver& obsrvr)
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

// Push mode
gint TMSClientSinkBodyImpl::BufferProcessed(TMSBuffer* buffer)
    {
    // TODO send stream attributes here
    gint ret(TMS_RESULT_SUCCESS);

    if (iProxy)
        {
        ret = iProxy->BufferEmptied(TMS_CALL_IP,
                                    TMS_STREAM_UPLINK,
                                    1, //TODO: must use strm_id
                                    *buffer);
        }
    else
        {
        ret = TMS_RESULT_UNINITIALIZED_OBJECT;
        }
    return ret;
    }

gint TMSClientSinkBodyImpl::GetType(TMSSinkType& sinktype)
    {
    gint ret(TMS_RESULT_SUCCESS);
    sinktype = TMS_SINK_CLIENT;
    return ret;
    }

void TMSClientSinkBodyImpl::SetProxy(TMSCallProxy* aProxy,
        gpointer queuehandler)
    {
    iProxy = aProxy;
    if (queuehandler)
        {
        ((CQueueHandler*) queuehandler)->AddObserver(*this, TMS_SINK_CLIENT);
        }
    }

void TMSClientSinkBodyImpl::QueueEvent(TInt aEventType, TInt /*aError*/,
        void* user_data)
    {
    if (iObserver)
        {
        switch (aEventType)
            {
            case TMS_EVENT_SINK_PROCESS_BUFFER:
                iObserver->ProcessBuffer(((TMSBuffer*) user_data));
                break;
            default:
                break;
            }
        }
    }

// End of file
