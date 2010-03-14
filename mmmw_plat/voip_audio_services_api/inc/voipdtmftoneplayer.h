/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Defines VoIP DTMF tone player APIs.
 *
 */

#ifndef __VOIPDTMFTONEPLAYER_H
#define __VOIPDTMFTONEPLAYER_H

// FORWARD DECLARATIONS
class CVoIPAudioUplinkStream;
class CVoIPAudioDownlinkStream;
class CDTMFTonePlayer;
class CDTMFTonePlayerImpl;

/**
 *  MDTMFToneObserver class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *
 */
class MDTMFToneObserver
    {
public:
    static const TUint KOpenCompleteDNL = 1;
    static const TUint KOpenCompleteUPL = 2;

public:
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CDTMFTonePlayer& - DTMF tone player reference
     * @param TInt - Event type
     * @param TInt - Status
     * @return void
     */
    virtual void Event(const CDTMFTonePlayer& aPlayer, TInt aEventType,
            TInt aError) = 0;
    };

/**
 *  CDTMFTonePlayer class
 *
 *  DTMF Tone Player class
 *
 *  @lib VoIPAudioIntfc.dll
 *
 */
class CDTMFTonePlayer
    {
public:

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CDTMFTonePlayer();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MDTMFToneObserver&
     * @return TInt
     */
    IMPORT_C TInt Open(MDTMFToneObserver& aObserver);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TPtr -
     * @return TInt -
     */
    IMPORT_C TInt Play(const TPtr aTones);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt Stop();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    IMPORT_C void Close();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioUplinkStream& -
     * @return TInt
     */
    IMPORT_C TInt AddVoIPAudioUplinkStream(CVoIPAudioUplinkStream& aStream);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioUplinkStream&
     * @return TInt
     */
    IMPORT_C TInt RemoveVoIPAudioUplinkStream(CVoIPAudioUplinkStream& aStream);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioDownlinkStream& -
     * @return TInt
     */
    IMPORT_C TInt AddVoIPAudioDownlinkStream(CVoIPAudioDownlinkStream& aStream);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioDownlinkStream& -
     * @return TInt
     */
    IMPORT_C TInt RemoveVoIPAudioDownlinkStream(
            CVoIPAudioDownlinkStream& aStream);

protected:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CDTMFTonePlayer();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ConstructL(CDTMFTonePlayerImpl* aDTMFTonePlayerImpl);

private:

    CDTMFTonePlayerImpl* iDTMFTonePlayerImpl;

    };

#endif //__VOIPDTMFTONEPLAYER_H

// End of file
