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


#ifndef __MP3CODEC_H__
#define __MP3CODEC_H__

// INCLUDES
#include <E32Base.h>

// CONSTANTS

// DATA TYPES

// FORWARD DECLARATION
class CMp3ToPcmDecoderImpl;

// CLASS DEFINITIONS
/*
-----------------------------------------------------------------------------

    TMp3DecParams

    MP3 decoding parameters.

    @since 2.1

-----------------------------------------------------------------------------
*/
class TMp3DecParams
    {
public:
    inline TMp3DecParams();
public:
    TInt iStereoToMono;    // stereo to mono switch (TRUE or default FALSE)
    TInt iLeftRight;       // decode left or right channel (1 left, 2 right, 0 default all)
    TInt iDecimFactor;     // decimation factor (2, 4 or default 1)
    TInt iConcealment;     // error concealment level (0 none, default 1)
    TInt iSampleLength;    // resolution of PCM samples (default 16 bits)
    TInt iSamplingFrequency; //Output sampling frequency
    };

TMp3DecParams::TMp3DecParams() :
iStereoToMono(0), iLeftRight(0), iDecimFactor(1), iConcealment(1), iSampleLength(16), iSamplingFrequency(0) {}



/*
-----------------------------------------------------------------------------

    TMp3FrameInfo

    MP3 frame info struct.

    @since 2.1

-----------------------------------------------------------------------------
*/
class TMp3FrameInfo
    {
public:
    inline TMp3FrameInfo();
public:
    IMPORT_C static TInt MinFrameSize();
    IMPORT_C static TInt MaxFrameSize();
    IMPORT_C static TInt MaxFrameSamples();
    IMPORT_C static TInt MaxChannels();
    IMPORT_C static TInt MaxAncillaryBits();
    IMPORT_C static TInt FrameHeaderSize();
    IMPORT_C static TInt FrameSize(const TMp3FrameInfo& aInfo);
public:
    TInt iId;              // id of algorithm (1 MPEG-1, 0 MPEG-2)
    TInt iLayer;           // layer number (1, 2, 3 or 4)
    TInt iErrorProtection; // err prot flag (TRUE or FALSE, TRUE if err protected)
    TInt iBitrate;         // bitrate (kbit/s)
    TInt iSamplingRate;    // sampling frequency (Hz)
    TInt iPadding;         // padding flag (TRUE or FALSE, TRUE if p slot exists)
    TInt iPrivateBit;      // bit for private use
    TInt iMode;            // audio mode (0 stereo, 1 jstereo, 2 dual, 3 mono)
    TInt iChannels;        // number of audio channels (1 for mono, otherwise 2)
    TInt iCopyright;       // copyright flag (TRUE or FALSE, TRUE if copyrighted)
    TInt iOrigCopy;        // original flag (TRUE or FALSE, TRUE if original)
    TInt iFrameSize;       // size of encoded frame (bytes)
    TInt iFrameSamples;    // length of decoded frame (samples per channel)
public:
    TInt iSamplingRateOut; // sampling frequency after conversion(s) (Hz)
    TInt iChannelsOut;     // number of audio channels after conversion(s) (1 or 2)
    TInt iFrameSamplesOut; // length of decoded frame after conversion(s) (samples per channel)
    };

TMp3FrameInfo::TMp3FrameInfo() {}



/*
-----------------------------------------------------------------------------

    CMp3ToPcmDecoder

    Low level MP3 decoding API.

    @since 2.1

-----------------------------------------------------------------------------
*/
class CMp3ToPcmDecoder : public CBase
    {
protected:
    IMPORT_C CMp3ToPcmDecoder();
    IMPORT_C void ConstructL();

public:
    IMPORT_C static CMp3ToPcmDecoder* NewL(const TMp3DecParams& aConf);
    IMPORT_C virtual ~CMp3ToPcmDecoder();
    IMPORT_C void Reset();
    IMPORT_C void Reset(const TMp3DecParams& aConf);
    IMPORT_C static TInt FrameInfo(const TUint8* aBuf, TInt aBufLen, TMp3FrameInfo& aInfo);
    IMPORT_C static TInt FrameInfo(const TUint8* aBuf, TInt aBufLen, TMp3FrameInfo& aInfo, const TMp3DecParams& aConf);
    IMPORT_C static TInt SeekSync(const TUint8* aBuf, TInt aBufLen);
    IMPORT_C TInt Decode(TUint8* aSrc, TInt& aSrcUsed, TUint8* aDst, TInt& aDstLen, TInt aMuteFlag);
    IMPORT_C TInt Decode(TUint8* aSrc, TInt& aSrcUsed, TUint8* aDst1, TUint8* aDst2, TUint8* aDst3, TInt& aDstLen1,  TInt& aDstLen2,  TInt& aDstLen3, TInt aMuteFlag);
private:
    CMp3ToPcmDecoderImpl* iState;
    };

#endif //__MP3CODEC_H__
//-----------------------------------------------------------------------------
//  End of File
//-----------------------------------------------------------------------------
