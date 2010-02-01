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

#ifndef TMS_PROXY_H
#define TMS_PROXY_H

// INCLUDES
#include <mmfbase.h>
#include <mmfutilities.h> //for TFourCC
#include <tms.h>
#include "tmsqueuehandler.h"
#include "tmsclientserver.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSEffect;
class TMSEffectObserver;
class TMSGlobalRouting;
class TMSGlobalRoutingObserver;

// TMSProxy class
class TMSProxy : public RSessionBase,
                 public MQueueHandlerObserver
    {
public:
    IMPORT_C TMSProxy();
    IMPORT_C virtual ~TMSProxy();
    /**
     * Opens a connection to the server.
     * @return KErrNone if successful connection created
     */
    IMPORT_C gint Connect();

    /**
     * @return The server version this session can connct to
     */
    IMPORT_C TVersion Version() const;

    /**
     * Close the session to the server
     */
    IMPORT_C void Close();

    IMPORT_C gint GetTMSCallSessionHandle();

    IMPORT_C gint GetSupportedDecoders(RArray<TUint32>& aDecoders,
            gint& aFrameSize);
    IMPORT_C gint GetSupportedEncoders(RArray<TUint32>& aEncoders,
            gint& aFrameSize);

    // TMSGlobalRouting
    IMPORT_C gint SetOutput(TMSAudioOutput output);
    IMPORT_C gint GetOutput(TMSAudioOutput& output);
    IMPORT_C gint GetPreviousOutput(TMSAudioOutput& output);
    IMPORT_C gint GetAvailableOutputs(OutputVector& outputs);

    // TMSGlobal Volume & Gain
    IMPORT_C gint GetLevel(guint& level);
    IMPORT_C gint GetMaxLevel(guint& level);
    IMPORT_C gint SetLevel(guint level);
    IMPORT_C gint GetGain(guint& level);
    IMPORT_C gint GetMaxGain(guint& level);
    IMPORT_C gint SetGain(guint level);

    // Global notifiers
    IMPORT_C gint StartDTMFNotifier();
    IMPORT_C gint CancelDTMFNotifier();
    IMPORT_C gint StartGlobalEffectNotifier();
    IMPORT_C gint CancelGlobalEffectNotifier();
    IMPORT_C gint StartRoutingNotifier();
    IMPORT_C gint CancelRoutingNotifier();

    // Message queue
    IMPORT_C gint SetMsgQueueNotifier(TMSMsgQueueNotifierType type,
            gpointer obsrvr, gpointer parent, gint clientid);
    IMPORT_C gint RemoveMsgQueueNotifier(TMSMsgQueueNotifierType type,
            gpointer obsrvr);

    // From MQueueHandlerObserver
    virtual void QueueEvent(gint aEventType, gint aError, void* user_data);

private:
    void PopulateArrayL(TMSClientServerRequest aRequest,
            RArray<TUint32>& aDecoders, gint aCount);
    void GetAvailableOutputsL(OutputVector& outputs);

    gint AddGlobalEffectObserver(TMSEffectObserver& obsrvr,
            TMSEffect& parent, gint clientid);
    gint RemoveGlobalEffectObserver(TMSEffectObserver& obsrvr);
    gint AddRoutingObserver(TMSGlobalRoutingObserver& obsrvr,
            TMSGlobalRouting& parent, gint clientid);
    gint RemoveRoutingObserver(TMSGlobalRoutingObserver& obsrvr);

    gint CreateQueue(const gint aNumSlots);
    void ReceiveMsgQHandlerEventsL();

private:
    TRoutingMsgBufPckg ipckg;
    RPointerArray<TMSEffectObserver> iEffectsObsrvrList;
    RPointerArray<TMSEffect> iEffectsParentList;
    RPointerArray<TMSGlobalRoutingObserver> iRoutingObsrvrList;
    RPointerArray<TMSGlobalRouting> iRoutingParentList;

    // Message queue and the handler
    RMsgQueue<TmsMsgBuf> iMsgQueue;
    CQueueHandler* iMsgQHandler;
    };

} //namespace TMS

#endif // TMS_PROXY_H

// End of file
