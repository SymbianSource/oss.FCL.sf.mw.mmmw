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
* Description:  Radio FM Tuner Utility interface implementation.
*
*/


#include <RadioFmTunerUtility.h>
#include "RadioFmTunerUtilityBody.h"

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::NewL
// Destructor
// -----------------------------------------------------------------------------
//
CRadioFmTunerUtility* CRadioFmTunerUtility::NewL(
    RRadioSession& aRadioSession,
    MRadioFmTunerObserver& aObserver)
    {
    CRadioFmTunerUtility* self = new(ELeave) CRadioFmTunerUtility;
    CleanupStack::PushL(self);
    self->iBody = CRadioFmTunerUtility::CBody::NewL(aRadioSession, aObserver);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::~CRadioFmTunerUtility
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioFmTunerUtility::~CRadioFmTunerUtility()
    {
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CRadioFmTunerUtility
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRadioFmTunerUtility::CRadioFmTunerUtility()
    {
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::RequestTunerControl
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::RequestTunerControl()
    {
    ASSERT(iBody);
    iBody->RequestTunerControl();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::Close
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::Close()
    {
    ASSERT(iBody);
    iBody->Close();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetCapabilities
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetCapabilities(
    TFmTunerCapabilities& aCaps)
    {
    ASSERT(iBody);
    return iBody->GetCapabilities(aCaps);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::EnableTunerInOfflineMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::EnableTunerInOfflineMode(
    TBool aEnable )

    {
    ASSERT(iBody);
    return iBody->EnableTunerInOfflineMode(aEnable);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::SetFrequency
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::SetFrequency(
    TInt aFrequency )
    {
    ASSERT(iBody);
    iBody->SetFrequency(aFrequency);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CancelSetFrequency
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::CancelSetFrequency()
    {
    ASSERT(iBody);
    iBody->CancelSetFrequency();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetFrequency
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetFrequency(
    TInt& aFrequency ) const
    {
    ASSERT(iBody);
    return iBody->GetFrequency(aFrequency);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::SetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::SetFrequencyRange(
    TFmRadioFrequencyRange aRange )
    {
    ASSERT(iBody);
    iBody->SetFrequencyRange(aRange);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CancelSetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::CancelSetFrequencyRange()
    {
    ASSERT(iBody);
    iBody->CancelSetFrequencyRange();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetFrequencyRange(
    TFmRadioFrequencyRange& aRange,
    TInt& aMinFreq,
    TInt& aMaxFreq ) const
    {
    ASSERT(iBody);
    return iBody->GetFrequencyRange(aRange, aMinFreq, aMaxFreq);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::StationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::StationSeek(
    TBool aUpwards )
    {
    ASSERT(iBody);
    iBody->StationSeek(aUpwards);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CancelStationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmTunerUtility::CancelStationSeek()
    {
    ASSERT(iBody);
    iBody->CancelStationSeek();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetSignalStrength
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetSignalStrength(
    TInt& aSignalStrength ) const
    {
    ASSERT(iBody);
    return iBody->GetSignalStrength(aSignalStrength);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetMaxSignalStrength
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetMaxSignalStrength(
    TInt& aMaxSignalStrength ) const
    {
    ASSERT(iBody);
    return iBody->GetMaxSignalStrength(aMaxSignalStrength);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetStereoMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetStereoMode(
    TBool& aStereo ) const
    {
    ASSERT(iBody);
    return iBody->GetStereoMode(aStereo);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::ForceMonoReception
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::ForceMonoReception(
    TBool aForcedMono )
    {
    ASSERT(iBody);
    return iBody->ForceMonoReception(aForcedMono);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetForcedMonoReception
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetForcedMonoReception(
    TBool& aForcedMono ) const
    {
    ASSERT(iBody);
    return iBody->GetForcedMonoReception(aForcedMono);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::SetSquelch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::SetSquelch(
    TBool aSquelch )
    {
    ASSERT(iBody);
    return iBody->SetSquelch(aSquelch);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetSquelch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmTunerUtility::GetSquelch(
    TBool& aSquelch ) const
    {
    ASSERT(iBody);
    return iBody->GetSquelch(aSquelch);
    }

// End of File
