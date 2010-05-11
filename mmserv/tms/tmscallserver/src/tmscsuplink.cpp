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
#include "tmscsuplink.h"
#include "tmscsdevsoundobserver.h"

using namespace TMS;

// Mute value
const gint KSetMuteToDevSound = 0;

// -----------------------------------------------------------------------------
// Static constructor.
// -----------------------------------------------------------------------------
//
TMSCSUplink* TMSCSUplink::NewL(TMSCSPDevSoundObserver& aObserver)
    {
    TMSCSUplink* self = new (ELeave) TMSCSUplink(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
TMSCSUplink::~TMSCSUplink()
    {
    }

// -----------------------------------------------------------------------------
// Gives mic mute state
// -----------------------------------------------------------------------------
//
TBool TMSCSUplink::IsMuted()
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
    //   CSPLOGSTRING( CSPINT, "TMSCSUplink::IsMuted" );
    return isMuted;
    }

// -----------------------------------------------------------------------------
// Set mic muted.
// -----------------------------------------------------------------------------
//
void TMSCSUplink::SetMuted()
    {
    if (iDevSound)
        {
        iDevSound->SetGain(KSetMuteToDevSound);
        }
    }

// -----------------------------------------------------------------------------
// Set mic unmuted
// -----------------------------------------------------------------------------
//
void TMSCSUplink::SetUnmuted()
    {
    if (iDevSound)
        {
        iDevSound->SetGain(iDevSound->MaxGain());
        }
    }

// -----------------------------------------------------------------------------
// Sets gain
// -----------------------------------------------------------------------------
//
void TMSCSUplink::SetGain(gint aGain)
    {
    if (iDevSound)
        {
        iDevSound->SetGain(aGain);
        }
    }

// -----------------------------------------------------------------------------
// Gives volume
// -----------------------------------------------------------------------------
//
gint TMSCSUplink::Gain()
    {
    gint gain = 0;
    if (iDevSound)
        {
        gain = iDevSound->Gain();
        }
    return gain;
    }

// -----------------------------------------------------------------------------
// Gives max gain
// -----------------------------------------------------------------------------
//
gint TMSCSUplink::MaxGain()
    {
    gint gain = 0;
    if (iDevSound)
        {
        gain = iDevSound->MaxGain();
        }
    return gain;
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Activation was successfull.
// -----------------------------------------------------------------------------
//
void TMSCSUplink::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
    {
    //  CSPLOGSTRING( CSPINT, "TMSCSUplink::BufferToBeEmptied" );

    // We dont react to devsound messages unless we are activating.
    if (IsActivationOngoing())
        {
        iActive = ETrue;
        iActivationOngoing = EFalse;
        iObserver.UplinkActivatedSuccessfully();
        }
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Activation feiled
// -----------------------------------------------------------------------------
//
void TMSCSUplink::RecordError(TInt aError)
    {
    //  CSPLOGSTRING( CSPINT, "TMSCSUplink::RecordError" );

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

// -----------------------------------------------------------------------------
// From class TMSCSPDevSound
// Tries to activate mic stream. Stream becomes active when BufferToBeFilled
// gets called.
// -----------------------------------------------------------------------------
//
void TMSCSUplink::DoActivateL()
    {
    if (iDevSound)
        {
        iDevSound->RecordInitL();
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
TMSCSUplink::TMSCSUplink(TMSCSPDevSoundObserver& aObserver) :
    TMSCSPDevSound(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// Second phase constructor
// -----------------------------------------------------------------------------
//
void TMSCSUplink::ConstructL()
    {
    TMSCSPDevSound::ConstructL(EMMFStateRecording, KAudioPrefCSCallUplink,
            KAudioPriorityCSCallUplink);
    }

//  End of File
