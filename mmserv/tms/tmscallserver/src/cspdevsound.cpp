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

// -----------------------------------------------------------------------------
// TMSCSPDevSound
// -----------------------------------------------------------------------------
//
TMSCSPDevSound::TMSCSPDevSound(TMSCSPDevSoundObserver& aObserver) :
    iObserver(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::ConstructL(TMMFState aMode, gint aAudioPreference,
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

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
TMSCSPDevSound::~TMSCSPDevSound()
    {
    delete iDevSound;
    }

// -----------------------------------------------------------------------------
// Tries to activate the audio stream if not active or activating
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::Activate()
    {
    if (!IsActive() && !IsActivationOngoing())
        {
        iActivationOngoing = ETrue;
        gint err = KErrNone;
        TRAP(err, DoActivateL());
        }
    }

// -----------------------------------------------------------------------------
// Deactivates the audio device.
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::Deactivate()
    {
    if (iDevSound && (IsActive() || IsActivationOngoing()))
        {
        iDevSound->Stop();
        iActive = EFalse;
        iActivationOngoing = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// ActivationOngoing
// -----------------------------------------------------------------------------
//
TBool TMSCSPDevSound::IsActivationOngoing() const
    {
    return iActivationOngoing;
    }

// -----------------------------------------------------------------------------
// IsActive
// -----------------------------------------------------------------------------
//
TBool TMSCSPDevSound::IsActive() const
    {
    return iActive;
    }

// -----------------------------------------------------------------------------
// DevSound
// -----------------------------------------------------------------------------
//
CMMFDevSound& TMSCSPDevSound::DevSound()
    {
    return *iDevSound;
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::InitializeComplete(TInt aError)
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

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::PlayError(TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::ToneFinished(TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::RecordError(TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::ConvertError(TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
void TMSCSPDevSound::DeviceMessage(TUid /*aMessageType*/,
        const TDesC8& /*aMsg*/)
    {
    }

//  End of File
