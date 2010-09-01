/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Radio Monitor body implementation
*
*/


#include <RadioMonitor.h>
#include "RadioMonitorBody.h"
#include "RadioServerData.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CRadioMonitor::CBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRadioMonitor::CBody* CRadioMonitor::CBody::NewL(
    MRadioMonitorObserver& aObserver )
    {
    CRadioMonitor::CBody* s = new(ELeave) CRadioMonitor::CBody();
    s->iRadioMonitorClient = &aObserver;
    CleanupStack::PushL(s);
    s->ConstructL();
    CleanupStack::Pop();
    return s;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CRadioMonitor::CBody::~CBody()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CRadioMonitor::CBody::CBody
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRadioMonitor::CBody::CBody() :
    CActive(EPriorityStandard)
    {
    }

// -----------------------------------------------------------------------------
// CRadioMonitor::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CRadioMonitor::CBody::ConstructL()
    {
    CActiveScheduler::Add(this);

    // Attach property
    User::LeaveIfError(iProperty.Attach(KRadioServerPropertyCategory,
                                        KRadioServPsMonitorState));

    iProperty.Subscribe(iStatus);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CRadioMonitor::IsRadioOn
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CRadioMonitor::CBody::IsRadioOn() const
    {
    TBool radioState = ERadioStateOff;
    RProperty::Get(KRadioServerPropertyCategory,
                   KRadioServPsMonitorState,
                   radioState);

    return radioState;
    }

// -----------------------------------------------------------------------------
// CRadioMonitor::RunL
// Called when property value changes
// -----------------------------------------------------------------------------
//
void CRadioMonitor::CBody::RunL()
    {
    TBool radioState = ERadioStateOff;

    // Resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();

    TInt error = iProperty.Get(radioState);
    if ( error == KErrNone )
        {
        iRadioMonitorClient->MrmEvent((TRadioMonitorEvent)radioState);
        }
    else if ( error == KErrNotFound )
        {
        iRadioMonitorClient->MrmEvent(ERadioStateOff);
        }
    else
        {
        // pass
        }
    }

// -----------------------------------------------------------------------------
// CRadioMonitor::DoCancel
// Cancels event listening
// -----------------------------------------------------------------------------
//
void CRadioMonitor::CBody::DoCancel()
    {
    iProperty.Cancel();
    }

// End of File
