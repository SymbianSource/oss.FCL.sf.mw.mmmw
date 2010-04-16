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

#include <pthread.h>
#include <gst.h>
#include "XAMediaRecorderAdaptCtx.h"
#include "XARecordItfAdaptation.h"
#include "XAAdaptation.h"

/*forward declaration of position updater callback*/
gboolean XARecordItfAdapt_PositionUpdate(gpointer ctx);

extern XAboolean cameraRealized;
extern XACameraAdaptationCtx_* cameraCtx;

/*
 * XAresult XARecordItfAdapt_SetRecordState(XAAdaptationBaseCtx *ctx, XAuint32 state)
 * Sets record state to GStreamer.
 * @param XAAdaptationBaseCtx *ctx - Adaptation context
 * XAuint32 state - Record state to be set
 * @return XAresult ret - Success value
 */
XAresult XARecordItfAdapt_SetRecordState(XAAdaptationBaseCtx *bCtx, XAuint32 state)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAboolean closeSink = XA_BOOLEAN_FALSE;
    XAboolean requestStateChange = XA_BOOLEAN_FALSE;
    GstStateChangeReturn gstRet = GST_STATE_CHANGE_SUCCESS;
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XAboolean recording = XA_BOOLEAN_FALSE;
    DEBUG_API_A1("->XARecordItfAdapt_SetRecordState %s",RECORDSTATENAME(state));
    if(!bCtx || bCtx->ctxId != XAMediaRecorderAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARecordItfAdapt_SetRecordState");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;

    mCtx->isRecord = XA_BOOLEAN_TRUE;

    switch ( state )
    {
        case XA_RECORDSTATE_STOPPED:
        {
        	if ( cameraCtx && cameraRealized && mCtx->isobjvsrc && mCtx->videosource  )
			{
				cameraCtx->recording = XA_BOOLEAN_FALSE;

	            if(!cameraCtx->playing && !cameraCtx->snapshotting)
	            {
	            	/* Neither view finder or recorder is running -> pause camera */
	            	if ( GST_STATE( GST_ELEMENT(mCtx->videosource)) == GST_STATE_PLAYING )
	            	{
	            		GstStateChangeReturn gret;
	        			DEBUG_INFO("Stop camera source");
	        			gret = gst_element_set_state( GST_ELEMENT(mCtx->videosource), GST_STATE_PAUSED );
	        			gret = gst_element_get_state( GST_ELEMENT(mCtx->videosource), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC );
	            	}
	            }
			}

            if ( bCtx->pipeSinkThrCtx.state != CPStateNull )
            {
                bCtx->pipeSinkThrCtx.state = CPStateStopped;
            }
            bCtx->binWantedState = GST_STATE_PAUSED;
            closeSink = XA_BOOLEAN_TRUE;
            if(mCtx->runpositiontimer > 0)
            {
                g_source_remove(mCtx->runpositiontimer);
                mCtx->runpositiontimer=0;
            }

            if ( mCtx->recThrCtx.bufInsufficientSem )
            {
                DEBUG_INFO("No buffer-insufficient received, posting record thr semaphore.");
                if ( XAImpl_PostSemaphore( mCtx->recThrCtx.bufInsufficientSem ) != XA_RESULT_SUCCESS)
                {
                    DEBUG_ERR("Posting buffer-insufficient semaphore FAILED!");
                }
            }

            break;
        }
        case XA_RECORDSTATE_PAUSED:
        {
        	if ( cameraCtx && cameraRealized && mCtx->isobjvsrc && mCtx->videosource  )
			{
				cameraCtx->recording = XA_BOOLEAN_FALSE;
				if(!cameraCtx->playing && !cameraCtx->snapshotting)
				{
					/* Neither view finder or recorder is running -> pause camera */
					if ( GST_STATE( GST_ELEMENT(mCtx->videosource)) == GST_STATE_PLAYING )
					{
						GstStateChangeReturn gret;
						DEBUG_INFO("Stop camera source");
						gret = gst_element_set_state( GST_ELEMENT(mCtx->videosource), GST_STATE_PAUSED );
						gret = gst_element_get_state( GST_ELEMENT(mCtx->videosource), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC );
					}
				}
			}

            if ( bCtx->pipeSinkThrCtx.state != CPStateNull )
            {
                bCtx->pipeSinkThrCtx.state = CPStatePaused;
            }
            if(mCtx->xaRecordState==XA_RECORDSTATE_STOPPED && mCtx->encodingchanged)
            {
                XAMediaRecorderAdapt_ChangeEncoders( mCtx );
                mCtx->encodingchanged = XA_BOOLEAN_FALSE;
            }
            bCtx->binWantedState = GST_STATE_PAUSED;
            if(mCtx->runpositiontimer > 0)
            {
                g_source_remove(mCtx->runpositiontimer);
                mCtx->runpositiontimer=0;
            }
            break;
        }
        case XA_RECORDSTATE_RECORDING:
        {
        	if ( cameraCtx && mCtx->isobjvsrc )
			{
				cameraCtx->recording = XA_BOOLEAN_TRUE;
			}

        	if( bCtx->pipeSinkThrCtx.pipe )
        	{
            if ( bCtx->pipeSinkThrCtx.state != CPStateNull )
            {
                XAresult retVal = XA_RESULT_SUCCESS;

                if ( bCtx->pipeSinkThrCtx.state == CPStateInitialized )
                { /* Start thread if it's not running */
                    retVal = XAImpl_StartThread( &(bCtx->pipeSinkThr), NULL, &XAAdaptationBase_ContentPipeSinkThrFunc, &(bCtx->pipeSinkThrCtx) );
                    if ( retVal != XA_RESULT_SUCCESS )
                    {
                        DEBUG_ERR("Could not start content pipe thread!");
                    }
                }
                if ( bCtx->pipeSinkThrCtx.state == CPStatePaused ||
                     bCtx->pipeSinkThrCtx.state == CPStateInitialized ||
                     bCtx->pipeSinkThrCtx.state == CPStateStarted )
                retVal = XAImpl_PostSemaphore( bCtx->pipeSinkThrCtx.stateSem );
                if ( retVal != XA_RESULT_SUCCESS )
                {
                    DEBUG_ERR("Could not post content pipe semaphore!");
                }
            }
        	}
            if(mCtx->xaRecordState==XA_RECORDSTATE_STOPPED && (mCtx->encodingchanged))
            {
                XAMediaRecorderAdapt_ChangeEncoders( mCtx );
                mCtx->encodingchanged = XA_BOOLEAN_FALSE;
            }
            /*Causes segmentation fault during contentpipe, so we must ignore
            contentpipe at this point*/
            if( ! bCtx->pipeSinkThrCtx.pipe )
            {
            	XAMetadataAdapt_TryWriteTags(bCtx, GST_BIN(mCtx->codecbin));
            }
            if ( mCtx->recThrCtx.bufInsufficientSem )
            {
                /* Recording to address and recording thread semaphora is created */
                XAImpl_StartThread(&(mCtx->recordingEventThr),NULL, &XAMediaRecorderAdapt_RecordEventThr, (void*)mCtx );
            }

            bCtx->binWantedState = GST_STATE_PLAYING;
            recording = XA_BOOLEAN_TRUE;
            break;
        }
        default:
            DEBUG_ERR("Unhandled state");
            ret = XA_RESULT_PARAMETER_INVALID;
            break;
    }

    if( ret == XA_RESULT_SUCCESS )
    {
        mCtx->xaRecordState = state;
    }

    /* launch Gstreamer state change only if necessary */
    if( GST_STATE_TARGET(bCtx->bin) == bCtx->binWantedState )
    {
        DEBUG_INFO("Gst already transitioning to wanted state!!");
        requestStateChange = XA_BOOLEAN_FALSE;
    }
    else
    {
        if( (GST_STATE(bCtx->bin) == bCtx->binWantedState) &&
            (GST_STATE_PENDING(bCtx->bin) == GST_STATE_VOID_PENDING) )
        {
            DEBUG_ERR_A3("WARNING : gststate %d == wanted %d != gsttarget %d and no statechange pending",
                          GST_STATE(bCtx->bin), bCtx->binWantedState, GST_STATE_TARGET(bCtx->bin));
        }
        requestStateChange = XA_BOOLEAN_TRUE;
    }

    if( requestStateChange )
    {
        XAAdaptationBase_PrepareAsyncWait(bCtx);
        DEBUG_INFO_A1("Sending change state request to state %d", bCtx->binWantedState);
        gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
        switch ( gstRet )
        {
            case GST_STATE_CHANGE_FAILURE:
                DEBUG_ERR_A1("FAILED to change state (target %d)",bCtx->binWantedState);
                bCtx->binWantedState = GST_STATE(bCtx->bin);
                ret = XA_RESULT_INTERNAL_ERROR;
                break;
            case GST_STATE_CHANGE_ASYNC:
                DEBUG_INFO_A1("Change state will happen asyncronously (target %d)",bCtx->binWantedState);
                XAAdaptationBase_StartAsyncWait(bCtx);
                ret = XA_RESULT_SUCCESS;
                break;
            case GST_STATE_CHANGE_NO_PREROLL:
                DEBUG_INFO("GST_STATE_CHANGE_NO_PREROLL");
                /* deliberate fall-through */
            case GST_STATE_CHANGE_SUCCESS:
                DEBUG_INFO_A1("Successfully changed state (target %d)",bCtx->binWantedState);
                ret = XA_RESULT_SUCCESS;
                break;
            default:
                DEBUG_ERR_A1("Unhandled error (%d)",gstRet);
                ret = XA_RESULT_UNKNOWN_ERROR;
                break;
        }
        bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
    }
    if( (GST_STATE(bCtx->bin) > GST_STATE_READY) && closeSink )
    {   /* close the sink*/
        gst_element_send_event(bCtx->bin,gst_event_new_flush_start());
        gst_element_send_event(bCtx->bin,gst_event_new_flush_stop());
    }

    if ( recording && mCtx->isobjvsrc && mCtx->videosource )
    {
    	GstPad *pad = gst_element_get_static_pad( GST_ELEMENT(mCtx->videosource), "MRObjSrc");
		if( pad && gst_pad_is_linked(pad) )
		{
			DEBUG_INFO_A2("unblock element:%s pad:%s",
					gst_element_get_name(mCtx->videosource),
					gst_pad_get_name(pad));
			gst_pad_set_blocked_async(pad, FALSE, XAAdaptationBase_PadBlockCb, NULL);
		}

		if ( GST_STATE( GST_ELEMENT(mCtx->videosource)) != GST_STATE_PLAYING )
		{
			GstStateChangeReturn gret = GST_STATE_CHANGE_SUCCESS;
			DEBUG_INFO("Start camera source");
			gret = gst_element_set_state( GST_ELEMENT(mCtx->videosource), GST_STATE_PLAYING );
			gret = gst_element_get_state( GST_ELEMENT(mCtx->videosource), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC );
		}
    }
    DEBUG_API("<-XARecordItfAdapt_SetRecordState");
    return ret;
}

/*
 * XAresult XARecordItfAdapt_GetRecordState(XAAdaptationBaseCtx *bCtx, XAuint32 *state)
 * Description: Return record state
 */
XAresult XARecordItfAdapt_GetRecordState(XAAdaptationBaseCtx *bCtx, XAuint32 *state)
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XARecordItfAdapt_GetRecordState");

    if(!bCtx || bCtx->ctxId != XAMediaRecorderAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARecordItfAdapt_GetRecordState");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;

    *state = mCtx->xaRecordState;

    DEBUG_API("<-XARecordItfAdapt_GetRecordState");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XARecordItfAdapt_GetPosition(XAAdaptationBaseCtx *ctx, AdaptationContextIDS ctxIDs, XAmillisecond *pMsec)
 * @param XAAdaptationBaseCtx *ctx - Adaptation context, this will be casted to correct type regarding to contextID value given as 2nd parameter
 * XAmillisecond *pMsec - Pointer where to store current position in stream.
 * @return XAresult ret - Success value
 */
XAresult XARecordItfAdapt_GetPosition(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    gint64 position=0;
    GstFormat format = GST_FORMAT_TIME;
    DEBUG_API("->XARecordItfAdapt_GetPosition");

    if(!bCtx || bCtx->ctxId != XAMediaRecorderAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARecordItfAdapt_GetPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    if ( gst_element_query_position( GST_ELEMENT(bCtx->bin), &format, &position  ) )
    {
        ret = XA_RESULT_SUCCESS;
        *pMsec = GST_TIME_AS_MSECONDS(position); /*Warning ok due to used API specification*/
        DEBUG_INFO_A1("Gst: Position in microseconds : %u", *pMsec );
    }
    else
    {
        DEBUG_ERR("WARNING: Gst: could not get position");
        /* probably not fully prerolled - safe assumption for position = 0 */
        *pMsec = 0;
        ret = XA_RESULT_SUCCESS;
    }

    DEBUG_API("<-XARecordItfAdapt_GetPosition");
    return ret;
}


/*
 * gboolean XARecordItfAdapt_PositionUpdate(gpointer ctx)
 * callback.
 * If position tracking enabled, periodic timer calls this method every XA_ADAPT_PU_INTERVAL msecs
 * @return false to stop periodic calls
 */
gboolean XARecordItfAdapt_PositionUpdate(gpointer ctx)
{
    XAAdaptationBaseCtx *bCtx = (XAAdaptationBaseCtx*) ctx;
    XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*) ctx;
    DEBUG_API("->XARecordItfAdapt_PositionUpdate");

    if( mCtx && mCtx->trackpositionenabled )
    {
        GstFormat format = GST_FORMAT_TIME;
        gint64 position = 0;
        if ( gst_element_query_position( GST_ELEMENT(bCtx->bin), &format, &position ) )
        {
            XAuint32 posMsec = GST_TIME_AS_MSECONDS(position);/*Warning ok due to used API specification*/
            XAAdaptEvent event = {XA_RECORDITFEVENTS, XA_ADAPT_POSITION_UPDATE_EVT, 1, NULL};
            event.data = &posMsec;
            DEBUG_API_A1("XARecordItfAdapt_PositionUpdate: pos %lu ms", posMsec);
            /* send needed events */
            XAAdaptationBase_SendAdaptEvents(bCtx, &event );
        }
        else
        {
            DEBUG_ERR("Gst: Failed to get position");
        }
    }
    DEBUG_API_A1("<-XARecordItfAdapt_PositionUpdate: %d", mCtx->runpositiontimer);
    /* return false to stop timer */
    return( mCtx->runpositiontimer );
}

/*
 * XAresult XARecordItfAdapt_EnablePositionTracking
 * Enable/disable periodic position tracking callbacks
 */
XAresult XARecordItfAdapt_EnablePositionTracking(XAAdaptationBaseCtx *bCtx, XAboolean enable)
{
    XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    DEBUG_API_A1("->XARecordItfAdapt_EnablePositionTracking (enable: %lu)", enable);

    if(!bCtx || bCtx->ctxId != XAMediaRecorderAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARecordItfAdapt_EnablePositionTracking");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }


    /* create a timer to track position of playback */
    if(enable && !(mCtx->trackpositionenabled))
    {
        mCtx->trackpositionenabled = XA_BOOLEAN_TRUE;
        mCtx->positionCb = &XARecordItfAdapt_PositionUpdate;
        /* if recording is already on, create a timer to track position of recording */
        if( GST_STATE(bCtx->bin) == GST_STATE_PLAYING )
        {
            mCtx->runpositiontimer = g_timeout_add(XA_ADAPT_PU_INTERVAL, mCtx->positionCb, mCtx);
        }
    }
    else if (!enable && (mCtx->trackpositionenabled))
    {
        mCtx->trackpositionenabled = XA_BOOLEAN_FALSE;
        if(mCtx->runpositiontimer > 0)
        {
            g_source_remove(mCtx->runpositiontimer);
            mCtx->runpositiontimer=0;
        }
    }

    DEBUG_API("<-XARecordItfAdapt_EnablePositionTracking");
    return XA_RESULT_SUCCESS;
}

