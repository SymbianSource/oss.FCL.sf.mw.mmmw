/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 * The file provides the implementation of the client side session
 * to the STS Server.
 */

#include "rstssession.h"
#include "stsclientservercommon.h"

const TUint KNumSlots = 30;

/*static*/TInt RStsSession::CallBackThreadMain(TAny* aSession)
    {
    TInt err = KErrNoMemory;

    RThread myThread;
    myThread.SetPriority(EPriorityAbsoluteHigh);
    myThread.Close();

    CTrapCleanup* cleanup = CTrapCleanup::New();

    if (cleanup)
        {
        // Run the server and request a thread rendezvous.
        TRAP( err, ((RStsSession*)aSession)->RunThreadL() );
        delete cleanup;
        }

    return err;
    }

void RStsSession::RunThreadL()
    {
    // Initialisation complete, now signal the client, if requested.
    RThread::Rendezvous(KErrNone);

    while (true)
        {
        TStsCallBack message;
        iMsgQueue.ReceiveBlocking(message);
        TStsCallBackType type = message.callBackType;
        if (type == EStsPlayAlarmComplete)
            {
            message.observer->PlayAlarmComplete(message.alarmContext);
            }
        else if (type == EStsShutdown)
            {
            break;
            }
        else
            {
            //TODO: Log error message
            }
        }
    }

TInt RStsSession::StartMsgQueue()
    {
    // Create a nameless global message queue, then pass the handle to the queue to the server.
    TInt err = iMsgQueue.CreateGlobal(KNullDesC, 30);
    if (err == KErrNone)
        {
        err = SendReceive(StsMsg_RegisterMsgQueue, TIpcArgs(iMsgQueue));
        }
    return err;
    }

TInt RStsSession::StartServer()
    {
    TInt err = KErrNone;

    // Launch the server executable (i.e. in it its own process).

    // Create a new server process. Simultaneous launching of two such processes 
    // should be detected when the second one attempts to create the server 
    // object, failing with KErrAlreadyExists.
    RProcess server;
    err = server.Create(KStsServerFile, KNullDesC);

    if (err == KErrNone)
        {
        TRequestStatus rendezvousStatus;
        server.Rendezvous(rendezvousStatus);
        server.Resume();

        // wait for start or death
        User::WaitForRequest(rendezvousStatus);

        // we can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone  
        if (server.ExitType() == EExitPanic)
            {
            err = KErrGeneral;
            }
        else
            {
            err = rendezvousStatus.Int();
            }
        }

    server.Close();

    return err;
    }

TInt RStsSession::StartThread()
    {
    TInt result = iThread.Create(KNullDesC,
            RStsSession::CallBackThreadMain, KDefaultStackSize,
            &User::Heap(), (TAny*) this);

    if (result == KErrNone)
        {
        TRequestStatus rendezvousStatus = KRequestPending;

        //  Register for rendezvous notification when thread is started.
        iThread.Rendezvous(rendezvousStatus);

        // Start the thread execution
        iThread.Resume();

        // Wait for thread to start.
        User::WaitForRequest(rendezvousStatus);

        result = rendezvousStatus.Int();

        if (result != KErrNone)
            {
            iThread.Kill(result);
            }
        }

    return result;
    }

TInt RStsSession::Connect()
    {
    // Try to create a session with the server
    TInt result = CreateSession(KStsServerName, TVersion(
            KStsServerMajorVersion, KStsServerMinorVersion, KStsServerBuild),
            KNumSlots, EIpcSession_Sharable);

    // If the server wasn't found, start the server and try creating a session again
    if (result == KErrNotFound)
        {
        result = StartServer();
        if (result == KErrNone || result == KErrAlreadyExists)
            {
            result = CreateSession(KStsServerName, TVersion(
                    KStsServerMajorVersion, KStsServerMinorVersion,
                    KStsServerBuild), KNumSlots, EIpcSession_Sharable);
            }
        }

    // Create thread for receiving asynch callbacks from the server
    if (result == KErrNone)
        {
        result = StartMsgQueue();
        if (result == KErrNone)
            {
            result = StartThread();
            }
        }

    return result;
    }

void RStsSession::Close()
    {
    TRequestStatus logonStatus = KRequestPending;
    iThread.Logon(logonStatus);
    RSessionBase::Close();
    User::WaitForRequest(logonStatus);
    iThread.Close();
    iMsgQueue.Close();
    }

TInt RStsSession::SendPlayTone(CSystemToneService::TToneType aTone)
    {
    return SendReceive(StsMsg_PlayTone, TIpcArgs(aTone));
    }

TInt RStsSession::SendPlayAlarm(CSystemToneService::TAlarmType aAlarm,
        unsigned int& aAlarmContext, MStsPlayAlarmObserver& aObserver)
    {
    TPckg<unsigned int> alarmContextPckg(aAlarmContext);
    return SendReceive(StsMsg_PlayAlarm, TIpcArgs(aAlarm, &alarmContextPckg,
            &aObserver));
    }

TInt RStsSession::SendStopAlarm(unsigned int aAlarmContext)
    {
    return SendReceive(StsMsg_StopAlarm, TIpcArgs(aAlarmContext));
    }
