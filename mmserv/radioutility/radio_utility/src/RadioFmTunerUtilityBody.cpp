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
* Description:  Radio FM Tuner Utility implementation body.
*
*/


#include "RadioFmTunerUtilityBody.h"
#include "RadioUtilityBody.h"

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRadioFmTunerUtility::CBody* CRadioFmTunerUtility::CBody::NewL(
    RRadioSession& aRadioSession,
    MRadioFmTunerObserver& aObserver )
    {
    CRadioFmTunerUtility::CBody* s = new(ELeave) CRadioFmTunerUtility::CBody();
    s->iRadioFmTunerUtilityClient = &aObserver;
    s->iRadioSession = &aRadioSession;
    CleanupStack::PushL(s);
    s->ConstructL();
    CleanupStack::Pop();
    return s;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CBody
// -----------------------------------------------------------------------------
//
CRadioFmTunerUtility::CBody::CBody()
    {
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::~CBody
// -----------------------------------------------------------------------------
//
CRadioFmTunerUtility::CBody::~CBody()
    {
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::RequestTunerControl
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::RequestTunerControl()
    {
    iRadioSession->RequestTunerControl( ERsTunerFm );
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::Close
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::Close()
    {
    // Consider releasing tuner control here.
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetCapabilities
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetCapabilities(
    TFmTunerCapabilities& aCaps ) const
    {
    TRsTunerCapabilities caps;
    TInt error = iRadioSession->GetTunerCapabilities( caps );

    if ( !error )
        {
        aCaps.iTunerBands = caps.iFrequencyRange;
        aCaps.iTunerFunctions = caps.iCapabilities;
        aCaps.iAdditionalFunctions1 = caps.iAdditionalFunctions1;
        aCaps.iAdditionalFunctions2 = caps.iAdditionalFunctions2;

        }
    return error;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::EnableTunerInOfflineMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::EnableTunerInOfflineMode(
    TBool aEnable )
    {
    return iRadioSession->EnableTunerInOfflineMode( aEnable );
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetFrequencyRange(
    TFmRadioFrequencyRange& aRange,
    TInt& aMinFreq,
    TInt& aMaxFreq ) const
    {

    TRsFrequencyRange range;
    TInt error = iRadioSession->GetFrequencyRange( range, aMinFreq, aMaxFreq );
    if ( !error )
        {
    switch ( range )
        {
        case ERsRangeFmEuroAmerica:
            aRange = EFmRangeEuroAmerica;
            break;
        case ERsRangeFmJapan:
            aRange = EFmRangeJapan;
            break;
        default:
            break;
            }
        }
#ifdef _DEBUG
    RDebug::Print(_L("CRadioFmTunerUtility::CBody::GetFrequencyRange, aRange = %d, aMinFreq = %d, aMaxFreq = %d"),
        aRange, aMinFreq, aMaxFreq);
#endif
    return error;
    }


// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::SetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::SetFrequencyRange(
    TFmRadioFrequencyRange aRange )
    {
#ifdef _DEBUG
    RDebug::Print(_L("CRadioFmTunerUtility::CBody::SetFrequencyRange, aRange = %d"), aRange);
#endif
    TRsFrequencyRange range = ERsRangeFmEuroAmerica;    //default
    switch (aRange)
        {
        case EFmRangeEuroAmerica:
            range = ERsRangeFmEuroAmerica;
            break;
        case EFmRangeJapan:
            range = ERsRangeFmJapan;
            break;
        default:
            break;
        }
    iRadioSession->SetFrequencyRange( range );
    }


// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CancelSetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::CancelSetFrequencyRange()
    {
    iRadioSession->CancelSetFrequencyRange();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::SetFrequency
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::SetFrequency(
    TInt aFrequency )
    {
#ifdef _DEBUG
    RDebug::Print(_L("CRadioFmTunerUtility::CBody::SetFrequency, aFrequency = %d"), aFrequency);
#endif
    iRadioSession->SetFrequency(aFrequency);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CancelSetFrequencyRange
// Body of CancelSetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::CancelSetFrequency()
    {
    iRadioSession->CancelSetFrequency();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetFrequency
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetFrequency(
    TInt& aFrequency ) const
    {
    return iRadioSession->GetFrequency(aFrequency);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::StationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::StationSeek(
    TBool aUpwards )
    {
#ifdef _DEBUG
    RDebug::Print(_L("CRadioFmTunerUtility::CBody::StationSeek, aUpwards = %d"), aUpwards);
#endif
    iRadioSession->StationSeek(aUpwards);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CancelStationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::CancelStationSeek()
    {
    iRadioSession->CancelStationSeek();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetSignalStrength
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetSignalStrength(
    TInt& aSignalStrength ) const
    {
    return iRadioSession->GetSignalStrength(aSignalStrength);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetMaxSignalStrength
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetMaxSignalStrength(
    TInt& aMaxSignalStrength ) const
    {
    return iRadioSession->GetMaxSignalStrength(aMaxSignalStrength);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetStereoMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetStereoMode(
    TBool& aStereo ) const
    {
    return iRadioSession->GetStereoMode(aStereo);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::ForceMonoReception
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::ForceMonoReception(
    TBool aForcedMono)
    {
    return iRadioSession->ForceMonoReception(aForcedMono);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetForcedMonoReception
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetForcedMonoReception(
    TBool& aForcedMono ) const
    {
    return iRadioSession->GetForceMonoReception(aForcedMono);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::SetSquelch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::SetSquelch(
    TBool aSquelch )
    {
    return iRadioSession->SetSquelch(aSquelch);
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetSquelch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetSquelch(
    TBool& aSquelch ) const
    {
    return iRadioSession->GetSquelch(aSquelch);
    }

// End of File





