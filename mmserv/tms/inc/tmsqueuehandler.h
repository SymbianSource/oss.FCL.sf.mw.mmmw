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

#ifndef TMS_QUEUEHANDLER_H
#define TMS_QUEUEHANDLER_H

// INCLUDES
#include <e32base.h>
#include <e32msgqueue.h>
#include <glib.h>
#include "tmsclientserver.h"   // for TmsMsgBuf

namespace TMS {

// FORWARD DECLARATIONS
class TMSBuffer; //TMSDataBuffer
class TMSGlobalContext;

/**
 *  MQueueHandlerObserver class
 *  Responsible for sending callbacks to the TMS client app
 */
class MQueueHandlerObserver
    {
public:
    virtual void QueueEvent(TInt aEventType, TInt aError, void* user_data) = 0;
    };

/**
 *  CQueueHandler class
 */
NONSHARABLE_CLASS(CQueueHandler) : public CActive
    {
public:

    /**
     * NewL()
     * Contructor
     *
     */
    static CQueueHandler* NewL(RMsgQueue<TmsMsgBuf>* aMsgQueue,
            TMSGlobalContext* glblCtx);

    /**
     * destructor
     */
    virtual ~CQueueHandler();

    /**
     * Start
     * Starts listening for events.
     *
     */
    void Start();

    /**
     * Returns request status.
     */
    TRequestStatus* Status();

    TInt AddObserver(MQueueHandlerObserver& obsvr, TInt aClientId);
    TInt RemoveObserver(MQueueHandlerObserver& obsvr);

private:

    /**
     * constructor
     */
    CQueueHandler(RMsgQueue<TmsMsgBuf>* aMsgQueue,
            TMSGlobalContext* glblCtx);

    void ConstructL();

    // from CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);
    void DoFillBuffer(TInt aBufLen, TInt aStatus, TBool flag, TUint32 key);
    void DoEmptyBuffer(TInt aBufLen, TInt aStatus, TBool flag, TUint32 key);
    TInt FindStreamInList();
    TInt FindGainEffectInList();
    TInt FindVolEffectInList();
    TMSStreamState ConvertToStreamState(TInt request);

private:
    // data

    RPointerArray<MQueueHandlerObserver> iObserversList; // (not owned)
    MQueueHandlerObserver* iObserver; //not owned
    RArray<TInt> iClientList;
    RMsgQueue<TmsMsgBuf>* iMsgQueue;

    RChunk iChunk;
    TPtr8 iChunkDataPtr;
    TMSBuffer* iBuffer;
    TMSGlobalContext *iTMSGlobalContext;
    };

} //namespace TMS

#endif // TMS_QUEUEHANDLER_H

// End of File
