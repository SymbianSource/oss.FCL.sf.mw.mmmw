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

#ifndef TMS_FORMAT_BODY_H
#define TMS_FORMAT_BODY_H

#include <glib.h>

namespace TMS {

// FORWARD DECLARATIONS
class TMSCallProxy;

// Class TMSFormatBody
class TMSFormatBody
    {
public:
    virtual ~TMSFormatBody() {}
    virtual gint GetSupportedBitRates(BitRateVector& aVector) = 0;
    virtual gint SetBitRate(const guint aBitrate) = 0;
    virtual gint GetBitRate(guint& aBitrate) = 0;
    virtual gint GetType(TMSFormatType& fmttype) = 0;
    virtual void SetProxy(TMSCallProxy* aProxy, gpointer queuehandler) = 0;
    };

} //namespace TMS

#endif /* TMS_STREAM_BODY_H */
