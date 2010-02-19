/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Telephony Multimedia Service
 *
 */

// INCLUDE FILES
#include "globaleffectssettings.h"
#include "tmsclientserver.h"

using namespace TMS;
// CONSTANTS
const TInt KDefaultVolume = 4;
const TInt KDefaultMaxVolume = 10;
const TInt KDefaultGain = 1;
const TInt KDefaultMaxGain = 64;

TUid KCRUidTmseffects = {0x10207C80};
const TUint32 KTmsEarPieceVolume = 0x00000001;
const TUint32 KTmsLoudSpkrVolume = 0x00000002;

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::TMSGlobalEffectsSettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TMSGlobalEffectsSettings::TMSGlobalEffectsSettings() :
    iGain(KDefaultGain),
    iMaxVolume(KDefaultMaxVolume),
    iMaxGain(KDefaultMaxGain),
    iEarVolume(KDefaultVolume),
    iLoudSpkrVolume(KDefaultVolume)
    {
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void TMSGlobalEffectsSettings::ConstructL()
    {
    // Create repository instance
    iRepository = CRepository::NewL(KCRUidTmseffects);
    if (iRepository)
        {
        iRepository->Get(KTmsLoudSpkrVolume, iLoudSpkrVolume);
        iRepository->Get(KTmsEarPieceVolume, iEarVolume);
        }
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TMSGlobalEffectsSettings* TMSGlobalEffectsSettings::NewL()
    {
    TMSGlobalEffectsSettings* self = new (ELeave) TMSGlobalEffectsSettings();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
TMSGlobalEffectsSettings::~TMSGlobalEffectsSettings()
    {
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::SetLoudSpkrVolume
// -----------------------------------------------------------------------------
//
void TMSGlobalEffectsSettings::SetLoudSpkrVolume(TInt aVolume)
    {
    TInt status(KErrNone);
    if (iRepository)
        {
        status = iRepository->Set(KTmsLoudSpkrVolume, aVolume);
        if (status == KErrNone)
            {
            iLoudSpkrVolume = aVolume;
            }
        }
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::SetEarPieceVolume
// -----------------------------------------------------------------------------
//
void TMSGlobalEffectsSettings::SetEarPieceVolume(TInt aVolume)
    {
    TInt status(KErrNone);
    if (iRepository)
        {
        status = iRepository->Set(KTmsEarPieceVolume, aVolume);
        if (status == KErrNone)
            {
            iEarVolume = aVolume;
            }
        }
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::GetLoudSpkrVolume
// -----------------------------------------------------------------------------
//
void TMSGlobalEffectsSettings::GetLoudSpkrVolume(TInt& aVolume)
    {
    aVolume = iLoudSpkrVolume;
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::GetEarPieceVolume
// -----------------------------------------------------------------------------
//
void TMSGlobalEffectsSettings::GetEarPieceVolume(TInt& aVolume)
    {
    aVolume = iEarVolume;
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::MaxVolume
// -----------------------------------------------------------------------------
//
TInt TMSGlobalEffectsSettings::MaxVolume() const
    {
    return iMaxVolume;
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::SetGain
// -----------------------------------------------------------------------------
//
void TMSGlobalEffectsSettings::SetGain(TInt aGain)
    {
    iGain = aGain;
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::Gain
// -----------------------------------------------------------------------------
//
TInt TMSGlobalEffectsSettings::Gain() const
    {
    return iGain;
    }

// -----------------------------------------------------------------------------
// TMSGlobalEffectsSettings::MaxGain
// -----------------------------------------------------------------------------
//
TInt TMSGlobalEffectsSettings::MaxGain() const
    {
    return iMaxGain;
    }

//  End of File
