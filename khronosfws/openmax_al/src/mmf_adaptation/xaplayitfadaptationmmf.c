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

#include "unistd.h"
#include "xamediaplayeradaptctxmmf.h"
#include "xaplayitfadaptationmmf.h"
#include "xaadaptationmmf.h"
#include "cmmfbackendengine.h"
#include <glib.h>

extern XAboolean cameraRealized;

/*forward declaration of position updater callback*/
gboolean XAPlayItfAdapt_PositionUpdate(gpointer ctx);

/*
 * XAresult XAPlayItfAdapt_SetPlayState(XAAdaptationBaseCtx *bCtx, XAuint32 state)
 * Sets play state to GStreamer.
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAuint32 state - Play state to be set
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptMMF_SetPlayState(XAAdaptationBaseMMFCtx *bCtx, XAuint32 state)
{
    XAresult ret = XA_RESULT_SUCCESS;
    
    XAMediaPlayerAdaptationMMFCtx* mCtx = NULL;


    
    DEBUG_API_A1("->XAPlayItfAdapt_SetPlayState %s",PLAYSTATENAME(state));

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;
    switch ( state )
    {
        case XA_PLAYSTATE_STOPPED:
        {
            mmf_stop_playback(mCtx->mmfContext);
            mmf_close(mCtx->mmfContext);
            break;
        }
        case XA_PLAYSTATE_PAUSED:
            mmf_pause_playback(mCtx->mmfContext);
            break;
        case XA_PLAYSTATE_PLAYING:
        {
            mmf_resume_playback(mCtx->mmfContext);
            break;
        }
        default:
            ret = XA_RESULT_PARAMETER_INVALID;
            break;
    }

   DEBUG_API("<-XAPlayItfAdapt_SetPlayState");
    return ret;
}

/*
 * XAresult XAPlayItfAdapt_GetDuration(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec)
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAmillisecond *pMsec - Pointer where to store duration of stream.
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptMMF_GetDuration(XAAdaptationBaseMMFCtx *bCtx, XAmillisecond *pMsec)
{
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAPlayItfAdapt_GetDuration");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }


    DEBUG_API("<-XAPlayItfAdapt_GetDuration");
    return ret;
}

/*
 * XAresult XAPlayItfAdapt_GetPosition(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec)
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID value
 * XAmillisecond *pMsec - Pointer where to store current position in stream.
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptMMF_GetPosition(XAAdaptationBaseMMFCtx *bCtx, XAmillisecond *pMsec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    
    DEBUG_API("->XAPlayItfAdapt_GetPosition");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    
    DEBUG_API("<-XAPlayItfAdapt_GetPosition");
    return ret;
}

/*
 * XAresult XAPlayItfAdapt_EnablePositionTracking
 * Enable/disable periodic position tracking callbacks
 */
XAresult XAPlayItfAdaptMMF_EnablePositionTracking(XAAdaptationBaseMMFCtx *bCtx, XAboolean enable)
{
    DEBUG_API_A1("->XAPlayItfAdapt_EnablePositionTracking (enable: %d)", (int)enable);
    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XAPlayItfAdapt_EnablePositionTracking");
    return XA_RESULT_SUCCESS;
}

