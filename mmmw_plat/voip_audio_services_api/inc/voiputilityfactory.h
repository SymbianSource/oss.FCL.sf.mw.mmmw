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
 * Description:  Defines factory object for creating VoIP downlink and uplink
 *                streams as well as DTMF tone player.
 *
 */

#ifndef __VOIPUTILITYFACTORY_H
#define __VOIPUTILITYFACTORY_H

#include <voipaudiocommon.h>
#include <voipdatabuffer.h>

// FORWARD DECLARATIONS
class CVoIPUtilityFactoryImpl;
class CVoIPAudioDownlinkStream;
class CVoIPAudioUplinkStream;
class CDTMFTonePlayer;
class CVoIPJitterBufferIntfc;

/**
 *  CVoIPUtilityFactory class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *
 */
class CVoIPUtilityFactory
    {
public:
    /**
     *  TVoIPCallType
     *  ?description
 *
     */
    enum TVoIPCallType
        {
        EVoIPCall,
        EPoCCall
        };

public:

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPUtilityFactory*&
     * @return TInt
     */
    IMPORT_C static TInt CreateFactory(CVoIPUtilityFactory*& aFactory);

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C ~CVoIPUtilityFactory();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVersion
     * @param TCallType
     * @param CVoIPAudioDownlinkStream*&
     * @return TInt
     */
    IMPORT_C TInt CreateDownlinkStream(const TVersion aVersion,
            const TVoIPCallType aCallType, CVoIPAudioDownlinkStream*& aDnLink);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVersion
     * @param CVoIPJitterBufferIntfc*&
     * @param CVoIPAudioDownlinkStream*&
     * @return TInt
     */
    IMPORT_C TInt CreateDownlinkStream(const TVersion aVersion,
            CVoIPJitterBufferIntfc*& aJBIntfc,
            CVoIPAudioDownlinkStream*& aDnLink);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVersion
     * @param TCallType
     * @param CVoIPAudioUplinkStream*&
     * @return TInt
     */
    IMPORT_C TInt CreateUplinkStream(const TVersion aVersion,
            const TVoIPCallType aCallType, CVoIPAudioUplinkStream*& aUpLink);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CDTMFTonePlayer*&
     * @return TInt
     */
    IMPORT_C TInt CreateDTMFTonePlayer(CDTMFTonePlayer*& aDtmfPlayer);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPDataBuffer*&
     * @param const CVoIPDataBuffer::TVoIPBufferType
     * @param const TVoIPCodecFormat aCodecFormat - default EG711 assumes
     *        request to create 162-byte buffer for G711 codec.
     * @return TInt
     */
    IMPORT_C TInt CreateBuffer(CVoIPDataBuffer*& aBuffer,
            const CVoIPDataBuffer::TVoIPBufferType aType,
            const TVoIPCodecFormat aCodecFormat = EG711);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param RArray<TVoIPCodecFormat>&
     * @return TInt -
     */
    IMPORT_C TInt GetSupportedDownlinkFormats(
            RArray<TVoIPCodecFormat>& aFormats);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param RArray<TVoIPCodecFormat>&
     * @return TInt -
     */
    IMPORT_C TInt GetSupportedUplinkFormats(
            RArray<TVoIPCodecFormat>& aFormats);

private:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPUtilityFactory();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ConstructL();

private:

    CVoIPUtilityFactoryImpl* iVoIPUtilityFactoryImpl;

    };

#endif //__VOIPUTILITYFACTORY_H

// End of file
