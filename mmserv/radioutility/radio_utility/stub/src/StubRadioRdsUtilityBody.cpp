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
* Description:  Radio RDS Utility body's stub implementation
*
*/


#include "RadioRdsUtilityBody.h"
#include "RadioServerData.h"
#include "RadioSession.h"
#include "RadioStubManager.h"

#define STUB  CRadioStubManager::GetRadioStubManager()->iRds

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRadioRdsUtility::CBody* CRadioRdsUtility::CBody::NewL(
    RRadioSession& aRadioSession,
    MRadioRdsObserver& aObserver )
    {
    if ( STUB.iLeaveNewL.iError )
        {
        User::Leave( STUB.iLeaveNewL.iError );
        }
    CRadioRdsUtility::CBody* s = new(ELeave) CRadioRdsUtility::CBody( aRadioSession, aObserver );
    CleanupStack::PushL(s);
    s->ConstructL();
    CleanupStack::Pop();
    return s;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::ConstructL()
    {
    if ( STUB.iLeaveErrorForConstructL.iError )
        {
        User::Leave( STUB.iLeaveErrorForConstructL.iError );
        }
    STUB.iRadioRdsUtilityClient = &iRadioRdsUtilityClient;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CBody
// -----------------------------------------------------------------------------
//
CRadioRdsUtility::CBody::CBody(
        RRadioSession& aRadioSession,
        MRadioRdsObserver& aObserver )
    : iRadioSession(aRadioSession),
    iRadioRdsUtilityClient(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CRadioRdsUtility::CBody::~CBody()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::Close
// Cancel all requested RDS notifications if any were requested.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::Close()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetCapabilities
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetCapabilities(
    TRdsCapabilities& aCaps ) const
    {
    if ( !STUB.iGetCapabilitiesError.Error() )
        {
        aCaps.iRdsFunctions = STUB.iCaps.iRdsFunctions;
        aCaps.iAdditionalFunctions1 = STUB.iCaps.iAdditionalFunctions1;
        aCaps.iAdditionalFunctions2 = STUB.iCaps.iAdditionalFunctions2;
        }
    return STUB.iGetCapabilitiesError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetRdsSignalStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetRdsSignalStatus(
    TBool& aRdsSignal ) const
    {
    aRdsSignal = STUB.iRdsSignal;
    return STUB.iGetRdsSignalStatusError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::NotifyRdsDataChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::NotifyRdsDataChange(
    TRdsData aRdsData )
    {
    iRdsDataReq.iRdsFunctions = aRdsData.iRdsFunctions;
    iRdsDataReq.iAdditionalFunctions1 = aRdsData.iAdditionalFunctions1;
    iRdsDataReq.iAdditionalFunctions2 = aRdsData.iAdditionalFunctions2;
    return STUB.iNotifyRdsDataChangeError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelNotifyRdsDataChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelNotifyRdsDataChange()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::SetAutomaticSwitching
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::SetAutomaticSwitching(
    TBool aAuto )
    {
    STUB.iAutomaticSwitching = aAuto;
    return STUB.iSetAutomaticSwitchingError.Error();

    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetAutomaticSwitching
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetAutomaticSwitching(
    TBool& aAuto )
    {
    aAuto = STUB.iAutomaticSwitching;
    return STUB.iGetAutomaticSwitchingError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelAFSearch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelAFSearch()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::SetAutomaticTrafficAnnouncement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::SetAutomaticTrafficAnnouncement(
    TBool aAuto )
    {
    STUB.iTaStatus = aAuto;
    return STUB.iSetAutomaticTrafficAnnouncementError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetAutomaticTrafficAnnouncement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetAutomaticTrafficAnnouncement(
    TBool& aAuto )
    {
    aAuto = STUB.iTaStatus;
    return STUB.iGetAutomaticTrafficAnnouncementError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::StationSeekByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::StationSeekByPTY(
    TRdsProgrammeType /*aPty*/,
    TBool /*aUpwards*/ )
    {
    iRadioRdsUtilityClient.MrroStationSeekByPTYComplete( 
            STUB.iStationSeekByPTYError.Error(), STUB.iFrequency );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::StationSeekByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::StationSeekByTA(
    TBool /*aSeekUp*/ )
    {
    iRadioRdsUtilityClient.MrroStationSeekByTAComplete( STUB.iStationSeekByTAError.Error(), STUB.iFrequency );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::StationSeekByTP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::StationSeekByTP(
    TBool /*aSeekUp*/ )
    {
    iRadioRdsUtilityClient.MrroStationSeekByTPComplete( STUB.iStationSeekByTPError.Error(), STUB.iFrequency );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelRdsStationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelRdsStationSeek()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetFreqByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetFreqByPTY(
    TRdsProgrammeType /*aPty*/ )
    {
    iRadioRdsUtilityClient.MrroGetFreqByPTYComplete( STUB.iGetFreqByPTYError.Error(), STUB.iFreqList );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetFreqByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetFreqByPTY()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetFreqByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetFreqByTA()
    {
    iRadioRdsUtilityClient.MrroGetFreqByTAComplete( STUB.iGetFreqByTAError.Error(), STUB.iFreqList );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetFreqByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetFreqByTA()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetPSByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetPSByPTY(
    TRdsProgrammeType /*aPty*/ )
    {
    iRadioRdsUtilityClient.MrroGetPSByPTYComplete( STUB.iGetPSByPTYError.Error(), STUB.iPsList );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetPSByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetPSByPTY()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetPSByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetPSByTA()
    {
    iRadioRdsUtilityClient.MrroGetPSByPTYComplete( STUB.iGetPSByTAError.Error(), STUB.iPsList );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetPSByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetPSByTA()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetProgrammeIdentification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetProgrammeIdentification(
    TInt& /*aPi*/ )
    {
    return STUB.iGetProgrammeIdentificationError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetProgrammeType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetProgrammeType(
    TRdsProgrammeType& /*aPty*/ )
    {
    return STUB.iGetProgrammeTypeError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetProgrammeService
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetProgrammeService(
    TRdsPSName& aPs )
    {
    if ( STUB.iPs.Length() )
        {
        aPs.Copy( STUB.iPs );
        }

    return STUB.iGetProgrammeServiceError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetRadioText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetRadioText(
    TRdsRadioText& aRt )
    {
    if ( STUB.iRtPlusData.Length() )
       {
       aRt.Copy( STUB.iRtPlusData );
       }

    return STUB.iGetRadioTextError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetRadioTextPlus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetRadioTextPlus(
    TRdsRTplusClass /*aRtPlusClass*/,
    TRdsRadioText& aRtPlusData )
    {

    if ( STUB.iRtPlusData.Length() )
        {
        aRtPlusData.Copy( STUB.iRtPlusData );
        }

    return STUB.iGetRadioTextPlusError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetClockTime
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetClockTime(
    TDateTime& aCt )
    {
    aCt = STUB.iCt;
    return STUB.iGetClockTimeError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetTrafficAnnouncementStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetTrafficAnnouncementStatus(
    TBool& aTaStatus )
    {
    STUB.iTaStatus = aTaStatus;
    return STUB.iGetTrafficAnnouncementStatusError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetTrafficProgrammeStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetTrafficProgrammeStatus(
    TBool& aTpStatus )
    {
    aTpStatus = STUB.iTpStatus;
    return STUB.iGetTrafficProgrammeStatusError.Error();
    }


//End of File
