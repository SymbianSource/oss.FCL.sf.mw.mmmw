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
#include "tmsdtmfeventhandler.h"
#include "tmsclientserver.h"
#include "tmsutility.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// TMSDtmfEventHandler::TMSDtmfEventHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TMSDtmfEventHandler::TMSDtmfEventHandler(TMSServer* aServer) :
    CActive(EPriorityStandard),
    iTMSSer(aServer)
    {
    }

// -----------------------------------------------------------------------------
// TMSDtmfEventHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void TMSDtmfEventHandler::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    CActiveScheduler::Add(this);
    User::LeaveIfError(iProperty.Attach(KTMSPropertyCategory, EDtmfPs));
    iStatus = KRequestPending;
    iProperty.Subscribe(iStatus);
    SetActive();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSDtmfEventHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TMSDtmfEventHandler* TMSDtmfEventHandler::NewL(TMSServer* aServer)
    {
    TMSDtmfEventHandler* self = new (ELeave) TMSDtmfEventHandler(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
TMSDtmfEventHandler::~TMSDtmfEventHandler()
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
// TMSDtmfEventHandler::DoCancel
// -----------------------------------------------------------------------------
//
void TMSDtmfEventHandler::DoCancel()
    {
    TRACE_PRN_FN_ENT;
    iProperty.Cancel();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSDtmfEventHandler::RunL
// -----------------------------------------------------------------------------
//
void TMSDtmfEventHandler::RunL()
    {
    TRACE_PRN_FN_ENT;
// Subscribe immediately before analyzing the notification to ensure that we
// don't miss further updates.
//if (iStatus.Int() == KErrNone)
//    {
    iStatus = KRequestPending;
    iProperty.Subscribe(iStatus);
    SetActive();
    TmsMsgBufPckg dtmfpckg;
    iProperty.Get(dtmfpckg);
    iTMSSer->NotifyDtmfClients(dtmfpckg);
//    }
    TRACE_PRN_FN_EXT;
    }

//  End of File

