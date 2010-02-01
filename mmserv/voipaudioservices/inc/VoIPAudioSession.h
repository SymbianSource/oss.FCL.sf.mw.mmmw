/*
 * Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Declares RVoIPAudioSession - client side interface to
 *                VoIP server.
 *
 */

#ifndef __VOIPAUDIOSESSION_H
#define __VOIPAUDIOSESSION_H

// INCLUDES
#include <mmfbase.h>
#include <mmfutilities.h> //for TFourCC
#include <voipaudiocommon.h>
#include <voipdownlinkstream.h>
#include <voipformatintfc.h>
#include <voipjitterbufferintfc.h>
#include "VoIPAudioClientServer.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CVoIPFormatIntfc;

// CLASS DECLARATION

/**
 * RVoIPAudioSession API
 */
class RVoIPAudioSession : public RSessionBase
    {
public:
    /**
     * Opens a connection to the server.
     * @return KErrNone if successful connection created
     */
    IMPORT_C TInt Connect();
    /**
     * @return The server version this session can connct to
     */
    IMPORT_C TVersion Version() const;
    /**
     * Close the session to the server
     */
    IMPORT_C void Close();

    // Downlink stream methods

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt OpenDownlink(TMMFPrioritySettings aPriority);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt GetMaxVolume();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt SetVolume(const TInt aVolume);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt GetVolume();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const TMMFPrioritySettings
     * @param RArray<TUint32>&
     * @param TInt&
     * @return TInt
     */
    IMPORT_C TInt GetSupportedDecoders(const TMMFPrioritySettings aPriority,
            RArray<TUint32>& aDecoders, TInt& aFrameSize);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt SetDecoder(const TUint32 aDecoder);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt SetAudioDevice(
            const CVoIPAudioDownlinkStream::TVoIPOutputDevice aDevice);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt GetAudioDevice(
            CVoIPAudioDownlinkStream::TVoIPOutputDevice& aDevice);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt StartDownlink();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt StopDownlink();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C void CloseDownlink();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TPtr8
     * @param const TInt64
     * @return TInt
     */
    IMPORT_C TInt BufferFilled(TPtr8 aBuffer, const TUint aBufferSequence = 0);

    // Uplink stream methods

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const TMMFPrioritySettings
     * @return TInt
     */
    IMPORT_C TInt OpenUplink(const TMMFPrioritySettings aPriority);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt GetMaxGain();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt SetGain(const TInt aGain);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt GetGain();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const TMMFPrioritySettings
     * @param RArray<TUint32>&
     * @param TInt&
     * @return TInt
     */
    IMPORT_C TInt GetSupportedEncoders(const TMMFPrioritySettings aPriority,
            RArray<TUint32>& aEncoders, TInt& aFrameSize);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt SetEncoder(const TUint32 aEncoder);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt StartUplink();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt StopUplink();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C void CloseUplink();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TPtr8
     * @return TInt
     */
    IMPORT_C TInt BufferEmptied(TPtr8 aBuffer);

    // Codec CI methods

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TArray<TUint>&
     * @return void
     */
    IMPORT_C void GetSupportedBitRatesL(RArray<TUint>& aArray);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const TUint
     * @return TInt
     */
    IMPORT_C TInt SetBitRate(const TUint aBitrate);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TUint&
     * @return TInt
     */
    IMPORT_C TInt GetBitRate(TUint& aBitrate);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetVAD(const TBool aVad);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetVAD(TBool& aVad);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt FrameModeRequiredForEC(TBool& aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetFrameMode(const TBool aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetFrameMode(TBool& aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt ConcealErrorForNextBuffer();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TG711CodecMode
     * @return TInt
     */
    IMPORT_C TInt SetMode(const CVoIPFormatIntfc::TG711CodecMode aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TG711CodecMode&
     * @return TInt
     */
    IMPORT_C TInt GetMode(CVoIPFormatIntfc::TG711CodecMode& aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TILBCCodecMode
     * @return TInt
     */
    IMPORT_C TInt SetMode(const CVoIPFormatIntfc::TILBCCodecMode aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TILBCCodecMode&
     * @return TInt
     */
    IMPORT_C TInt GetMode(CVoIPFormatIntfc::TILBCCodecMode& aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetCNG(const TBool aCng);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetCNG(TBool& aCng);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetPLC(const TBool aPlc);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetPLC(TBool& aPlc);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt BadLsfNextBuffer();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt OpenDTMFTonePlayer();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt PlayDTMFTone(const TDesC& aTones);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt StopDTMFTonePlay();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C void CloseDTMFTonePlayer();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt OpenRingTonePlayer();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TDesC&
     * @return TInt
     */
    IMPORT_C TInt OpenRingTonePlayer(const TDesC& aFileName);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param RFile&
     * @return TInt
     */
    IMPORT_C TInt OpenRingTonePlayer(const RFile& aFileHandle);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TDes&
     * @param TDes&
     * @return TInt
     */
    IMPORT_C TInt OpenRingTonePlayer(const TDesC8& aDesData);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt PlayRingTone();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt PauseRingTone();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt ResumeRingTone();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt StopRingTone();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    IMPORT_C void CloseRingTonePlayer();

    /**
     * ConfigureJitterBuffer
     *
     * @since S60 v5.0
     * @param const TVoIPJBConfig&
     * @return TInt
     */
    IMPORT_C TInt ConfigureJitterBuffer(const TVoIPJBConfig& aJbConfig);

    /**
     * ResetJitterBuffer
     *
     * @since S60 v5.0
     * @param const TBool
     * @return TInt
     */
    IMPORT_C TInt ResetJitterBuffer(const TBool aPlayTone);

    /**
     * DelayDown
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt DelayDown();

    /**
     * DelayUp
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt DelayUp();

private:
    /**
     * PopulateArrayL
     *
     * @since S60 v5.0
     * @param TVoIPAudioClientServerRequest
     * @param RArray<TUint32>&
     * @param TInt
     * @return void
     */
    void PopulateArrayL(TVoIPAudioClientServerRequest aRequest,
            RArray<TUint32>& aDecoders, TInt aCount);

    TInt SendClientRequest(TVoIPAudioClientServerRequest aRequest,
            const TIpcArgs& aArgs) const;

    };

#endif // __VOIPAUDIOSESSION_H

// End of file
