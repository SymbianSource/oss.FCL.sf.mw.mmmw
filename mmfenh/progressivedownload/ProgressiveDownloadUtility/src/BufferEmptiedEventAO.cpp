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
* Description:  Progressive Download Utility  Implementation of the BufTypeSupEventAO class.
*
*/


#include "BufferEmptiedEventAO.h"
#include <DataBuffer.h>

using namespace multimedia;

CBufferEmptiedEventAO::CBufferEmptiedEventAO(MBufferEmptiedObserver& aObserver )
: CActive(CActive::EPriorityStandard),
iObserver(aObserver),
iError(KErrNone)
    {
    CActiveScheduler::Add(this);
    }

CBufferEmptiedEventAO::~CBufferEmptiedEventAO()
    {
    Cancel();
    }

CBufferEmptiedEventAO* CBufferEmptiedEventAO::NewL( MBufferEmptiedObserver& aObserver )
    {
    CBufferEmptiedEventAO* self = new (ELeave)CBufferEmptiedEventAO( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CBufferEmptiedEventAO::ConstructL()
    {
    }

void CBufferEmptiedEventAO::SetActive()
    {
    if (!IsActive())
        {
        CActive::SetActive();
        }
    }

TInt CBufferEmptiedEventAO::Error()
    {
    return iError;
    }

// From CActive
void CBufferEmptiedEventAO::RunL()
    {
    // Save the error code
    iError = iStatus.Int();
    // Signal the observer that this request is serviced
    iObserver.BufferEmptiedBySource();
    }

void CBufferEmptiedEventAO::DoCancel()
    {
    if(iStatus.Int() != 0)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status,KErrCancel);
        }
    }

TInt CBufferEmptiedEventAO::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

void CBufferEmptiedEventAO::SetBuffer(MDataBuffer* aBuffer)
    {
    iBuffer = aBuffer;
    }

MDataBuffer* CBufferEmptiedEventAO::GetBuffer()
    {
    return iBuffer;
    }

// End of File
