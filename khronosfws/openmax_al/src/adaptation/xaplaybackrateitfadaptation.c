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
#include "XAPlaybackRateItfAdaptation.h"
#include "XAPlayItfAdaptation.h"

/*XA_RATEPROP_STAGGEREDVIDEO,XA_RATEPROP_SMOOTHVIDEO,XA_RATEPROP_SILENTAUDIO,
  XA_RATEPROP_STAGGEREDAUDIO,XA_RATEPROP_NOPITCHCORAUDIO,XA_RATEPROP_PITCHCORAUDIO*/
#define SUPPORTEDPROPS (XA_RATEPROP_SMOOTHVIDEO | XA_RATEPROP_SILENTAUDIO)
/* NOTE: currently rewind rates do not seem to work */
#define MINRATE (100)
#define MAXRATE 8000
/*
 * XAresult XAPlaybackRateItfAdapt_SetRate(XAAdaptationBaseCtx *bCtx, XApermille rate)
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID value
 * @param XApermille rate - new playback rate in permilles of original speed
 * @return XAresult ret - Success value
 */
XAresult XAPlaybackRateItfAdapt_SetRate(XAAdaptationBaseCtx *bCtx, XApermille rate)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    DEBUG_API_A1("->XAPlaybackRateItfAdapt_SetRate %d permilles", rate);

    if( !bCtx || bCtx->ctxId != XAMediaPlayerAdaptation )
    {
        DEBUG_ERR("Invalid context!");
        return XA_RESULT_PARAMETER_INVALID;
    }
    if( rate<MINRATE || rate>MAXRATE )
    {
        DEBUG_ERR("Invalid rate!");
        return XA_RESULT_PARAMETER_INVALID;
    }
    if(rate != 0)
    {
        mCtx->playrate = ((gdouble)rate)/1000;
        if( GST_STATE(bCtx->bin) < GST_STATE_PAUSED )
        {   /* This should not happen */
            DEBUG_ERR("WARNING: Gst not prerolled yet!");
        }
        else
        {
            /* apply immediately */
            XAAdaptationBase_PrepareAsyncWait(bCtx);
            DEBUG_INFO_A1("Apply new playrate %f.", mCtx->playrate);
            if(!gst_element_seek( bCtx->bin, mCtx->playrate, GST_FORMAT_TIME,
                                GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                                GST_SEEK_TYPE_NONE, 0,
                                GST_SEEK_TYPE_NONE, -1))
            {
                DEBUG_ERR("WARN: gst reports seek not handled");
            }
            /* flushed seeks always asynchronous */
            XAAdaptationBase_StartAsyncWait(bCtx);
            DEBUG_INFO("New playrate handled.")
            bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
        }
    }
    else
    {  /* Gst do not accept rate of 0, pause instead */
        mCtx->playrate = 1.0;
        if( GST_STATE(bCtx->bin) > GST_STATE_PAUSED )
        {
            DEBUG_ERR("Pause playback");
            XAPlayItfAdapt_SetPlayState(bCtx, XA_PLAYSTATE_PAUSED);
        }

    }

    DEBUG_API("<-XAPlaybackRateItfAdapt_SetRate");
    return ret;
}

/*
 * XAresult XAPlaybackRateItfAdapt_SetPropertyConstraints(XAAdaptationBaseCtx *bCtx,
 *                                                       XAuint32 constraints)
 */
XAresult XAPlaybackRateItfAdapt_SetPropertyConstraints(XAAdaptationBaseCtx *bCtx,
                                                       XAuint32 constraints)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XAPlaybackRateItfAdapt_SetPropertyConstraints");

    if( !bCtx || bCtx->ctxId != XAMediaPlayerAdaptation )
    {
        DEBUG_ERR("Invalid context!");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    if( (constraints & SUPPORTEDPROPS ) == 0 )
    {
        DEBUG_ERR("constraints cannot be satisfied!!");
        ret = XA_RESULT_FEATURE_UNSUPPORTED;
    }
    else
    {
        mCtx->rateprops = SUPPORTEDPROPS & constraints;
    }
    DEBUG_API("<-XAPlaybackRateItfAdapt_SetPropertyConstraints");
    return ret;
}

/*
 * XAresult XAPlaybackRateItfAdapt_GetProperties(XAAdaptationBaseCtx *bCtx,
 *                                              XAuint32 *pProperties)
 */
XAresult XAPlaybackRateItfAdapt_GetProperties(XAAdaptationBaseCtx *bCtx,
                                               XAuint32 *pProperties)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XAPlaybackRateItfAdapt_GetProperties");

    if( !bCtx || bCtx->ctxId != XAMediaPlayerAdaptation )
    {
        DEBUG_ERR("Invalid context!");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    *pProperties = mCtx->rateprops;

    DEBUG_API("<-XAPlaybackRateItfAdapt_GetProperties");
    return ret;
}


XAresult XAPlaybackRateItfAdapt_GetCapabilitiesOfRate(XAAdaptationBaseCtx *bCtx,
                                                       XApermille rate,
                                                       XAuint32 *pCapabilities)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XAPlaybackRateItfAdapt_GetCapabilitiesOfRate");
    if( !bCtx || bCtx->ctxId != XAMediaPlayerAdaptation )
    {
        DEBUG_ERR("Invalid context!");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    if( rate<MINRATE || rate>MAXRATE )
    {
        DEBUG_ERR("Invalid rate!");
        *pCapabilities = 0;
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        *pCapabilities = SUPPORTEDPROPS;
    }

    DEBUG_API("<-XAPlaybackRateItfAdapt_GetCapabilitiesOfRate");
    return ret;
}


XAresult XAPlaybackRateItfAdapt_GetRateRange(XAAdaptationBaseCtx *bCtx,
                                             XAuint8 index,
                                             XApermille *pMinRate,
                                             XApermille *pMaxRate,
                                             XApermille *pStepSize,
                                             XAuint32 *pCapabilities)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XAPlaybackRateItfAdapt_GetRateRange");

    if( !bCtx || bCtx->ctxId != XAMediaPlayerAdaptation )
    {
        DEBUG_ERR("Invalid context!");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    /* NOTE: hardcoded values, cannot be queried from gst */
    /* only one range supported */
    if( index>0 )
    {
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        *pMinRate = MINRATE;
        *pMaxRate = MAXRATE;
        *pStepSize = 0; /* continuous range */
        *pCapabilities = SUPPORTEDPROPS;
    }

    DEBUG_API("<-XAPlaybackRateItfAdapt_GetRateRange");
    return ret;
}

