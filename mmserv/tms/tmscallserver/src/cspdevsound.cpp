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

#include <AudioPreference.h>
#include <glib.h>
#include <S60FourCC.h>
#include "cspdevsound.h"
#include "mcspdevsoundobserver.h"

using namespace TMS;

// ---------------------------------------------------------------------------
// CSPDevSound
// ---------------------------------------------------------------------------
//
CSPDevSound::CSPDevSound(MCSPDevSoundObserver& aObserver) :
    iObserver(aObserver)
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CSPDevSound::ConstructL(TMMFState aMode, gint aAudioPreference,
        gint aAudioPriority)
    {
    TMMFPrioritySettings audioPriority;
    TFourCC modemFourCC;
    modemFourCC.Set(KS60FourCCCodeModem);
    iDevSound = CMMFDevSound::NewL();
    if (iDevSound)
        {
#ifndef __WINSCW__
        iDevSound->InitializeL(*this, modemFourCC, aMode);
#else //For testing TMS in WINSCW
        iDevSound->InitializeL(*this, KMMFFourCCCodePCM16, aMode);
#endif
        iStreamType = aAudioPreference;
        audioPriority.iPriority = aAudioPriority;
        audioPriority.iPref = aAudioPreference;
        audioPriority.iState = aMode;
        iDevSound->SetPrioritySettings(audioPriority);
        }
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
CSPDevSound::~CSPDevSound()
    {
    delete iDevSound;
    }

// ---------------------------------------------------------------------------
// Tries to activate the audio stream if not active or activating
// ---------------------------------------------------------------------------
//
void CSPDevSound::Activate()
    {
    if (!IsActive() && !IsActivationOngoing())
        {
        iActivationOngoing = ETrue;
        gint err = KErrNone;
        TRAP(err, DoActivateL());
        }
    }

// ---------------------------------------------------------------------------
// Deactivates the audio device.
// ---------------------------------------------------------------------------
//
void CSPDevSound::Deactivate()
    {
    if (iDevSound && (IsActive() || IsActivationOngoing()))
        {
        iDevSound->Stop();
        iActive = EFalse;
        iActivationOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// ActivationOngoing
// ---------------------------------------------------------------------------
//
TBool CSPDevSound::IsActivationOngoing() const
    {
    return iActivationOngoing;
    }

// ---------------------------------------------------------------------------
// IsActive
// ---------------------------------------------------------------------------
//
TBool CSPDevSound::IsActive() const
    {
    return iActive;
    }

// ---------------------------------------------------------------------------
// DevSound
// ---------------------------------------------------------------------------
//
CMMFDevSound& CSPDevSound::DevSound()
    {
    return *iDevSound;
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::InitializeComplete(TInt aError)
    {
    if (iStreamType == KAudioPrefCSCallDownlink)
        {
        iObserver.DownlinkInitCompleted(aError);
        }
    else
        {
        iObserver.UplinkInitCompleted(aError);
        }
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
    {
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::PlayError(TInt /*aError*/)
    {
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::ToneFinished(TInt /*aError*/)
    {
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
    {
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::RecordError(TInt /*aError*/)
    {
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::ConvertError(TInt /*aError*/)
    {
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// ---------------------------------------------------------------------------
//
void CSPDevSound::DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/)
    {
    }

//  End of File
