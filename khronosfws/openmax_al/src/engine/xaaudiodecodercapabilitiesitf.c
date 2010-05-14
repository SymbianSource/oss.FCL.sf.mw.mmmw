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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "xaglobals.h"
#include "xaadptbasectx.h"
#include "xaaudiodecodercapabilitiesitf.h"
#include "xacapabilitiesmgr.h"
/* XAAudioDecoderCapabilitiesItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAAudioDecoderCapabilitiesItfImpl* GetImpl(XAAudioDecoderCapabilitiesItf self)
{
    if( self )
    {
        XAAudioDecoderCapabilitiesItfImpl* impl = (XAAudioDecoderCapabilitiesItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XAAudioDecoderCapabilitiesItf implementation
 *****************************************************************************/

/* XAresult XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoders
 * Description: Retrieves the available audio decoders.
 */
XAresult XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoders(
                            XAAudioDecoderCapabilitiesItf self,
                            XAuint32* pNumDecoders,
                            XAuint32* pDecoderIds)
{
    XAAudioDecoderCapabilitiesItfImpl* impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoders");

    if( !impl || !pNumDecoders )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( pDecoderIds )
        {   /* query array of decoders */
            if( *pNumDecoders < impl->numCodecs )
            {
                DEBUG_ERR("XA_RESULT_BUFFER_INSUFFICIENT");
                res = XA_RESULT_BUFFER_INSUFFICIENT;
            }
            else
            {
 
            XAuint32 i = 0;
            XACapabilities temp;
                for( i=0; i<impl->numCodecs; i++ )
                {
                    /* query decoder id from adaptation using index value */
                    XACapabilitiesMgr_GetCapsByIdx(NULL, (XACapsType)(XACAP_DECODER|XACAP_AUDIO), i, &temp);
                    pDecoderIds[i] = temp.xaid;
                }

            }
        }
        /* return number of decoders */
        *pNumDecoders = impl->numCodecs;
    }
    DEBUG_API("<-XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoders");
    return res;
}

/* XAresult XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoderCapabilities
 * Description: Queries for the audio decoder�s capabilities.
 */
XAresult XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoderCapabilities(
                            XAAudioDecoderCapabilitiesItf self,
                            XAuint32 decoderId,
                            XAuint32* pIndex,
                            XAAudioCodecDescriptor* pDescriptor)
{
    XAAudioDecoderCapabilitiesItfImpl* impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoderCapabilities");

    if( !impl || !pIndex )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( !pDescriptor )
        {   /* query number of capa structures */
            *pIndex = 1;
        }
        else
        {   /* query capabilities */
            if( *pIndex >= 1 )
            {
                DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
                res = XA_RESULT_PARAMETER_INVALID;
            }
            else
            {
 
                /* query capabilities from adaptation using codec id */
                XACapabilities temp;
                memset(pDescriptor,0,sizeof(XAAudioCodecDescriptor));
                res = XACapabilitiesMgr_GetCapsById(NULL, (XACapsType)(XACAP_DECODER|XACAP_AUDIO), decoderId, &temp);
                if( res == XA_RESULT_SUCCESS )
                {
                    XAAudioCodecDescriptor* desc = ((XAAudioCodecDescriptor*)(temp.pEntry));
                    /* map applicable values to XAAudioCodecCapabilities */
                    pDescriptor->maxChannels = desc->maxChannels;
                    pDescriptor->minSampleRate= desc->minSampleRate*1000; /* milliHz */
                    if (desc->maxSampleRate < (0xFFFFFFFF / 1000))
                    {
                        pDescriptor->maxSampleRate = desc->maxSampleRate*1000;
                    }
                    else
                    {
                        pDescriptor->maxSampleRate = 0xFFFFFFFF;
                    }
                    pDescriptor->minBitsPerSample=desc->minBitsPerSample;
                    pDescriptor->maxBitsPerSample=desc->maxBitsPerSample;
                    pDescriptor->isFreqRangeContinuous=XA_BOOLEAN_TRUE;
                    pDescriptor->minBitRate=desc->minBitRate;
                    pDescriptor->maxBitRate=desc->maxBitRate;
                    pDescriptor->numBitratesSupported = desc->numBitratesSupported;
                    pDescriptor->isBitrateRangeContinuous=XA_BOOLEAN_TRUE;
                    if (temp.xaid == XA_AUDIOCODEC_PCM )
                    {
                        pDescriptor->profileSetting=XA_AUDIOPROFILE_PCM;
                        pDescriptor->modeSetting=0; /* no chanmode for pcm defined */
                    }
                    else if (temp.xaid == XA_ADAPTID_VORBIS) /* for ogg */
                    {
                        if (desc->maxChannels == 1)
                        {
                            pDescriptor->profileSetting=XA_AUDIOPROFILE_MPEG1_L3;
                            pDescriptor->modeSetting=XA_AUDIOCHANMODE_MP3_MONO;
                        }
                        else
                        {
                            pDescriptor->profileSetting=XA_AUDIOPROFILE_MPEG2_L3;
                            pDescriptor->modeSetting=XA_AUDIOCHANMODE_MP3_STEREO;
                        }
                    }
                    else
                    {
                        /* do nothing */
                    }
                    /*other caps undefined*/
                }
                
            }
        }
    }

    DEBUG_API("<-XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoderCapabilities");
    return res;
}


/*****************************************************************************
 * XAAudioDecoderCapabilitiesItfImpl -specific methods
 *****************************************************************************/

/* XAAudioDecoderCapabilitiesItfImpl_Create
 * Description: Allocate and initialize XAAudioDecoderCapabilitiesItfImpl
 */
XAAudioDecoderCapabilitiesItfImpl* XAAudioDecoderCapabilitiesItfImpl_Create()
{
    XAAudioDecoderCapabilitiesItfImpl* self = (XAAudioDecoderCapabilitiesItfImpl*)
        calloc(1,sizeof(XAAudioDecoderCapabilitiesItfImpl));

    DEBUG_API("->XAAudioDecoderCapabilitiesItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.GetAudioDecoders =
            XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoders;
        self->itf.GetAudioDecoderCapabilities =
            XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoderCapabilities;

        /* init variables */
 
        assert( XACapabilitiesMgr_GetCapsCount(NULL, (XACapsType)(XACAP_DECODER|XACAP_AUDIO),
                                  &(self->numCodecs) ) == XA_RESULT_SUCCESS );

        self->self = self;
    }
    DEBUG_API("<-XAAudioDecoderCapabilitiesItfImpl_Create");
    return self;
}

/* void XAAudioDecoderCapabilitiesItfImpl_Free
 * Description: Free all resources reserved at XAAudioDecoderCapabilitiesItfImpl_Create
 */
void XAAudioDecoderCapabilitiesItfImpl_Free(XAAudioDecoderCapabilitiesItfImpl* self)
{
    DEBUG_API("->XAAudioDecoderCapabilitiesItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAAudioDecoderCapabilitiesItfImpl_Free");
}
