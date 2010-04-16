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

#ifndef CPEAUDIOINBANDTONEPLAYER_H
#define CPEAUDIOINBANDTONEPLAYER_H

// INCLUDES
#include <mdaaudiotoneplayer.h>
#include <ccpdefs.h>
#include <tms.h>

namespace TMS {

// FORWARD DECLARATIONS
class TMSAudioToneUtility;

// CLASS DECLARATION

/**
 *  This class is used to play inband tones using media server.
 *
 */
class TMSAudioInbandTonePlayer : public MMdaAudioToneObserver
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static TMSAudioInbandTonePlayer* NewL();

    /**
     * Destructor.
     */
    ~TMSAudioInbandTonePlayer();

    /**
     * Plays given tone. Originates from CCP plugin.
     * @param aTone inband tone needed to play
     */
    void PlayInbandTone(TMSInbandToneType aTone);

    /**
     * Cancels playing.
     */
    void Cancel();

    /**
     * Sets volume.
     * @param    aVolume     Volume in 0...10 scale.
     */
    void SetVolume(TInt aVolume);

private:

    /**
     * C++ default constructor.
     */
    TMSAudioInbandTonePlayer();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Playes current tone.
     * @param None.
     * @return None.
     */
    void PlayCurrentTone();

    //From MMdaAudioToneObserver

    /**
     * Handles completion of preparation.
     * @param aError Isn't used.
     * @return None.
     */
    void MatoPrepareComplete(TInt aError);

    //From MMdaAudioToneObserver
    /**
     * Handles completion of playing.
     * @param aError Isn't used.
     * @return None.
     */
    void MatoPlayComplete(TInt aError);

    /**
     * Calculates media server volume level.
     * @param   aVolume     Volume level in 0...10 scale.
     * @return  Volume level in media server scale.
     */
    TInt CalculateMediaServerVolume(TInt aVolume) const;

    /**
     * Updates tone player's volume to the current audio volume level.
     */
    void UpdateTonePlayerVolume();

private:
    // Data

    // Currently playing, NULL or tone sequence.
    TMSAudioToneUtility* iCurrent;
    //Member variable for inband tone
    TMSInbandToneType iToneName;
    //NetworkBusy sequence
    HBufC8* iResourceBusySeq;
    //NetworkBusy sequence
    HBufC8* iResourceReorderSeq;
    //NetworkBusy sequence
    HBufC8* iResourceCongestionSeq;
    //NetworkBusy sequence
    HBufC8* iResourceSpecialSeq;
    //NetworkBusy sequence
    HBufC8* iResourceRadioPathSeq;
    //NetworkBusy sequence
    HBufC8* iResourceRingGoingSeq;
    //CallWaiting sequence
    HBufC8* iResourceCallWaitingSeq;
    //Ringing type
    //       TProfileRingingType iRingingType;
    //Data call tone sequence
    HBufC8* iPlayDataSequence;
    //Data call silent sequence
    HBufC8* iPlayNoSoundSequence;
    //Data call beep sequence
    HBufC8* iPlayBeepSequence;
    //Handle to a file server session
    RFs iFsSession;

    };
} // namespace TMS

#endif      // CPEAUDIOINBANDTONEPLAYER_H

// End of File
