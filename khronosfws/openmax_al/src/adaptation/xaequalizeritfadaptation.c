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
#include "XAAdaptation.h"
#include "XAMediaPlayerAdaptCtx.h"
#include "XAMediaRecorderAdaptCtx.h"
#include "XAOutputMixAdaptCtx.h"
#include "XAEqualizerItfAdaptation.h"
#include "XAStaticCapsAdaptation.h"

static const XAmilliHertz centerFrequencies[EQUALIZER_NUM_OF_BANDS] = {
        29000, 59000, 119000, 227000, 474000,
        947000, 1889000, 3770000, 7523000, 15011000 };

static const char* band_names[EQUALIZER_NUM_OF_BANDS] = {
        "band0", "band1", "band2", "band3", "band4",
        "band5", "band6", "band7", "band8", "band9" };

static const XAmilliHertz bandFreqRangeMin = 0;
static const XAmilliHertz bandFreqRangeMax = 0;
static const XAmillibel bandLevelRangeMin = -2400;
static const XAmillibel bandLevelRangeMax = 1200;


/*
 * XAresult XAEqualizerItfAdapt_GetBandLevelRange(XAAdaptationBaseCtx *bCtx,
 *                                                XAmillibel *pMin,
 *                                                XAmillibel *pMax)
 */
XAresult XAEqualizerItfAdapt_GetBandLevelRange(XAAdaptationBaseCtx *bCtx,
                                               XAmillibel *pMin,
                                               XAmillibel *pMax)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAEqualizerItfAdapt_GetBandLevelRange");

    if( (!pMin && !pMax) ) /* other may be NULL */
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if (pMin)
    {
        *pMin = bandLevelRangeMin;
    }
    if (pMax)
    {
        *pMax = bandLevelRangeMax;
    }

    DEBUG_API("<-XAEqualizerItfAdapt_GetBandLevelRange");
    return ret;
}

/*
 * XAresult XAEqualizerItfAdapt_SetBandLevel(XAAdaptationBaseCtx *bCtx,
 *                                           XAuint16 band,
 *                                           XAmillibel level)
 */
XAresult XAEqualizerItfAdapt_SetBandLevel(XAAdaptationBaseCtx *bCtx,
                                          XAuint16 band,
                                          XAmillibel level)
{
    XAresult ret = XA_RESULT_SUCCESS;
    GstElement *equ=NULL, *audiopp=NULL;
    DEBUG_API("->XAEqualizerItfAdapt_SetBandLevel");

    if(!bCtx ||
        band >= EQUALIZER_NUM_OF_BANDS  ||
        level < bandLevelRangeMin ||
        level > bandLevelRangeMax)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( bCtx->ctxId == XAMediaPlayerAdaptation || bCtx->ctxId == XAMediaRecorderAdaptation )
        {
            audiopp = gst_bin_get_by_name( GST_BIN(bCtx->bin), "audiopp" );
            equ = gst_bin_get_by_name( GST_BIN(audiopp), "pp_equ" );
            if(equ)
            {
                g_object_set( G_OBJECT(equ), band_names[band], (gdouble)(level/1000), NULL );
            }
            else
            {
                DEBUG_ERR("Media object equalizer element not found!!");
            }
        }
        else if( bCtx->ctxId == XAOutputMixAdaptation )
        {
    	    XAOutputMixAdaptationCtx* context = (XAOutputMixAdaptationCtx*) bCtx;
    	    guint iterator;
    	    for ( iterator = 0; iterator < context->connectedObjects->len; iterator++ )
            {
                GstBin* basebin = GST_BIN(g_array_index(context->connectedObjects,XAOMixAdaptConnObj,iterator).ctx->bin);
                equ=NULL;
                audiopp=NULL;
                audiopp = gst_bin_get_by_name( basebin, "audiopp" );
                if(audiopp)
                {
                    equ = gst_bin_get_by_name( GST_BIN(audiopp), "pp_equ" );
                }
                if(equ)
                {
                    g_object_set( G_OBJECT(equ), band_names[band], (gdouble)(level/1000), NULL );
                }
                else
                {
                    DEBUG_ERR_A1("Could not find equalizer for player %u!!", iterator);
                }
            }
        }
        else
        {
            DEBUG_ERR_A1("Not supported adaptation element: %d", bCtx->ctxId);
            return XA_RESULT_PARAMETER_INVALID;
        }
    }
    if(equ)
    {
        gst_object_unref(equ);
    }
	if(audiopp)
    {
        gst_object_unref(audiopp);
    }
	
    DEBUG_API("<-XAEqualizerItfAdapt_SetBandLevel");
    return ret;
}

/*
 * XAresult XAEqualizerItfAdapt_GetCenterFreq(XAAdaptationBaseCtx *bCtx,
 *                                            XAuint16 band,
 *                                            XAmilliHertz *pCenter)
 */

XAresult XAEqualizerItfAdapt_GetCenterFreq(XAAdaptationBaseCtx *bCtx,
                                           XAuint16 band,
                                           XAmilliHertz *pCenter)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAEqualizerItfAdapt_GetCenterFreq");

    if(!pCenter || band >= EQUALIZER_NUM_OF_BANDS)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pCenter = centerFrequencies[band];

    DEBUG_API("<-XAEqualizerItfAdapt_GetCenterFreq");
    return ret;
}

/*
 * XAresult XAEqualizerItfAdapt_GetBandFreqRange(XAAdaptationBaseCtx *bCtx,
 *                                               XAuint16 band,
 *                                               XAmilliHertz *pMin,
 *                                               XAmilliHerts *pMax)
 */
XAresult XAEqualizerItfAdapt_GetBandFreqRange(XAAdaptationBaseCtx *bCtx,
                                              XAuint16 band,
                                              XAmilliHertz *pMin,
                                              XAmilliHertz *pMax)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAEqualizerItfAdapt_GetBandFreqRange");

    /* pMin or pMax may be NULL */
    if((!pMin && !pMax)  || band >= EQUALIZER_NUM_OF_BANDS)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* in this implementation there are no ranges */
    if (pMin)
    {
        *pMin = centerFrequencies[band];
    }
    if (pMax)
    {
        *pMax = centerFrequencies[band];
    }

    DEBUG_API("<-XAEqualizerItfAdapt_GetBandFreqRange");
    return ret;
}

/*
 * XAresult XAEqualizerItfAdapt_GetBand(XAAdaptationBaseCtx *bCtx,
 *                                      XAmilliHertz frequency,
 *                                      XAuint16 *pBand)
 */
XAresult XAEqualizerItfAdapt_GetBand(XAAdaptationBaseCtx *bCtx,
                                     XAmilliHertz frequency,
                                     XAuint16 *pBand)
{
    XAuint16 index=0;
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAEqualizerItfAdapt_GetBand");

    if(!pBand)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pBand = XA_EQUALIZER_UNDEFINED;

    /* as there are no ranges, it must match exactly */
    for (index = 0; index < EQUALIZER_NUM_OF_BANDS; index++)
    {
        if (frequency == centerFrequencies[index])
        {
            *pBand = index;
            break;
        }
    }

    DEBUG_API("<-XAEqualizerItfAdapt_GetBand");
    return ret;
}

/*
 * XAresult XAEqualizerItfAdapt_GetDefaultBandLevel(XAAdaptationBaseCtx *bCtx,
 *                                           XAuint16 band,
 *                                           XAmillibel *pLevel)
 */
XAresult XAEqualizerItfAdapt_GetDefaultBandLevel(XAAdaptationBaseCtx *bCtx,
                                                 XAuint16 band,
                                                 XAmillibel *pLevel)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAEqualizerItfAdapt_GetDefaultBandLevel");

    if(!pLevel)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    pLevel = EQUALIZER_DEFAULT_BAND_LEVEL;

    DEBUG_API("<-XAEqualizerItfAdapt_GetDefaultBandLevel");
    return ret;
}
/*
 * XAresult XAEqualizerItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adapCtx)
 * Description: Safety thread entry
 */
XAresult XAEqualizerItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adaptCtx)
{
    DEBUG_API("->XAEqualizerItfAdapt_ThreadEntry");
    if((!adaptCtx ) ||( adaptCtx->ctxId != XAMediaPlayerAdaptation &&
                        adaptCtx->ctxId != XAMediaRecorderAdaptation &&
                        adaptCtx->ctxId != XAOutputMixAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfAdapt_ThreadEntry");
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

    DEBUG_API("<-XAEqualizerItfAdapt_ThreadEntry");
    return XA_RESULT_SUCCESS;
}
/*
 * XAresult XAEqualizerItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx)
 * Description: Safety thread exit
 */
XAresult XAEqualizerItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx)
{
    DEBUG_API("->XAEqualizerItfAdapt_ThreadExit");
    if((!adaptCtx ) ||( adaptCtx->ctxId != XAMediaPlayerAdaptation &&
                        adaptCtx->ctxId != XAMediaRecorderAdaptation &&
                        adaptCtx->ctxId != XAOutputMixAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfAdapt_ThreadExit");
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


    DEBUG_API("<-XAEqualizerItfAdapt_ThreadExit");
    return XA_RESULT_SUCCESS;
}
