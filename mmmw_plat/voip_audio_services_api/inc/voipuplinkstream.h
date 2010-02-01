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
* Description:  Defines VoIP uplink stream APIs.
*
*/


#ifndef __VOIPUPLINKSTREAM_H
#define __VOIPUPLINKSTREAM_H

#include <voipaudiocommon.h>
#include <voipdtmftoneplayer.h>


// FORWARD DECLARATIONS
class CVoIPDataBuffer;
class CVoIPFormatIntfc;
class CVoIPAudioUplinkStream;
class CVoIPAudioUplinkStreamImpl;


/**
 *  MVoIPUplinkObserver class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class MVoIPUplinkObserver
    {
public:
    static const TUint KOpenComplete = 1;
    static const TUint KUplinkError  = 2;
    static const TUint KUplinkClosed = 3;

public:
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioUplinkStream&
     * @param TPtr8
     * @return void
     */
    virtual void EmptyBuffer(const CVoIPAudioUplinkStream& aSrc,
                             CVoIPDataBuffer* aBuffer) = 0;

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioUplinkStream&
     * @param TInt - Event type
     * @param TInt - Status
     * @return void
     */
    virtual void Event(const CVoIPAudioUplinkStream& aSrc,
                       TInt aEventType,
                       TInt aError) = 0;
    };


/**
 *  CVoIPAudioUplinkStream class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CVoIPAudioUplinkStream
    {
public:

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPAudioUplinkStream();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MVoIPUplinkObserver&
     * @return TInt
     */
    IMPORT_C TInt Open(MVoIPUplinkObserver& aObserver);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVersion&
     * @return TInt
     */
    IMPORT_C TInt GetVersion(TVersion& aVersion);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt&
     * @return TInt
     */
    IMPORT_C TInt GetMaxGain(TInt& aGain);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt
     * @return TInt
     */
    IMPORT_C TInt SetGain(TInt aGain);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt&
     * @return TInt
     */
    IMPORT_C TInt GetGain(TInt& aGain);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPCodecFormat
     * @param CVoIPFormatIntfc&
     * @return TInt
     */
    IMPORT_C TInt SetFormat(TVoIPCodecFormat aFormat,
                            CVoIPFormatIntfc*& aIntfc);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt Start();

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
     * @param CVoIPDataBuffer*
     * @return TInt
     */
    IMPORT_C TInt BufferEmptied(CVoIPDataBuffer* aBuffer);

protected:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPAudioUplinkStream();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ConstructL(CVoIPAudioUplinkStreamImpl* aVoIPAudioUplinkStreamImpl);

private:
    /**
     * For internal use only
     */
    friend class CDTMFTonePlayer;

private:

    CVoIPAudioUplinkStreamImpl* iVoIPAudioUplinkStreamImpl;

    };

#endif //__VOIPUPLINKSTREAM_H


// End of file
