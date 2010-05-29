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
#include "cmmfradiobackendengine.h"
#include <glib.h>

extern XAboolean cameraRealized;

/*forward declaration of position updater callback*/
gboolean XAPlayItfAdapt_PositionUpdate(gpointer ctx);

/*
 * XAresult XAPlayItfAdaptMMF_SetPlayState(XAAdaptationBaseCtx *bCtx, XAuint32 state)
 * Sets play state to GStreamer.
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAuint32 state - Play state to be set
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptMMF_SetPlayState(XAAdaptationBaseCtx *bCtx, XAuint32 state)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API_A1("->XAPlayItfAdaptMMF_SetPlayState %s",PLAYSTATENAME(state));

    /* bCtx and parameter pointer validation happens in the calling function.
     * We don't need to repeat it here*/
    switch ( state )
    {
        case XA_PLAYSTATE_STOPPED:
        {
        	if ( bCtx->ctxId == XARadioAdaptation )
    		{
    			mmf_set_player_adapt_context(cmmfradiobackendengine_init(), bCtx);	    			
    			stop_radio(cmmfradiobackendengine_init());
    		}
    		else
    		{
    			ret = mmf_playitf_stop_playback(mCtx->mmfContext);
          	}
          break;
        }
        case XA_PLAYSTATE_PAUSED:
            ret = mmf_playitf_pause_playback(mCtx->mmfContext);
            break;
        case XA_PLAYSTATE_PLAYING:
        {
   				if ( bCtx->ctxId == XARadioAdaptation )
    			{
    				mmf_set_player_adapt_context(cmmfradiobackendengine_init(), bCtx);	    				
    				play_radio(cmmfradiobackendengine_init());    	
    			} 
    			else
    			{ 
          			ret = mmf_playitf_resume_playback(mCtx->mmfContext);
        		}  
          break;
        }
        default:
            ret = XA_RESULT_PARAMETER_INVALID;
            break;
    }

    DEBUG_API("<-XAPlayItfAdaptMMF_SetPlayState");
    return ret;
}

/*
 * XAresult XAPlayItfAdaptMMF_GetPlayState(XAAdaptationBaseCtx *bCtx, XAuint32 *pState)
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAuint32 *state - XAmillisecond *pMsec - Pointer where to store play state
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptMMF_GetPlayState(XAAdaptationBaseCtx *bCtx, XAuint32 *pState)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_GetPlayState");

    /* If playhead reaches eof, state will transition to paused.
     * This object does not have visibility to callback*/
    ret = mmf_playitf_get_play_state(mCtx->mmfContext, pState);

    DEBUG_API("<-XAPlayItfAdaptMMF_GetPlayState");
    return ret;

    }

/*
 * XAresult XAPlayItfAdaptMMF_GetDuration(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec)
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAmillisecond *pMsec - Pointer where to store duration of stream.
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptMMF_GetDuration(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_GetDuration");

    ret = mmf_playitf_get_duration(mCtx->mmfContext, pMsec);

    DEBUG_API("<-XAPlayItfAdaptMMF_GetDuration");
    return ret;
}

/*
 * XAresult XAPlayItfAdaptMMF_GetPosition(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec)
 * @param XAAdaptationBaseCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID value
 * XAmillisecond *pMsec - Pointer where to store current position in stream.
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptMMF_GetPosition(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_GetPosition");

    ret = mmf_playitf_get_position(mCtx->mmfContext, pMsec);

    DEBUG_API("<-XAPlayItfAdaptMMF_GetPosition");
    return ret;
}

/*
 * XAresult XAPlayItfAdaptMMF_RegisterCallback(XAAdaptationBaseCtx *bCtx, xaPlayCallback callback)
 * Description: Sets the playback callback function.
 */
XAresult XAPlayItfAdaptMMF_RegisterCallback(XAAdaptationBaseCtx *bCtx, xaPlayCallback callback)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_RegisterCallback");

    ret = mmf_playitf_register_callback(mCtx->mmfContext, callback);

    DEBUG_API("<-XAPlayItfAdaptMMF_RegisterCallback");
    return ret;
}

/**
 * XAresult XAPlayItfAdaptMMF_SetCallbackEventsMask(XAAdaptationBaseCtx *bCtx, XAuint32 eventFlags)
 * Description: Enables/disables notification of playback events.
 **/
XAresult XAPlayItfAdaptMMF_SetCallbackEventsMask(XAAdaptationBaseCtx *bCtx, XAuint32 eventFlags)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_SetCallbackEventsMask");

    ret = mmf_playitf_set_callback_events_mask(mCtx->mmfContext, eventFlags);

    DEBUG_API("<-XAPlayItfAdaptMMF_SetCallbackEventsMask");
    return ret;
}

/**
 * XAresult XAPlayItfAdaptMMF_SetMarkerPosition(XAAdaptationBaseCtx *bCtx, XAmillisecond mSec)
 * Description: Sets marker position.
 **/
XAresult XAPlayItfAdaptMMF_SetMarkerPosition(XAAdaptationBaseCtx *bCtx, XAmillisecond mSec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_SetMarkerPosition");

    ret = mmf_playitf_set_marker_position(mCtx->mmfContext, mSec);

    DEBUG_API("<-XAPlayItfAdaptMMF_SetMarkerPosition");
    return ret;
}

/**
 * XAresult XAPlayItfAdaptMMF_SetCallbackEventsMask(XAAdaptationBaseCtx *bCtx, XAuint32 eventFlags)
 * Description: Clears marker position.
 **/
XAresult XAPlayItfAdaptMMF_ClearMarkerPosition(XAAdaptationBaseCtx *bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_ClearMarkerPosition");

    ret = mmf_playitf_clear_marker_position(mCtx->mmfContext);

    DEBUG_API("<-XAPlayItfAdaptMMF_ClearMarkerPosition");
    return ret;
}

/**
 * XAPlayItfAdaptMMF_SetPositionUpdatePeriod(XAAdaptationBaseCtx *bCtx, XAmillisecond mSec)
 * Description: Sets position update period.
 **/
XAresult XAPlayItfAdaptMMF_SetPositionUpdatePeriod(XAAdaptationBaseCtx *bCtx, XAmillisecond mSec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaPlayerAdaptationMMFCtx* mCtx = (XAMediaPlayerAdaptationMMFCtx*) bCtx;

    DEBUG_API("->XAPlayItfAdaptMMF_SetPositionUpdatePeriod");

    ret = mmf_playitf_set_position_update_period(mCtx->mmfContext, mSec);

    DEBUG_API("<-XAPlayItfAdaptMMF_SetPositionUpdatePeriod");
    return ret;
}
/*
 * XAresult XAPlayItfAdapt_StateChange
 * Handle callback from Radio utility 
 */
void XAPlayItfAdaptMMF_StateChange(XAAdaptationBaseCtx *bCtx, XAboolean playing)
{
	  if (playing)
	  {
   		XAAdaptEvent event = {XA_PLAYITFEVENTS, XA_PLAYEVENT_HEADMOVING,1,0}; 
   		XAAdaptationBase_SendAdaptEvents(bCtx, &event );  	   		
   	}
   	else
   	{
   		XAAdaptEvent event = {XA_PLAYITFEVENTS, XA_PLAYEVENT_HEADSTALLED,1,0};  
   		XAAdaptationBase_SendAdaptEvents(bCtx, &event );  	   		  		
   	}

}
