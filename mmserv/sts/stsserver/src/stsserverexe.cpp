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
 * The file containt the main method of the STS Server.
 */

#include "stsserver.h"
#include "stsclientservercommon.h"

TInt E32Main()
    {
    TInt returnValue = KErrNoMemory;

    RThread myThread;
    myThread.SetPriority(EPriorityAbsoluteRealTime1);

    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();

    if (cleanup)
        {
        // Run the server and request a process rendezvous.
        TRAP( returnValue, CStsServer::RunServerL( ETrue ) );
        delete cleanup;
        }
    __UHEAP_MARKEND;

    return returnValue;
    } // end E32Main
