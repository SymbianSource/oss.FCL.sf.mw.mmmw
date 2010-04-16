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
#include <mdaaudiotoneplayer.h>
#include <AudioPreference.h>
#include "tmsutility.h"
#include "cpeaudiodtmftoneplayer.h"

// CONSTANTS
const gint KBufSize = 10;
const gint KMaxVolumeLevel = 10;

_LIT(KPhoneTone_Asterisk, "*+w");

//from voip
const guint KDTMFToneLengthOn = 250000;
const guint KDTMFToneLengthOff = 50000;
const guint KDTMFToneLengthPause = 2500000;

using namespace TMS;

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TMSAudioDtmfTonePlayer* TMSAudioDtmfTonePlayer::NewL(
        TMSDTMFTonePlayerObserver& obsrvr, guint aAudioPreference,
        guint aAudioPriority)
    {
    //iObserver = obsrvr;
    TMSAudioDtmfTonePlayer* self = new (ELeave) TMSAudioDtmfTonePlayer(
            obsrvr, aAudioPreference, aAudioPriority);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

//Destructor
TMSAudioDtmfTonePlayer::~TMSAudioDtmfTonePlayer()
    {
    TRACE_PRN_FN_ENT;
    Cancel();
    delete iDTMFPlayer;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::TMSAudioDtmfTonePlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TMSAudioDtmfTonePlayer::TMSAudioDtmfTonePlayer(
        TMSDTMFTonePlayerObserver& obsrvr, guint aAudioPreference,
        guint aAudioPriority) :
    iObserver(obsrvr),
    iPref(aAudioPreference),
    iPrior(aAudioPriority)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::ConstructL()
    {
    TRACE_PRN_FN_ENT;

    TMMFPrioritySettings dtmfClickPlayerPrioritySettings;
    iDTMFPlayer = CMMFDevSound::NewL();
    iDTMFPlayer->InitializeL(*this, EMMFStateTonePlaying);

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::SetVolume
// Sets the volume level that is used when the dtmf tone is played.
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::SetVolume(gint aVolume)
    {
    TRACE_PRN_FN_ENT;
    gint vol = ConvertVolume(aVolume);

    if (vol != iVolume)
        {
        iVolume = vol;
        iDTMFPlayer->SetVolume(iVolume);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::PlayDtmfTone
// Searches the given DTMF tone from iToneArray and calls PlayCurrentTone
// member function.
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::PlayDtmfTone(TDes& aTone)
    {
    TRACE_PRN_FN_ENT;
    Cancel();

    // DTMF signalling.
    if (iDTMFPlayer)
        {
        TBuf<KBufSize> key;
        key.Append(aTone);
        TRAP_IGNORE(iDTMFPlayer->PlayDTMFStringL(aTone));
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::Cancel
// Cancels the currently playing tone.
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::Cancel()
    {
    TRACE_PRN_FN_ENT;
    iDTMFPlayer->Stop();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::Normalize
// Lowers case for chars and if its '*','+', or 'w' sets it to asterisk
// char ('*').
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::Normalize(TChar& aTone)
    {
    TRACE_PRN_FN_ENT;
    aTone.LowerCase();

    TPtrC ast(KPhoneTone_Asterisk);

    if (ast.Locate(aTone) != KErrNotFound)
        {
        aTone = ast[0];
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::InitializeComplete
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::InitializeComplete(TInt aError)
    {
    TRACE_PRN_FN_ENT;

    if (aError == KErrNone)
        {
        TMMFPrioritySettings dtmfPlayerSettings;
        dtmfPlayerSettings.iPref = (TMdaPriorityPreference) iPref;
        dtmfPlayerSettings.iPriority = iPrior;
        dtmfPlayerSettings.iState = EMMFStateTonePlaying;
        iDTMFPlayer->SetPrioritySettings(dtmfPlayerSettings);

        TTimeIntervalMicroSeconds32 length(KDTMFToneLengthOn);
        TTimeIntervalMicroSeconds32 pause(KDTMFToneLengthOff);
        TTimeIntervalMicroSeconds32 zero(KDTMFToneLengthPause);
        iDTMFPlayer->SetDTMFLengths(length, pause, zero);
        iDTMFPlayer->SetVolume(iDTMFPlayer->MaxVolume());
        }

    iObserver.DTMFInitCompleted(aError);

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::ToneFinished
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::ToneFinished(TInt aError)
    {
    TRACE_PRN_FN_ENT;
    iObserver.DTMFToneFinished(aError);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::BufferToBeFilled
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::BufferToBeFilled(CMMFBuffer* /*aBuffer*/)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::PlayError
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::PlayError(TInt /*aError*/)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::BufferToBeEmptied
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::BufferToBeEmptied(CMMFBuffer* /*aBuffer*/)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::RecordError
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::RecordError(TInt /*aError*/)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::ConvertError
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::ConvertError(TInt /*aError*/)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::DeviceMessage
// -----------------------------------------------------------------------------
//
void TMSAudioDtmfTonePlayer::DeviceMessage(TUid /*aMessageType*/,
        const TDesC8& /*aMsg*/)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioDtmfTonePlayer::ConvertVolume
// -----------------------------------------------------------------------------
//
gint TMSAudioDtmfTonePlayer::ConvertVolume(gint aVolume)
    {
    TRACE_PRN_FN_ENT;
    gint result = iDTMFPlayer->MaxVolume() * aVolume / KMaxVolumeLevel;
    TRACE_PRN_FN_EXT;
    return result;
    }

//  End of File
