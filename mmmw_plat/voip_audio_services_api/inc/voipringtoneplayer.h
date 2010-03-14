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
 * Description:  Defines APIs for VoIP ring tone player.
 *
 */

#ifndef __RINGTONEPLAYER_H
#define __RINGTONEPLAYER_H

// FORWARD DECLARATIONS
class RFile;
class CRingTonePlayer;
class CRingTonePlayerImpl;

/**
 *  MRingToneObserver class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *
 */
class MRingToneObserver
    {
public:
    static const TUint KOpenComplete = 1;
    static const TUint KPlaybackComplete = 2;

public:
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CRingTonePlayer& - Ring tone player reference
     * @param TInt - Event type
     * @param TInt - Status
     * @return void
     */
    virtual void Event(const CRingTonePlayer& aSrc, TInt aEventType,
            TInt aError) = 0;
    };

/**
 *  CRingTonePlayer class
 *
 *  Ring Tone Player class
 *
 *  @lib VoIPAudioIntfc.dll
 *
 */
class CRingTonePlayer
    {
public:

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CRingTonePlayer();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @return TInt
     */
    IMPORT_C TInt Open(MRingToneObserver& aObserver);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @param TPtr
     * @return TInt
     */
    IMPORT_C TInt Open(MRingToneObserver& aObserver, TPtr aFileName);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @param RFile&
     * @return TInt
     */
    IMPORT_C TInt Open(MRingToneObserver& aObserver, RFile& aFileHandle);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @param TPtr8
     * @param TPtr8
     * @return TInt
     */
    IMPORT_C TInt Open(MRingToneObserver& aObserver, TPtr8 aDesTone,
            TPtr8 aMimeType);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt Play();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt Pause();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt Resume();

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
     * @return TInt
     */
    IMPORT_C void Close();

protected:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CRingTonePlayer();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ConstructL(CRingTonePlayerImpl* aRingTonePlayerImpl);

private:

    CRingTonePlayerImpl* iRingTonePlayerImpl;

    };

#endif //__RINGTONEPLAYER_H

// End of file
