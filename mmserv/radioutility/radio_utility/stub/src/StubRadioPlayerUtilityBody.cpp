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
* Description:  Radio Player Utility body's stub implementation
*
*/


#include <custominterfaceutility.h>
#include "RadioServerData.h"
#include "RadioPlayerUtilityBody.h"
#include "RadioSession.h"
#include "RadioStubManager.h"

#define STUB  CRadioStubManager::GetRadioStubManager()->iPlayer

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CRadioPlayerUtility::CBody* CRadioPlayerUtility::CBody::NewL(
    RRadioSession& aRadioSession,
    MRadioPlayerObserver& aObserver )
    {
    if ( STUB.iLeaveNewL.iError )
        {
        User::Leave( STUB.iLeaveNewL.iError );
        }
    CRadioPlayerUtility::CBody* s = new(ELeave) CRadioPlayerUtility::CBody( aRadioSession, aObserver);
    CleanupStack::PushL(s);
    s->ConstructL();
    CleanupStack::Pop();
    return s;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CRadioPlayerUtility::CBody::ConstructL()
    {
    if ( STUB.iLeaveErrorForConstructL.iError )
        {
        User::Leave( STUB.iLeaveErrorForConstructL.iError );
        }
    STUB.iRadioPlayerUtilityClient = &iRadioPlayerUtilityClient;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::CBody
// -----------------------------------------------------------------------------
//
CRadioPlayerUtility::CBody::CBody( 
        RRadioSession& aRadioSession,
        MRadioPlayerObserver& aObserver )
    :iRadioSession( aRadioSession ),
    iRadioPlayerUtilityClient( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CRadioPlayerUtility::CBody::~CBody()
    {
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::PlayerState
// Get the player's state.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
//
void CRadioPlayerUtility::CBody::PlayerState(
    TPlayerState& aPlayerState ) const
    {
    aPlayerState = STUB.iPlayerState;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::Close
// Stops radio playback only if no other primary clients are connected to the
// radio server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioPlayerUtility::CBody::Close()
    {
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::Play
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioPlayerUtility::CBody::Play()
    {
    STUB.iPlayerState = ERadioPlayerPlaying;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::Stop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioPlayerUtility::CBody::Stop()
    {
    STUB.iPlayerState = ERadioPlayerIdle;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::Mute
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioPlayerUtility::CBody::Mute(
    TBool aMute )
    {
    STUB.iMuteStatus = aMute;
    return STUB.iMuteError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::IsMute
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CRadioPlayerUtility::CBody::IsMute()
    {
    return STUB.iMuteStatus;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::SetVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioPlayerUtility::CBody::SetVolume(
    TInt aVolume )
    {
    STUB.iVolume = aVolume;
    return STUB.iSetVolumeError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::GetVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioPlayerUtility::CBody::GetVolume(
    TInt& aVolume ) const
    {
    aVolume = STUB.iVolume;
    return STUB.iGetVolumeError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::SetVolumeRamp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioPlayerUtility::CBody::SetVolumeRamp(
    const TTimeIntervalMicroSeconds& aRampInterval )
    {
#ifdef _DEBUG
    RDebug::Print(_L("CRadioPlayerUtility::CBody::SetVolumeRamp"));
#endif
    STUB.iRampInterval = aRampInterval;
    return STUB.iSetVolumeRampError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::GetMaxVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioPlayerUtility::CBody::GetMaxVolume(
    TInt& aMaxVolume ) const
    {
    aMaxVolume = STUB.iMaxVolume;
    return STUB.iGetMaxVolumeError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::SetBalance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioPlayerUtility::CBody::SetBalance(
    TInt aLeftPercentage,
    TInt aRightPercentage )
    {
#ifdef _DEBUG
    RDebug::Print(_L("CRadioPlayerUtility::CBody::SetBalance, aLeftPercentage = %d, aRightPercentage = %d"),
        aLeftPercentage, aRightPercentage);
#endif
    STUB.iLeftPercentage = aLeftPercentage;
    STUB.iRightPercentage = aRightPercentage;
    return STUB.iSetBalanceError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::GetBalance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioPlayerUtility::CBody::GetBalance(
    TInt& aLeftPercentage,
    TInt& aRightPercentage ) const
    {
    aLeftPercentage = STUB.iLeftPercentage;
    aRightPercentage = STUB.iRightPercentage;
    return STUB.iGetBalanceError.Error();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CBody::CustomInterface
// Get a custom interface for the specified interface Id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny* CRadioPlayerUtility::CBody::CustomInterface(
    TUid aInterfaceId )
    {
//    TAny* ci = NULL;
//    return ci;
#ifdef _DEBUG
    RDebug::Print(_L("CRadioPlayerUtility::CBody::CustomInterface, aInterfaceId = %d"), aInterfaceId);
#endif
    TAny* ci = NULL;
    CCustomInterfaceUtility* customInterfaceUtil = NULL;

    TRAPD( error, customInterfaceUtil = CCustomInterfaceUtility::NewL( iRadioSession ) );

    if ( !error )
        {
        ci = customInterfaceUtil->CustomInterface( aInterfaceId );
        if ( !ci )
            {
            // The custom interface utility is owned by the custom interface, implicitly.
            // The custom interface utility instance is destroyed when the
            // custom interface is finally destroyed!
            // But if there's no valid ci, we need to take care of deleting the
            // custom interface utility instance.
            delete customInterfaceUtil;
            }
        }
    return ci;
    }

//End of File
