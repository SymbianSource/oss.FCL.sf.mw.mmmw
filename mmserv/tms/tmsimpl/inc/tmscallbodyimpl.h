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

#ifndef TMS_CALL_BODY_IMPL_H
#define TMS_CALL_BODY_IMPL_H

#include <glib.h>
#include "tmscallbody.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSStream;
class TMSDTMF;
class TMSRingTone;

// TMSCallBodyImpl class
class TMSCallBodyImpl : public TMSCallBody
    {
public:
    static gint Create(TMSCallBody*& bodyimpl);
    virtual ~TMSCallBodyImpl();

    // From TMSCallBody
    virtual TMSCallType GetCallType();
    virtual gint GetCallContextId(guint& ctxid);
    virtual gint CreateStream(TMSStreamType type, TMSStream*& strm);
    virtual gint DeleteStream(TMSStream*& strm);

private:
    TMSCallBodyImpl();
    gint PostConstruct();
    };

} //namespace TMS

#endif // TMS_CALL_BODY_IMPL_H

// End of file
