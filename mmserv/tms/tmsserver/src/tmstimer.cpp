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
 *
 */

// INCLUDE FILES
#include "tmstimer.h"

using namespace TMS;

// ---------------------------------------------------------
// TMSTimer::TMSTimer
// ---------------------------------------------------------
//
TMSTimer::TMSTimer(TInt aPriority) :
    CTimer(aPriority)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// TMSTimer::ConstructL
// ---------------------------------------------------------
//
void TMSTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------
// TMSTimer::NewL
// ---------------------------------------------------------
//
EXPORT_C TMSTimer* TMSTimer::NewL(TInt aPriority)
    {
    TMSTimer* self = new (ELeave) TMSTimer(aPriority);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------
// TMSTimer::~TMSTimer
// ---------------------------------------------------------
//
EXPORT_C TMSTimer::~TMSTimer()
    {
    Cancel();
    }

// ---------------------------------------------------------
// TMSTimer::RunL()
// ---------------------------------------------------------
//
void TMSTimer::RunL()
    {
    if (iStatus != KErrNone)
        {
        //error code is ignored, as CPeriodic.
        return;
        }

    if (!iTimerObserver)
        {
        //"TMSTimer::RunL CallBack" );
        iCallBack.CallBack();
        }
    else
        {
        //"TMSTimer::RunL HandleTimeOutL" );
        iTimerObserver->HandleTimeOutL();
        }
    }

// ---------------------------------------------------------
// TMSTimer::RunL()
// ---------------------------------------------------------
//
TInt TMSTimer::RunError(TInt aError)
    {
    //TODO: process error if HandleTimeOutL() in RunL leaves
    return aError;
    }

// ---------------------------------------------------------
// TMSTimer::After()
// ---------------------------------------------------------
//
EXPORT_C void TMSTimer::After(TTimeIntervalMicroSeconds32 anInterval,
        TCallBack aCallBack)
    {
    if (IsActive())
        {
        Cancel();
        }
    iTimerObserver = NULL;
    iCallBack = aCallBack;
    CTimer::After(anInterval);
    }

// ---------------------------------------------------------
// TMSTimer::After()
// ---------------------------------------------------------
//

EXPORT_C void TMSTimer::After(TTimeIntervalMicroSeconds32 anInterval,
        TMSTimerObsrv* aObserver)
    {
    //__ASSERT_DEBUG( aObserver, Panic( EPhoneUtilsParameterNotInitialized ) );

    if (IsActive())
        {
        Cancel();
        }
    iTimerObserver = aObserver;
    CTimer::After(anInterval);
    }

// ---------------------------------------------------------
//  TMSTimer::CancelTimer
// ---------------------------------------------------------
//
EXPORT_C void TMSTimer::CancelTimer()
    {
    Cancel();
    }

// ---------------------------------------------------------
//  TMSTimer::DoCancel
// ---------------------------------------------------------
//
void TMSTimer::DoCancel()
    {
    iTimerObserver = NULL;
    CTimer::DoCancel();
    }

//  End of File
