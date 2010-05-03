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

        if (rendezvousStatus != KRequestPending)
            {
            server.Kill(0); // abort startup
            }
        else
            {
            server.Resume(); // logon OK - start the server thread
            } // end if

        User::WaitForRequest(rendezvousStatus); // wait for start or death

        // we can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone  
        err = (server.ExitType() == EExitPanic)
                                                ? KErrGeneral
                                                   : rendezvousStatus.Int();
        server.Close();
        }

    return err;
    }

TInt RStsSession::Connect()
    {
    TInt result = CreateSession(KStsServerName, TVersion(
            KStsServerMajorVersion, KStsServerMinorVersion, KStsServerBuild),
            KNumSlots, EIpcSession_Sharable);

    if (result == KErrNotFound)
        {
        result = StartServer();
        if (result == KErrNone || result == KErrAlreadyExists)
            {
            result = CreateSession(KStsServerName, TVersion(
                    KStsServerMajorVersion, KStsServerMinorVersion,
                    KStsServerBuild), KNumSlots);
            }
        }

    if (result == KErrNone)
        {
        result = ShareAuto();

        if (result != KErrNone)
            {
            Close();
            }
        }

    return result;
    }

void RStsSession::Close()
    {
    RSessionBase::Close();
    }

TInt RStsSession::SendPlayTone(CSystemToneService::TToneType aToneType,
        unsigned int& aPlayToneContext)
    {
    TPckg<unsigned int> playToneContextPckg(aPlayToneContext);
    return SendReceive(StsMsg_PlayTone, TIpcArgs((TInt) aToneType,
            &playToneContextPckg));
    }

TInt RStsSession::SendStopTone(unsigned int aPlayToneContext)
    {
    return SendReceive(StsMsg_StopTone, TIpcArgs(aPlayToneContext));
    }
