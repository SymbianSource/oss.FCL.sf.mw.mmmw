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

#include <e32svr.h>
#include <e32uid.h>
#include <e32capability.h>
#include "tmsutility.h"
#include "tmsclientserver.h"
#include "tmsserver.h"
#include "tmsservershutdown.h"
#include "tmsserversession.h"
#include "tmscallserverstartparam.h"
#include "tmscallserver.h"
#include "tmscallclisrv.h"
#include "globaleffectssettings.h"
#include "tareventhandler.h"
#include "cspaudiohandler.h"

using namespace TMS;

// CONSTANTS
const TInt KShutDownDelayTime = 5000000; // 5 sec delay time
const TInt KOutputsArraySize = 10;

// -----------------------------------------------------------------------------
// TMSServer::NewL
//
// -----------------------------------------------------------------------------
//
TMSServer* TMSServer::NewL()
    {
    TMSServer* self = new (ELeave) TMSServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// TMSServer::TMSServer
//
// -----------------------------------------------------------------------------
//
TMSServer::TMSServer() :
    CServer2(CActive::EPriorityHigh, ESharableSessions),
    iSession(0)
    {
    }

// -----------------------------------------------------------------------------
// TMSServer::~TMSServer
//
// -----------------------------------------------------------------------------
//
TMSServer::~TMSServer()
    {
    TRACE_PRN_FN_ENT;

    delete iShutdownTimer;
    iTMSCallServList.ResetAndDestroy();
    iTMSCallServList.Close();
    iDnlCodecs.Reset();
    iDnlCodecs.Close();
    iUplCodecs.Reset();
    iUplCodecs.Close();
    delete iEffectSettings;
    CancelRoutingNotifier();
    CancelCenRepHandler();

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSServer::NewSessionL
//
// -----------------------------------------------------------------------------
//
CSession2* TMSServer::NewSessionL(const TVersion& aVersion,
        const RMessage2& aMessage) const
    {
    TRACE_PRN_FN_ENT;

    if (!aMessage.HasCapability(ECapabilityMultimediaDD))
        {
        User::Leave(KErrPermissionDenied);
        }

    const TVersion version(KTMSServMajorVersionNumber,
                           KTMSServMinorVersionNumber,
                           KTMSServBuildVersionNumber);

    if (!User::QueryVersionSupported(version, aVersion))
        {
        User::Leave(KErrNotSupported);
        }

    TMSServerSession* session = TMSServerSession::NewL(*((TMSServer*) this));

    TRACE_PRN_FN_EXT;
    return session;
    }

// -----------------------------------------------------------------------------
// TMSServer::ConstructL
//
// -----------------------------------------------------------------------------
//
void TMSServer::ConstructL()
    {
    TRACE_PRN_FN_ENT;

    iShutdownTimer = TMSServerShutDown::NewL();
    StartL(KTMSServerName);
    RThread().SetPriority(EPriorityRealTime);
    iEffectSettings = GlobalEffectsSettings::NewL();
    iTarHandler = NULL;
    iAudioCenRepHandler = NULL;
    iCurrentRouting = TMS_AUDIO_OUTPUT_NONE;

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSServer::AddSession
//
// -----------------------------------------------------------------------------
//
void TMSServer::AddSession()
    {
    iSession++;
    // If shutdown timer is active, cancel it here.
    iShutdownTimer->Cancel();

    TRACE_PRN_N1(_L("TMS->SRV: AddSession->Active Sessions: [%d]"), iSession);
    }

// -----------------------------------------------------------------------------
// TMSServer::DropSession
//
// -----------------------------------------------------------------------------
//
void TMSServer::DropSession()
    {
    iSession--;

    // If session count is zero, start server shutdown
    if (iSession == 0)
        {
        iShutdownTimer->SetDelay(TTimeIntervalMicroSeconds32(
                KShutDownDelayTime));
        }

    TRACE_PRN_N1(_L("TMS->DNL: DropSession->Active Sessions: [%d]"), iSession);
    }

// -----------------------------------------------------------------------------
// TMSServer::SessionCount
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::SessionCount()
    {
    return iSession;
    }

// -----------------------------------------------------------------------------
// TMSServer::SetDnLinkSession
//
// -----------------------------------------------------------------------------
//
void TMSServer::SetDnLinkSession(TBool aSession)
    {
    iDnlinkSession = aSession;
    }

// -----------------------------------------------------------------------------
// TMSServer::SetUpLinkSession
//
// -----------------------------------------------------------------------------
//
void TMSServer::SetUpLinkSession(TBool aSession)
    {
    iUplinkSession = aSession;
    }

// -----------------------------------------------------------------------------
// TMSServer::HasDnLinkSession
//
// -----------------------------------------------------------------------------
//
TBool TMSServer::HasDnLinkSession() const
    {
    return iDnlinkSession;
    }

// -----------------------------------------------------------------------------
// TMSServer::HasUpLinkSession
//
// -----------------------------------------------------------------------------
//
TBool TMSServer::HasUpLinkSession() const
    {
    return iUplinkSession;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetNewTMSCallSessionHandleL
//
// -----------------------------------------------------------------------------
//
void TMSServer::GetNewTMSCallSessionHandleL(RHandleBase& aHandle)
    {
    TRACE_PRN_FN_ENT;

    TInt i = 0;
    while (i < iTMSCallServList.Count())
        {
        CStartAndMonitorTMSCallThread* callThread = iTMSCallServList[i];
        if (!callThread->IsActive())
            {
            iTMSCallServList.Remove(i);
            delete callThread;
            }
        else
            {
            i++;
            }
        }
    iTMSCallServList.Compress();

    TMSCallProxyLocal tmsCallSessionHandle;
    User::LeaveIfError(StartTMSCallServer(tmsCallSessionHandle));
    aHandle = tmsCallSessionHandle;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSServer::StartTMSCallServer
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::StartTMSCallServer(TMSCallProxyLocal& aHandle)
    {
    TRACE_PRN_FN_ENT;
    TInt status = KErrNone;

    CStartAndMonitorTMSCallThread* callServerThread = NULL;
    TRAP(status, callServerThread =
    CStartAndMonitorTMSCallThread::NewL(const_cast<TMSServer*>(this)));
    if (status != KErrNone)
        {
        delete callServerThread;
        }
    else
        {
        status = iTMSCallServList.Append(callServerThread);
        if (callServerThread && status == KErrNone)
            {
            status = callServerThread->StartTMSCallServer(aHandle);
            TInt count = 0;
            count = iTMSCallServList.Count();

            iTMSCallServList[count - 1]->iTMSCallProxyLocal = aHandle;

            //cache global vol and global gain to call server thread.
            TInt volume;
            switch (iCurrentRouting)
                {
                case TMS_AUDIO_OUTPUT_PUBLIC:
                case TMS_AUDIO_OUTPUT_LOUDSPEAKER:
                    {
                    iEffectSettings->GetLoudSpkrVolume(volume);
                    }
                    break;
                default:
                    {
                    iEffectSettings->GetEarPieceVolume(volume);
                    }
                    break;
                }
            aHandle.SendToCallServer(TMS_EFFECT_GLOBAL_VOL_SET, volume);
            aHandle.SendToCallServer(TMS_EFFECT_GLOBAL_GAIN_SET,
                    iEffectSettings->Gain());
            }
        else
            {
            delete callServerThread;
            }
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::StartRoutingNotifierL
//
// -----------------------------------------------------------------------------
//
void TMSServer::StartRoutingNotifierL()
    {
    if (!iTarHandler)
        {
        iTarHandler = CTarEventHandler::NewL((const_cast<TMSServer*> (this)));
        }
    }

// -----------------------------------------------------------------------------
// TMSServer::CancelRoutingNotifier
//
// -----------------------------------------------------------------------------
//
void TMSServer::CancelRoutingNotifier()
    {
    delete iTarHandler;
    iTarHandler = NULL;
    }

// -----------------------------------------------------------------------------
// TMSServer::StartCenRepHandlerL
//
// -----------------------------------------------------------------------------
//
void TMSServer::StartCenRepHandlerL()
    {
#ifdef _USE_TELEPHONY_CENREP_
    if (!iAudioCenRepHandler)
        {
        iAudioCenRepHandler = CSPAudioHandler::NewL(
                (const_cast<TMSServer*> (this)));
        }
#endif
    }

// -----------------------------------------------------------------------------
// TMSServer::CancelCenRepHandler
//
// -----------------------------------------------------------------------------
//
void TMSServer::CancelCenRepHandler()
    {
    delete iAudioCenRepHandler;
    iAudioCenRepHandler = NULL;
    }

// -----------------------------------------------------------------------------
// TMSServer::SetOutput
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::SetOutput(CSession2* /*sid*/, TInt output)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNone);

    status = SendMessageToCallServ(TMS_ROUTING_OUTPUT_SET, output);

    if (status == KErrNone)
        {
        iCurrentRouting = output;
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetOutput
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetOutput(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;

    TUint output;
    TInt i = 0;
    TInt status(KErrNone);
    while (i < iTMSCallServList.Count())
        {
        CStartAndMonitorTMSCallThread* callThread = iTMSCallServList[i];

        if (callThread)
            {
            status = callThread->iTMSCallProxyLocal.ReceiveFromCallServer(
                    TMS_ROUTING_OUTPUT_GET, output);
            if (status != KErrNone)
                {
                break;
                }
            }
        i++;
        }
    TPckgBuf<TInt> p(output);
    aMessage.Write(0, p);
    aMessage.Complete(KErrNone);
    TRACE_PRN_FN_EXT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetPreviousOutput
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetPreviousOutput(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    TUint output;
    TInt i = 0;
    TInt status(KErrNone);
    while (i < iTMSCallServList.Count())
        {
        CStartAndMonitorTMSCallThread* callThread = iTMSCallServList[i];

        if (callThread)
            {
            status = callThread->iTMSCallProxyLocal.ReceiveFromCallServer(
                    TMS_ROUTING_PREVIOUSOUTPUT_GET, output);
            if (status != KErrNone)
                {
                break;
                }
            }
        i++;
        }
    TPckgBuf<TInt> p(output);
    aMessage.Write(0, p);
    aMessage.Complete(KErrNone);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetAvailableOutputs
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetAvailableOutputs(const RMessage2& aMessage)
    {
    TRAPD(status, GetAvailableOutputsL(aMessage));
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetAvailableOutputsL
//
// -----------------------------------------------------------------------------
//
void TMSServer::GetAvailableOutputsL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;

    TInt status = KErrNone;
    OutputVector outputs;
    HBufC8* buf = HBufC8::NewLC(KOutputsArraySize * sizeof(TUint32));
    TPtr8 ptr = buf->Des();
    TPckgBuf<TInt> countpkg;
    TIpcArgs args;
    args.Set(0, &countpkg);
    args.Set(1, &ptr);

    TInt i = 0;

    while (i < iTMSCallServList.Count())
        {
        CStartAndMonitorTMSCallThread* callThread = iTMSCallServList[i];

        if (callThread)
            {
            status = callThread->iTMSCallProxyLocal.ReceiveFromCallServer(
                    TMS_ROUTING_AVAILABLE_OUTPUT_GET, args);
            if (status != KErrNone)
                {
                break;
                }
            }
        i++;
        }

    aMessage.WriteL(0, countpkg);
    aMessage.WriteL(1, buf->Des());
    aMessage.Complete(status);

    CleanupStack::PopAndDestroy(buf);

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetMaxLevel
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetMaxLevel(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNone);
    TPckgBuf<TUint> pckg;

    pckg() = iEffectSettings->MaxVolume();
    if (status == KErrNone)
        {
        status = aMessage.Write(0, pckg);
        aMessage.Complete(status);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetLevel
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetLevel(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNone);
    TPckgBuf<TUint> pckg;
    TInt volume;

    switch (iCurrentRouting)
        {
        case TMS_AUDIO_OUTPUT_PUBLIC:
        case TMS_AUDIO_OUTPUT_LOUDSPEAKER:
            iEffectSettings->GetLoudSpkrVolume(volume);
            break;
        default:
            iEffectSettings->GetEarPieceVolume(volume);
            break;
        }

    pckg() = volume;
    if (status == KErrNone)
        {
        aMessage.Write(0, pckg);
        aMessage.Complete(status);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::SetLevel
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::SetLevel(CSession2* /*sid*/, TBool tmsclient, TInt level)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNone);

    status = SendMessageToCallServ(TMS_EFFECT_GLOBAL_VOL_SET, level);

    if (status == KErrNone)
        {
        switch (iCurrentRouting)
            {
            case TMS_AUDIO_OUTPUT_PUBLIC:
            case TMS_AUDIO_OUTPUT_LOUDSPEAKER:
                {
                if (tmsclient && iAudioCenRepHandler)
                    {
                    iAudioCenRepHandler->SetLoudSpeakerVol(level);
                    }
                iEffectSettings->SetLoudSpkrVolume(level);
                }
                break;
            case TMS_AUDIO_OUTPUT_NONE:
            case TMS_AUDIO_OUTPUT_PRIVATE:
            case TMS_AUDIO_OUTPUT_HANDSET:
            case TMS_AUDIO_OUTPUT_WIRED_ACCESSORY:
            case TMS_AUDIO_OUTPUT_ACCESSORY:
            case TMS_AUDIO_OUTPUT_ETTY:
                {
                if (tmsclient && iAudioCenRepHandler)
                    {
                    iAudioCenRepHandler->SetEarPieceVol(level);
                    }
                iEffectSettings->SetEarPieceVolume(level);
                }
                break;
            default:
                break;
            }

        iSessionIter.SetToFirst();
        TMSServerSession* serverSession =
                static_cast<TMSServerSession*> (iSessionIter++);

        while (serverSession != NULL)
            {
            serverSession->HandleGlobalEffectChange(
                    TMS_EVENT_EFFECT_VOL_CHANGED);

            serverSession = static_cast<TMSServerSession*> (iSessionIter++);
            }
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetMaxGain
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetMaxGain(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNone);
    TPckgBuf<TUint> pckg;

    pckg() = iEffectSettings->MaxGain();
    if (status == KErrNone)
        {
        status = aMessage.Write(0, pckg);
        aMessage.Complete(status);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetGain
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetGain(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNone);
    TPckgBuf<TUint> pckg;

    pckg() = iEffectSettings->Gain();
    if (status == KErrNone)
        {
        status = aMessage.Write(0, pckg);
        aMessage.Complete(status);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::SetGain
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::SetGain(CSession2* /*sid*/, TInt level)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNone);

    status = SendMessageToCallServ(TMS_EFFECT_GLOBAL_GAIN_SET, level);

    if (status == KErrNone)
        {
        iEffectSettings->SetGain(level);
        iSessionIter.SetToFirst();

        TMSServerSession* serverSession =
                static_cast<TMSServerSession*> (iSessionIter++);

        while (serverSession != NULL)
            {
            serverSession->HandleGlobalEffectChange(
                    TMS_EVENT_EFFECT_GAIN_CHANGED);
            serverSession = static_cast<TMSServerSession*> (iSessionIter++);
            }
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::GetSupportedCodecs
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::GetSupportedCodecs(const TMSStreamType strmType,
        RArray<TFourCC>*& aCodecs)
    {
    if (strmType == TMS_STREAM_UPLINK)
        {
        aCodecs = &iUplCodecs;
        }
    else if (strmType == TMS_STREAM_DOWNLINK)
        {
        aCodecs = &iDnlCodecs;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// TMSServer::SendMessageToCallServ
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::SendMessageToCallServ(TInt func, TInt value)
    {
    TInt status(KErrNone);
    TInt i = 0;
    while (i < iTMSCallServList.Count())
        {
        CStartAndMonitorTMSCallThread* callThread = iTMSCallServList[i];

        if (callThread)
            {
            if (!callThread->IsActive())
                {
                iTMSCallServList.Remove(i);
                delete callThread;
                }
            else
                {
                status = callThread->iTMSCallProxyLocal.SendToCallServer(
                        func, value);
                if (status != KErrNone)
                    {
                    break;
                    }
                }
            }
        i++;
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::SendMessageToCallServ
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::SendMessageToCallServ(TInt func, TIpcArgs args)
    {
    TInt status(KErrNone);
    TInt i = 0;
    while (i < iTMSCallServList.Count())
        {
        CStartAndMonitorTMSCallThread* callThread = iTMSCallServList[i];

        if (callThread)
            {
            if (!callThread->IsActive())
                {
                iTMSCallServList.Remove(i);
                delete callThread;
                }
            else
                {
                status = callThread->iTMSCallProxyLocal.SendToCallServer(
                        func, args);
                if (status != KErrNone)
                    {
                    break;
                    }
                }
            }
        i++;
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSServer::NotifyTarClients
//
// -----------------------------------------------------------------------------
//
TInt TMSServer::NotifyTarClients(TRoutingMsgBufPckg routingpckg)
    {
    iCurrentRouting = routingpckg().iOutput;
    iSessionIter.SetToFirst();
    TMSServerSession* serverSession =
            static_cast<TMSServerSession*> (iSessionIter++);

    while (serverSession != NULL)
        {
        serverSession->HandleRoutingChange(routingpckg);
        serverSession = static_cast<TMSServerSession*> (iSessionIter++);
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RunServerL
//
// -----------------------------------------------------------------------------
//
static void RunServerL()
    {
    TRACE_PRN_N(_L("TMS->RunServerL"));

    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    // Create the server (leave it on the cleanup stack)
    TMSServer* server = TMSServer::NewL();

    // Initialisation complete, now signal the client
    RProcess::Rendezvous(KErrNone);

    // Start the scheduler and wait for client requests
    CActiveScheduler::Start();

    // Ready to exit.
    // Cleanup scheduler and delete the server
    delete server;
    CleanupStack::PopAndDestroy(scheduler);

    TRACE_PRN_N(_L("TMS->RunServerL - TMS server closed"));
    }

// -----------------------------------------------------------------------------
// CStartAndMonitorTMSCallThread::CStartAndMonitorTMSCallThread
// Perhaps we need to move this to a .cpp?
// -----------------------------------------------------------------------------
//
CStartAndMonitorTMSCallThread::CStartAndMonitorTMSCallThread(
        TMSServer* aServer) :
    CActive(EPriorityStandard),
    iTMSServer(aServer)
    {
    TRACE_PRN_FN_ENT;
    CActiveScheduler::Add(this);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CStartAndMonitorTMSCallThread::~CStartAndMonitorTMSCallThread
//
// -----------------------------------------------------------------------------
//
CStartAndMonitorTMSCallThread::~CStartAndMonitorTMSCallThread()
    {
    TRACE_PRN_FN_ENT;
    Cancel();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CStartAndMonitorTMSCallThread::NewL
//
// -----------------------------------------------------------------------------
//
CStartAndMonitorTMSCallThread* CStartAndMonitorTMSCallThread::NewL(
        TMSServer* aServer)
    {
    TRACE_PRN_FN_ENT;
    CStartAndMonitorTMSCallThread* self =
            new (ELeave) CStartAndMonitorTMSCallThread(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    TRACE_PRN_FN_EXT;
    return self;
    }

// -----------------------------------------------------------------------------
// CStartAndMonitorTMSCallThread::ConstructL
//
// -----------------------------------------------------------------------------
//
void CStartAndMonitorTMSCallThread::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CStartAndMonitorTMSCallThread::StartTMSCallServer
//
// -----------------------------------------------------------------------------
//
TInt CStartAndMonitorTMSCallThread::StartTMSCallServer(TMSCallProxyLocal& aHndl)
    {
    TRACE_PRN_FN_ENT;

    TInt status(KErrNone);
    TMSCallServerStartParam start(iTMSServer, iCallSrvrHndl);
    const TUidType serverUid(KNullUid, KNullUid, KUidTMSCallServerUid3);
    TThreadFunction serverFunc = TMSCallServer::StartThread;

    status = iServerThread.Create(_L(""),
                                  serverFunc,
                                  KTMSCallServerStackSize,
                                  KTMSCallServerInitHeapSize,
                                  KTMSCallServerMaxHeapSize,
                                  &start,
                                  EOwnerProcess);

    if (status != KErrNone)
        {
        return status;
        }

    //NOTE: set priority to EPriorityRealTime or speech codec audio will choke.
    iServerThread.SetPriority(EPriorityRealTime);

    // Synchronise with the server
    TRequestStatus reqStatus;
    iServerThread.Rendezvous(reqStatus);

    if (reqStatus != KRequestPending)
        {
        iServerThread.Kill(0);
        }
    else
        {
        // Start the test harness
        iServerThread.Resume();
        // Server will call the reciprocal static synchronise call
        }

    User::WaitForRequest(reqStatus); // wait for start or death
    if (reqStatus.Int() != KErrNone)
        {
        iServerThread.Close();
        iCallSrvrHndl.Close();
        return reqStatus.Int();
        }
    status = aHndl.Open(iCallSrvrHndl);

    if (status != KErrNone)
        {
        iServerThread.Close();
        iCallSrvrHndl.Close();
        return status;
        }
    aHndl.ShareProtected();
    iServerThread.Logon(iStatus);
    SetActive();

    TRACE_PRN_FN_EXT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CStartAndMonitorTMSCallThread::RunL
// From CActive
// -----------------------------------------------------------------------------
//
void CStartAndMonitorTMSCallThread::RunL()
    {
    TRACE_PRN_FN_ENT;
    iServerThread.Close();
    //NOTE: This is causing a panic when closing down tms server.
    //iCallSrvrHndl.Close();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CStartAndMonitorTMSCallThread::DoCancel
// From CActive
// -----------------------------------------------------------------------------
//
void CStartAndMonitorTMSCallThread::DoCancel()
    {
    TRACE_PRN_FN_ENT;
    iServerThread.LogonCancel(iStatus);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallProxyLocal::Open
// Perhaps we need to move this to a .cpp?
// -----------------------------------------------------------------------------
TInt TMSCallProxyLocal::Open(RServer2& aTMSCallServerHandle)
    {
    TRACE_PRN_FN_ENT;
    TInt status(KErrNotSupported);
    status = CreateSession(aTMSCallServerHandle,
                           TVersion(KTMSCallServerMajorVersionNumber,
                                    KTMSCallServerMinorVersionNumber,
                                    KTMSCallServerBuildVersionNumber),
                           -1,
                           EIpcSession_GlobalSharable);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallProxyLocal::SendToCallServer
//
// -----------------------------------------------------------------------------
//
TInt TMSCallProxyLocal::SendToCallServer(TInt aFunc, TUint value)
    {
    TInt status(KErrNone);
    status = SendReceive(aFunc, TIpcArgs(value));
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallProxyLocal::SendToCallServer
//
// -----------------------------------------------------------------------------
//
TInt TMSCallProxyLocal::SendToCallServer(TInt aFunc, TIpcArgs args)
    {
    TInt status(KErrNone);
    status = SendReceive(aFunc, args);
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallProxyLocal::ReceiveFromCallServer
//
// -----------------------------------------------------------------------------
//
TInt TMSCallProxyLocal::ReceiveFromCallServer(TInt aFunc, TUint& value)
    {
    TInt status(KErrNone);
    TPckgBuf<TUint> pckg;
    TIpcArgs args(&pckg);
    status = SendReceive(aFunc, args);
    if (status == KErrNone)
        {
        value = pckg();
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallProxyLocal::ReceiveFromCallServer
//
// -----------------------------------------------------------------------------
//
TInt TMSCallProxyLocal::ReceiveFromCallServer(TInt aFunc, TIpcArgs args)
    {
    TInt status(KErrNone);
    status = SendReceive(aFunc, args);
    return status;
    }

// -----------------------------------------------------------------------------
// E32Main
// Entry point for the server
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if (cleanup)
        {
        TRAP(r, RunServerL());
        delete cleanup;
        }
    __UHEAP_MARKEND;
    return r;
    }

// End of file
