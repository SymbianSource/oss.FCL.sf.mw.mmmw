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
 * Description:  Central Repository listening
 *
 */

// INCLUDE FILES
#include "cspcenreplistener.h"
#include "mcspcenrepobserver.h"
#include "tmsutility.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// CSPCenRepListener::CSPCenRepListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSPCenRepListener* CSPCenRepListener::NewL(TUid aUid,
        TUint32 aMonitorSetting, MCSPCenRepObserver* aObserver)
    {
    TRACE_PRN_FN_ENT;
    CSPCenRepListener* self = new (ELeave) CSPCenRepListener(aUid,
            aMonitorSetting, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    TRACE_PRN_FN_EXT;
    return self;
    }

// Destructor
CSPCenRepListener::~CSPCenRepListener()
    {
    TRACE_PRN_FN_ENT;
    Cancel();
    delete iRepository;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CSPCenRepListener::Get
// -----------------------------------------------------------------------------
//
TInt CSPCenRepListener::Get(TInt& aValue)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    return (iRepository->Get(iMonitorSetting, aValue));

    }

// -----------------------------------------------------------------------------
// CSPCenRepListener::Set
// -----------------------------------------------------------------------------
//
TInt CSPCenRepListener::Set(TInt aValue)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    return (iRepository->Set(iMonitorSetting, aValue));

    }
// -----------------------------------------------------------------------------
// CSPCenRepListener::DoCancel
// -----------------------------------------------------------------------------
//
void CSPCenRepListener::DoCancel()
    {
    TRACE_PRN_FN_ENT;
    iRepository->NotifyCancel(iMonitorSetting);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CSPCenRepListener::RunError
// -----------------------------------------------------------------------------
//
TInt CSPCenRepListener::RunError(
#ifdef _DEBUG
        TInt aError // Log the leavecode from RunL
#else
        TInt /*aError*/
#endif
        )
    {
    TRACE_PRN_FN_ENT;
#ifdef _DEBUG
    TRACE_PRN_IF_ERR(aError);
#endif
    TRACE_PRN_FN_EXT;
    return ( KErrNone );
    }

// -----------------------------------------------------------------------------
// CSPCenRepListener::RunL
// -----------------------------------------------------------------------------
//
void CSPCenRepListener::RunL()
    {
    TRACE_PRN_FN_ENT;
    // Don't resubmit the request on error
    // Central repositry completes notifications with id of setting
    // so check only that value of iStatus is negative
    User::LeaveIfError(iStatus.Int() < KErrNone);
    SubmitNotifyRequestL();

    TInt volume;
    // The loudspeaker volume has changed in repository.
    // Retrieve the current volume from repository.
    User::LeaveIfError(Get(volume));

    iObserver->HandleNotifyCenRepL(iUid, iMonitorSetting, volume);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CSPCenRepListener::CSPCenRepListener
// -----------------------------------------------------------------------------
//
CSPCenRepListener::CSPCenRepListener(TUid aUid, TUint32 aMonitorSetting,
        MCSPCenRepObserver* aObserver) :
    CActive(EPriorityStandard),
    iUid(aUid),
    iMonitorSetting(aMonitorSetting),
    iObserver(aObserver)
    {
    TRACE_PRN_FN_ENT;
    CActiveScheduler::Add(this);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CSPCenRepListener::ConstructL
// -----------------------------------------------------------------------------
//
void CSPCenRepListener::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    // Create repository instance
    iRepository = CRepository::NewL(iUid);
    // Start monitoring
    SubmitNotifyRequestL();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CSPCenRepListener::SubmitNotifyRequestL
// -----------------------------------------------------------------------------
//
void CSPCenRepListener::SubmitNotifyRequestL()
    {
    TRACE_PRN_FN_ENT;
    iRepository->NotifyRequest(iMonitorSetting, iStatus);
    SetActive();
    TRACE_PRN_FN_EXT;
    }

// End of file
