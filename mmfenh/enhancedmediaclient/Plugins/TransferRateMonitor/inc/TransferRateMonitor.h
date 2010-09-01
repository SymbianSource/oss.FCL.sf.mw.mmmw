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


#ifndef TRANSFERRATEMONITOR_H
#define TRANSFERRATEMONITOR_H

#include <e32base.h>

class MTransferRateObserver
    {
    public:
        // Called by CTransferRateMonitor when transfer rate changes by delta.
        virtual void TransferRateChanged() = 0;
        virtual TInt GetCurrentSize( TUint& aCurrentSize ) = 0;
    };

class CTransferRateMonitor : public CBase
    {
    public:
        IMPORT_C static CTransferRateMonitor* NewL( MTransferRateObserver& aObserver,
            TUint aFrequency,
            TUint aDelta ) ;
        IMPORT_C ~CTransferRateMonitor();
        
        // Returns previously calculated transfer rate.
        IMPORT_C TUint TransferRate();
        
        IMPORT_C TInt Start();
        IMPORT_C void Stop();
        IMPORT_C TInt Reset();
        
    private:
        CTransferRateMonitor( MTransferRateObserver& aObserver,
            TUint aFrequency,
            TUint aDelta ) ;
        void ConstructL();
        
        static TInt Tick(TAny* aObject);
        void DoTick();
        
    private:
        // Pointer reference to observer
        MTransferRateObserver* iObserver;
        // Frequency in seconds after which timer need to go off
        TUint iFrequency;
        // Change in transfer rate that will trigger observer callback
        TUint iDelta;
        // Current transfer rate
        TUint iTransferRate;
        // Reference transfer rate at which observer callback was triggered
        TUint iRefTransferRate;
        // Size from last transfer rate calculation
        TUint iPrevSize;
        // timer
        CPeriodic* iPeriodic;
    };

#endif // TRANSFERRATEMONITOR_H

//  End of File
