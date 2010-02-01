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
    }

// ----------------------------------------------------------------------------
// CQueueHandler::~CQueueHandler
// Destructor.
// ----------------------------------------------------------------------------
//
CQueueHandler::~CQueueHandler()
    {
    Cancel();
    iObserversList.Reset();
    iClientList.Reset();
    iChunk.Close();
    delete iBuffer;
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
    if (!IsActive() && iMsgQueue)
        {
        iMsgQueue->NotifyDataAvailable(iStatus);
        SetActive();
        }
    }

TInt CQueueHandler::AddObserver(MQueueHandlerObserver& aObserver,
        TInt aClientId)
    {
    // Add to list if observer is not already added
    TInt status = iObserversList.Find(&aObserver);
    if (status == KErrNotFound)
        {
        status = iObserversList.Append(&aObserver);
        status = iClientList.Append(aClientId);
        }
    else
        {
        status = KErrAlreadyExists;
        }
    return status;
    }

// Marks observer as inactive in the list
TInt CQueueHandler::RemoveObserver(MQueueHandlerObserver& aObserver)
    {
    TInt status(KErrNone);
    TInt index = iObserversList.Find(&aObserver);
    // If found status has index to observer in the array
    // else it would contain KErrNotFound
    if (index >= 0)
        {
        iObserversList.Remove(index);
        iClientList.Remove(index);
        status = KErrNone;
        }
    else
        {
        status = KErrNotFound;
        }
    return status;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::DoCancel
// Cancel outstanding request
// ----------------------------------------------------------------------------
//
void CQueueHandler::DoCancel()
    {
    if (iMsgQueue)
        {
        iMsgQueue->CancelDataAvailable();
        }
    }

// ----------------------------------------------------------------------------
// CQueueHandler::RunL
// Process requests.
// ----------------------------------------------------------------------------
//
void CQueueHandler::RunL()
    {
    TmsMsgBuf msgBuf;
    TInt err = KErrNone;

    if (iMsgQueue)
        {
        iMsgQueue->Receive(msgBuf);
        }
    else
        {
        err = KErrGeneral;
        }

    // Start monitoring for more events before calling the observer as client
    // may decide to destroy us before this RunL completes executing.
    Start();

    if (err == KErrNone)
        {
        switch (msgBuf.iRequest)
            {
            case ECmdDownlinkInitComplete:
            case ECmdUplinkInitComplete:
            case ECmdDownlinkDeInitComplete:
            case ECmdUplinkDeInitComplete:
            case ECmdDownlinkStarted:
            case ECmdUplinkStarted:
            case ECmdDownlinkPaused:
            case ECmdUplinkPaused:
                {
                TInt index = FindStreamInList();
                if (index != KErrNotFound)
                    {
                    TMSStreamState streamstate = ConvertToStreamState(
                            msgBuf.iRequest);
                    iObserversList[index]->QueueEvent(streamstate,
                            msgBuf.iStatus, NULL);
                    }
                else
                    {
                    // This should never happen
                    err = KErrNotFound;
                    }
                break;
                }
            case ECmdFillBuffer:
                {
                DoFillBuffer(msgBuf.iInt, msgBuf.iStatus, msgBuf.iBool,
                        msgBuf.iUint32);
                break;
                }
            case ECmdEmptyBuffer:
                {
                DoEmptyBuffer(msgBuf.iInt, msgBuf.iStatus, msgBuf.iBool,
                        msgBuf.iUint32);
                break;
                }
            case ECmdDownlinkClosed:
                {
                iChunk.Close();
                break;
                }
            case ECmdUplinkClosed:
                {
                iChunk.Close();
                break;
                }
            case ECmdDnLinkError:
                {
                break;
                }
            case ECmdUpLinkError:
                {
                break;
                }
            case ECmdSetGain:
                {
                TInt index = FindGainEffectInList();
                if (index != KErrNotFound)
                    {
                    iObserversList[index]->QueueEvent(TMS_EVENT_EFFECT_GAIN_CHANGED,
                            msgBuf.iStatus, NULL);
                    }
                }
                break;
            case ECmdSetVolume:
                {
                TInt index = FindVolEffectInList();
                if (index != KErrNotFound)
                    {
                    iObserversList[index]->QueueEvent(TMS_EVENT_EFFECT_VOL_CHANGED,
                            msgBuf.iStatus, NULL);
                    }
                }
                break;
            default:
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CQueueHandler::DoFillBuffer
//
// ----------------------------------------------------------------------------
//
void CQueueHandler::DoFillBuffer(TInt aBufLen, TInt aStatus,
        TBool aOpenChunk, TUint32 key)
    {
    TInt err = KErrNone;

    // See if chunk needs to be opened
    if (aOpenChunk)
        {
        if (iChunk.Handle())
            {
            iChunk.Close();
            }
        delete iBuffer;
        iBuffer = NULL;

        // Get handle to chunk from proxy
        TInt hndl(0);
        err = KErrNotReady;
        if (iTMSGlobalContext->CallProxy)
            {
            hndl = (iTMSGlobalContext->CallProxy)->GetDataXferChunkHandle(
                    iTMSGlobalContext->CallType,
                    iTMSGlobalContext->StreamType,
                    iTMSGlobalContext->StreamId,
                    key);
            err = iChunk.SetReturnedHandle(hndl);
            }
        }

    if (err == KErrNone)
        {
        iChunkDataPtr.Set(iChunk.Base(), 0, aBufLen);
        if (!iBuffer)
            {
            TMSMemBuffer::Create((guint) aBufLen,
                    const_cast<guint8*> (iChunkDataPtr.Ptr()), iBuffer);
            }
        iBuffer->SetDataSize(aBufLen);

        TInt index = iClientList.Find(TMS_SOURCE_CLIENT);
        if (index != KErrNotFound)
            {
            iObserversList[index]->QueueEvent(
                    TMS_EVENT_SOURCE_FILL_BUFFER, aStatus, iBuffer);
            }
        else
            {
            err = KErrNotFound;
            }
        }
    else
        {
        // TODO handle error here
        }
    }

// ----------------------------------------------------------------------------
// CQueueHandler::DoEmptyBuffer
//
// ----------------------------------------------------------------------------
//
void CQueueHandler::DoEmptyBuffer(TInt aBufLen, TInt aStatus,
        TBool aOpenChunk, TUint32 key)
    {
    TInt err(KErrNone);

    // See if chunk needs to be opened
    if (aOpenChunk)
        {
        if (iChunk.Handle())
            {
            iChunk.Close();
            }

        // Get handle to chunk from proxy
        TInt hndl(0);
        err = KErrNotReady;
        if (iTMSGlobalContext->CallProxy)
            {
            hndl = (iTMSGlobalContext->CallProxy)->GetDataXferChunkHandle(
                    iTMSGlobalContext->CallType,
                    iTMSGlobalContext->StreamType,
                    iTMSGlobalContext->StreamId,
                    key);
            err = iChunk.SetReturnedHandle(hndl);
            }
        // TODO handle error here
        delete iBuffer;
        iBuffer = NULL;
        }

    if (err == KErrNone)
        {
        iChunkDataPtr.Set(iChunk.Base(), aBufLen, aBufLen);
        if (!iBuffer)
            {
            TMSMemBuffer::Create((guint) aBufLen,
                    const_cast<guint8*> (iChunkDataPtr.Ptr()), iBuffer);
            }
        iBuffer->SetDataSize(aBufLen);
        TInt index = iClientList.Find(TMS_SINK_CLIENT);
        if (index != KErrNotFound)
            {
            iObserversList[index]->QueueEvent(
                    TMS_EVENT_SINK_PROCESS_BUFFER, aStatus, iBuffer);
            }
        else
            {
            err = KErrNotFound;
            }
        }
    else
        {
        //iObserver->Event(MVoIPUplinkObserver::KUplinkError, aStatus);
        }
    }

// ----------------------------------------------------------------------------
// CQueueHandler::ConvertToStreamState
//
// ----------------------------------------------------------------------------
//
TMSStreamState CQueueHandler::ConvertToStreamState(TInt request)
    {
    TMSStreamState state = TMS_STREAM_UNINITIALIZED;
    switch (request)
        {
        case ECmdDownlinkInitComplete:
        case ECmdUplinkInitComplete:
            state = TMS_STREAM_INITIALIZED;
            break;
        case ECmdDownlinkStarted:
        case ECmdUplinkStarted:
            state = TMS_STREAM_STARTED;
            break;
        case ECmdDownlinkPaused:
        case ECmdUplinkPaused:
            state = TMS_STREAM_PAUSED;
            break;
        case ECmdDownlinkDeInitComplete:
        case ECmdUplinkDeInitComplete:
            state = TMS_STREAM_UNINITIALIZED;
            break;
        default:
            break;
        }
    return state;
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

// ----------------------------------------------------------------------------
// CQueueHandler::FindStreamInList
// Return stream index.
// ----------------------------------------------------------------------------
//
TInt CQueueHandler::FindStreamInList()
    {
    gint index(-1);
    index = iClientList.Find(TMS_STREAM_UPLINK);
    if (index == KErrNotFound)
        {
        index = iClientList.Find(TMS_STREAM_DOWNLINK);
        return index;
        }
    return index;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::FindGainEffectInList
// Return effect index.
// ----------------------------------------------------------------------------
//
TInt CQueueHandler::FindGainEffectInList()
    {
    gint index(-1);
    index = iClientList.Find(TMS_EFFECT_GAIN);
    return index;
    }

// ----------------------------------------------------------------------------
// CQueueHandler::FindVolEffectInList
// Return effect index.
// ----------------------------------------------------------------------------
//
TInt CQueueHandler::FindVolEffectInList()
    {
    gint index(-1);
    index = iClientList.Find(TMS_EFFECT_VOLUME);
    return index;
    }

// End of File
