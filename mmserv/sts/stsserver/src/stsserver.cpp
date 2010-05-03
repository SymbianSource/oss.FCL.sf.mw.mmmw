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
 * This file is the implementation of the Symbian STS Server.  This
 * file handles the Symbian specific server creation/deletation
 * activities, and adding/removing sessions.  The actual STS
 * functionality is handled in the STS class.
 */

#include "stsserver.h"

#include <ecom/ecom.h>
#include "stsserversession.h"
#include "sts.h"

// TODO: IMPLEMENT PLATFORM SECURITY CHECKS ON THIS API.

// Total number of ranges
const TUint KStsRangeCount = 2;

// Definition of the ranges of IPC numbers
const TInt KStsRanges[KStsRangeCount] =
    {
    0, KStsCmdLast + 1
    };

// Policy to implement for each of the above ranges        
const TUint8 KStsElementsIndex[KStsRangeCount] =
    {
    CPolicyServer::EAlwaysPass, CPolicyServer::ENotSupported
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy KStsPolicy =
    {
    CPolicyServer::EAlwaysPass, // specifies all connect attempts should pass
            KStsRangeCount, // number of ranges                                   
            KStsRanges, // ranges array
            KStsElementsIndex, // elements<->ranges index
            NULL
    };

// CStsServer IMPLEMENTATION

CStsServer* CStsServer::NewLC()
    {
    CStsServer* self = new (ELeave) CStsServer();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CStsServer::CStsServer() :
    CPolicyServer(0, KStsPolicy, ESharableSessions), iSts(0)
    {
    }

void CStsServer::ConstructL()
    {
    StartL(KStsServerName);
    iSts = CSts::Create();
    if (iSts == 0)
        {
        User::Leave(KErrNoMemory);
        }
    }

CStsServer::~CStsServer()
    {
    iSessions.ResetAndDestroy();
    CSts::Delete(iSts);
    REComSession::FinalClose();
    }

CSession2* CStsServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
    {
    if (aVersion.iMajor != KStsServerMajorVersion || aVersion.iMinor
            != KStsServerMinorVersion || aVersion.iBuild != KStsServerBuild)
        {
        User::Leave(KErrNotSupported);
        } // end if

    CStsServer& nonConstThis = *const_cast<CStsServer*> (this);

    // Construct a new session, passing it a pointer to the server object.  This function
    // is const, so the const-ness must be cast away from the this pointer.
    CSession2* returnValue = new (ELeave) CStsServerSession(nonConstThis, *iSts);

    return returnValue;
    }

void CStsServer::AddSession(CStsServerSession* aSession)
    {
    iSessions.Append(aSession);
    }

void CStsServer::DropSession(CStsServerSession* aSession)
    {
    // Find the current session in the list of sessions.
    TInt index = iSessions.Find(aSession);

    if (index != KErrNotFound)
        {
        // Remove the session from the list of sessions.
        iSessions.Remove(index);
        } // end if

    if (iSessions.Count() == 0)
        {
        // No more clients. Shutdown the server.
        CActiveScheduler::Stop();
        }
    }

// SERVER LAUNCHING FUNCTIONALITY

EXPORT_C void CStsServer::RunServerL(bool aPerformProcessRendezvous,
        bool aPerformThreadRendezvous)
    {
    // naming the server thread after the server helps to debug panics
    User::LeaveIfError(User::RenameThread(KStsServerName));

    // create and install the active scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    // create the server (leave it on the cleanup stack)
    CStsServer* server = CStsServer::NewLC();

    // Initialisation complete, now signal the client, if requested.

    if (aPerformProcessRendezvous)
        {
        RProcess::Rendezvous(KErrNone);
        }

    if (aPerformThreadRendezvous)
        {
        RThread::Rendezvous(KErrNone);
        }

    // Ready to run
    // Start wait loop, will return when server is shutdown
    CActiveScheduler::Start();

    // Cleanup the server
    CleanupStack::PopAndDestroy(server);

    // Cleanup scheduler after shutdown
    CleanupStack::PopAndDestroy(scheduler);
    }
