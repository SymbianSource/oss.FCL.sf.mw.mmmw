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

#include <gst.h>
#include <mixer.h>
#include "XAAdaptation.h"
#include "XAVolumeItfAdaptation.h"
#include "XAMediaPlayerAdaptCtx.h"
#include "XAStaticCapsAdaptation.h"
#include "XAOutputMixAdaptCtx.h"
#include "XAMediaRecorderAdaptCtx.h"

/*
 * XAresult XAVolumeItfAdapt_FlushBin(XAAdaptationBaseCtx *ctx)
 */
XAresult XAVolumeItfAdapt_FlushBin(XAAdaptationBaseCtx *ctx)
{
    DEBUG_API("->XAVolumeItfAdapt_FlushBin");
    if(!ctx || ( ctx->ctxId != XAMediaPlayerAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    if (GST_STATE(ctx->bin) != GST_STATE_PLAYING)
    {
        gint64 position = 0;
        GstFormat format = GST_FORMAT_TIME;

        gst_element_query_position( GST_ELEMENT(ctx->bin), &format, &position  ) ;

        XAAdaptationBase_PrepareAsyncWait(ctx);
        /* in effect seeks to current position and flushing the buffer, due to
         * gstreamer implementation actual position might change, if stopped, seek to beginning
         */
        if(!gst_element_seek( ctx->bin, ((XAMediaPlayerAdaptationCtx*)ctx)->playrate, GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE,
                            GST_SEEK_TYPE_SET, position,
                            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
        {
            DEBUG_ERR("WARN: gst reports seek not handled");
        }
        /* flushed seeks always asynchronous */
        XAAdaptationBase_StartAsyncWait(ctx);
        DEBUG_INFO("Bin flushed.");
        ctx->waitingasyncop = XA_BOOLEAN_FALSE;
    }
    DEBUG_API("<-XAVolumeItfAdapt_FlushBin");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAVolumeItfAdapt_SetVolumeLevel(void *ctx, AdaptationContextIDS ctx->ctxId,
 *                                          XAmillibel level)
 * @param void *ctx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAmillibel level - Requested volume level, in between XA_MILLIBEL_MIN and XA_MILLIBEL_MAX
 * @return XAresult ret - Success value
 */
XAresult XAVolumeItfAdapt_SetVolumeLevel(XAAdaptationBaseCtx *ctx, XAmillibel level)
{
    GstElement *vol=NULL, *audiopp=NULL;
    gdouble gstVolume = 0;
    XAmillibel tempVolumeLevel = 0;
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API_A1("->XAVolumeItfAdapt_SetVolumeLevel (level %d)", level);
    if(!ctx || ( ctx->ctxId != XAMediaPlayerAdaptation &&
                 ctx->ctxId != XAMediaRecorderAdaptation &&
                 ctx->ctxId != XAOutputMixAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_SetVolumeLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( level <= MIN_SUPPORT_VOLUME_LEVEL )
    {
        tempVolumeLevel = MIN_SUPPORT_VOLUME_LEVEL;
    }
    else if( level >= MAX_SUPPORT_VOLUME_LEVEL )
    {
        tempVolumeLevel = MAX_SUPPORT_VOLUME_LEVEL;
    }
    else
    {
        tempVolumeLevel = level;
    }
    gstVolume = ( ( (gdouble)(VOLUME_LEVEL_RATIO + tempVolumeLevel)/ VOLUME_LEVEL_RATIO) );

    if ( ctx->ctxId == XAMediaPlayerAdaptation )
    {
        XAMediaPlayerAdaptationCtx* context = (XAMediaPlayerAdaptationCtx*) ctx;
        audiopp = gst_bin_get_by_name( GST_BIN(context->baseObj.bin), "audiopp" );
        if(audiopp)
        {
            vol = gst_bin_get_by_name( GST_BIN(audiopp), "pp_vol" );
        }
        if(vol)
        {
            DEBUG_INFO_A1("Setting gst level to %f",gstVolume);
            g_object_set( G_OBJECT(vol), "volume", (gdouble)gstVolume, NULL );

            XAVolumeItfAdapt_FlushBin(ctx);
        }
        else
        {
            DEBUG_ERR("Could not find gst volume controller element!!");
        }
    }
    else if ( ctx->ctxId == XAMediaRecorderAdaptation )
    {
        XAMediaRecorderAdaptationCtx* context = (XAMediaRecorderAdaptationCtx*) ctx;
        audiopp = gst_bin_get_by_name( GST_BIN(context->baseObj.bin), "audiopp" );
        if(audiopp)
        {
            vol = gst_bin_get_by_name( GST_BIN(audiopp), "pp_vol" );
        }
        if(vol)
        {
            DEBUG_INFO_A1("Setting gst level to %f",gstVolume);
            g_object_set( G_OBJECT(vol), "volume", (gdouble)gstVolume, NULL );
        }
        else
        {
            DEBUG_ERR("Could not find gst volume controller element!!");
        }
    }
    else if (ctx->ctxId == XAOutputMixAdaptation )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        guint iterator = 0;
        for ( iterator = 0; iterator < context->connectedObjects->len; iterator++ )
        {
            GstBin* basebin = NULL;
            XAAdaptationBaseCtx* bCtx = g_array_index(context->connectedObjects,XAOMixAdaptConnObj,iterator).ctx;
            if(!bCtx)
            {
                DEBUG_ERR_A1("Context in connected objects array (index %u) is NULL!", iterator);
                DEBUG_API("<-XAVolumeItfAdapt_SetVolumeLevel");
                return XA_RESULT_INTERNAL_ERROR;
            }

            basebin = GST_BIN(bCtx->bin);
            if(!basebin)
            {
                DEBUG_ERR_A1("Bin in connected objects context (index %u) is NULL!", iterator);
                DEBUG_API("<-XAVolumeItfAdapt_SetVolumeLevel");
                return XA_RESULT_INTERNAL_ERROR;
            }

            audiopp = gst_bin_get_by_name( basebin, "audiopp" );
            if(audiopp)
            {
                vol = gst_bin_get_by_name( GST_BIN(audiopp), "pp_vol" );
            }
            if(vol)
            {
                DEBUG_INFO_A1("Setting gst level to %f",gstVolume);
                g_object_set( G_OBJECT(vol), "volume", (gdouble)gstVolume, NULL );

                XAVolumeItfAdapt_FlushBin(bCtx);
            }
            else
            {
                DEBUG_ERR_A1("Could not find gst volume controller for player %u!!", iterator);
            }
        }
    }
    DEBUG_API("<-XAVolumeItfAdapt_SetVolumeLevel");
    return res;
}

/*
 * XAresult XAVolumeItfAdapt_GetMaxVolumeLevel(void *ctx, AdaptationContextIDS ctx->ctxId,
                                            XAmillibel *pMaxLevel)
 * @param void *ctx - Adaptation context, this will be casted to correct type regarding to contextID value given as 2nd parameter
 * @param AdaptationContextIDS ctx->ctxId - Type specifier for context, this will be used to cast ctx pointer to correct type.
 * XAmillibel *pMaxLevel - Maximum volume level
 * @return XAresult ret - Success value
 */
XAresult XAVolumeItfAdapt_GetMaxVolumeLevel(XAAdaptationBaseCtx *ctx, XAmillibel *pMaxLevel)
{
    DEBUG_API("->XAVolumeItfAdapt_GetMaxVolumeLevel");

    if(!ctx || ( ctx->ctxId != XAMediaPlayerAdaptation &&
                 ctx->ctxId != XAMediaRecorderAdaptation &&
                 ctx->ctxId != XAOutputMixAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_GetMaxVolumeLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pMaxLevel = MAX_SUPPORT_VOLUME_LEVEL;

    DEBUG_API("<-XAVolumeItfAdapt_GetMaxVolumeLevel");

    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAVolumeItfAdapt_SetMute(void *ctx, AdaptationContextIDS ctx->ctxId, XAboolean mute)
 * @param void *ctx - Adaptation context, this will be casted to correct type regarding to contextID value given as 2nd parameter
 * @param AdaptationContextIDS ctx->ctxId - Type specifier for context, this will be used to cast ctx pointer to correct type.
 * @param XAboolean mute - status of mute value
 * @return XAresult ret - Success value
 */
XAresult XAVolumeItfAdapt_SetMute(XAAdaptationBaseCtx *ctx, XAboolean mute)
{
    GstElement *vol=NULL, *audiopp=NULL;
    gboolean gmute = 0;

    DEBUG_API("->XAVolumeItfAdapt_SetMute");
    if(!ctx || ( ctx->ctxId != XAMediaPlayerAdaptation &&
                 ctx->ctxId != XAMediaRecorderAdaptation &&
                 ctx->ctxId != XAOutputMixAdaptation ) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_SetMute");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( ctx->ctxId == XAMediaPlayerAdaptation )
    {
        XAMediaPlayerAdaptationCtx* context = (XAMediaPlayerAdaptationCtx*) ctx;
        context->mute = mute;
        audiopp = gst_bin_get_by_name( GST_BIN(context->baseObj.bin), "audiopp" );
        if(audiopp)
        {
            vol = gst_bin_get_by_name( GST_BIN(audiopp), "pp_vol" );
        }
        if(vol)
        {
            if ( mute )
            {
                gmute = 1;
            }
            else
            {
                gmute = 0;
            }
            g_object_set( G_OBJECT(vol), "mute", gmute, NULL );
        }
        else
        {
            DEBUG_ERR("Could not find gst volume controller element!!");
        }
    }
    else if ( ctx->ctxId == XAMediaRecorderAdaptation )
    {
        XAMediaRecorderAdaptationCtx* context = (XAMediaRecorderAdaptationCtx*) ctx;
        context->mute = mute;
        audiopp = gst_bin_get_by_name( GST_BIN(context->baseObj.bin), "audiopp" );
        if(audiopp)
        {
            vol = gst_bin_get_by_name( GST_BIN(audiopp), "pp_vol" );
        }
        if(vol)
        {
            if ( mute )
            {
                gmute = 1;
            }
            else
            {
                gmute = 0;
            }
            g_object_set( G_OBJECT(vol), "mute", gmute, NULL );
        }
        else
        {
            DEBUG_ERR("Could not find gst volume controller element!!");
        }
    }
    else if ( ctx->ctxId == XAOutputMixAdaptation )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        guint iterator = 0;
        context->mute = mute;
        for ( iterator = 0; iterator < context->connectedObjects->len; iterator++ )
        {
            GstBin* basebin = NULL;
            XAAdaptationBaseCtx* bCtx = g_array_index(context->connectedObjects,XAOMixAdaptConnObj,iterator).ctx;
            if(!bCtx)
            {
                DEBUG_ERR_A1("Context in connected objects array (index %u) is NULL!", iterator);
                DEBUG_API("<-XAVolumeItfAdapt_SetMute");
                return XA_RESULT_INTERNAL_ERROR;
            }

            basebin = GST_BIN(bCtx->bin);
            if(!basebin)
            {
                DEBUG_ERR_A1("Bin in connected objects context (index %u) is NULL!", iterator);
                DEBUG_API("<-XAVolumeItfAdapt_SetMute");
                return XA_RESULT_INTERNAL_ERROR;
            }

            audiopp = gst_bin_get_by_name( basebin, "audiopp" );
            if(audiopp)
            {
                vol = gst_bin_get_by_name( GST_BIN(audiopp), "pp_vol" );
            }
            if(vol)
            {
                if ( mute )
                {
                    gmute = 1;
                }
                else
                {
                    gmute = 0;
                }
                g_object_set( G_OBJECT(vol), "mute", gmute, NULL );
            }
            else
            {
                DEBUG_ERR_A1("Could not find gst volume controller for player %u!!", iterator);
            }
        }
    }
    DEBUG_API("<-XAVolumeItfAdapt_SetMute");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAVolumeItfAdapt_EnableStereoPosition(XAAdaptationBaseCtx *ctx, XAboolean enable)
 * @param XAAdaptationBaseCtx *ctx - Adaptation context
 * @param XAboolean enable - Enable Stereo Position
 * @return XAresult - Success value
 */
XAresult XAVolumeItfAdapt_EnableStereoPosition(XAAdaptationBaseCtx *ctx, XAboolean enable)
{
    DEBUG_API("->XAVolumeItfAdapt_EnableStereoPosition");

    if(!ctx || ( ctx->ctxId != XAMediaPlayerAdaptation &&
                 ctx->ctxId != XAMediaRecorderAdaptation &&
                 ctx->ctxId != XAOutputMixAdaptation ) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_EnableStereoPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( ctx->ctxId == XAMediaPlayerAdaptation  )
    {
        XAMediaPlayerAdaptationCtx* context = (XAMediaPlayerAdaptationCtx*) ctx;
        context->isStereoPosition = enable;
    }
    else if ( ctx->ctxId == XAMediaRecorderAdaptation  )
    {
        XAMediaRecorderAdaptationCtx* context = (XAMediaRecorderAdaptationCtx*) ctx;
        context->isStereoPosition = enable;
    }
    else if ( ctx->ctxId == XAOutputMixAdaptation  )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        context->isStereoPosition = enable;
    }

    DEBUG_API("<-XAVolumeItfAdapt_EnableStereoPosition");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAVolumeItfAdapt_SetStereoPosition(XAAdaptationBaseCtx *ctx, XApermille stereoPosition)
 * @param XAAdaptationBaseCtx *ctx - Adaptation context
 * @param XApermille stereoPosition - Stereo Position to be set
 * @return XAresult - Success value
 */
XAresult XAVolumeItfAdapt_SetStereoPosition(XAAdaptationBaseCtx *ctx,
                                            XApermille stereoPosition)
{
    GstElement *audiopp = NULL, *pan = NULL;
    gfloat gstPosition = 0.0;

    DEBUG_API("->XAVolumeItfAdapt_SetStereoPosition");
    if(!ctx || ( ctx->ctxId != XAMediaPlayerAdaptation &&
                 ctx->ctxId != XAMediaRecorderAdaptation &&
                 ctx->ctxId != XAOutputMixAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
        return XA_RESULT_PARAMETER_INVALID;
    }

    gstPosition = ( (gfloat)stereoPosition / STEREO_POSITION_RATIO );

    if ( ctx->ctxId == XAMediaPlayerAdaptation  )
    {
        XAMediaPlayerAdaptationCtx* context = (XAMediaPlayerAdaptationCtx*) ctx;

        audiopp = gst_bin_get_by_name( GST_BIN(context->baseObj.bin), "audiopp");
        if( !audiopp )
        {
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
            return XA_RESULT_INTERNAL_ERROR;
        }

        pan = gst_bin_get_by_name( GST_BIN(audiopp), "pp_pan" );
        if( !pan )
        {
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
            return XA_RESULT_INTERNAL_ERROR;
        }

        g_object_set( G_OBJECT(pan), "panorama", gstPosition, NULL );

        XAVolumeItfAdapt_FlushBin(ctx);
    }
    else if ( ctx->ctxId == XAMediaRecorderAdaptation  )
    {
        XAMediaRecorderAdaptationCtx* context = (XAMediaRecorderAdaptationCtx*) ctx;

        audiopp = gst_bin_get_by_name( GST_BIN(context->baseObj.bin), "audiopp");
        if( !audiopp )
        {
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
            return XA_RESULT_INTERNAL_ERROR;
        }

        pan = gst_bin_get_by_name( GST_BIN(audiopp), "pp_pan" );
        if( !pan )
        {
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
            return XA_RESULT_INTERNAL_ERROR;
        }

        g_object_set( G_OBJECT(pan), "panorama", gstPosition, NULL );
    }

    else if ( ctx->ctxId == XAOutputMixAdaptation )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        guint iterator = 0;
        for ( iterator = 0; iterator < context->connectedObjects->len; iterator++ )
        {
            GstBin* basebin = NULL;
            XAAdaptationBaseCtx* bCtx = g_array_index(context->connectedObjects,XAOMixAdaptConnObj,iterator).ctx;
            if(!bCtx)
            {
                DEBUG_ERR_A1("Context in connected objects array (index %u) is NULL!", iterator);
                return XA_RESULT_INTERNAL_ERROR;
            }

            basebin = GST_BIN(bCtx->bin);
            if(!basebin)
            {
                DEBUG_ERR_A1("Bin in connected objects context (index %u) is NULL!", iterator);
                return XA_RESULT_INTERNAL_ERROR;
            }

            audiopp = gst_bin_get_by_name( basebin, "audiopp" );
            if( !audiopp )
            {
                DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
                DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
                return XA_RESULT_INTERNAL_ERROR;
            }
            pan = gst_bin_get_by_name( GST_BIN(audiopp), "pp_pan" );
            if( !pan )
            {
                DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
                DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
                return XA_RESULT_INTERNAL_ERROR;
            }
            g_object_set( G_OBJECT(pan), "panorama", gstPosition, NULL );

            XAVolumeItfAdapt_FlushBin(bCtx);
        }
    }

    DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAVolumeItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adapCtx)
 * Description: Safety thread entry
 */
XAresult XAVolumeItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adaptCtx)
{
    DEBUG_API("->XAVolumeItfAdapt_ThreadEntry");
    if((!adaptCtx ) ||( adaptCtx->ctxId != XAMediaPlayerAdaptation &&
                        adaptCtx->ctxId != XAMediaRecorderAdaptation &&
                        adaptCtx->ctxId != XAOutputMixAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_ThreadEntry");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;

    }

    if ( adaptCtx->ctxId == XAMediaPlayerAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaPlayer);
    }
    else if( adaptCtx->ctxId == XAMediaRecorderAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);
    }
    else if (adaptCtx->ctxId == XAOutputMixAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_ENTRY(XATSOutputMix);
    }

    DEBUG_API("<-XAVolumeItfAdapt_ThreadEntry");
    return XA_RESULT_SUCCESS;
}
/*
 * XAresult XAVolumeItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx)
 * Description: Safety thread exit
 */
XAresult XAVolumeItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx)
{
    DEBUG_API("->XAVolumeItfAdapt_ThreadExit");
    if((!adaptCtx ) ||( adaptCtx->ctxId != XAMediaPlayerAdaptation &&
                        adaptCtx->ctxId != XAMediaRecorderAdaptation &&
                        adaptCtx->ctxId != XAOutputMixAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_ThreadExit");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;

    }

    if ( adaptCtx->ctxId == XAMediaPlayerAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaPlayer);
    }
    else if( adaptCtx->ctxId == XAMediaRecorderAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    }
    else if (adaptCtx->ctxId == XAOutputMixAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
    }

    DEBUG_API("<-XAVolumeItfAdapt_ThreadExit");
    return XA_RESULT_SUCCESS;
}
