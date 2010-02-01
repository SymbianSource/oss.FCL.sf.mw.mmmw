/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of EventAO class.
*
*/


#include "EventAO.h"

using namespace multimedia;

CEventAO::CEventAO(MSourceStateObserver& aObserver )
: CActive(CActive::EPriorityStandard),
iObserver(aObserver),
iError(KErrNone)
    {
    CActiveScheduler::Add(this);
    }

CEventAO::~CEventAO()
    {
    Cancel();
    }

CEventAO* CEventAO::NewL( MSourceStateObserver& aObserver )
    {
    CEventAO* self = new (ELeave)CEventAO( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CEventAO::ConstructL()
    {
    }

void CEventAO::SetActive()
    {
    if (!IsActive())
        {
        CActive::SetActive();
        }
    }

TInt CEventAO::Error()
    {
    return iError;
    }

// From CActive
void CEventAO::RunL()
    {
    // Save the error code
    iError = iStatus.Int();
    // Signal the observer that this request is serviced
    iObserver.SourceStateChanged();
    }

void CEventAO::DoCancel()
    {
    if(iStatus.Int() != 0)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status,KErrCancel);
        }
    }

TInt CEventAO::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// End of File
