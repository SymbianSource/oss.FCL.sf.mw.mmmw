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
* Description: 
*
*/

#ifndef XASTATICCAPSADAPTATION_H
#define XASTATICCAPSADAPTATION_H

#include "XAGlobals.h"
#include "XADebug.h"

/* Adaptation device IDs */
//#define XA_ADAPTID_ALSASRC          0xAD7E5001  //Krishna commented
#define XA_ADAPTID_DEVSOUNDSRC          0xAD7E5001
//#define XA_ADAPTID_ALSASINK         0xAD7E5002
#define XA_ADAPTID_DEVSOUNDSINK     0xAD7E5002
#define XA_ADAPTID_AUTOAUDIOSINK    0xAD7E5003
#define XA_ADAPTID_AUDIOTESTSRC     0xAD7E5004
#define XA_ADAPTID_AUTOVIDEOSINK    0xAD7E5005
#define XA_ADAPTID_VIDEOTESTSRC     0xAD7E5006
#define XA_ADAPTID_V4L2SRC          0xAD7E5007
#define XA_ADAPTID_JACKSINK         0xAD7E5008
#define XA_ADAPTID_MOTIONJPEG       0xAD7E5009
#define XA_ADAPTID_THEORA           0xAD7E500A
#define XA_ADAPTID_VORBIS           0xAD7E500B

#define XA_ADAPTID_VIBRA            0xAD7E500C
#define XA_ADAPTID_LEDARRAY         0xAD7E500D

#define XA_ADAPTID_RAWVIDEO         0xAD7E500E

//#define XA_ADAPT_DEFAULTAUDIOOUTPUT XA_ADAPTID_ALSASINK
#define XA_ADAPT_DEFAULTAUDIOOUTPUT XA_ADAPTID_DEVSOUNDSINK

#ifdef XA_USE_TEST_PLUGINS
#define XA_ADAPT_DEFAULTAUDIOINPUT  XA_ADAPTID_AUDIOTESTSRC
#else
//#define XA_ADAPT_DEFAULTAUDIOINPUT  XA_ADAPTID_ALSASRC //Krishna commented
#define XA_ADAPT_DEFAULTAUDIOINPUT  XA_ADAPTID_DEVSOUNDSRC
#endif


#define XA_ADAPT_OGGPROFILE_UNKNOWN 0x1

#define XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_8   0x1
#define XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_16  0x2
#define XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_20  0x4
#define XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_24  0x8
#define XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_28  0x10
#define XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_32  0x20

#define XA_ADAPT_MAX_BITRATE_COUNT 10

typedef enum
{
    XACAP_DECODER  = 0x1,
    XACAP_ENCODER  = 0x2,
    XACAP_DEVSNK   = 0x4,
    XACAP_DEVSRC   = 0x8,

    XACAP_AUDIO    = 0x10,
    XACAP_VIDEO    = 0x20,
    XACAP_IMAGE    = 0x40
}XACapsType;

/* Structure to hold commonly used capabilities of codec/device.
 */
typedef struct XAStaticCapsData_
{
    XAuint32    captype;
    XAuint32    xaid;       /* OpenMAX id (e.g. XA_VIDEOCODEC macro) */
    XAchar*     adaptId;    /* e.g. plugin name */
    XAuint32    maxCh;      /* channels */
    XAuint32    minBPS;     /* bits per sample */
    XAuint32    maxBPS;
    XAuint32    minSR;      /* samplerate */
    XAuint32    maxSR;
    XAuint32    minBR;      /* bitrate */
    XAuint32    maxBR;
    XAuint32    maxFR;      /* framerate */
    XAuint32    maxW;       /* width */
    XAuint32    maxH;       /* height */
    XAuint32    pcmProfilesSupported;
    XAuint32    numBitrates;
    XAuint32    bitratesSupported[XA_ADAPT_MAX_BITRATE_COUNT];

    XAuint32    extra;      /* some type-specific extra info */
    /*add if/when needed*/
} XAStaticCapsData;


XAresult XAStaticCapsAdapt_GetCapsCount(XACapsType filter, XAuint32 *count);

XAresult XAStaticCapsAdapt_GetCapsById(XACapsType filter, XAuint32 maxId, XAStaticCapsData* data);

XAresult XAStaticCapsAdapt_GetCapsByIdx(XACapsType filter, XAuint32 idx, XAStaticCapsData* data);
XAresult XAStaticCapsAdapt_InitCaps();
XAresult XAStaticCapsAdapt_QueryColorFormats(XAuint32* pIndex, XAuint32* pColorFormats);
#endif /* XASTATICCAPSADAPTATION_H */
