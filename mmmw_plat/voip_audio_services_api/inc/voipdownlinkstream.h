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
* Description:  Defines VoIP downlink stream APIs.
*
*/


#ifndef __VOIPDOWNLINKSTREAM_H
#define __VOIPDOWNLINKSTREAM_H

#include <voipaudiocommon.h>
#include <voipdtmftoneplayer.h>


// FORWARD DECLARATIONS
class CVoIPAudioDownlinkStream;
class CVoIPAudioDownlinkStreamImpl;
class CVoIPDataBuffer;
class CVoIPFormatIntfc;

/**
 *  MVoIPDownlinkObserver class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class MVoIPDownlinkObserver
    {
public:
    static const TUint KOpenComplete       = 1;
    static const TUint KDownlinkError      = 2;
    static const TUint KDownlinkClosed     = 3;
    static const TUint KAudioDeviceChanged = 4;

public:
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const CVoIPAudioDownlinkStream&
     * @param CVoIPDataBuffer*
     * @return void
     */
    virtual void FillBuffer(const CVoIPAudioDownlinkStream& aSrc,
                            CVoIPDataBuffer* aBuffer) = 0;
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const CVoIPAudioDownlinkStream&
     * @param TInt - Event type
     * @param TInt - Status
     * @return void
     */
    virtual void Event(const CVoIPAudioDownlinkStream& aSrc,
                       TInt aEventType,
                       TInt aError) = 0;
    };


/**
 *  CVoIPAudioDownlinkStream class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CVoIPAudioDownlinkStream
    {
public:
    /**
     *  TVoIPOutputDevice
     *  ?description
     *  @since S60 v5.0
     */
    enum TVoIPOutputDevice
        {
        EHandset,
        ELoudSpeaker,
        ENoPreference
        };

public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPAudioDownlinkStream();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MVoIPDownlinkObserver&
     * @return TInt
     */
    IMPORT_C TInt Open(MVoIPDownlinkObserver& aObserver);

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
    IMPORT_C TInt GetMaxVolume(TInt& aVolume);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt
     * @return TInt
     */
    IMPORT_C TInt SetVolume(TInt aVolume);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt&
     * @return TInt
     */
    IMPORT_C TInt GetVolume(TInt& aVolume);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPCodecFormat
     * @param CVoIPFormatIntfc*&
     * @return TInt
     */
    IMPORT_C TInt SetFormat(TVoIPCodecFormat aFormat,
                            CVoIPFormatIntfc*& aIntfc);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPOutputDevice
     * @return TInt
     */
    IMPORT_C TInt SetAudioDevice(TVoIPOutputDevice aDevice);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPOutputDevice&
     * @return TInt
     */
    IMPORT_C TInt GetAudioDevice(TVoIPOutputDevice& aDevice);

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
    IMPORT_C TInt BufferFilled(CVoIPDataBuffer* aBuffer);

protected:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    CVoIPAudioDownlinkStream();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
     void ConstructL(
          CVoIPAudioDownlinkStreamImpl* aVoIPAudioDownlinkStreamImpl);

private:
    /**
     * For internal use only
     */
    friend class CDTMFTonePlayer;

private:

    CVoIPAudioDownlinkStreamImpl* iVoIPAudioDownlinkStreamImpl;

    };

#endif //__VOIPDOWNLINKSTREAM_H


// End of file
