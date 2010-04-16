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
 * This class provides callback mechanism for TMSDTMF listeners.
 *
 */

#ifndef TMS_DTMF_OBSRVR_H
#define TMS_DTMF_OBSRVR_H

// Include files
#include <glib.h>

namespace TMS {

// FORWARD DECLARATIONS
class TMSDTMF;

/**
 * TMSDTMFObserver class
 *
 * This class provides callback mechanism for TMSDTMF listeners.
 *
 * Client class that wants to receive events from TMS must inherit from
 * TMSDTMFObserver class and register itself with the TMSDTMF object.
 *
 * Functions declared in this class are called upon TMS events.
 *
 * The client must not do any blocking, or perform any CPU-intensive operations
 * in the callback methods. It is recommended that the client saves event data,
 * returns from this function immediately and if necessary, responds to
 * the event in asynchronous way.
 *
 * @lib tmsapi.lib
 *
 */
class TMSDTMFObserver
    {
public:
    /**
     * Notified events:
     *  TMS_EVENT_DTMF_TONE_STARTED
     *  TMS_EVENT_DTMF_TONE_STOPPED

     * @param  dtmf
     *      Pointer to the object that is being signaled.
     *
     * @param  event
     *      Event descriptor.
     */
    virtual void DTMFEvent(TMSDTMF* dtmf, TMSSignalEvent event) = 0;
    };

} //namespace TMS

#endif // TMS_DTMF_OBSRVR_H

// End of file
