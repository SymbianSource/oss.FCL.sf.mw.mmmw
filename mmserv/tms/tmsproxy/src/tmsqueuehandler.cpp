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
#include <tmsbuffer.h>
#include "tmsutility.h"
#include "tmsshared.h"
#include "tmsqueuehandler.h"
#include "tmsmembuffer.h"
#include "tmsglobalcontext.h"
#include "tmscallproxy.h"

using namespace TMS;

// ----------------------------------------------------------------------------
// CQueueHandler::NewL
// Symbian constructor
// ----------------------------------------------------------------------------
//
CQueueHandler* CQueueHandler::NewL(RMsgQueue<TmsMsgBuf>* aMsgQueue,
        TMSGlobalContext* glblCtx)
    {
    CQueueHandler* self = new (ELeave) CQueueHandler(aMsgQueue, glblCtx);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::ConstructL
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CQueueHandler::ConstructL()
    {
    iObserver = NULL;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::~CQueueHandler
// Destructor.
// ----------------------------------------------------------------------------
//
CQueueHandler::~CQueueHandler()
    {
    Cancel();
    if (iMsgQueue->Handle() > 0)
        {
        iMsgQueue->Close();
        }
    iObserversList.Reset();
    iClientList.Reset();
    iChunk.Close();
    delete iBuffer;
    iObserver = NULL;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::CQueueHandler
// Constructor.
// ----------------------------------------------------------------------------
//
CQueueHandler::CQueueHandler(RMsgQueue<TmsMsgBuf>* aMsgQueue,
        TMSGlobalContext* glblCtx) :
    CActive(CActive::EPriorityStandard),
    iMsgQueue(aMsgQueue),
    iChunkDataPtr(0, 0, 0)
    {
    CActiveScheduler::Add(this);
    iTMSGlobalContext = glblCtx;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::Start
// Start listening for events on queue 0.
// ----------------------------------------------------------------------------
//
void CQueueHandler::Start()
    {
    if (!IsActive())
        {
        iMsgQueue->NotifyDataAvailable(iStatus);
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CQueueHandler::AddObserver
//
// ----------------------------------------------------------------------------
//
TInt CQueueHandler::AddObserver(MQueueHandlerObserver& aObserver,
        TInt /*aClientId*/)
    {
    TInt status = KErrNone;

    if (iObserver == NULL)
        {
        iObserver = &aObserver;
        }
    else
        {
        status = KErrAlreadyExists;
        }
    return status;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::AddObserver
// Marks observer as inactive in the list
// ----------------------------------------------------------------------------
//
TInt CQueueHandler::RemoveObserver(MQueueHandlerObserver& /*aObserver*/)
    {
    iObserver = NULL;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::DoCancel
// Cancel outstanding request
// ----------------------------------------------------------------------------
//
void CQueueHandler::DoCancel()
    {
    iMsgQueue->CancelDataAvailable();
    }

// ----------------------------------------------------------------------------
// CQueueHandler::RunL
// Process requests.
// ----------------------------------------------------------------------------
//
void CQueueHandler::RunL()
    {
    TmsMsgBuf msgBuf;
    TInt err = iMsgQueue->Receive(msgBuf);

    // Start monitoring for more events before calling the observer in case
    // client decides to destroy us before this RunL completes executing.
    Start();

    if (iObserver == NULL)
        {
        return;
        }

    if (err == KErrNone)
        {
        switch (msgBuf.iRequest)
            {
            case ECmdGlobalEffectChange:
                iObserver->QueueEvent(msgBuf.iInt, msgBuf.iStatus, NULL);
                break;
            case ECmdRingToneOpenComplete:
                iObserver->QueueEvent(TMS_EVENT_RINGTONE_OPEN_COMPLETE,
                        msgBuf.iStatus, &msgBuf.iInt64);
                break;
            case ECmdRingTonePlayComplete:
                iObserver->QueueEvent(TMS_EVENT_RINGTONE_PLAY_COMPLETE,
                        msgBuf.iStatus, NULL);
                break;
            case ECmdGlobalRoutingChange:
                iObserver->QueueEvent(msgBuf.iInt, msgBuf.iStatus,
                        &msgBuf.iUint);
                break;
            case ECmdDTMFOpenDnlinkComplete:
            case ECmdDTMFOpenUplinkComplete:
                iObserver->QueueEvent(TMS_EVENT_DTMF_TONE_STARTED,
                        msgBuf.iStatus, NULL);
                break;
            case ECmdDTMFTonePlayFinished:
                iObserver->QueueEvent(TMS_EVENT_DTMF_TONE_STOPPED,
                        msgBuf.iStatus, NULL);
                break;
            default:
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CQueueHandler::RunError
// Process requests.
// ----------------------------------------------------------------------------
//
TInt CQueueHandler::RunError(TInt /*aError*/)
    {
    // Current implementation of RunL does not leave
    return 0;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::Status
// Return request status.
// ----------------------------------------------------------------------------
//
TRequestStatus* CQueueHandler::Status()
    {
    return &iStatus;
    }

// End of File
