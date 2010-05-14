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

#include <gst/gst.h>
#include <gst/interfaces/mixer.h>
#include "xaadaptationgst.h"
#include "xavolumeitfadaptation.h"
#include "xamediaplayeradaptctx.h"

#include "xaoutputmixadaptctx.h"
#include "xamediarecorderadaptctx.h"

/*
 * XAresult XAVolumeItfAdapt_FlushBin(XAAdaptationGstCtx *ctx)
 */
XAresult XAVolumeItfAdapt_FlushBin(XAAdaptationGstCtx *ctx)
{
    DEBUG_API("->XAVolumeItfAdapt_FlushBin");
    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    if (GST_STATE(ctx->bin) != GST_STATE_PLAYING)
    {
        gint64 position = 0;
        GstFormat format = GST_FORMAT_TIME;

        if (! gst_element_query_position( GST_ELEMENT(ctx->bin), &format, &position  ))
            {
                DEBUG_ERR("Gst: Failed to get position");
				return XA_RESULT_INTERNAL_ERROR;
            }
        XAAdaptationGst_PrepareAsyncWait(ctx);
        /* in effect seeks to current position and flushing the buffer, due to
         * gstreamer implementation actual position might change, if stopped, seek to beginning
         */
        if(!gst_element_seek( ctx->bin, ((XAMediaPlayerAdaptationCtx*)ctx)->playrate, GST_FORMAT_TIME,
                            (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE),
                            GST_SEEK_TYPE_SET, position,
                            GST_SEEK_TYPE_NONE, (gint64)GST_CLOCK_TIME_NONE))
        {
            DEBUG_ERR("WARN: gst reports seek not handled");
        }
        /* flushed seeks always asynchronous */
        XAAdaptationGst_StartAsyncWait(ctx);
        DEBUG_INFO("Bin flushed.");
        ctx->waitingasyncop = XA_BOOLEAN_FALSE;
    }
    DEBUG_API("<-XAVolumeItfAdapt_FlushBin");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAVolumeItfAdapt_SetVolumeLevel(void *ctx, AdaptationContextIDS ctx->baseObj.ctxId,
 *                                          XAmillibel level)
 * @param void *ctx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAmillibel level - Requested volume level, in between XA_MILLIBEL_MIN and XA_MILLIBEL_MAX
 * @return XAresult ret - Success value
 */
XAresult XAVolumeItfAdapt_SetVolumeLevel(XAAdaptationGstCtx *ctx, XAmillibel level)
{
    GstElement *vol=NULL, *audiopp=NULL;
    gdouble gstVolume = 0;
    XAmillibel tempVolumeLevel = 0;
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API_A1("->XAVolumeItfAdapt_SetVolumeLevel (level %d)", level);
    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation &&
                 ctx->baseObj.ctxId != XAMediaRecorderAdaptation &&
                 ctx->baseObj.ctxId != XAOutputMixAdaptation) )
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

    if ( ctx->baseObj.ctxId == XAMediaPlayerAdaptation )
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
    else if ( ctx->baseObj.ctxId == XAMediaRecorderAdaptation )
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
    else if (ctx->baseObj.ctxId == XAOutputMixAdaptation )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        guint iterator = 0;
        for ( iterator = 0; iterator < context->connectedObjects->len; iterator++ )
        {
            GstBin* basebin = NULL;
            XAAdaptationGstCtx* bCtx = g_array_index(context->connectedObjects,XAOMixAdaptConnObj,iterator).ctx;
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
 * XAresult XAVolumeItfAdapt_GetMaxVolumeLevel(void *ctx, AdaptationContextIDS ctx->baseObj.ctxId,
                                            XAmillibel *pMaxLevel)
 * @param void *ctx - Adaptation context, this will be casted to correct type regarding to contextID value given as 2nd parameter
 * @param AdaptationContextIDS ctx->baseObj.ctxId - Type specifier for context, this will be used to cast ctx pointer to correct type.
 * XAmillibel *pMaxLevel - Maximum volume level
 * @return XAresult ret - Success value
 */
XAresult XAVolumeItfAdapt_GetMaxVolumeLevel(XAAdaptationGstCtx *ctx, XAmillibel *pMaxLevel)
{
    DEBUG_API("->XAVolumeItfAdapt_GetMaxVolumeLevel");

    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation &&
                 ctx->baseObj.ctxId != XAMediaRecorderAdaptation &&
                 ctx->baseObj.ctxId != XAOutputMixAdaptation) )
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
 * XAresult XAVolumeItfAdapt_SetMute(void *ctx, AdaptationContextIDS ctx->baseObj.ctxId, XAboolean mute)
 * @param void *ctx - Adaptation context, this will be casted to correct type regarding to contextID value given as 2nd parameter
 * @param AdaptationContextIDS ctx->baseObj.ctxId - Type specifier for context, this will be used to cast ctx pointer to correct type.
 * @param XAboolean mute - status of mute value
 * @return XAresult ret - Success value
 */
XAresult XAVolumeItfAdapt_SetMute(XAAdaptationGstCtx *ctx, XAboolean mute)
{
    GstElement *vol=NULL, *audiopp=NULL;
    gboolean gmute = 0;

    DEBUG_API("->XAVolumeItfAdapt_SetMute");
    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation &&
                 ctx->baseObj.ctxId != XAMediaRecorderAdaptation &&
                 ctx->baseObj.ctxId != XAOutputMixAdaptation ) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_SetMute");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( ctx->baseObj.ctxId == XAMediaPlayerAdaptation )
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
    else if ( ctx->baseObj.ctxId == XAMediaRecorderAdaptation )
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
    else if ( ctx->baseObj.ctxId == XAOutputMixAdaptation )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        guint iterator = 0;
        context->mute = mute;
        for ( iterator = 0; iterator < context->connectedObjects->len; iterator++ )
        {
            GstBin* basebin = NULL;
            XAAdaptationGstCtx* bCtx = g_array_index(context->connectedObjects,XAOMixAdaptConnObj,iterator).ctx;
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
 * XAresult XAVolumeItfAdapt_EnableStereoPosition(XAAdaptationGstCtx *ctx, XAboolean enable)
 * @param XAAdaptationGstCtx *ctx - Adaptation context
 * @param XAboolean enable - Enable Stereo Position
 * @return XAresult - Success value
 */
XAresult XAVolumeItfAdapt_EnableStereoPosition(XAAdaptationGstCtx *ctx, XAboolean enable)
{
    DEBUG_API("->XAVolumeItfAdapt_EnableStereoPosition");

    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation &&
                 ctx->baseObj.ctxId != XAMediaRecorderAdaptation &&
                 ctx->baseObj.ctxId != XAOutputMixAdaptation ) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_EnableStereoPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( ctx->baseObj.ctxId == XAMediaPlayerAdaptation  )
    {
        XAMediaPlayerAdaptationCtx* context = (XAMediaPlayerAdaptationCtx*) ctx;
        context->isStereoPosition = enable;
    }
    else if ( ctx->baseObj.ctxId == XAMediaRecorderAdaptation  )
    {
        XAMediaRecorderAdaptationCtx* context = (XAMediaRecorderAdaptationCtx*) ctx;
        context->isStereoPosition = enable;
    }
    else if ( ctx->baseObj.ctxId == XAOutputMixAdaptation  )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        context->isStereoPosition = enable;
    }

    DEBUG_API("<-XAVolumeItfAdapt_EnableStereoPosition");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAVolumeItfAdapt_SetStereoPosition(XAAdaptationGstCtx *ctx, XApermille stereoPosition)
 * @param XAAdaptationGstCtx *ctx - Adaptation context
 * @param XApermille stereoPosition - Stereo Position to be set
 * @return XAresult - Success value
 */
XAresult XAVolumeItfAdapt_SetStereoPosition(XAAdaptationGstCtx *ctx,
                                            XApermille stereoPosition)
{
    GstElement *audiopp = NULL, *pan = NULL;
    gfloat gstPosition = 0.0;

    DEBUG_API("->XAVolumeItfAdapt_SetStereoPosition");
    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation &&
                 ctx->baseObj.ctxId != XAMediaRecorderAdaptation &&
                 ctx->baseObj.ctxId != XAOutputMixAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVolumeItfAdapt_SetStereoPosition");
        return XA_RESULT_PARAMETER_INVALID;
    }

    gstPosition = ( (gfloat)stereoPosition / STEREO_POSITION_RATIO );

    if ( ctx->baseObj.ctxId == XAMediaPlayerAdaptation  )
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
    else if ( ctx->baseObj.ctxId == XAMediaRecorderAdaptation  )
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

    else if ( ctx->baseObj.ctxId == XAOutputMixAdaptation )
    {
        XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) ctx;
        guint iterator = 0;
        for ( iterator = 0; iterator < context->connectedObjects->len; iterator++ )
        {
            GstBin* basebin = NULL;
            XAAdaptationGstCtx* bCtx = g_array_index(context->connectedObjects,XAOMixAdaptConnObj,iterator).ctx;
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


