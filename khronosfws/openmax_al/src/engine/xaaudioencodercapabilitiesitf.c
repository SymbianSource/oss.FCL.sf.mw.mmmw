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
 * Description: Audio Encoder capabilities Itf Implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "xaglobals.h"
#include "xaadptbasectx.h"
#include "xaaudioencodercapabilitiesitf.h"
#include "xacapabilitiesmgr.h"

/* XAAudioEncoderCapabilitiesItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAAudioEncoderCapabilitiesItfImpl* GetImpl(
        XAAudioEncoderCapabilitiesItf self)
    {
    if (self)
        {
        XAAudioEncoderCapabilitiesItfImpl* impl =
                (XAAudioEncoderCapabilitiesItfImpl*) (*self);
        if (impl && (impl == impl->self))
            {
            return impl;
            }
        }
    return NULL;
    }

/*****************************************************************************
 * Base interface XAAudioEncoderCapabilitiesItf implementation
 *****************************************************************************/

/* XAresult XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoders
 * Description: Retrieves the available audio encoders.
 */
XAresult XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoders(
        XAAudioEncoderCapabilitiesItf self, XAuint32* pNumEncoders,
        XAuint32* pEncoderIds)
    {
    XAAudioEncoderCapabilitiesItfImpl* impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoders");

    if (!impl || !pNumEncoders)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {
        if (pEncoderIds)
            { /* query array of encoders */
            if (*pNumEncoders < impl->numCodecs)
                {
                DEBUG_ERR("XA_RESULT_BUFFER_INSUFFICIENT");
                res = XA_RESULT_BUFFER_INSUFFICIENT;
                }
            else
                {

                XAuint32 i = 0;
                XACapabilities temp;
                for (i = 0; i < impl->numCodecs; i++)
                    {
                    /* query encoder id from adaptation using index value */
                    XACapabilitiesMgr_GetCapsByIdx(impl->capslist,
                            (XACapsType) (XACAP_ENCODER | XACAP_AUDIO), i,
                            &temp);
                    pEncoderIds[i] = temp.xaid;
                    }

                pEncoderIds[0] = XA_AUDIOCODEC_AMR;
                pEncoderIds[1] = XA_AUDIOCODEC_AAC;
                pEncoderIds[2] = XA_AUDIOCODEC_PCM;
                }
            }

        /* return number of encoders */
        *pNumEncoders = impl->numCodecs;
        }DEBUG_API("<-XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoders");
    return res;
    }

/* XAresult XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoderCapabilities
 * Description: Queries for the audio encoders capabilities.
 */
XAresult XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoderCapabilities(
        XAAudioEncoderCapabilitiesItf self, XAuint32 encoderId,
        XAuint32 *pIndex, XAAudioCodecDescriptor *pDescriptor)
    {
    XAAudioEncoderCapabilitiesItfImpl* impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    XACapabilities temp;

    DEBUG_API("->XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoderCapabilities");

    /*if( !impl || !pIndex || !pDescriptor )*/
    /*Removing the pDescriptor because the client can pass
     * it as NULL to query the pIndex for number of Codec/Mode
     * pair capabilities for each encoder */
    if (!impl || !pIndex)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }
    else
        {
        *pIndex = 1;
        if (!pDescriptor)
            {
            return res;
            }
        }

    /* query capabilities from adaptation using codec id */
    memset(pDescriptor, 0, sizeof(XAAudioCodecDescriptor));

    switch (encoderId)
        {
        case XA_AUDIOCODEC_AMR:
            {
            impl->sampleRateArray[0] = 8000000;

            impl->bitRateArray[0] = 4750;
            impl->bitRateArray[1] = 5150;
            impl->bitRateArray[2] = 5900;
            impl->bitRateArray[3] = 6700;
            impl->bitRateArray[4] = 7400;
            impl->bitRateArray[5] = 7950;
            impl->bitRateArray[6] = 10200;
            impl->bitRateArray[7] = 12200;

            pDescriptor->maxChannels = 1;
            pDescriptor->minBitsPerSample = 8;
            pDescriptor->maxBitsPerSample = 8;
            pDescriptor->minSampleRate = 8000000; /*milliHz*/
            pDescriptor->maxSampleRate = 8000000;
            pDescriptor->isFreqRangeContinuous = XA_BOOLEAN_FALSE;
            pDescriptor->pSampleRatesSupported
                    = (XAmilliHertz*) (&(impl->sampleRateArray));
            pDescriptor->numSampleRatesSupported = 1;
            pDescriptor->minBitRate = 4750;
            pDescriptor->maxBitRate = 12200;
            pDescriptor->isBitrateRangeContinuous = XA_BOOLEAN_FALSE;
            pDescriptor->pBitratesSupported
                    = (XAuint32*) (&(impl->bitRateArray));
            pDescriptor->numBitratesSupported = 8;
            pDescriptor->profileSetting = XA_AUDIOPROFILE_AMR;
            pDescriptor->modeSetting = 0;
            }
            break;
        case XA_AUDIOCODEC_AAC:
            {
            impl->sampleRateArray[0] = 8000000;
            impl->sampleRateArray[1] = 11025000;
            impl->sampleRateArray[2] = 16000000;
            impl->sampleRateArray[3] = 22050000;
            impl->sampleRateArray[4] = 32000000;
            impl->sampleRateArray[5] = 44100000;
            impl->sampleRateArray[6] = 48000000;

            impl->bitRateArray[0] = 32000;
            impl->bitRateArray[1] = 64000;
            impl->bitRateArray[2] = 96000;
            impl->bitRateArray[3] = 128000;
            impl->bitRateArray[4] = 160000;
            impl->bitRateArray[5] = 192000;
            impl->bitRateArray[6] = 224000;
            impl->bitRateArray[7] = 256000;

            pDescriptor->maxChannels = 2;
            pDescriptor->minBitsPerSample = 16;
            pDescriptor->maxBitsPerSample = 16;
            pDescriptor->minSampleRate = 8000 * 1000; /*milliHz*/
            pDescriptor->maxSampleRate = 48000 * 1000;
            pDescriptor->isFreqRangeContinuous = XA_BOOLEAN_FALSE;
            pDescriptor->pSampleRatesSupported
                    = (XAmilliHertz*) (&(impl->sampleRateArray));
            pDescriptor->numSampleRatesSupported = 7;
            pDescriptor->minBitRate = 32000;
            pDescriptor->maxBitRate = 256000;
            pDescriptor->isBitrateRangeContinuous = XA_BOOLEAN_FALSE;
            pDescriptor->pBitratesSupported
                    = (XAuint32*) (&(impl->bitRateArray));
            pDescriptor->numBitratesSupported = 8;
            pDescriptor->profileSetting = XA_AUDIOPROFILE_AAC_AAC;
            pDescriptor->modeSetting = XA_AUDIOMODE_AAC_LC;
            }
            break;
        case XA_AUDIOCODEC_PCM:
            {
            impl->sampleRateArray[0] = 12000000;
            impl->sampleRateArray[1] = 16000000;
            impl->sampleRateArray[2] = 22050000;
            impl->sampleRateArray[3] = 24000000;
            impl->sampleRateArray[4] = 32000000;
            impl->sampleRateArray[5] = 44100000;
            impl->sampleRateArray[6] = 48000000;
            impl->sampleRateArray[7] = 64000000;
            impl->sampleRateArray[8] = 88200000;
            impl->sampleRateArray[9] = 96000000;

            pDescriptor->maxChannels = 2;
            pDescriptor->minBitsPerSample = 16;
            pDescriptor->maxBitsPerSample = 16;
            pDescriptor->minSampleRate = 8000000; /*milliHz*/
            pDescriptor->maxSampleRate = 96000000;
            pDescriptor->isFreqRangeContinuous = XA_BOOLEAN_FALSE;
            pDescriptor->pSampleRatesSupported
                    = (XAmilliHertz*) (&(impl->sampleRateArray));
            pDescriptor->numSampleRatesSupported = 10;
            pDescriptor->minBitRate = 0;
            pDescriptor->maxBitRate = 0;
            pDescriptor->isBitrateRangeContinuous = XA_BOOLEAN_FALSE;
            pDescriptor->pBitratesSupported = NULL;
            pDescriptor->numBitratesSupported = 0;
            pDescriptor->profileSetting = XA_AUDIOPROFILE_PCM;
            pDescriptor->modeSetting = 0;
            }
            break;
        }

    res = XACapabilitiesMgr_GetCapsById(impl->capslist,
            (XACapsType) (XACAP_ENCODER | XACAP_AUDIO), encoderId, &temp);
    if (res == XA_RESULT_SUCCESS)
        {
        XAAudioCodecDescriptor* desc =
                ((XAAudioCodecDescriptor*) (temp.pEntry));
        /* map applicable values to XAAudioCodecCapabilities */
        pDescriptor->maxChannels = desc->maxChannels;
        pDescriptor->minSampleRate = desc->minSampleRate * 1000; /* milliHz */
        if (desc->maxSampleRate < (0xFFFFFFFF / 1000))
            {
            pDescriptor->maxSampleRate = desc->maxSampleRate * 1000;
            }
        else
            {
            pDescriptor->maxSampleRate = 0xFFFFFFFF;
            }
        pDescriptor->minBitsPerSample = desc->minBitsPerSample;
        pDescriptor->maxBitsPerSample = desc->maxBitsPerSample;
        pDescriptor->isFreqRangeContinuous = XA_BOOLEAN_TRUE;
        pDescriptor->minBitRate = desc->minBitRate;
        pDescriptor->maxBitRate = desc->maxBitRate;
        pDescriptor->numBitratesSupported = desc->numBitratesSupported;
        pDescriptor->isBitrateRangeContinuous = XA_BOOLEAN_TRUE;
        if (temp.xaid == XA_AUDIOCODEC_PCM)
            {
            pDescriptor->profileSetting = XA_AUDIOPROFILE_PCM;
            pDescriptor->modeSetting = 0; /* no chanmode for pcm defined */
            }
        else if (temp.xaid == XA_ADAPTID_VORBIS) /* for ogg */
            {
            if (desc->maxChannels == 1)
                {
                pDescriptor->profileSetting = XA_AUDIOPROFILE_MPEG1_L3;
                pDescriptor->modeSetting = XA_AUDIOCHANMODE_MP3_MONO;
                }
            else
                {
                pDescriptor->profileSetting = XA_AUDIOPROFILE_MPEG2_L3;
                pDescriptor->modeSetting = XA_AUDIOCHANMODE_MP3_STEREO;
                }
            }
        else
            {
            /* do nothing */
            }

        }

    DEBUG_API("<-XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoderCapabilities");
    return res;
    }

/*****************************************************************************
 * XAAudioEncoderCapabilitiesItfImpl -specific methods
 *****************************************************************************/

/* XAAudioEncoderCapabilitiesItfImpl_Create
 * Description: Allocate and initialize XAAudioEncoderCapabilitiesItfImpl
 */
XAAudioEncoderCapabilitiesItfImpl* XAAudioEncoderCapabilitiesItfImpl_Create(
        XACapabilities* caps)
    {
    XAAudioEncoderCapabilitiesItfImpl* self =
            (XAAudioEncoderCapabilitiesItfImpl*) calloc(1,
                    sizeof(XAAudioEncoderCapabilitiesItfImpl));

    DEBUG_API("->XAAudioEncoderCapabilitiesItfImpl_Create");

    if (self)
        {
        /* init itf default implementation */
        self->itf.GetAudioEncoders
                = XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoders;
        self->itf.GetAudioEncoderCapabilities
                = XAAudioEncoderCapabilitiesItfImpl_GetAudioEncoderCapabilities;
        self->capslist = caps;

        /* init variables */
        assert( XACapabilitiesMgr_GetCapsCount( caps, (XACapsType)((XACapsType)(XACAP_ENCODER|XACAP_AUDIO)),
                        &(self->numCodecs) ) == XA_RESULT_SUCCESS );

        /*self->mmfEngine = (void*)mmf_capability_engine_init();*/
        self->numCodecs = 3;
        self->self = self;

        }DEBUG_API("<-XAAudioEncoderCapabilitiesItfImpl_Create");
    return self;
    }

/* void XAAudioEncoderCapabilitiesItfImpl_Free
 * Description: Free all resources reserved at XAAudioEncoderCapabilitiesItfImpl_Create
 */
void XAAudioEncoderCapabilitiesItfImpl_Free(
        XAAudioEncoderCapabilitiesItfImpl* self)
    {
    DEBUG_API("->XAAudioEncoderCapabilitiesItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAAudioEncoderCapabilitiesItfImpl_Free");
    }
