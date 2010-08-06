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
* Description:  Radio FM Tuner Utility implementation body's stub.
*
*/


#include "RadioFmTunerUtilityBody.h"
#include "RadioUtilityBody.h"
#include "RadioStubManager.h"


#define STUB  CRadioStubManager::GetRadioStubManager()->iTuner

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRadioFmTunerUtility::CBody* CRadioFmTunerUtility::CBody::NewL(
    RRadioSession& aRadioSession,
    MRadioFmTunerObserver& aObserver )
    {
    if ( STUB.iLeaveNewL.iError )
        {
        User::Leave( STUB.iLeaveNewL.iError );
        }
    CRadioFmTunerUtility::CBody* s = new(ELeave) CRadioFmTunerUtility::CBody( aRadioSession, aObserver );
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
    if ( STUB.iLeaveErrorForConstructL.iError )
        {
        User::Leave( STUB.iLeaveErrorForConstructL.iError );
        }
    STUB.iRadioFmTunerUtilityClient = &iRadioFmTunerUtilityClient;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CBody
// -----------------------------------------------------------------------------
//
CRadioFmTunerUtility::CBody::CBody(
        RRadioSession& aRadioSession,
        MRadioFmTunerObserver& aObserver )
    :iRadioSession( aRadioSession ), 
    iRadioFmTunerUtilityClient( aObserver )
    
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
    aCaps.iTunerBands = STUB.iCaps.iTunerBands;
    aCaps.iTunerFunctions = STUB.iCaps.iTunerFunctions;
    aCaps.iAdditionalFunctions1 = STUB.iCaps.iAdditionalFunctions1;
    aCaps.iAdditionalFunctions2 = STUB.iCaps.iAdditionalFunctions2;
    return STUB.iGetCapabilitiesError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::EnableTunerInOfflineMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::EnableTunerInOfflineMode(
    TBool aEnable )
    {
    STUB.iTunerInOfflineMode = aEnable;
    return STUB.iEnableTunerInOfflineModeError.Error();
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

    aRange = STUB.iRange;
    aMinFreq = STUB.iMinFreq;
    aMaxFreq = STUB.iMaxFreq;
#ifdef _DEBUG
    RDebug::Print(_L("CRadioFmTunerUtility::CBody::GetFrequencyRange, aRange = %d, aMinFreq = %d, aMaxFreq = %d"),
        aRange, aMinFreq, aMaxFreq);
#endif
    return STUB.iGetFrequencyRangeError.Error();
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
    STUB.iPreviousRange = STUB.iRange;
    STUB.iRange = aRange;
    }


// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CancelSetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::CancelSetFrequencyRange()
    {
    STUB.iRange = STUB.iPreviousRange;
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
    STUB.iPreviousFrequency = STUB.iFrequency;
    STUB.iFrequency = aFrequency;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CancelSetFrequencyRange
// Body of CancelSetFrequencyRange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::CancelSetFrequency()
    {
    STUB.iFrequency = STUB.iPreviousFrequency;
    STUB.iPreviousFrequency = 0;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetFrequency
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetFrequency(
    TInt& aFrequency ) const
    {
    aFrequency = STUB.iFrequency;
    return STUB.iGetFrequencyError.Error();
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
    STUB.iStationSeekUpwards = aUpwards;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::CancelStationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmTunerUtility::CBody::CancelStationSeek()
    {
    STUB.iStationSeekUpwards = EFalse;
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetSignalStrength
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetSignalStrength(
    TInt& aSignalStrength ) const
    {
    aSignalStrength = STUB.iSignalStrength;
    return STUB.iGetSignalStrengthError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::GetMaxSignalStrength
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetMaxSignalStrength(
    TInt& aMaxSignalStrength ) const
    {
    aMaxSignalStrength = STUB.iMaxSignalStrength;
    return STUB.iGetMaxSignalStrengthError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetStereoMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetStereoMode(
    TBool& aStereo ) const
    {
    aStereo = STUB.iStereoMode;
    return STUB.iGetStereoModeError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::ForceMonoReception
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::ForceMonoReception(
    TBool aForcedMono)
    {
    STUB.iForcedMono = aForcedMono;
    return STUB.iForcedMonoReceptionError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetForcedMonoReception
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetForcedMonoReception(
    TBool& aForcedMono ) const
    {
    aForcedMono = STUB.iForcedMono;
    return STUB.iGetForcedMonoReceptionError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::SetSquelch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::SetSquelch(
    TBool aSquelch )
    {
    STUB.iSquelch = aSquelch;
    return STUB.iSetSquelchError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioFmTunerUtility::CBody::GetSquelch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmTunerUtility::CBody::GetSquelch(
    TBool& aSquelch ) const
    {
    aSquelch = STUB.iSquelch;
    return STUB.iGetSquelchError.Error();
    }

// End of File





