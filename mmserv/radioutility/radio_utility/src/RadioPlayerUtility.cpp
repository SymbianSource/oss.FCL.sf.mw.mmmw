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
* Description:  Radio Player Utility interface implementation
*
*/


#include <RadioPlayerUtility.h>
#include "RadioPlayerUtilityBody.h"
#include "RadioSession.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRadioPlayerUtility* CRadioPlayerUtility::NewL(
     RRadioSession& aRadioSession,
     MRadioPlayerObserver& aObserver)
    {
    CRadioPlayerUtility* self = new(ELeave) CRadioPlayerUtility;
    CleanupStack::PushL(self);
    self->iBody = CRadioPlayerUtility::CBody::NewL(aRadioSession, aObserver);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CRadioPlayerUtility
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioPlayerUtility::~CRadioPlayerUtility()
    {
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CRadioPlayerUtility
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRadioPlayerUtility::CRadioPlayerUtility()
    {
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::PlayerState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TPlayerState CRadioPlayerUtility::PlayerState() const
    {
    TPlayerState playerState;
    ASSERT(iBody);
    iBody->PlayerState(playerState);

    return playerState;
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::Close
// Abstraction of Close
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioPlayerUtility::Close()
    {
    ASSERT(iBody);
    iBody->Close();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::Play
// Abstraction of Play
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioPlayerUtility::Play()
    {
    ASSERT(iBody);
    iBody->Play();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::Stop
// Abstraction of Stop
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioPlayerUtility::Stop()
    {
    ASSERT(iBody);
    iBody->Stop();
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::Mute
// Abstraction of Mute
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPlayerUtility::Mute(TBool aMute)
    {
    ASSERT(iBody);
    return iBody->Mute(aMute);
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::IsMute
// Abstraction of IsMute
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CRadioPlayerUtility::IsMute()
    {
    ASSERT(iBody);
    return iBody->IsMute();
    }


// -----------------------------------------------------------------------------
// CRadioPlayerUtility::SetVolume
// Abstraction of SetVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPlayerUtility::SetVolume(TInt aVolume)
    {
    ASSERT(iBody);
    return iBody->SetVolume(aVolume);
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::GetVolume
// Abstraction of GetVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPlayerUtility::GetVolume(TInt& aVolume) const
    {
    ASSERT(iBody);
    return iBody->GetVolume(aVolume);
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::SetVolumeRamp
// Abstraction of SetVolumeRamp
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPlayerUtility::SetVolumeRamp(
    const TTimeIntervalMicroSeconds& aRampInterval)
    {
    ASSERT(iBody);
    return iBody->SetVolumeRamp(aRampInterval);
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::GetMaxVolume
// Abstraction of Utility MaxVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPlayerUtility::GetMaxVolume(
    TInt& aMaxVolume ) const
    {
    ASSERT(iBody);
    return iBody->GetMaxVolume(aMaxVolume);
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::SetBalance
// Abstraction of SetBalance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPlayerUtility::SetBalance(
    TInt aLeftPercentage,
    TInt aRightPercentage )
    {
    ASSERT(iBody);
    return iBody->SetBalance(aLeftPercentage, aRightPercentage);
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::GetBalance
// Abstraction of GetBalance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPlayerUtility::GetBalance(
    TInt& aLeftPercentage,
    TInt& aRightPercentage ) const
    {
    ASSERT(iBody);
    return iBody->GetBalance(aLeftPercentage, aRightPercentage);
    }

// -----------------------------------------------------------------------------
// CRadioPlayerUtility::CustomInterface
// Abstraction of CustomInterface
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* CRadioPlayerUtility::CustomInterface(
    TUid aInterfaceId )
    {
    ASSERT(iBody);
    return iBody->CustomInterface(aInterfaceId);
    }



// End of File
