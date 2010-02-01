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

#ifndef TMS_CLIENT_SINK_BODY_IMPL_H
#define TMS_CLIENT_SINK_BODY_IMPL_H

#include <tms.h>
#include "tmsclientsinkbody.h"
#include "tmsqueuehandler.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSClientSinkObserver;
class TMSBuffer;
class TMSCallProxy;

// TMSClientSinkBodyImpl class
class TMSClientSinkBodyImpl : public TMSClientSinkBody,
                              public MQueueHandlerObserver
    {
public:
    static gint Create(TMSClientSinkBody*& bodyimpl);
    // From TMSClientSinkBody begins

    virtual ~TMSClientSinkBodyImpl();

    virtual gint AddObserver(TMSClientSinkObserver& obsrvr,
            gpointer user_data);

    virtual gint RemoveObserver(TMSClientSinkObserver& obsrvr);

    // Push mode
    virtual gint BufferProcessed(TMSBuffer* buffer);

    virtual gint GetType(TMSSinkType& sinktype);

    // From TMSClientSinkBody ends

    // From MQueueHandlerObserver starts
    virtual void QueueEvent(TInt aEventType, TInt aError, void* user_data);
    // From MQueueHandlerObserver ends

    void SetProxy(TMSCallProxy* aProxy, gpointer queuehandler);

private:
    TMSClientSinkBodyImpl();
    gint PostConstruct();

private:
    TMSClientSinkObserver* iObserver;
    gpointer iUserData;
    TMSCallProxy* iProxy;
    };

} //namespace TMS

#endif // TMS_CLIENT_SINK_BODY_IMPL_H

// End of file
