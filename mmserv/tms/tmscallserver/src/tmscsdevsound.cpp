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
#include <S60FourCC.h>
#include "tmsutility.h"
#include "tmscsdevsound.h"
#include "tmscsdevsoundobserver.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// TMSCSPDevSound
// -----------------------------------------------------------------------------
//
TMSCSDevSound::TMSCSDevSound(TMSCSDevSoundObserver& observer) :
    iObserver(observer)
    {
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void TMSCSDevSound::ConstructL(const TMSStreamType strmtype)
    {
    TRACE_PRN_FN_ENT;
    iStreamType = strmtype;

    if (strmtype == TMS_STREAM_UPLINK)
        {
        iMode = EMMFStateRecording;
        iPriority = KAudioPriorityCSCallUplink;
        iPreference = KAudioPrefCSCallUplink;
        }
    else if (strmtype == TMS_STREAM_DOWNLINK)
        {
        iMode = EMMFStatePlaying;
        iPriority = KAudioPriorityCSCallDownlink;
        iPreference = KAudioPrefCSCallDownlink;
        }
    InitializeL();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// InitializeL
// -----------------------------------------------------------------------------
//
void TMSCSDevSound::InitializeL()
    {
    TRACE_PRN_FN_ENT;
    TMMFPrioritySettings audioPriority;
    TFourCC modemFourCC;
    modemFourCC.Set(KS60FourCCCodeModem);

    delete iDevSound;
    iDevSound = NULL;
    iDevSound = CMMFDevSound::NewL();
    if (iDevSound)
        {
#ifndef __WINSCW__
        iDevSound->InitializeL(*this, modemFourCC, iMode);
#else
        //For testing TMS in WINSCW
        iDevSound->InitializeL(*this, KMMFFourCCCodePCM16, iMode);
#endif
        audioPriority.iPriority = iPriority;
        audioPriority.iPref = iPreference;
        audioPriority.iState = iMode;
        iDevSound->SetPrioritySettings(audioPriority);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// Not implemented
// -----------------------------------------------------------------------------
//
TMSCSDevSound::~TMSCSDevSound()
    {
    delete iDevSound;
    }

// -----------------------------------------------------------------------------
// Tries to activate the audio stream if not already active or activating
// -----------------------------------------------------------------------------
//
void TMSCSDevSound::Activate()
    {
    if (!iActive && !iActivationOngoing)
        {
        iActivationOngoing = ETrue;
        TRAP_IGNORE(DoActivateL());
        }
    }

// -----------------------------------------------------------------------------
// Deactivates the audio device.
// -----------------------------------------------------------------------------
//
void TMSCSDevSound::Deactivate()
    {
    if (iDevSound && (iActive || iActivationOngoing))
        {
        iDevSound->Stop();
        iActive = EFalse;
        iActivationOngoing = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// DevSound
// -----------------------------------------------------------------------------
//
CMMFDevSound& TMSCSDevSound::DevSound()
    {
    return *iDevSound;
    }

// -----------------------------------------------------------------------------
// From class MDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCSDevSound::InitializeComplete(TInt aError)
    {
    TRACE_PRN_FN_ENT;
    if (aError == TMS_RESULT_SUCCESS)
        {
        NotifyEvent(aError);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCSDevSound::NotifyEvent
// -----------------------------------------------------------------------------
//
void TMSCSDevSound::NotifyEvent(gint error)
    {
    if (iStreamType == TMS_STREAM_DOWNLINK)
        {
        iObserver.DownlinkInitCompleted(error);
        }
    else if (iStreamType == TMS_STREAM_UPLINK)
        {
        iObserver.UplinkInitCompleted(error);
        }
    }

//  End of File
