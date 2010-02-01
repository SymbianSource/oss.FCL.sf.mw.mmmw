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
#include "csuplink.h"
#include "mcspdevsoundobserver.h"

using namespace TMS;

// Mute value
const gint KSetMuteToDevSound = 0;

// ---------------------------------------------------------------------------
// Static constructor.
// ---------------------------------------------------------------------------
//
CSUplink* CSUplink::NewL(MCSPDevSoundObserver& aObserver)
    {
    CSUplink* self = new (ELeave) CSUplink(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSUplink::~CSUplink()
    {
    }

// ---------------------------------------------------------------------------
// Gives mic mute state
// ---------------------------------------------------------------------------
//
TBool CSUplink::IsMuted()
    {
    TBool isMuted = EFalse;
    gint gain = 0;
    if (iDevSound)
        {
        gain = iDevSound->Gain();
        }
    if (!gain)
        {
        // Mute is on
        isMuted = ETrue;
        }
    //   CSPLOGSTRING( CSPINT, "CSUplink::IsMuted" );
    return isMuted;
    }

// ---------------------------------------------------------------------------
// Set mic muted.
// ---------------------------------------------------------------------------
//
void CSUplink::SetMuted()
    {
    if (iDevSound)
        {
        iDevSound->SetGain(KSetMuteToDevSound);
        }
    }

// ---------------------------------------------------------------------------
// Set mic unmuted
// ---------------------------------------------------------------------------
//
void CSUplink::SetUnmuted()
    {
    if (iDevSound)
        {
        iDevSound->SetGain(iDevSound->MaxGain());
        }
    }

// ---------------------------------------------------------------------------
// Sets gain
// ---------------------------------------------------------------------------
//
void CSUplink::SetGain(gint aGain)
    {
    if (iDevSound)
        {
        iDevSound->SetGain(aGain);
        }
    }

// ---------------------------------------------------------------------------
// Gives volume
// ---------------------------------------------------------------------------
//
gint CSUplink::Gain()
    {
    gint gain = 0;
    if (iDevSound)
        {
        gain = iDevSound->Gain();
        }
    return gain;
    }

// ---------------------------------------------------------------------------
// Gives max gain
// ---------------------------------------------------------------------------
//
gint CSUplink::MaxGain()
    {
    gint gain = 0;
    if (iDevSound)
        {
        gain = iDevSound->MaxGain();
        }
    return gain;
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Activation was successfull.
// ---------------------------------------------------------------------------
//
void CSUplink::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
    {
    //  CSPLOGSTRING( CSPINT, "CSUplink::BufferToBeEmptied" );

    // We dont react to devsound messages unless we are activating.
    if (IsActivationOngoing())
        {
        iActive = ETrue;
        iActivationOngoing = EFalse;
        iObserver.UplinkActivatedSuccessfully();
        }
    }

// ---------------------------------------------------------------------------
// From class MDevSoundObserver
// Activation feiled
// ---------------------------------------------------------------------------
//
void CSUplink::RecordError(TInt aError)
    {
    //  CSPLOGSTRING( CSPINT, "CSUplink::RecordError" );

    // We dont react to devsound messages unless we are activating.
    if (IsActivationOngoing())
        {
        if (aError == KErrAccessDenied)
            {
            iActivationOngoing = EFalse;
            iObserver.UplinkActivationFailed();
            }
        }
    }

// ---------------------------------------------------------------------------
// From class CSPDevSound
// Tries to activate mic stream. Stream becomes active when BufferToBeFilled
// gets called.
// ---------------------------------------------------------------------------
//
void CSUplink::DoActivateL()
    {
    if (iDevSound)
        {
        iDevSound->RecordInitL();
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CSUplink::CSUplink(MCSPDevSoundObserver& aObserver) :
    CSPDevSound(aObserver)
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CSUplink::ConstructL()
    {
    CSPDevSound::ConstructL(EMMFStateRecording, KAudioPrefCSCallUplink,
            KAudioPriorityCSCallUplink);
    }

//  End of File
