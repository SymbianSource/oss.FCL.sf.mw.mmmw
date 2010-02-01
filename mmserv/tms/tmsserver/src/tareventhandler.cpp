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

// INCLUDE FILES
#include "tareventhandler.h"
#include "tmsclientserver.h"
#include "tmsutility.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// CTarEventHandler::CTarEventHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTarEventHandler::CTarEventHandler(TMSServer* aServer) :
    CActive(EPriorityStandard),
    iTMSSer(aServer)
    {
    }

// -----------------------------------------------------------------------------
// CTarEventHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTarEventHandler::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    CActiveScheduler::Add(this);
    User::LeaveIfError(iProperty.Attach(KTMSPropertyCategory, ERoutingPs));
    iProperty.Subscribe(iStatus);
    SetActive();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CTarEventHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTarEventHandler* CTarEventHandler::NewL(TMSServer* aServer)
    {
    CTarEventHandler* self = new (ELeave) CTarEventHandler(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CTarEventHandler::~CTarEventHandler()
    {
    TRACE_PRN_FN_ENT;
    if (IsActive())
        {
        Cancel();
        }
    iProperty.Close();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CTarEventHandler::DoCancel
// -----------------------------------------------------------------------------
//
void CTarEventHandler::DoCancel()
    {
    TRACE_PRN_FN_ENT;
    iProperty.Cancel();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CTarEventHandler::RunL
// -----------------------------------------------------------------------------
//
void CTarEventHandler::RunL()
    {
    TRACE_PRN_FN_ENT;
    // Subscribe immediately before analyzing the notification to ensure that we
    // don't miss further updates.
    if (iStatus.Int() == KErrNone)
        {
        iProperty.Subscribe(iStatus);
        SetActive();
        TRoutingMsgBufPckg routingpckg;
        iProperty.Get(routingpckg);
        iTMSSer->NotifyTarClients(routingpckg);
        }
    TRACE_PRN_FN_EXT;
    }

//  End of File

