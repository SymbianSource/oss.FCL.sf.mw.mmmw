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
* Description:  Radio Utility API implementation.
*
*/


#include "radioutilitybody.h"


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CRadioUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioUtility* CRadioUtility::NewL(
    TBool aPrimaryClient )
    {
    CRadioUtility* self = new(ELeave) CRadioUtility();
    CleanupStack::PushL(self);
    self->iBody = CRadioUtility::CBody::NewL(aPrimaryClient);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioUtility::~CRadioUtility()
    {
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CRadioUtility::CRadioUtility
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRadioUtility::CRadioUtility()
    {
    }

// -----------------------------------------------------------------------------
// CRadioUtility::RadioFmTunerUtilityL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioFmTunerUtility& CRadioUtility::RadioFmTunerUtilityL(
    MRadioFmTunerObserver& aObserver )
    {
    ASSERT(iBody);
    return iBody->RadioFmTunerUtilityL(aObserver);
    }

// -----------------------------------------------------------------------------
// CRadioUtility::RadioPlayerUtilityL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioPlayerUtility& CRadioUtility::RadioPlayerUtilityL(
    MRadioPlayerObserver& aObserver )
    {
    ASSERT(iBody);
    return iBody->RadioPlayerUtilityL(aObserver);
    }

// -----------------------------------------------------------------------------
// CRadioUtility::RadioRdsUtilityL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioRdsUtility& CRadioUtility::RadioRdsUtilityL(
    MRadioRdsObserver& aObserver )
    {
    ASSERT(iBody);
    return iBody->RadioRdsUtilityL(aObserver);
    }



// End of File
