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
* Description:  Radio RDS Utility API implementation
*
*/


#include "RadioRdsUtilityBody.h"

// -----------------------------------------------------------------------------
// CRadioRdsUtility::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRadioRdsUtility* CRadioRdsUtility::NewL(
    RRadioSession& aRadioSession,
    MRadioRdsObserver& aObserver)
    {
    CRadioRdsUtility* self = new(ELeave) CRadioRdsUtility;
    CleanupStack::PushL(self);
    self->iBody = CRadioRdsUtility::CBody::NewL(aRadioSession, aObserver);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::~CRadioRdsUtility
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRadioRdsUtility::~CRadioRdsUtility()
    {
        delete iBody;
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CRadioRdsUtility
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRadioRdsUtility::CRadioRdsUtility()
    {
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::Close
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::Close()
    {
    ASSERT(iBody);
    return iBody->Close();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetCapabilities
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetCapabilities(
    TRdsCapabilities& aCaps ) const
    {
    ASSERT(iBody);
    return iBody->GetCapabilities(aCaps);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetRdsSignalStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetRdsSignalStatus(
    TBool& aRdsSignal ) const
    {
    ASSERT(iBody);
    return iBody->GetRdsSignalStatus( aRdsSignal );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::NotifyRdsDataChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::NotifyRdsDataChange(
    TRdsData aRdsData )
    {
    ASSERT(iBody);
    return iBody->NotifyRdsDataChange( aRdsData );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CancelNotifyRdsDataChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::CancelNotifyRdsDataChange()
    {
    ASSERT(iBody);
    iBody->CancelNotifyRdsDataChange();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::SetAutomaticSwitching
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::SetAutomaticSwitching(
    TBool aAuto )
    {
    ASSERT(iBody);
    return iBody->SetAutomaticSwitching(aAuto);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetAutomaticSwitching
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetAutomaticSwitching(
    TBool& aAuto )
    {
    ASSERT(iBody);
    return iBody->GetAutomaticSwitching(aAuto);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CancelAFSearch
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::CancelAFSearch()
    {
    ASSERT(iBody);
    iBody->CancelAFSearch();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::SetAutomaticTrafficAnnouncement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::SetAutomaticTrafficAnnouncement(
    TBool aAuto )
    {
    ASSERT(iBody);
    return iBody->SetAutomaticTrafficAnnouncement(aAuto);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetAutomaticTrafficAnnouncement
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetAutomaticTrafficAnnouncement(
    TBool& aAuto )
    {
    ASSERT(iBody);
    return iBody->GetAutomaticTrafficAnnouncement(aAuto);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::StationSeekByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::StationSeekByPTY(
    TRdsProgrammeType aPty,
    TBool aUpwards )
    {
    ASSERT(iBody);
    iBody->StationSeekByPTY(aPty, aUpwards);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::StationSeekByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::StationSeekByTA(
    TBool aSeekUp )
    {
    ASSERT(iBody);
    iBody->StationSeekByTA(aSeekUp);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::StationSeekByTP
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::StationSeekByTP(
    TBool aSeekUp )
    {
    ASSERT(iBody);
    iBody->StationSeekByTP(aSeekUp);
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CancelRdsStationSeek
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::CancelRdsStationSeek()
    {
    ASSERT(iBody);
    iBody->CancelRdsStationSeek();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetFreqByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::GetFreqByPTY(
    TRdsProgrammeType aPty )
    {
    ASSERT(iBody);
    iBody->GetFreqByPTY( aPty );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CancelGetFreqByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::CancelGetFreqByPTY()
    {
    ASSERT(iBody);
    iBody->CancelGetFreqByPTY();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetFreqByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::GetFreqByTA()
    {
    ASSERT(iBody);
    iBody->GetFreqByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CancelGetFreqByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::CancelGetFreqByTA()
    {
    ASSERT(iBody);
    iBody->CancelGetFreqByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetPSByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::GetPSByPTY(
    TRdsProgrammeType aPty )
    {
    ASSERT(iBody);
    iBody->GetPSByPTY( aPty );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CancelGetPSByPTY
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::CancelGetPSByPTY()
    {
    ASSERT(iBody);
    iBody->CancelGetPSByPTY();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetPSByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::GetPSByTA()
    {
    ASSERT(iBody);
    iBody->GetPSByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::CancelGetPSByTA
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CRadioRdsUtility::CancelGetPSByTA()
    {
    ASSERT(iBody);
    iBody->CancelGetPSByTA();
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetProgrammeIdentification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetProgrammeIdentification(
    TInt& aPi )
    {
    ASSERT(iBody);
    return iBody->GetProgrammeIdentification( aPi );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetProgrammeType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetProgrammeType(
    TRdsProgrammeType& aPty )
    {
    ASSERT(iBody);
    return iBody->GetProgrammeType( aPty );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetProgrammeService
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetProgrammeService(
    TRdsPSName& aPs )
    {
    ASSERT(iBody);
    return iBody->GetProgrammeService( aPs );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetRadioText
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetRadioText(
    TRdsRadioText& aRt )
    {
    ASSERT(iBody);
    return iBody->GetRadioText( aRt );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetRadioTextPlus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetRadioTextPlus(
    TRdsRTplusClass aRtPlusClass,
    TRdsRadioText& aRtPlusData )
    {
    ASSERT(iBody);
    return iBody->GetRadioTextPlus( aRtPlusClass,aRtPlusData );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetClockTime
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetClockTime(
    TDateTime& aCt )
    {
    ASSERT(iBody);
    return iBody->GetClockTime( aCt );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetTrafficAnnouncementStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetTrafficAnnouncementStatus(
    TBool& aTaStatus )
    {
    ASSERT(iBody);
    return iBody->GetTrafficAnnouncementStatus( aTaStatus );
    }

// -----------------------------------------------------------------------------
// CRadioRdsUtility::GetTrafficProgrammeStatus
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt CRadioRdsUtility::GetTrafficProgrammeStatus(
    TBool& aTpStatus )
    {
    ASSERT(iBody);
    return iBody->GetTrafficProgrammeStatus( aTpStatus );
    }


// End of File
