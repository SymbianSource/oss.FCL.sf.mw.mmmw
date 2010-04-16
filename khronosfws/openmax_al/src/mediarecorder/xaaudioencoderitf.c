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
#include "xaaudioencoderitf.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAudioEncoderItfAdaptation.h"
#include "XAStaticCapsAdaptation.h"
#include "XARecordItfAdaptation.h"
#endif
#include "xathreadsafety.h"
#include "xaadaptationmmf.h"
#include "cmmfbackendengine.h"

/* XAAudioEncoderItfImpl* GetImpl(XAAudioEncoderItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 **/
static XAAudioEncoderItfImpl* GetImpl(XAAudioEncoderItf self)
{
    if( self )
    {
        XAAudioEncoderItfImpl* impl = (XAAudioEncoderItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XAAudioEncoderItf implementation
 *****************************************************************************/

/**
 * XAresult XAAudioEncoderItfImpl_SetEncoderSettings(XAAudioEncoderItf self,
 *                                                   XAAudioEncoderSettings *pSettings)
 * Description: Set audio encoder settings.
 **/
XAresult XAAudioEncoderItfImpl_SetEncoderSettings(XAAudioEncoderItf self,
                                                  XAAudioEncoderSettings *pSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 recState = XA_RECORDSTATE_STOPPED;
    XAAudioEncoderItfImpl *impl = GetImpl(self);
    XAMediaRecorderAdaptationMMFCtx* mCtx;
    XAAudioEncoderSettings currentSettings;

    DEBUG_API("->XAAudioEncoderItfImpl_SetEncoderSettings");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaRecorder );

    if( !impl || !pSettings )
        {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAAudioEncoderItfImpl_SetEncoderSettings");
        return XA_RESULT_PARAMETER_INVALID;
        }

    mCtx = (XAMediaRecorderAdaptationMMFCtx*) impl->pObjImpl->adaptationCtxMMF;
    
    if(mCtx->xaRecordState != recState)
        {
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED");
        DEBUG_API("<-XAAudioEncoderItfImpl_SetEncoderSettings");
        return XA_RESULT_PRECONDITIONS_VIOLATED;
        }

    if(impl->pObjImpl->isMMFRecord)
        {
        XAAudioEncoderItfImpl_GetEncoderSettings(self, &currentSettings);
        if(pSettings->encoderId == currentSettings.encoderId)
            {
            if(pSettings->channelsIn !=  currentSettings.channelsIn)
                {
                ret = mmf_set_destination_channels(mCtx->mmfContext, &(pSettings->channelsIn));
                }
            
            if(pSettings->channelsOut != currentSettings.channelsOut)
                {
                ret = mmf_set_destination_channels(mCtx->mmfContext, &(pSettings->channelsOut));
                }
            
            if(pSettings->sampleRate != currentSettings.sampleRate)
                {
                ret = mmf_set_destination_samplerate(mCtx->mmfContext, &(pSettings->sampleRate));
                }

            if(pSettings->bitRate != currentSettings.bitRate)
                {
                ret = mmf_set_destination_bitrate(mCtx->mmfContext, &(pSettings->bitRate));
                }
            
            if(pSettings->rateControl != currentSettings.rateControl)
                {
                ret = XA_RESULT_PARAMETER_INVALID;
                }
            if(pSettings->channelMode != currentSettings.channelMode)
                {
                ret = XA_RESULT_PARAMETER_INVALID;
                }
            if(pSettings->encodeOptions != currentSettings.encodeOptions)
                {
                ret = XA_RESULT_PARAMETER_INVALID;
                }
            if(pSettings->blockAlignment != currentSettings.blockAlignment)
                {
                ret = XA_RESULT_PARAMETER_INVALID;
                }    
            if(pSettings->bitsPerSample != currentSettings.bitsPerSample)
                {
                ret = XA_RESULT_PARAMETER_INVALID;
                }    
            if(pSettings->profileSetting != currentSettings.profileSetting)
                {
                ret = XA_RESULT_PARAMETER_INVALID;
                }    
            if(pSettings->levelSetting != currentSettings.levelSetting)
                {
                ret = XA_RESULT_PARAMETER_INVALID;
                }    
            }
        else
            {
            ret = XA_RESULT_PARAMETER_INVALID;
            }
        }
    else
        {
#ifdef _GSTREAMER_BACKEND_
        XAStaticCapsData temp;
    
        ret = XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_AUDIO, pSettings->encoderId, &temp);
        if( ret == XA_RESULT_SUCCESS )
            {
            ret = XARecordItfAdapt_GetRecordState( impl->adapCtx, &recState );
            if( ret == XA_RESULT_SUCCESS )
                {
                if( XA_RECORDSTATE_STOPPED == recState )
                    {
                    ret = XAAudioEncoderItfAdapt_SetEncoderSettings( impl->adapCtx, pSettings);
                    }
                else
                    {
                    DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED");
                    DEBUG_API("<-XAAudioEncoderItfImpl_SetEncoderSettings");
                    return XA_RESULT_PRECONDITIONS_VIOLATED;
                    }
                }
            }
        else
            {
            DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
            DEBUG_API("<-XAAudioEncoderItfImpl_SetEncoderSettings");
            return XA_RESULT_FEATURE_UNSUPPORTED;
            }
#endif
        }    
    
    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
    DEBUG_API("<-XAAudioEncoderItfImpl_SetEncoderSettings");
    return ret;
}

/**
 * XAresult XAAudioEncoderItfImpl_GetEncoderSettings(XAAudioEncoderItf self,
 *                                                   XAAudioEncoderSettings *pSettings)
 * Description: Get audio encoder settings.
 **/
XAresult XAAudioEncoderItfImpl_GetEncoderSettings(XAAudioEncoderItf self,
                                                  XAAudioEncoderSettings *pSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 encoderId;
    XAuint32 channelsIn;
    XAuint32 channelsOut;
    XAmilliHertz sampleRate;
    XAuint32 bitRate;
    XAMediaRecorderAdaptationMMFCtx* mCtx;
   
    XAAudioEncoderItfImpl *impl = GetImpl(self);
    
    DEBUG_API("->XAAudioEncoderItfImpl_GetEncoderSettings");
    if(!impl || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaRecorderAdaptationMMFCtx*) (impl->pObjImpl->adaptationCtxMMF);
    


    if(impl->pObjImpl->isMMFRecord)
        {
        mmf_get_encoder_id(mCtx->mmfContext, &encoderId);
        
        switch(encoderId)
            {
            case 0x36315020:
                pSettings->encoderId = XA_AUDIOCODEC_PCM;
                pSettings->bitsPerSample = 16;
                pSettings->profileSetting = XA_AUDIOPROFILE_PCM;
                pSettings->levelSetting = 0;
                pSettings->streamFormat = 0;
                break;
            case 0x524d4120:
                pSettings->encoderId = XA_AUDIOCODEC_AMR;
                pSettings->bitsPerSample = 8;
                pSettings->profileSetting = XA_AUDIOPROFILE_AMR;
                pSettings->levelSetting = 0;
                pSettings->streamFormat = XA_AUDIOSTREAMFORMAT_FSF;
                break;
            case 0x43414120:
                pSettings->encoderId = XA_AUDIOCODEC_AAC;
                pSettings->bitsPerSample = 16;
                pSettings->profileSetting = XA_AUDIOPROFILE_AAC_AAC;
                pSettings->levelSetting = XA_AUDIOMODE_AAC_LC;
                pSettings->streamFormat = XA_AUDIOSTREAMFORMAT_RAW;
                break;
            }
        
        mmf_get_destination_channels(mCtx->mmfContext, &channelsIn);
        pSettings->channelsIn =  channelsIn;
        mmf_get_destination_channels(mCtx->mmfContext, &channelsOut);
        pSettings->channelsOut = channelsOut;
        mmf_get_destination_samplerate(mCtx->mmfContext, &sampleRate);
        pSettings->sampleRate = sampleRate;
        mmf_get_destination_bitrate(mCtx->mmfContext, &bitRate);
        pSettings->bitRate = bitRate;
        pSettings->rateControl = XA_RATECONTROLMODE_CONSTANTBITRATE;
        pSettings->channelMode = 0;
        pSettings->encodeOptions = 0;
        pSettings->blockAlignment = 0;
        }
    else
        {
#ifdef _GSTREAMER_BACKEND_
        ret = XAAudioEncoderItfAdapt_GetEncoderSettings(impl->adapCtx, pSettings);
#endif    
        }
    DEBUG_API("<-XAAudioEncoderItfImpl_GetEncoderSettings");
    return ret;
}

/*****************************************************************************
 * XAAudioEncoderItfImpl -specific methods
 *****************************************************************************/

/* XAAudioEncoderItfImpl* XAAudioEncoderItfImpl_Create()
 * Description: Allocate and initialize XAAudioEncoderItfImpl
 */
XAAudioEncoderItfImpl* XAAudioEncoderItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBaseCtx *adapCtx,
#endif
        XAMediaRecorderImpl* impl
        )
{
    XAAudioEncoderItfImpl* self = (XAAudioEncoderItfImpl*)
        calloc(1,sizeof(XAAudioEncoderItfImpl));
    DEBUG_API("->XAAudioEncoderItfImpl_Create");
    if( self )
    {
        /* init itf default implementation */
        self->itf.GetEncoderSettings = XAAudioEncoderItfImpl_GetEncoderSettings;
        self->itf.SetEncoderSettings = XAAudioEncoderItfImpl_SetEncoderSettings;
        self->pObjImpl = impl;
        /* init variables */
#ifdef _GSTREAMER_BACKEND_
        self->adapCtx = adapCtx;
#endif        

        self->self = self;
    }
    DEBUG_API("<-XAAudioEncoderItfImpl_Create");
    return self;
}

/* void XAAudioEncoderItfImpl_Free(XAAudioEncoderItfImpl* self)
 * Description: Free all resources reserved at XAAudioEncoderItfImpl_Create
 */
void XAAudioEncoderItfImpl_Free(XAAudioEncoderItfImpl* self)
{
    DEBUG_API("->XAAudioEncoderItfImpl_Free");
    assert( self==self->self );
    free( self );
    DEBUG_API("<-XAAudioEncoderItfImpl_Free");
}
