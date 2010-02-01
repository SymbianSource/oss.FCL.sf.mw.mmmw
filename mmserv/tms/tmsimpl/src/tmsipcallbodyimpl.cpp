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
#include <tmsstream.h>
#include "tmscallproxy.h"
#include "tmsstreamimpl.h"
#include "tmsutility.h"
#include "tmsipcallbodyimpl.h"

using namespace TMS;

TMSIPCallBodyImpl::TMSIPCallBodyImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

TMSIPCallBodyImpl::~TMSIPCallBodyImpl()
    {
    TRACE_PRN_FN_ENT;
    if (iSession)
        {
        iSession->DeleteCall();
        iSession->Close();
        delete iSession;
        }
    TRACE_PRN_FN_EXT;
    }

gint TMSIPCallBodyImpl::Create(TMSCallBody*& bodyimpl)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TMSIPCallBodyImpl* self = new TMSIPCallBodyImpl;

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
    bodyimpl = self;
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSIPCallBodyImpl::PostConstruct()
    {
    gint ret(TMS_RESULT_SUCCESS);
    TRACE_PRN_FN_ENT;
    iSession = new TMSCallProxy();

    if (iSession)
        {
        if (iSession->Connect() != TMS_RESULT_SUCCESS)
            {
            delete iSession;
            iSession = NULL;
            ret = TMS_RESULT_FATAL_ERROR;
            }
        else
            {
            ret = iSession->CreateCall(TMS_CALL_IP);
            if (ret != TMS_RESULT_SUCCESS && ret != TMS_RESULT_ALREADY_EXIST)
                {
                iSession->Close();
                delete iSession;
                ret = TMS_RESULT_FATAL_ERROR;
                }
            }
        }
    else
        {
        ret = TMS_RESULT_INSUFFICIENT_MEMORY;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

TMSCallType TMSIPCallBodyImpl::GetCallType()
    {
    TMSCallType ctype(TMS_CALL_IP);
    return ctype;
    }

gint TMSIPCallBodyImpl::GetCallContextId(guint& ctxid)
    {
    gint ret(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    ctxid = 0;
    return ret;
    }

gint TMSIPCallBodyImpl::CreateStream(TMSStreamType type, TMSStream*& strm)
    {
    gint ret(TMS_RESULT_CALL_TYPE_NOT_SUPPORTED);

    if (strm)
        {
        return TMS_RESULT_INVALID_ARGUMENT;
        }

    switch (type)
        {
        case TMS_STREAM_UPLINK:
        case TMS_STREAM_DOWNLINK:
            ret = TMSStreamImpl::Create(TMS_CALL_IP, type, iSession, strm);
            if (ret == TMS_RESULT_SUCCESS)
                {
                ret = AddStreamToList(strm);
                }           
            //TODO:Need longer term fix to not destory everything
            //if more the one stream is trying to be created.
            else if (ret == TMS_RESULT_ALREADY_EXIST)
                {
                break;
                }
            else
                {
                if (iSession)
                    {
                    //TODO: delete call only if no active sessions left
                    iSession->DeleteCall();
                    iSession->Close();
                    delete iSession;
                    iSession = NULL;
                    }
                else
                    {
                    ret = TMS_RESULT_UNINITIALIZED_OBJECT;
                    }
                }
            break;
        default:
            break;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSIPCallBodyImpl::DeleteStream(TMSStream*& strm)
    {
    gint ret(TMS_RESULT_SUCCESS);
    TRACE_PRN_FN_ENT;
    ret = RemStreamFromList(strm);
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSIPCallBodyImpl::AddStreamToList(TMSStream* strm)
    {
    gint ret(TMS_RESULT_SUCCESS);
    TRACE_PRN_FN_ENT;
    iStreamsVector.push_back(strm);
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint TMSIPCallBodyImpl::RemStreamFromList(TMSStream*& strm)
    {
    gint ret(TMS_RESULT_DOES_NOT_EXIST);
    gint nArraySize = (gint) iStreamsVector.size();
    // get an iterator to the start of the array
    std::vector<TMSStream*>::iterator itStrm = iStreamsVector.begin();

    // display the arrays' contents

    TRACE_PRN_FN_ENT;
    for (; itStrm < iStreamsVector.end(); itStrm++)
        {
        if (strm == *itStrm)
            {
            break;
            }
        }

    if (itStrm)
        {
        iStreamsVector.erase(itStrm); // Remove from array
        // Don't delete itStrm as it is now pointing to the next item on the list
        delete strm; // Free memory
        strm = NULL;
        ret = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

// End of file
