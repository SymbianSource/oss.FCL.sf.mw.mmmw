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
#include "csdownlink.h"
#include "mcspdevsoundobserver.h"

using namespace TMS;

const gint KDefaultVolume = 4;
const gint KDefaultMaxVolume = 10;

// -----------------------------------------------------------------------------
// Static constructor
// -----------------------------------------------------------------------------
//
TMSCSDownlink* TMSCSDownlink::NewL(TMSCSPDevSoundObserver& aObserver)
    {
    TMSCSDownlink* self = new (ELeave) TMSCSDownlink(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
TMSCSDownlink::~TMSCSDownlink()
    {
    }

// -----------------------------------------------------------------------------
// Sets volume
// -----------------------------------------------------------------------------
//
void TMSCSDownlink::SetVolume(gint aVolume)
    {
    if (iDevSound)
        {
        gint maxVolume(iDevSound->MaxVolume());
        maxVolume = (maxVolume > 0) ? maxVolume : KDefaultMaxVolume;
        gint scaledVolume = (aVolume * maxVolume) / KDefaultMaxVolume;
        iDevSound->SetVolume(scaledVolume);
        }
    }

// -----------------------------------------------------------------------------
// Gives volume
// -----------------------------------------------------------------------------
//
gint TMSCSDownlink::Volume()
    {
    gint vol = 0;
    if (iDevSound)
        {
        vol = iDevSound->Volume();
        }
    return vol;
    }

// -----------------------------------------------------------------------------
// Gives max volume
// -----------------------------------------------------------------------------
//
gint TMSCSDownlink::MaxVolume()
    {
    gint vol = 0;
    if (iDevSound)
        {
        vol = iDevSound->MaxVolume();
        }
    return vol;
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Downlink stream has been activated successfully.
// -----------------------------------------------------------------------------
//
void TMSCSDownlink::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
    {
    //CSPLOGSTRING( CSPINT, "TMSCSDownlink:: activated" );
    // We dont react to devsound messages unless we are activating.
    if (IsActivationOngoing())
        {
        iActive = ETrue;
        iActivationOngoing = EFalse;
        iObserver.DownlinkActivatedSuccessfully();
        }
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Downlink stream activation failed
// -----------------------------------------------------------------------------
//
void TMSCSDownlink::PlayError(TInt aError)
    {
    //CSPLOGSTRING( CSPINT, "TMSCSDownlink::PlayError" );

    // We dont react to devsound messages unless we are activating.
    if (IsActivationOngoing())
        {
        //CSPLOGSTRING( CSPINT, "TMSCSDownlink::PlayError activation failed" );
        if (aError == KErrAccessDenied)
            {
            iActivationOngoing = EFalse;
            iObserver.DownlinkActivationFailed();
            }
        }
    }

// -----------------------------------------------------------------------------
// From class CSPDevsound
// Tries to activate Downlink stream.
// -----------------------------------------------------------------------------
//
void TMSCSDownlink::DoActivateL()
    {
    if (iDevSound)
        {
        iDevSound->PlayInitL();
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
TMSCSDownlink::TMSCSDownlink(TMSCSPDevSoundObserver& aObserver) :
    TMSCSPDevSound(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// Second phase constructor
// -----------------------------------------------------------------------------
//
void TMSCSDownlink::ConstructL()
    {
    TMSCSPDevSound::ConstructL(EMMFStatePlaying, KAudioPrefCSCallDownlink,
            KAudioPriorityCSCallDownlink);

    if (iDevSound)
        {
        iDevSound->SetVolume(KDefaultVolume);
        }
    }

//  End of File
