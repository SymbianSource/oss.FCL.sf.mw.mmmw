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
* Description:  Project file for EnhancedMediaClient Utility
*
*/


#include "TransferRateMonitor.h"
#include <MultimediaDataSource.h>

CTransferRateMonitor::CTransferRateMonitor( MTransferRateObserver& aObserver,
                                           TUint aFrequency,
                                           TUint aDelta )
                                           : iObserver(&aObserver),
                                           iFrequency(aFrequency),
                                           iDelta(aDelta)
    {
    // Create timer here
    }

EXPORT_C CTransferRateMonitor::~CTransferRateMonitor()
    {
    if ( iPeriodic )
        {
        iPeriodic->Cancel();
        }
    delete iPeriodic;
    }
EXPORT_C CTransferRateMonitor* CTransferRateMonitor::NewL( MTransferRateObserver& aObserver,
                                                          TUint aFrequency,
                                                          TUint aDelta )
    {
    CTransferRateMonitor* self = new (ELeave)CTransferRateMonitor( aObserver, aFrequency, aDelta );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CTransferRateMonitor::ConstructL()
    {
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    }

EXPORT_C TUint CTransferRateMonitor::TransferRate()
    {
    return iTransferRate;
    }

EXPORT_C TInt CTransferRateMonitor::Start()
    {
    TInt status(KErrNotReady);
    if ( !iPeriodic->IsActive() )
        {
        TTimeIntervalMicroSeconds32 interval( iFrequency * 1000000 ); // Convert to micro seconds
        iPeriodic->Start( interval, interval, TCallBack(Tick, this) );
        status = KErrNone;
        }
    return status;
    }

EXPORT_C void CTransferRateMonitor::Stop()
    {
    iPeriodic->Cancel();
    }

EXPORT_C TInt CTransferRateMonitor::Reset()
    {
    TInt status(KErrNotReady);
    if ( !iPeriodic->IsActive() )
        {
        iTransferRate = 0;
        iRefTransferRate = 0;
        iPrevSize = 0;
        status = KErrNone;
        }
    return status;
    }

EXPORT_C TInt CTransferRateMonitor::Tick(TAny* aObject)
    {
    ((CTransferRateMonitor*)aObject)->DoTick();
    return 1;
    }

void CTransferRateMonitor::DoTick()
    {
    TUint curSize(0);
    if (iObserver->GetCurrentSize(curSize) == KErrNone)
        {
        iTransferRate = ( curSize - iPrevSize ) * 8 / iFrequency;
        iPrevSize = curSize;
        TInt diffInTransferRate = Abs ( iTransferRate - iRefTransferRate );
        if ( diffInTransferRate >= iDelta )
            {
            iRefTransferRate = iTransferRate;
            iObserver->TransferRateChanged();
            }
        }
    }

//  End of File
