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

#include "tmsaudiotoneutilityimpl.h"

using namespace TMS;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TMSAudioToneUtilityImpl::TMSAudioToneUtilityImpl()
    {
    iToneUtility = NULL;
    }

// ---------------------------------------------------------------------------
// 2nd phase construcotor
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::ConstructL(MMdaAudioToneObserver& aToneObserver)
    {
    iToneUtility = CMdaAudioToneUtility::NewL(aToneObserver);
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
TMSAudioToneUtilityImpl* TMSAudioToneUtilityImpl::NewL(
        MMdaAudioToneObserver& aToneObserver)
    {
    TMSAudioToneUtilityImpl* self = new (ELeave) TMSAudioToneUtilityImpl;
    CleanupStack::PushL(self);
    self->ConstructL(aToneObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
TMSAudioToneUtilityImpl::~TMSAudioToneUtilityImpl()
    {
    delete iToneUtility;
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::PrepareToPlayDesSequence(const TDesC8& aSequence)
    {
    iToneUtility->PrepareToPlayDesSequence(aSequence);
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
TMdaAudioToneUtilityState TMSAudioToneUtilityImpl::State()
    {
    return iToneUtility->State();
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::CancelPlay()
    {
    // Do state specific cancellations, this way we do not end up in a
    // endless play state if CancelPlay() is called during preparation.
    if (EMdaAudioToneUtilityPlaying == iToneUtility->State())
        {
        iToneUtility->CancelPlay();
        }
    else if (EMdaAudioToneUtilityPrepared == iToneUtility->State())
        {
        iToneUtility->CancelPrepare();
        }
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
TInt TMSAudioToneUtilityImpl::MaxVolume()
    {
    return iToneUtility->MaxVolume();
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::SetVolume(TInt aVolume)
    {
    iToneUtility->SetVolume(aVolume);
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::SetRepeats(TInt aRepeatNumberOfTimes,
        const TTimeIntervalMicroSeconds& aTrailingSilence)
    {
    iToneUtility->SetRepeats(aRepeatNumberOfTimes, aTrailingSilence);
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::SetPriority(TInt aPriority,
        TMdaPriorityPreference aPref)
    {
    iToneUtility->SetPriority(aPriority, aPref);
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::SetVolumeRamp(
        const TTimeIntervalMicroSeconds& aRampDuration)
    {
    iToneUtility->SetVolumeRamp(aRampDuration);
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::Play()
    {
    iToneUtility->Play();
    }

// ---------------------------------------------------------------------------
// From class TMSAudioToneUtilityImpl
// ---------------------------------------------------------------------------
//
void TMSAudioToneUtilityImpl::CancelPrepare()
    {
    iToneUtility->CancelPrepare();
    }

