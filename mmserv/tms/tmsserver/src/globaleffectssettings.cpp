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
// GlobalEffectsSettings::GlobalEffectsSettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
GlobalEffectsSettings::GlobalEffectsSettings() :
    iGain(KDefaultGain),
    iMaxVolume(KDefaultMaxVolume),
    iMaxGain(KDefaultMaxGain),
    iEarVolume(KDefaultVolume),
    iLoudSpkrVolume(KDefaultVolume)
    {
    }

// -----------------------------------------------------------------------------
// GlobalEffectsSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void GlobalEffectsSettings::ConstructL()
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
// GlobalEffectsSettings::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
GlobalEffectsSettings* GlobalEffectsSettings::NewL()
    {
    GlobalEffectsSettings* self = new (ELeave) GlobalEffectsSettings();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
GlobalEffectsSettings::~GlobalEffectsSettings()
    {
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// GlobalEffectsSettings::SetLoudSpkrVolume
// -----------------------------------------------------------------------------
//
void GlobalEffectsSettings::SetLoudSpkrVolume(TInt aVolume)
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
// GlobalEffectsSettings::SetEarPieceVolume
// -----------------------------------------------------------------------------
//
void GlobalEffectsSettings::SetEarPieceVolume(TInt aVolume)
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
// GlobalEffectsSettings::GetLoudSpkrVolume
// -----------------------------------------------------------------------------
//
void GlobalEffectsSettings::GetLoudSpkrVolume(TInt& aVolume)
    {
    aVolume = iLoudSpkrVolume;
    }

// -----------------------------------------------------------------------------
// GlobalEffectsSettings::GetEarPieceVolume
// -----------------------------------------------------------------------------
//
void GlobalEffectsSettings::GetEarPieceVolume(TInt& aVolume)
    {
    aVolume = iEarVolume;
    }

// -----------------------------------------------------------------------------
// GlobalEffectsSettings::MaxVolume
// -----------------------------------------------------------------------------
//
TInt GlobalEffectsSettings::MaxVolume() const
    {
    return iMaxVolume;
    }

// -----------------------------------------------------------------------------
// GlobalEffectsSettings::SetGain
// -----------------------------------------------------------------------------
//
void GlobalEffectsSettings::SetGain(TInt aGain)
    {
    iGain = aGain;
    }

// -----------------------------------------------------------------------------
// GlobalEffectsSettings::Gain
// -----------------------------------------------------------------------------
//
TInt GlobalEffectsSettings::Gain() const
    {
    return iGain;
    }

// -----------------------------------------------------------------------------
// GlobalEffectsSettings::MaxGain
// -----------------------------------------------------------------------------
//
TInt GlobalEffectsSettings::MaxGain() const
    {
    return iMaxGain;
    }

//  End of File
