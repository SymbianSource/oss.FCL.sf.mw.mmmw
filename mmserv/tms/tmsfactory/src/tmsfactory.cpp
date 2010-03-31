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

#include <tmsfactory.h>
#include "tmsfactoryimpl.h"

using namespace TMS;

TMSFactory::TMSFactory()
    {
    impl = new TMSFactoryImpl();
    }

EXPORT_C TMSFactory::~TMSFactory()
    {
    delete impl;
    }

EXPORT_C gint TMSFactory::CreateFactory(TMSFactory*& tmsfactory,
        TMSVer& /*ver*/)
    {
    TMSFactory* self = new TMSFactory();
    tmsfactory = self;
    return TMS_RESULT_SUCCESS;
    }

EXPORT_C gint TMSFactory::CreateCall(TMSCallType ctype, TMSCall*& tmscall,
        guint ctxid)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->CreateCall(ctype, tmscall, ctxid);
        }
    return status;
    }

EXPORT_C gint TMSFactory::DeleteCall(TMSCall*& tmscall)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->DeleteCall(tmscall);
        }
    return status;
    }

EXPORT_C gint TMSFactory::IsCallTypeSupported(TMSCallType ctype,
        gboolean& flag)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->IsCallTypeSupported(ctype, flag);
        }
    return status;
    }

EXPORT_C gint TMSFactory::GetSupportedFormats(const TMSStreamType strmtype,
        FormatVector& fmtlist)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->GetSupportedFormats(strmtype, fmtlist);
        }
    return status;
    }

EXPORT_C gint TMSFactory::CreateFormat(TMSFormatType tmsfmttype,
        TMSFormat*& tmsfmt)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->CreateFormat(tmsfmttype, tmsfmt);
        }
    return status;
    }

EXPORT_C gint TMSFactory::DeleteFormat(TMSFormat*& tmsfmt)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->DeleteFormat(tmsfmt);
        }
    return status;
    }

EXPORT_C gint TMSFactory::CreateEffect(TMSEffectType tmseffecttype,
        TMSEffect*& tmseffect)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->CreateEffect(tmseffecttype, tmseffect);
        }
    return status;
    }

EXPORT_C gint TMSFactory::DeleteEffect(TMSEffect*& tmseffect)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->DeleteEffect(tmseffect);
        }
    return status;
    }

EXPORT_C gint TMSFactory::CreateBuffer(TMSBufferType buffertype, guint size,
        TMSBuffer*& tmsbuffer)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->CreateBuffer(buffertype, size, tmsbuffer);
        }
    return status;
    }

EXPORT_C gint TMSFactory::DeleteBuffer(TMSBuffer*& tmsbuffer)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->DeleteBuffer(tmsbuffer);
        }
    return status;
    }

EXPORT_C gint TMSFactory::CreateSource(TMSSourceType srctype,
        TMSSource*& tmssrc)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->CreateSource(srctype, tmssrc);
        }
    return status;
    }

EXPORT_C gint TMSFactory::DeleteSource(TMSSource*& tmssrc)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->DeleteSource(tmssrc);
        }
    return status;
    }

EXPORT_C gint TMSFactory::CreateSink(TMSSinkType sinktype, TMSSink*& tmssink)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->CreateSink(sinktype, tmssink);
        }
    return status;
    }

EXPORT_C gint TMSFactory::DeleteSink(TMSSink*& tmssink)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->DeleteSink(tmssink);
        }
    return status;
    }

EXPORT_C gint TMSFactory::CreateGlobalRouting(TMSGlobalRouting*& globrouting)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->CreateGlobalRouting(globrouting);
        }
    return status;
    }

EXPORT_C gint TMSFactory::DeleteGlobalRouting(TMSGlobalRouting*& globrouting)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (impl)
        {
        status = impl->DeleteGlobalRouting(globrouting);
        }
    return status;
    }

