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
* Description:  Radio RDS Utility body implementation
*
*/


#include "RadioRdsUtilityBody.h"
#include "RadioServerData.h"
#include "RadioSession.h"

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
    CRadioRdsUtility::CBody* s = new(ELeave) CRadioRdsUtility::CBody();
    s->iRadioRdsUtilityClient = &aObserver;
    s->iRadioSession = &aRadioSession;
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
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CBody
// -----------------------------------------------------------------------------
//
CRadioRdsUtility::CBody::CBody()
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
    if ( iRdsDataReq.iRdsFunctions || iRdsDataReq.iAdditionalFunctions1 ||
         iRdsDataReq.iAdditionalFunctions2 )
        {
        iRadioSession->CancelNotifyRdsDataChange();
        }
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetCapabilities
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetCapabilities(
    TRdsCapabilities& aCaps ) const
    {
    TRsRdsCapabilities Caps;
    TInt error = iRadioSession->GetRdsCapabilities(Caps);
    if ( !error )
        {
        aCaps.iRdsFunctions = Caps.iRdsFunctions;
        aCaps.iAdditionalFunctions1 = Caps.iAdditionalFunctions1;
        aCaps.iAdditionalFunctions2 = Caps.iAdditionalFunctions2;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetRdsSignalStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetRdsSignalStatus(
    TBool& aRdsSignal ) const
    {
    return iRadioSession->GetRdsSignalStatus( aRdsSignal );
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
    return iRadioSession->NotifyRdsDataChange( iRdsDataReq );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelNotifyRdsDataChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelNotifyRdsDataChange()
    {
    iRadioSession->CancelNotifyRdsDataChange();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::SetAutomaticSwitching
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::SetAutomaticSwitching(
    TBool aAuto )
    {
    return iRadioSession->SetAutomaticSwitching( aAuto );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetAutomaticSwitching
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetAutomaticSwitching(
    TBool& aAuto )
    {
    return iRadioSession->GetAutomaticSwitching( aAuto );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelAFSearch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelAFSearch()
    {
    iRadioSession->CancelAFSearch();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::SetAutomaticTrafficAnnouncement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::SetAutomaticTrafficAnnouncement(
    TBool aAuto )
    {
    return iRadioSession->SetAutomaticTrafficAnnouncement( aAuto );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetAutomaticTrafficAnnouncement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetAutomaticTrafficAnnouncement(
    TBool& aAuto )
    {
    return iRadioSession->GetAutomaticTrafficAnnouncement( aAuto );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::StationSeekByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::StationSeekByPTY(
    TRdsProgrammeType aPty,
    TBool aUpwards )
    {
    iRadioSession->StationSeekByPTY( aPty, aUpwards );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::StationSeekByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::StationSeekByTA(
    TBool aSeekUp )
    {
    iRadioSession->StationSeekByTA( aSeekUp );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::StationSeekByTP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::StationSeekByTP(
    TBool aSeekUp )
    {
    iRadioSession->StationSeekByTP( aSeekUp );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelRdsStationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelRdsStationSeek()
    {
    iRadioSession->CancelRdsStationSeek();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetFreqByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetFreqByPTY(
    TRdsProgrammeType aPty )
    {
    iRadioSession->GetFreqByPTY( aPty );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetFreqByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetFreqByPTY()
    {
    iRadioSession->CancelGetFreqByPTY();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetFreqByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetFreqByTA()
    {
    iRadioSession->GetFreqByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetFreqByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetFreqByTA()
    {
    iRadioSession->CancelGetFreqByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetPSByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetPSByPTY(
    TRdsProgrammeType aPty )
    {
    iRadioSession->GetPSByPTY( aPty );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetPSByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetPSByPTY()
    {
    iRadioSession->CancelGetPSByPTY();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetPSByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::GetPSByTA()
    {
    iRadioSession->GetPSByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::CancelGetPSByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioRdsUtility::CBody::CancelGetPSByTA()
    {
    iRadioSession->CancelGetPSByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetProgrammeIdentification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetProgrammeIdentification(
    TInt& aPi )
    {
    return iRadioSession->GetProgrammeIdentification( aPi );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetProgrammeType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetProgrammeType(
    TRdsProgrammeType& aPty )
    {
    return iRadioSession->GetProgrammeType( aPty );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetProgrammeService
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetProgrammeService(
    TRdsPSName& aPs )
    {
    TRsRdsPSName ps;
    TInt error = iRadioSession->GetProgrammeService( ps );

    if ( aPs.Length() )
        {
        aPs.Copy( ps );
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetRadioText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetRadioText(
    TRdsRadioText& aRt )
    {
    TRsRdsRadioText rt;
    TInt error = iRadioSession->GetRadioText( rt );

    if ( rt.Length() )
       {
       aRt.Copy( rt );
       }

    return error;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetRadioTextPlus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetRadioTextPlus(
    TRdsRTplusClass aRtPlusClass,
    TRdsRadioText& aRtPlusData )
    {
    aRtPlusClass = aRtPlusClass; // for compiler warning
    TRsRdsRadioText rt;
    TRsRdsRTplusClass rtPlusClass = ERsRTplusItemDummy;
    TInt error = iRadioSession->GetRadioTextPlus( rtPlusClass, rt );

    if ( rt.Length() )
        {
        aRtPlusData.Copy( rt );
        aRtPlusClass = (TRdsRTplusClass)rtPlusClass;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetClockTime
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetClockTime(
    TDateTime& aCt )
    {
    return iRadioSession->GetClockTime( aCt );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetTrafficAnnouncementStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetTrafficAnnouncementStatus(
    TBool& aTaStatus )
    {
    return iRadioSession->GetTrafficAnnouncementStatus( aTaStatus );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CBody::GetTrafficProgrammeStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioRdsUtility::CBody::GetTrafficProgrammeStatus(
    TBool& aTpStatus )
    {
    return iRadioSession->GetTrafficProgrammeStatus( aTpStatus );
    }


//End of File
