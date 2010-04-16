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

//  INCLUDE FILES

#include <tms.h>
#include <AudioPreference.h>
#include <barsc.h>
#include <barsread.h>
#include <data_caging_path_literals.hrh>
#include <defaultbeep.rsg>
#include "tmsutility.h"
#include "tmsaudioinbandtoneplayer.h"
#include "tmsaudiotoneutilityimpl.h"

using namespace TMS;

// CONSTANTS
const TInt KPhoneInbandVolumeMax = 10;
const TInt KPhoneInbandVolumeMin = 0;
const TInt KPhoneInbandToneZero = 0;

// Resourcefile destination.
_LIT( KSystemDefaultbeepRscDrive, "Z:" );
_LIT( KSystemDefaultbeepRscFile, "DEFAULTBEEP.rSC" );

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TMSAudioInbandTonePlayer* TMSAudioInbandTonePlayer::NewL()
    {
    TMSAudioInbandTonePlayer* self =
            new (ELeave) TMSAudioInbandTonePlayer();
    CleanupStack::PushL(self);
    self->ConstructL( /*aAudioFactory*/);
    CleanupStack::Pop(self);
    return self;
    }

//Destructor
TMSAudioInbandTonePlayer::~TMSAudioInbandTonePlayer()
    {
    TRACE_PRN_FN_ENT;
    iFsSession.Close();
    delete iPlayBeepSequence;
    delete iPlayNoSoundSequence;
    delete iPlayDataSequence;
    delete iResourceCallWaitingSeq;
    delete iResourceRingGoingSeq;
    delete iResourceRadioPathSeq;
    delete iResourceSpecialSeq;
    delete iResourceCongestionSeq;
    delete iResourceReorderSeq;
    delete iResourceBusySeq;
    delete iCurrent;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::TMSAudioInbandTonePlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TMSAudioInbandTonePlayer::TMSAudioInbandTonePlayer()
    {
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::ConstructL()
    {
    // iCurrent = aFactory.CreateAudioToneUtilityL( *this );
    TRACE_PRN_FN_ENT;
    iCurrent = TMSAudioToneUtilityImpl::NewL(*this);

    //Get the inband tone sequences from resource and and store to member
    //variables
    RResourceFile resourceFile;

    TFileName fileName(KSystemDefaultbeepRscDrive);
    fileName.Append(KDC_RESOURCE_FILES_DIR);
    fileName.Append(KSystemDefaultbeepRscFile);

    iFsSession.Connect();
    resourceFile.OpenL(iFsSession, fileName);
    CleanupClosePushL(resourceFile);

    HBufC8* resourceBusy = resourceFile.AllocReadL(R_NET_BUSY);
    HBufC8* resourceReorder = resourceFile.AllocReadL(R_NET_REORDER);
    HBufC8* resourceCongestion = resourceFile.AllocReadL(R_NET_CONGESTION);
    HBufC8* resourceSpecial = resourceFile.AllocReadL(
            R_NET_SPECIAL_INFORMATION);
    HBufC8* resourceRadioPath = resourceFile.AllocReadL(
            R_NET_RADIO_NOT_AVAILABLE);
    HBufC8* resourceRingGoing = resourceFile.AllocReadL(R_NET_RING_GOING);
    HBufC8* resourceCallWaiting = resourceFile.AllocReadL(R_NET_CALL_WAITING);
    HBufC8* dataCallTone = resourceFile.AllocReadL(R_DATA_CALL_TONE);
    HBufC8* noSoundSequence = resourceFile.AllocReadL(R_NO_SOUND_SEQUENCE);
    HBufC8* beepSequence = resourceFile.AllocReadL(R_BEEP_SEQUENCE);

    CleanupStack::PopAndDestroy(&resourceFile);

    TResourceReader reader;
    TInt i(0);
    TInt length(0);

    //SubscriberBusy
    reader.SetBuffer(resourceBusy);
    length = reader.ReadInt16();
    iResourceBusySeq = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendBusy(iResourceBusySeq->Des());

    for (i = 0; i < length; i++)
        {
        appendBusy.Append(reader.ReadUint16());
        }

    //Reorder
    reader.SetBuffer(resourceReorder);
    length = reader.ReadInt16();
    iResourceReorderSeq = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendReorder(iResourceReorderSeq->Des());

    for (i = 0; i < length; i++)
        {
        appendReorder.Append(reader.ReadUint16());
        }

    //Congestion
    reader.SetBuffer(resourceCongestion);
    length = reader.ReadInt16();
    iResourceCongestionSeq = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendCongestion(iResourceCongestionSeq->Des());

    for (i = 0; i < length; i++)
        {
        appendCongestion.Append(reader.ReadUint16());
        }

    //Special Information
    reader.SetBuffer(resourceSpecial);
    length = reader.ReadInt16();
    iResourceSpecialSeq = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendSpecial(iResourceSpecialSeq->Des());

    for (i = 0; i < length; i++)
        {
        appendSpecial.Append(reader.ReadUint16());
        }

    //RadioPathNotAvailable
    reader.SetBuffer(resourceRadioPath);
    length = reader.ReadInt16();
    iResourceRadioPathSeq = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendRadio(iResourceRadioPathSeq->Des());

    for (i = 0; i < length; i++)
        {
        appendRadio.Append(reader.ReadUint16());
        }

    //RemoteAlertingTone
    reader.SetBuffer(resourceRingGoing);
    length = reader.ReadInt16();
    iResourceRingGoingSeq = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendRingGoing(iResourceRingGoingSeq->Des());

    for (i = 0; i < length; i++)
        {
        appendRingGoing.Append(reader.ReadUint16());
        }
    //CallWaitingTone
    reader.SetBuffer(resourceCallWaiting);
    length = reader.ReadInt16();
    iResourceCallWaitingSeq = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendCallWaiting(iResourceCallWaitingSeq->Des());

    for (i = 0; i < length; i++)
        {
        appendCallWaiting.Append(reader.ReadUint16());
        }

    //DataCalTone
    reader.SetBuffer(dataCallTone);
    length = reader.ReadInt16();
    iPlayDataSequence = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendDataCallTone(iPlayDataSequence->Des());

    for (i = 0; i < length; i++)
        {
        appendDataCallTone.Append(reader.ReadUint16());
        }

    //NoSoundSequence
    reader.SetBuffer(noSoundSequence);
    length = reader.ReadInt16();
    iPlayNoSoundSequence = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendNoSoundSequence(iPlayNoSoundSequence->Des());

    for (i = 0; i < length; i++)
        {
        appendNoSoundSequence.Append(reader.ReadUint16());
        }

    //BeepSequence
    reader.SetBuffer(beepSequence);
    length = reader.ReadInt16();
    iPlayBeepSequence = HBufC8::NewL(length);
    //Tptr modifies member variables (HBufC8) length so it includes the memory
    //edited by Append()
    TPtr8 appendBeepSequence(iPlayBeepSequence->Des());

    for (i = 0; i < length; i++)
        {
        appendBeepSequence.Append(reader.ReadUint16());
        }

    delete dataCallTone;
    delete noSoundSequence;
    delete beepSequence;
    delete resourceBusy;
    delete resourceReorder;
    delete resourceCongestion;
    delete resourceSpecial;
    delete resourceRadioPath;
    delete resourceRingGoing;
    delete resourceCallWaiting;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::PlayInbandTone
// Searches the given Inband tone from iToneArray and calls PlayCurrentTone.
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::PlayInbandTone(
/*TCCPTone*/TMSInbandToneType aTone)
    {
    TRACE_PRN_FN_ENT;
    //Stop playing if there is something playing
    Cancel();

    UpdateTonePlayerVolume();

    iToneName = aTone;

    TPtr8 resourceBusySeq = iResourceBusySeq -> Des();
    TPtr8 resourceRadioPathSeq = iResourceRadioPathSeq -> Des();
    TPtr8 resourceCongestionSeq = iResourceCongestionSeq -> Des();
    TPtr8 resourceSpecialSeq = iResourceSpecialSeq -> Des();
    TPtr8 resourceReorderSeq = iResourceReorderSeq->Des();
    TPtr8 resourceRingGoingSeq = iResourceRingGoingSeq -> Des();
    TPtr8 resourceCallWaitingSeq = iResourceCallWaitingSeq -> Des();

    switch (aTone)
        {
        case TMS_INBAND_USER_BUSY://ECCPToneUserBusy:
            iCurrent->PrepareToPlayDesSequence(resourceBusySeq);
            break;
        case TMS_INBAND_RADIO_PATH_NOT_AVAIL://ECCPToneRadioPathNotAvailable:
            iCurrent->PrepareToPlayDesSequence(resourceRadioPathSeq);
            break;
        case TMS_INBAND_CONGESTION://ECCPToneCongestion:
            iCurrent->PrepareToPlayDesSequence(resourceCongestionSeq);
            break;
        case TMS_INBAND_SPECIAL_INFO://ECCPToneSpecialInformation:
            iCurrent->PrepareToPlayDesSequence(resourceSpecialSeq);
            break;
        case TMS_INBAND_REORDER://ECCPReorder:
            iCurrent->PrepareToPlayDesSequence(resourceReorderSeq);
            break;
        case TMS_INBAND_REMOTE_ALEARTING://ECCPRemoteAlerting:
            iCurrent->PrepareToPlayDesSequence(resourceRingGoingSeq);
            break;
        case TMS_INBAND_CALL_WAITING://ECCPCallWaiting:
            iCurrent->PrepareToPlayDesSequence(resourceCallWaitingSeq);
            break;
        case TMS_INBAND_NO_SEQUENCE://ECCPNoSoundSequence:
            break;
        default:
            break;
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::Cancel
// Cancels the currently playing tone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::Cancel()
    {
    TRACE_PRN_FN_ENT;

    if (iCurrent)
        {
        if (iCurrent->State() == EMdaAudioToneUtilityNotReady)
            {
            // Prepare is called, but toneplayer's state is not yet prepare,
            // then cancel to prepare is needed.
            iCurrent->CancelPrepare();
            }
        else
            {
            iCurrent->CancelPlay();
            }
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::PlayCurrentTone
// Cancels the currently playing tone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::PlayCurrentTone()
    {
    TRACE_PRN_FN_ENT;
    if (iCurrent)
        {
        if (iCurrent->State() == EMdaAudioToneUtilityPrepared)
            {
            UpdateTonePlayerVolume();

            switch (iToneName)
                {
                case TMS_INBAND_USER_BUSY://ECCPToneUserBusy:
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityNetMsg,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefBusy));
                    break;
                case TMS_INBAND_RADIO_PATH_NOT_AVAIL://ECCPToneRadioPathNotAvailable:
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityNetMsg,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefRadioNotAvailable));
                    break;
                case TMS_INBAND_CONGESTION://ECCPToneCongestion:
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityNetMsg,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefCongestion));
                    break;
                case TMS_INBAND_SPECIAL_INFO://ECCPToneSpecialInformation:
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityNetMsg,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefSpecialInformation));
                    break;
                case TMS_INBAND_REORDER://ECCPReorder:
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityNetMsg,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefReorder));
                    break;
                case TMS_INBAND_REMOTE_ALEARTING://ECCPRemoteAlerting:
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityNetMsg,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefRingGoing));
                    break;
                case TMS_INBAND_CALL_WAITING://ECCPCallWaiting:
                    iCurrent->SetRepeats(0, TTimeIntervalMicroSeconds(
                            KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityNetMsg,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefCallWaiting));
                    break;
                case TMS_INBAND_DATA_CALL://ECCPDataCallTone: //EProfileRingingTypeRinging, EProfileRingingTypeAscending, EProfileRingingTypeRingingOnce
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(KAudioPriorityPhoneCall,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefIncomingDataCall));

                    //      if ( iRingingType == EProfileRingingTypeAscending )
                    //           {
                    //           iCurrent->SetVolumeRamp(
                    //               TTimeIntervalMicroSeconds( KPERingingToneRampDuration ) );
                    //           }
                    //       else
                    //          {
                    //EProfileRingingTypeRinging, EProfileRingingTypeRingingOnce
                    iCurrent->SetVolumeRamp(TTimeIntervalMicroSeconds(
                            KPhoneInbandToneZero));
                    //           }
                    //      if ( iRingingType == EProfileRingingTypeRingingOnce )
                    //          {
                    iCurrent->SetRepeats(0, TTimeIntervalMicroSeconds(
                            KPhoneInbandToneZero));
                    //          }
                    break;
                case TMS_INBAND_NO_SEQUENCE://ECCPNoSoundSequence:
                case TMS_INBAND_BEEP_SEQUENCE://ECCPBeepSequence:
                    iCurrent->SetRepeats(KMdaAudioToneRepeatForever,
                            TTimeIntervalMicroSeconds(KPhoneInbandToneZero));
                    iCurrent->SetPriority(
                            KAudioPriorityPhoneCall,
                            static_cast<TMdaPriorityPreference> (
                                    KAudioPrefIncomingCall));
                    iCurrent->SetRepeats(0, TTimeIntervalMicroSeconds(
                            KPhoneInbandToneZero));
                    iCurrent->SetVolumeRamp(TTimeIntervalMicroSeconds(
                            KPhoneInbandToneZero));
                    break;
                default:
                    break;
                }

#ifdef __WINS__

#else
            //Play the tone
            iCurrent->Play();
#endif

            } // State()
        } //iCurrent
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::MatoPrepareComplete
// Updates flag values and plays current inbandtone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::MatoPrepareComplete(TInt aError)
    {
    TRACE_PRN_FN_ENT;
    if (aError == KErrNone)
        {
        PlayCurrentTone();
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::SetVolume
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::SetVolume(TInt aVolume)
    {
    TRACE_PRN_FN_ENT;
    TInt safeVolume = Max(Min(aVolume, KPhoneInbandVolumeMax),
            KPhoneInbandVolumeMin);

    if (iCurrent && EMdaAudioToneUtilityNotReady != iCurrent->State())
        {
        TInt mediaServerVolume = CalculateMediaServerVolume(safeVolume);
        iCurrent->SetVolume(mediaServerVolume);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::MatoPlayComplete
// -
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::MatoPlayComplete(TInt aError)
    {
    TRACE_PRN_FN_ENT;
    if (aError != KErrNone)
        {
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::CalculateMediaServerVolume
// -----------------------------------------------------------------------------
//
TInt TMSAudioInbandTonePlayer::CalculateMediaServerVolume(TInt aVolume) const
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    // Our volume level scale is 0...10, media server's can be anything.
    // This scales the volume to correct level.
    return (iCurrent->MaxVolume() * aVolume) /
            (KPhoneInbandVolumeMax - KPhoneInbandVolumeMin);
    }

// -----------------------------------------------------------------------------
// TMSAudioInbandTonePlayer::UpdateTonePlayerVolume
// -----------------------------------------------------------------------------
//
void TMSAudioInbandTonePlayer::UpdateTonePlayerVolume()
    {
    TRACE_PRN_FN_ENT;
    TInt volume( /*KPhoneInbandVolumeMin*/KPhoneInbandVolumeMax);
    // iOwner.GetAudioVolumeSync( volume );
    SetVolume(volume);
    TRACE_PRN_FN_EXT;
    }

// End of File
