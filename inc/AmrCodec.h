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


#ifndef __AMRCODEC_H__
#define __AMRCODEC_H__

// INCLUDES

#include <E32Base.h>


// FORWARD DECLARATIONS

class CAmrToPcmDecoderImpl;
class CPcmToAmrEncoderImpl;


// CLASS DECLARATION

/**
*    TAmrDecParams
*    AMR decoding parameters.
*/
class TAmrDecParams
    {
public:
    /**
    * Default constructor.
    */
    inline TAmrDecParams();
public:
    // error concealment level (0 none, 1 default)
    TInt iConcealment;
    };

TAmrDecParams::TAmrDecParams() :
iConcealment(1) {}

/**
*    TAmrEncParams
*    AMR encoding parameters.
*/
class TAmrEncParams
    {
public:
    /**
    * Default constructor.
    */
    inline TAmrEncParams();
public:
    // encoding mode 0-7 (0=MR475,1=MR515,...,7=MR122, default 7)
    TInt iMode;
    // DTX flag (TRUE or default FALSE)
    TInt iDTX;
    };

TAmrEncParams::TAmrEncParams() :
iMode(7), iDTX(0) {}


// CLASS DEFINITIONS

/**
*    TAmrFrameInfo
*    AMR frame info struct.
*/
class TAmrFrameInfo
    {
public:
    /**
    * Default constructor.
    */
    TAmrFrameInfo() {};
public:
    /**
    * Returns the size of smallest AMR audio frame, i.e., the size of one DTX
    * frame. NO DATA frames (size 1 byte) are not taken into account here.
    *
    * @since ?Series60_version
    * @param none
    * @return TInt
    */
    IMPORT_C static TInt MinFrameSize();

    /**
    * Returns the size of biggest AMR audio frame, i.e., the size of one EFR
    * frame.
    *
    * @since ?Series60_version
    * @param none
    * @return TInt
    */
    IMPORT_C static TInt MaxFrameSize();

    /**
    * Returns the maximum number of samples per one audio channel in one
    * AMR audio frame.
    *
    * @since ?Series60_version
    * @param none
    * @return TInt
    */
    IMPORT_C static TInt MaxFrameSamples();

    /**
    * Returns the maximum number audio channels in one frame.
    *
    * @since ?Series60_version
    * @param none
    * @return TInt
    */
    IMPORT_C static TInt MaxChannels();

    /**
    * Returns the size of one AMR frame header. The header must include
    * all bits needed for determining the actual frame length.
    *
    * @since ?Series60_version
    * @param none
    * @return TInt Size of AMR frame header
    */
    IMPORT_C static TInt FrameHeaderSize();

    /**
    * Returns the size of frame described by given coding parameters. The only
    * parameter used in calculation is the coding mode (iMode).
    *
    * @since ?Series60_version
    * @param none
    * @return TInt AMR frame size
    */
    IMPORT_C static TInt FrameSize(const TAmrFrameInfo& aInfo);
public:
    TInt iMode;            // encoding mode
    TInt iBitrate;         // bitrate (kbit/s)
    TInt iSamplingRate;    // sampling frequency (Hz)
    TInt iChannels;        // number of channels
    TInt iFrameSize;       // encoded size (bytes)
    TInt iFrameSamples;    // decoded size (samples per channel)
public:
    TInt iSamplingRateOut; // sampling frequency after conversion (Hz)
    TInt iChannelsOut;     // number of audio channels after conversion (1 or 2)
    TInt iFrameSamplesOut; // decoded size after conversion (samples per channel)
    };

/**
*    CAmrToPcmDecoder
*    Low level AMR decoding API.
*/
class CAmrToPcmDecoder : public CBase
    {
protected:
    /**
    * Default constructor.
    */
    IMPORT_C CAmrToPcmDecoder();

    /**
    * 2nd phase constructor. Constructs the decoder.
    */
    IMPORT_C void ConstructL();
public:
    /**
    * Two-phased constructor. First allocates and constructs decoder 
    * and then resets it according to the given configuration.
    *
    * @since ?Series60_version
    * @param aConf Decoder configuration params
    * @return CAmrToPcmDecoder* Pointer to constructed decoder
    */
    IMPORT_C static CAmrToPcmDecoder* NewL(const TAmrDecParams& aConf);

    /**
    * Destructor
    */
    IMPORT_C virtual ~CAmrToPcmDecoder();

    /**
    * Initializes decoder and resets it into the default decoding state.
    *
    * @since ?Series60_version
    * @param none
    * @return void
    */
    IMPORT_C void Reset();

    /**
    * Initializes decoder and resets it according to the given configuration.
    *
    * @since ?Series60_version
    * @param aConf Decoder configuration params
    * @return void
    */
    IMPORT_C void Reset(const TAmrDecParams& aConf);

    /**
    * Calculates the frame size and returns information about the frame which
    * starts from the beginning of aBuf. Returns 0, if the frame bit syntax is
    * incorrect or not enough bits are available for calculating the frame size
    * and the frame parameters (aBufLen < TAmrCodecParams::FrameHeaderSize).
    *
    * The frame parameters returned via aInfo are: mode, sampling frequency,
    * number of channels, frame size (same as return value) and number of
    * samples in this frame. See also TMdaRawAmrAudioCodec class.
    *
    * @since ?Series60_version
    * @param aBuf Pointer to frame buffer
    * @param aBufLen Length of frame buffer
    * @param aInfo Reference to frame parameters
    * @return TInt Length of frame in bytes
    */
    IMPORT_C static TInt FrameInfo(const TUint8* aBuf, TInt aBufLen, TAmrFrameInfo& aInfo);

    /**
    * A variation of the FrameInfo above, which takes into account the
    * decoding switches in calculating iSampligRateOut, iChannelsOut
    * and iFrameSamplesOut.Initializes decoder and resets it according 
    * to the given configuration.
    *
    * @since ?Series60_version
    * @param aBuf Pointer to frame buffer
    * @param aBufLen Length of frame buffer
    * @param aInfo Reference to fram parameters
    * @param aConf Reference to decoding switches
    * @return TInt Length of frame in bytes
    */
    IMPORT_C static TInt FrameInfo(const TUint8* aBuf, TInt aBufLen, TAmrFrameInfo& aInfo, const TAmrDecParams& aConf);

    /**
    * This routine seeks the start position of the next frame and returns
    * the byte position of its header. Returns aBufLen, if no valid frame
    * can not be found (see FrameInfo). The seek progresses from the start
    * of aBuf (0) toward the end of aBuf(aBufLen - 1).
    *    
    * The level of syntax check depends on the number of bits available. At
    * minimum the first frame header bits are checked only, but if more
    * bits are available, they can be used to make the sync seek more robust.
    * For succesful seek the whole frame does not need to exist in aBuf.    
    *
    * @since ?Series60_version
    * @param aBuf Pointer to frame buffer
    * @param aBufLen Length of frame buffer
    * @return TInt Frame position or aBufLen
    */
    IMPORT_C static TInt SeekSync(const TUint8* aBuf, TInt aBufLen);

    /**
    * Decodes one frame.    
    *
    * @since ?Series60_version
    * @param aSrc Pointer to bit stream buffer
    * @param aSrcUsed Number of consumed bytes
    * @param aDst Pointer to PCM buffer
    * @param aDstLen Number of produced bytes
    * @param aMuteFlag If ETrue this frame is muted
    * @return TInt Non-zero if decoding failed
    */
    IMPORT_C TInt Decode(TUint8* aSrc, TInt& aSrcUsed, TUint8* aDst, TInt& aDstLen, TBool aMuteFlag);

    /**
    * Decodes one frame from aSrc to aDst1 and aDst2. NOTE: aDst1 or
    * aDst2 can be NULL and in that case decoding to that buffer is not
    * requested.
    *
    * @since ?Series60_version
    * @param aSrc Pointer to bit stream buffer
    * @param aSrcUsed Number of consumed bytes
    * @param aDst1 Pointer to PCM sample buffer (time domain sample)
    * @param aDst2 Pointer to frequency sample buffer (frequency domain sample)
    * @param aDstLen1 Number of produced bytes in aDst1
    * @param aDstLen2 Number of produced bytes in aDst2
    * @param aMuteFlag If ETrue this frame is muted
    * @return TInt Non-zero if decoding failed
    */
    IMPORT_C TInt Decode(TUint8* aSrc, TInt& aSrcUsed, TUint8* aDst1, TUint8* aDst2, TInt& aDstLen1, TInt& aDstLen2, TInt aMuteFlag);
private:
    CAmrToPcmDecoderImpl* iState;
    };

/**
*    CPcmToAmrEncoder
*    Low level AMR encoding API.
*/
class CPcmToAmrEncoder : public CBase
    {
protected:

    /**
    * Default constructor.
    */
    IMPORT_C CPcmToAmrEncoder();

    /**
    * 2nd phase constructor. Constructs the encoder.
    */
   IMPORT_C void ConstructL();
public:

    /**
    * Two-phased constructor. First allocates and constructs encoder 
    * and then resets it according to the given configuration.
    *
    * @since ?Series60_version
    * @param aConf Encoder configuration params
    * @return CPcmToAmrEncoder* Pointer to constructed encoder
    */
    IMPORT_C static CPcmToAmrEncoder* NewL(const TAmrEncParams& aConf);

    /**
    * Destructor
    */
    IMPORT_C virtual ~CPcmToAmrEncoder();

    /**
    * Initializes encoder and resets encoder state.
    *
    * @since ?Series60_version
    * @param none
    * @return void
    */
    IMPORT_C void Reset();

    /**
    * Initializes encoder and resets it according to the given configuration.
    *
    * @since ?Series60_version
    * @param aConf Encoder configuration params
    * @return void
    */
    IMPORT_C void Reset(const TAmrEncParams& aConf);

    /**
    * Encodes one frame.
    *
    * @since ?Series60_version
    * @param aSrc Pointer to PCM buffer
    * @param aSrcUsed Number of consumed bytes
    * @param aDst Pointer to bit stream buffer
    * @param aDstLen Length of resulting frame
    * @return TInt Non-zero if encoding failed
    */
    IMPORT_C TInt Encode(TUint8* aSrc, TInt& aSrcUsed, TUint8* aDst, TInt& aDstLen);
private:
    CPcmToAmrEncoderImpl* iState;
    };

#endif //__AMRCODEC_H__

//  End of File
