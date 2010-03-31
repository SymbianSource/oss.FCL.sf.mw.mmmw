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

#include <s32mem.h>
#include <AudioPreference.h>
#include <tmseffectobsrvr.h>
#include <tmsglobalroutingobsrvr.h>
#include "tmsutility.h"
#include "tmsclientserver.h"
#include "tmsproxy.h"

using namespace TMS;

// CONSTANTS
const TUint KTMSServerConnectRetries = 2;
const TUint KSessionMessageSlots = 10;

// -----------------------------------------------------------------------------
// StartServer
// Static function to start the server process thread.
// Start the server process/thread which lives in an EPOCEXE object.
// Returns: gint: TMS_RESULT_SUCCESS (0) if no error
// -----------------------------------------------------------------------------
//
static gint StartServer()
    {
    const TUidType serverUid(KNullUid, KNullUid, KTMSServerUid3);

    // Only one instance of the server is allowed. Attempt of launching
    // second instance of the server will fail with KErrAlreadyExists.
    RProcess server;
    gint r = server.Create(KTMSServerName, KNullDesC, serverUid);

    if (r != TMS_RESULT_SUCCESS)
        {
        return r;
        }

    TRequestStatus stat;
    server.Rendezvous(stat);

    if (stat != KRequestPending)
        {
        server.Kill(0); // abort startup
        }
    else
        {
        server.Resume(); // logon OK - start the server
        }

    User::WaitForRequest(stat); // wait for start or death

    // Panic reason cannot be '0' as it would conflict with TMS_RESULT_SUCCESS
    r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }

// -----------------------------------------------------------------------------
// TMSProxy::TMSProxy
// -----------------------------------------------------------------------------
//
EXPORT_C TMSProxy::TMSProxy()
    {
    ResetObjectLists();
    iMsgQHandler = NULL;
    }

// -----------------------------------------------------------------------------
// TMSProxy::~TMSProxy
// -----------------------------------------------------------------------------
//
EXPORT_C TMSProxy::~TMSProxy()
    {
    ResetObjectLists();

    if (iMsgQHandler)
        {
        iMsgQHandler->Cancel();
        }
    delete iMsgQHandler;
    if (iMsgQueue.Handle() > 0)
        {
        iMsgQueue.Close();
        }
    if (Handle() > 0)
        {
        Close();
        }
    }

// -----------------------------------------------------------------------------
// TMSProxy::Connect
// Create a client-side session. Start the server if not started already.
// -----------------------------------------------------------------------------
//
EXPORT_C gint TMSProxy::Connect()
    {
    TRACE_PRN_FN_ENT;

    gint retry = KTMSServerConnectRetries;
    gint err(TMS_RESULT_GENERAL_ERROR);
    gint numMessageSlots = KSessionMessageSlots;

    for (;;)
        {
        // Try to create a new session with the server
        err = CreateSession(KTMSServerName, Version(), numMessageSlots);

        if ((err != KErrNotFound) && (err != KErrServerTerminated))
            {
            TRACE_PRN_N1(_L("[TMS session created; err==%d]"), err);
            break; // Connected to existing server - ok
            }

        if (--retry == 0)
            {
            break; // Failed.
            }

        // Server not running, try to start it.
        err = StartServer();
        TRACE_PRN_N1(_L("[TMS server started; err==%d]"), err);

        if ((err != TMS_RESULT_SUCCESS) && (err != KErrAlreadyExists))
            {
            break; // Server not launched - propagate error
            }
        }

    TRACE_PRN_IF_ERR(err);

    if (err == KErrNone)
        {
        err = CreateQueue(KTMSMsgQSlots);
        }

    TRACE_PRN_FN_EXT;
    return TMSRESULT(err);
    }

// -----------------------------------------------------------------------------
// TMSProxy::Version
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion TMSProxy::Version() const
    {
    return (TVersion(KTMSServMajorVersionNumber, KTMSServMinorVersionNumber,
            KTMSServBuildVersionNumber));
    }

// -----------------------------------------------------------------------------
// TMSProxy::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void TMSProxy::Close()
    {
    TRACE_PRN_FN_ENT;
    RSessionBase::Close();
    TRACE_PRN_FN_EXT;
    }

EXPORT_C gint TMSProxy::GetTMSCallSessionHandle()
    {
    gint err(TMS_RESULT_SUCCESS);
    err = SendReceive(ETMSCallSessionHandle);
    return TMSRESULT(err);
    }

EXPORT_C gint TMSProxy::GetSupportedDecoders(RArray<TUint32>& aDecoders,
        gint& aFrameSize)
    {
    TRACE_PRN_FN_ENT;
    gint err(TMS_RESULT_SUCCESS);

    TmsMsgBufPckg pckg1;
    TIpcArgs args1(&pckg1);
    err = SendReceive(ETMSGetSupportedDecodersCount, args1);
    gint count = 0;
    aFrameSize = 0;

    if (err == TMS_RESULT_SUCCESS)
        {
        count = pckg1().iInt;
        aFrameSize = pckg1().iUint; //for g711 10/20ms frame detection
        err = pckg1().iStatus;
        }

    if (count > 0 && err == TMS_RESULT_SUCCESS)
        {
        TRAP(err, PopulateArrayL(ETMSGetSupportedDecoders, aDecoders, count));
        }

    TRACE_PRN_FN_EXT;
    return TMSRESULT(err);
    }

EXPORT_C gint TMSProxy::GetSupportedEncoders(RArray<TUint32>& aEncoders,
        gint& aFrameSize)
    {
    TRACE_PRN_FN_ENT;
    gint err(TMS_RESULT_SUCCESS);

    TmsMsgBufPckg pckg1;
    TIpcArgs args1(&pckg1);
    err = SendReceive(ETMSGetSupportedEncodersCount, args1);
    gint count = 0;
    aFrameSize = 0;

    if (err == TMS_RESULT_SUCCESS)
        {
        count = pckg1().iInt;
        aFrameSize = pckg1().iUint; //for g711 10/20ms frame detection
        err = pckg1().iStatus;
        }

    if (count > 0 && err == TMS_RESULT_SUCCESS)
        {
        TRAP(err, PopulateArrayL(ETMSGetSupportedEncoders, aEncoders, count));
        }

    TRACE_PRN_FN_EXT;
    return TMSRESULT(err);
    }

EXPORT_C gint TMSProxy::SetOutput(TMSAudioOutput output)
    {
    TRACE_PRN_FN_ENT;
    gint status = TMS_RESULT_SUCCESS;
    if (output == TMS_AUDIO_OUTPUT_NONE)
        {
        status = TMS_RESULT_INVALID_ARGUMENT;
        }
    else
        {
        status = RSessionBase::SendReceive(ETMSSetOutput, TIpcArgs(output));
        }
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::GetOutput(TMSAudioOutput& output)
    {
    TRACE_PRN_FN_ENT;
    TPckgBuf<guint> pckg;
    TIpcArgs args(&pckg);
    gint status = RSessionBase::SendReceive(ETMSGetOutput, args);
    if (status == TMS_RESULT_SUCCESS)
        {
        output = pckg();
        }
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::GetPreviousOutput(TMSAudioOutput& output)
    {
    TRACE_PRN_FN_ENT;
    TPckgBuf<guint> pckg;
    TIpcArgs args(&pckg);
    gint status = RSessionBase::SendReceive(ETMSGetPreviousOutput, args);
    if (status == TMS_RESULT_SUCCESS)
        {
        output = pckg();
        }
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::GetAvailableOutputs(OutputVector& outputs)
    {
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    TRAP(status, GetAvailableOutputsL(outputs));
    return TMSRESULT(status);
    }

void TMSProxy::GetAvailableOutputsL(OutputVector& outputs)
    {
    HBufC8* buf = HBufC8::NewLC(10 * sizeof(TUint32));
    TPtr8 ptr = buf->Des();
    TPckgBuf<gint> countpkg;
    TIpcArgs args;
    args.Set(0, &countpkg);
    args.Set(1, &ptr);
    gint status = SendReceive(ETMSGetAvailableOutputs, args);
    outputs.clear();
    if (status != TMS_RESULT_SUCCESS)
        {
        User::Leave(status);
        }
    else
        {
        RDesReadStream stream(ptr);
        CleanupClosePushL(stream);

        for (gint i = 0; i < countpkg(); i++)
            {
            TMSAudioOutput output = stream.ReadUint32L();
            if (output != TMS_AUDIO_OUTPUT_NONE)
                {
                outputs.push_back(output);
                }
            //TRACE_PRN_N1(_L("TMS->TMSProxy: outputs: [%d]"), stream.ReadUint32L());
            }

        CleanupStack::PopAndDestroy(&stream);
        }
    CleanupStack::PopAndDestroy(buf);
    }

EXPORT_C gint TMSProxy::GetLevel(guint& level)
    {
    TRACE_PRN_FN_ENT;
    TPckgBuf<guint> pckg;
    TIpcArgs args(&pckg);
    gint status = RSessionBase::SendReceive(ETMSGetGlobalVol, args);
    if (status == TMS_RESULT_SUCCESS)
        {
        level = pckg();
        }
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::GetMaxLevel(guint& level)
    {
    TRACE_PRN_FN_ENT;
    TPckgBuf<guint> pckg;
    TIpcArgs args(&pckg);
    gint status = RSessionBase::SendReceive(ETMSGetMaxGlobalVol, args);
    if (status == TMS_RESULT_SUCCESS)
        {
        level = pckg();
        }
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::SetLevel(guint level)
    {
    TRACE_PRN_FN_ENT;
    gint status = RSessionBase::SendReceive(ETMSSetGlobalVol, TIpcArgs(level));
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::GetGain(guint& level)
    {
    TRACE_PRN_FN_ENT;
    TPckgBuf<guint> pckg;
    TIpcArgs args(&pckg);
    gint status = RSessionBase::SendReceive(ETMSGetGlobalGain, args);
    if (status == TMS_RESULT_SUCCESS)
        {
        level = pckg();
        }
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::GetMaxGain(guint& level)
    {
    TRACE_PRN_FN_ENT;
    TPckgBuf<guint> pckg;
    TIpcArgs args(&pckg);
    gint status = RSessionBase::SendReceive(ETMSGetMaxGlobalGain, args);
    if (status == TMS_RESULT_SUCCESS)
        {
        level = pckg();
        }
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::SetGain(guint level)
    {
    TRACE_PRN_FN_ENT;
    gint status = RSessionBase::SendReceive(ETMSSetGlobalGain,
            TIpcArgs(level));
    TRACE_PRN_FN_EXT;
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::StartGlobalEffectNotifier()
    {
    gint status(TMS_RESULT_SUCCESS);
    RSessionBase::SendReceive(ETMSStartGlobalEffectNotifier); //CenRepHandler
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::CancelGlobalEffectNotifier()
    {
    gint status(TMS_RESULT_SUCCESS);
    status = RSessionBase::SendReceive(ETMSCancelGlobalEffectNotifier);
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::StartRoutingNotifier()
    {
    gint status(TMS_RESULT_SUCCESS);
    status = RSessionBase::SendReceive(ETMSStartRoutingNotifier); //TAR
    return TMSRESULT(status);
    }

EXPORT_C gint TMSProxy::CancelRoutingNotifier()
    {
    gint status(TMS_RESULT_SUCCESS);
    status = RSessionBase::SendReceive(ETMSCancelRoutingNotifier);
    return TMSRESULT(status);
    }

// ---------------------------------------------------------------------------
// TMSProxy::SetMsgQueueNotifier
// ---------------------------------------------------------------------------
//
EXPORT_C gint TMSProxy::SetMsgQueueNotifier(TMSMsgQueueNotifierType type,
        gpointer obsrv, gpointer parent, gint clientid)
    {
    gint status(TMS_RESULT_SUCCESS);
    switch (type)
        {
        case EMsgQueueGlobalVolumeType:
        case EMsgQueueGlobalGainType:
            status = AddGlobalEffectObserver((*(TMSEffectObserver*) obsrv),
                    (*(TMSEffect*) parent), clientid);
            break;
        case EMsgQueueGlobalRoutingType:
            status = AddRoutingObserver((*(TMSGlobalRoutingObserver*) obsrv),
                    (*(TMSGlobalRouting*) parent), clientid);
            break;
        default:
            status = TMS_RESULT_INVALID_ARGUMENT;
            break;
        }
    return TMSRESULT(status);
    }

// ---------------------------------------------------------------------------
// TMSProxy::RemoveMsgQueueNotifier
// ---------------------------------------------------------------------------
//
EXPORT_C gint TMSProxy::RemoveMsgQueueNotifier(TMSMsgQueueNotifierType type,
        gpointer obsrv)
    {
    gint status(TMS_RESULT_SUCCESS);

    switch (type)
        {
        case EMsgQueueGlobalVolumeType:
        case EMsgQueueGlobalGainType:
            status = RemoveGlobalEffectObserver((*(TMSEffectObserver*) obsrv));
            break;
        case EMsgQueueGlobalRoutingType:
            status = RemoveRoutingObserver((*(TMSGlobalRoutingObserver*)
                    obsrv));
            break;
        default:
            status = TMS_RESULT_INVALID_ARGUMENT;
            break;
        }
    return TMSRESULT(status);
    }

void TMSProxy::PopulateArrayL(TMSClientServerRequest aRequest,
        RArray<TUint32>& aDecoders, gint aCount)
    {
    HBufC8* buf = HBufC8::NewLC(aCount * sizeof(TUint32));
    TPtr8 ptr = buf->Des();
    TIpcArgs args;
    args.Set(0, &ptr);
    gint err = SendReceive(aRequest, args);

    if (err == TMS_RESULT_SUCCESS)
        {
        RDesReadStream stream(ptr);
        CleanupClosePushL(stream);
        aDecoders.Reset();

        for (gint i = 0; i < aCount; i++)
            {
            aDecoders.Append(stream.ReadUint32L());
            }
        CleanupStack::PopAndDestroy(&stream);
        }
    CleanupStack::PopAndDestroy(buf);
    }

gint TMSProxy::AddGlobalEffectObserver(TMSEffectObserver& obsrv,
        TMSEffect& parent, gint /*clientid*/)
    {
    gint status = iEffectsObsrvrList.Find(&obsrv);
    if (status == KErrNotFound)
        {
        status = iEffectsObsrvrList.Append(&obsrv);
        status = iEffectsParentList.Append(&parent);
        }
    else
        {
        status = TMS_RESULT_ALREADY_EXIST;
        }
    return TMSRESULT(status);
    }

gint TMSProxy::RemoveGlobalEffectObserver(TMSEffectObserver& obsrv)
    {
    gint status(TMS_RESULT_SUCCESS);
    gint index = iEffectsObsrvrList.Find(&obsrv);
    if (index >= 0)
        {
        iEffectsObsrvrList.Remove(index);
        iEffectsParentList.Remove(index);
        }
    else
        {
        status = TMS_RESULT_DOES_NOT_EXIST;
        }
    return TMSRESULT(status);
    }

gint TMSProxy::AddRoutingObserver(TMSGlobalRoutingObserver& obsrv,
        TMSGlobalRouting& parent, gint /*clientid*/)
    {
    gint status = iRoutingObsrvrList.Find(&obsrv);
    if (status == KErrNotFound)
        {
        status = iRoutingObsrvrList.Append(&obsrv);
        status = iRoutingParentList.Append(&parent);
        }
    else
        {
        status = TMS_RESULT_ALREADY_EXIST;
        }
    return TMSRESULT(status);
    }

gint TMSProxy::RemoveRoutingObserver(TMSGlobalRoutingObserver& obsrv)
    {
    gint status(TMS_RESULT_SUCCESS);
    gint index = iRoutingObsrvrList.Find(&obsrv);
    if (index >= 0)
        {
        iRoutingObsrvrList.Remove(index);
        iRoutingParentList.Remove(index);
        }
    else
        {
        status = TMS_RESULT_DOES_NOT_EXIST;
        }
    return TMSRESULT(status);
    }

// ---------------------------------------------------------------------------
// TMSProxy::CreateQueue
// ---------------------------------------------------------------------------
//
gint TMSProxy::CreateQueue(const gint aNumSlots)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (iMsgQueue.Handle() <= 0)
        {
        status = iMsgQueue.CreateGlobal(KNullDesC, aNumSlots);
        if (status == TMS_RESULT_SUCCESS)
            {
            TRAP(status, ReceiveMsgQHandlerEventsL());
            if (status == TMS_RESULT_SUCCESS)
                {
                TIpcArgs args;
                args.Set(0, iMsgQueue);
                status = RSessionBase::SendReceive(ETMSSetMsgQueueHandle,
                        args);
                }
            }
        }
    return TMSRESULT(status);
    }

// ---------------------------------------------------------------------------
// TMSProxy::ReceiveMsgQHandlerEventsL
// Starts message queue handler (A/O) to monitor communication and transfer
// buffer events between client and the server.
// ---------------------------------------------------------------------------
//
void TMSProxy::ReceiveMsgQHandlerEventsL()
    {
    if (iMsgQHandler)
        {
        iMsgQHandler->Cancel();
        }
    else
        {
        iMsgQHandler = TMSQueueHandler::NewL(&iMsgQueue, NULL);
        iMsgQHandler->AddObserver(*this, -1);
        }
    iMsgQHandler->Start();
    }

// ---------------------------------------------------------------------------
// TMSProxy::QueueEvent
// Call from QueueHandler as a result of TMS Server callback.
// ---------------------------------------------------------------------------
//
void TMSProxy::QueueEvent(gint aEventType, gint aError, void* user_data)
    {
    TMSSignalEvent event;
    event.type = aEventType;
    event.reason = aError;
    event.user_data = user_data;

    switch (aEventType)
        {
        case TMS_EVENT_EFFECT_VOL_CHANGED:
        case TMS_EVENT_EFFECT_GAIN_CHANGED:
            {
            for (gint i = 0; i < iEffectsObsrvrList.Count(); i++)
                {
                iEffectsObsrvrList[i]->EffectsEvent(iEffectsParentList[i],
                        event);
                }
            break;
            }
        case TMS_EVENT_ROUTING_AVAIL_OUTPUTS_CHANGED:
        case TMS_EVENT_ROUTING_OUTPUT_CHANGED:
        case TMS_EVENT_ROUTING_SET_OUTPUT_COMPLETE:
            {
            guint output(0);
            if (user_data != NULL)
                {
                output = *((guint*) user_data);
                }
            for (gint i = 0; i < iRoutingObsrvrList.Count(); i++)
                {
                iRoutingObsrvrList[i]->GlobalRoutingEvent(
                        iRoutingParentList[i], event, output);
                }
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// TMSProxy::ResetObjectLists
// ---------------------------------------------------------------------------
//
void TMSProxy::ResetObjectLists()
    {
    iEffectsObsrvrList.Reset();
    iRoutingObsrvrList.Reset();

    iEffectsParentList.Reset();
    iRoutingParentList.Reset();
    }

// End of file
