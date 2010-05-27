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
#include "xaadaptationgst.h"
#include "xamediaplayeradaptctx.h"
#include "xaseekitfadaptation.h"

/*
 * XAresult XASeekItfAdapt_SetPosition(XAAdaptationGstCtx *bCtx, XAmillisecond pos, XAuint32 seekMode)
 * @param XAAdaptationGstCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID value
 * XAmillisecond pos - Position to be set in Milliseconds
 * XAuint32 seekMode - Preferred seek mode
 * @return XAresult ret - Success value
 */
XAresult XASeekItfAdapt_SetPosition(XAAdaptationGstCtx *bCtx, XAmillisecond pos, XAuint32 seekMode)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    gint64 gstSeekPos = pos * GST_MSECOND;
    XAmillisecond newPosMs=0;
    GstFormat format = GST_FORMAT_TIME;
    GstSeekFlags seekFlags = GST_SEEK_FLAG_FLUSH;
    DEBUG_API_A2("->XASeekItfAdapt_SetPosition (pos %ld ms, mode=%d)", pos, (int)seekMode);
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASeekItfAdapt_SetPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( GST_STATE(bCtx->bin) < GST_STATE_PAUSED )
    {   /* This should not happen */
        DEBUG_ERR("Gst not prerolled - can not seek!!");
        DEBUG_API("<-XASeekItfAdapt_SetPosition");
        return XA_RESULT_INTERNAL_ERROR;
    }

    if ( seekMode == XA_SEEKMODE_FAST )
    {
        seekFlags |= GST_SEEK_FLAG_KEY_UNIT;
    }
    else
    {
        seekFlags |= GST_SEEK_FLAG_ACCURATE;
    }

    XAAdaptationGst_PrepareAsyncWait(bCtx);
    DEBUG_INFO("Send gst_element_seek");
    mCtx->lastpos = gstSeekPos;
    if ( !gst_element_seek( bCtx->bin, mCtx->playrate, GST_FORMAT_TIME,  seekFlags,
                            GST_SEEK_TYPE_SET, gstSeekPos,
                            GST_SEEK_TYPE_NONE, (gint64)GST_CLOCK_TIME_NONE ) )
    {
        DEBUG_ERR("WARN: gst reports seek not handled");
    }
    /* flushed seeks always asynchronous */
    XAAdaptationGst_StartAsyncWait(bCtx);
    DEBUG_INFO("Seek handled.")
    bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
    /* update new position to playitf */
    {
        XAAdaptEvent event = {XA_PLAYITFEVENTS, XA_ADAPT_POSITION_UPDATE_EVT, 1, NULL};
        if ( !gst_element_query_position( GST_ELEMENT(bCtx->bin), &format, &mCtx->lastpos  ) )
        {
            DEBUG_ERR("WARNING: could not get position from GST")
        }
        DEBUG_INFO_A1("Current position %"GST_TIME_FORMAT, GST_TIME_ARGS(mCtx->lastpos));
        newPosMs = GST_TIME_AS_MSECONDS(mCtx->lastpos);
        event.data = &newPosMs;
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }

    ret = XA_RESULT_SUCCESS;

    DEBUG_API("<-XASeekItfAdapt_SetPosition");
    return ret;
}

/*
 * XAresult XASeekItfAdapt_SetLoop(void *bCtx, AdaptationContextIDS ctxIDs, XAboolean loopEnable, XAmillisecond startPos,
 *                                 XAmillisecond endPos)
 * @param XAAdaptationGstCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID value
 * XAboolean loopEnable - Enable/Disable looping
 * XAmillisecond startPos - Loop start position in milliseconds
 * XAmillisecond endPos - Loop end position in milliseconds
 * @return XAresult ret - Success value
 */
XAresult XASeekItfAdapt_SetLoop(XAAdaptationGstCtx *bCtx, XAboolean loopEnable, XAmillisecond startPos,
                                XAmillisecond endPos)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    GstFormat format = GST_FORMAT_TIME;

    DEBUG_API_A3("->XASeekItfAdapt_SetLoop (enable=%d, start=%ld, stop=%ld)", (int)loopEnable, startPos, endPos);
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASeekItfAdapt_SetLoop");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( loopEnable )
    {
        if(endPos==XA_TIME_UNKNOWN)
        {
            mCtx->loopend = (gint64)GST_CLOCK_TIME_NONE;
        }
        else
        {
            mCtx->loopend = (endPos*GST_MSECOND);
            DEBUG_INFO_A1("Set looping to %ld", mCtx->loopend);
        }
        DEBUG_INFO_A2("Enable looping from %"GST_TIME_FORMAT" to %"GST_TIME_FORMAT,
                      GST_TIME_ARGS(mCtx->loopstart), GST_TIME_ARGS(mCtx->loopend));
        mCtx->loopstart = (startPos*GST_MSECOND);
        if ( !gst_element_query_position( GST_ELEMENT(bCtx->bin), &format, &(mCtx->lastpos) ) )
        {
            DEBUG_ERR("WARNING: could not get position from GST")
        }
        DEBUG_INFO_A1("current head position %"GST_TIME_FORMAT"",GST_TIME_ARGS(mCtx->lastpos));
        mCtx->loopingenabled = XA_BOOLEAN_TRUE;
        XAMediaPlayerAdapt_UpdatePositionCbTimer(mCtx);
    }
    else
    {
        DEBUG_INFO("Disable looping");
        mCtx->loopstart = 0;
        mCtx->loopend = (gint64)GST_CLOCK_TIME_NONE;
        mCtx->loopingenabled = XA_BOOLEAN_FALSE;
        XAMediaPlayerAdapt_UpdatePositionCbTimer(mCtx);
    }

    DEBUG_API("<-XASeekItfAdapt_SetLoop");
    return ret;
}
