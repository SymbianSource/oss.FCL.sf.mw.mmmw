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
#include "unistd.h"
#include <pthread.h>
#include "xamediaplayeradaptctx.h"
#include "xaplayitfadaptation.h"
#include "xaadaptationgst.h"
#include "xacameraadaptctx.h"

extern XAboolean cameraRealized;
extern XACameraAdaptationCtx_* cameraCtx;

/*forward declaration of position updater callback*/
gboolean XAPlayItfAdapt_PositionUpdate(gpointer ctx);

/*
 * XAresult XAPlayItfAdaptGST_SetPlayState(XAAdaptationGstCtx *bCtx, XAuint32 state)
 * Sets play state to GStreamer.
 * @param XAAdaptationGstCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAuint32 state - Play state to be set
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptGST_SetPlayState(XAAdaptationBaseCtx *ctx, XAuint32 state)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAboolean requestStateChange = XA_BOOLEAN_FALSE;
    GstStateChangeReturn gstRet = GST_STATE_CHANGE_SUCCESS;
    XAMediaPlayerAdaptationCtx* mCtx = NULL;
    XAuint32 locType = 0;
    GstState gstOrigState = GST_STATE_PLAYING;
    XADataLocator_Address *address = NULL;
    XAboolean playing = XA_BOOLEAN_FALSE;
    XAAdaptationGstCtx *bCtx = (XAAdaptationGstCtx *)ctx;

    DEBUG_API_A1("->XAPlayItfAdaptGST_SetPlayState %s",PLAYSTATENAME(state));

    
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    switch ( state )
    {
        case XA_PLAYSTATE_STOPPED:
        {
           	if ( cameraCtx && cameraRealized && mCtx->isobjsrc && mCtx->source  )
			{
				cameraCtx->playing = XA_BOOLEAN_FALSE;
				if(!cameraCtx->recording && !cameraCtx->snapshotting)
				{
					/* Future improvement: We could block MPObjSrc pad from tee-eleement here, when
					 * tee-element supports sending stream to one pad when others are blocked */

					/* Neither view finder or recorder is running -> pause camera */
					if ( GST_STATE( GST_ELEMENT(mCtx->source)) == GST_STATE_PLAYING )
					{
						GstStateChangeReturn gret;
						DEBUG_INFO("Stop camera source");
						gret = gst_element_set_state( GST_ELEMENT(mCtx->source), GST_STATE_PAUSED );
						if(gret == GST_STATE_CHANGE_SUCCESS)
						    gret = gst_element_get_state( GST_ELEMENT(mCtx->source), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC );
					}
				}
			}

            gstOrigState = GST_STATE(bCtx->bin);
            if ( gstOrigState != GST_STATE_READY )
            {
                DEBUG_INFO("Set gst-bin to GST_STATE_READY");
                bCtx->binWantedState = GST_STATE_READY;
                XAAdaptationGst_PrepareAsyncWait(bCtx);
                gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), GST_STATE_READY);
                switch ( gstRet )
                {
                    case GST_STATE_CHANGE_FAILURE:
                        DEBUG_ERR_A1("FAILED to change state (target %s)",
                                      gst_element_state_get_name(bCtx->binWantedState));
                        bCtx->binWantedState = GST_STATE(bCtx->bin);
                        ret = XA_RESULT_INTERNAL_ERROR;
                        break;
                    case GST_STATE_CHANGE_ASYNC:
                        DEBUG_INFO_A1("Change state will happen asyncronously (target %s)",
                                       gst_element_state_get_name(bCtx->binWantedState));
                        XAAdaptationGst_StartAsyncWait(bCtx);
                        ret = XA_RESULT_SUCCESS;
                        break;
                    case GST_STATE_CHANGE_SUCCESS:
                        DEBUG_INFO_A1("Successfully changed state (target %s)",
                                       gst_element_state_get_name(bCtx->binWantedState));
                        break;
                    default:
                        DEBUG_ERR_A1("Unhandled error (%d)",gstRet);
                        break;
                }
                bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
                DEBUG_INFO_A1("Setted gst-bin to state %s", gst_element_state_get_name(GST_STATE(bCtx->bin)));

                DEBUG_INFO_A1("Restoring gst-bin state to state %s", gst_element_state_get_name(gstOrigState));
                bCtx->binWantedState = gstOrigState;
                XAAdaptationGst_PrepareAsyncWait(bCtx);
                gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), gstOrigState);
                switch ( gstRet )
                {
                    case GST_STATE_CHANGE_FAILURE:
                        DEBUG_ERR_A1("FAILED to change state (target %s)",
                                      gst_element_state_get_name(bCtx->binWantedState));
                        bCtx->binWantedState = GST_STATE(bCtx->bin);
                        ret = XA_RESULT_INTERNAL_ERROR;
                        break;
                    case GST_STATE_CHANGE_ASYNC:
                        DEBUG_INFO_A1("Change state will happen asyncronously (target %s)",
                                       gst_element_state_get_name(bCtx->binWantedState));
                        XAAdaptationGst_StartAsyncWait(bCtx);
                        ret = XA_RESULT_SUCCESS;
                        break;
                    case GST_STATE_CHANGE_SUCCESS:
                        DEBUG_INFO_A1("Successfully changed state (target %s)",
                                       gst_element_state_get_name(bCtx->binWantedState));
                        break;
                    default:
                        DEBUG_ERR_A1("Unhandled error (%d)",gstRet);
                        break;
                }
                bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
                gstOrigState = GST_STATE(bCtx->bin);
                DEBUG_INFO_A1("Restored gst-bin to state %s", gst_element_state_get_name(gstOrigState));
            }


/*        	if( bCtx->pipeSrcThrCtx.dataHandle )
        	{
        		XAresult retVal = XA_RESULT_SUCCESS;
				if ( bCtx->pipeSrcThrCtx.state != CPStateNull )
				{
					bCtx->pipeSrcThrCtx.state = CPStateStopped;
				}

				retVal = XAImpl_PostSemaphore( bCtx->pipeSrcThrCtx.stateSem );
				if ( retVal != XA_RESULT_SUCCESS )
				{
					DEBUG_ERR("Could not post content pipe semaphore!");
				}

        	}*/
            /* stop head and drive head to beginning */
            bCtx->binWantedState = GST_STATE_PAUSED;
            if(mCtx->runpositiontimer > 0)
            {
                g_source_remove(mCtx->runpositiontimer);
                mCtx->runpositiontimer=0;
            }

            gst_element_send_event(bCtx->bin,gst_event_new_flush_start());
            gst_element_send_event(bCtx->bin,gst_event_new_flush_stop());

            locType = *((XAuint32*)( mCtx->xaSource->pLocator ));
            if( locType == XA_DATALOCATOR_ADDRESS )
            {
                address = (XADataLocator_Address*)(mCtx->xaSource->pLocator);

                /* init gst buffer from datalocator */
                if( mCtx->source )
                {
                    GstBuffer* userBuf = NULL;

                    /* init GST buffer from XADataLocator*/
                    userBuf = gst_buffer_new();
                    if( userBuf )
                    {
                        userBuf->size = address->length;
                        userBuf->data = address->pAddress;
                        /* push the whole buffer to appsrc so it is ready for preroll */
                        DEBUG_INFO("Pushing buffer");
                        gst_app_src_push_buffer( GST_APP_SRC(mCtx->source), userBuf );
                        DEBUG_INFO_A1("Sent buffer at 0x%x to appsrc", userBuf );
                        gst_app_src_end_of_stream( GST_APP_SRC(mCtx->source) );
                    }
                    else
                    {
                        DEBUG_ERR("Failure allocating buffer!");
                    }
                }
            }
            break;
        }
        case XA_PLAYSTATE_PAUSED:

           	if ( cameraCtx && cameraRealized && mCtx->isobjsrc && mCtx->source  )
			{
				cameraCtx->playing = XA_BOOLEAN_FALSE;

				/* Future improvement: We could block MPObjSrc pad from tee-eleement here, when
				 * tee-element supports sending stream to one pad when others are blocked */

				if(!cameraCtx->recording && !cameraCtx->snapshotting)
				{
					/* Neither view finder or recorder is running -> pause camera */
					if ( GST_STATE( GST_ELEMENT(mCtx->source)) == GST_STATE_PLAYING )
					{
						GstStateChangeReturn gret;
						DEBUG_INFO("Stop camera source");
						gret = gst_element_set_state( GST_ELEMENT(mCtx->source), GST_STATE_PAUSED );
						if(gret == GST_STATE_CHANGE_SUCCESS)
						    gret = gst_element_get_state( GST_ELEMENT(mCtx->source), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC );
					}
				}
			}

/*            if ( bCtx->pipeSrcThrCtx.state != CPStateNull )
            {
                bCtx->pipeSrcThrCtx.state = CPStatePaused;
            }*/
            bCtx->binWantedState = GST_STATE_PAUSED;
            if(mCtx->runpositiontimer > 0)
            {
                g_source_remove(mCtx->runpositiontimer);
                mCtx->runpositiontimer=0;
            }
            break;
        case XA_PLAYSTATE_PLAYING:
        {
        	if ( cameraCtx && mCtx->isobjsrc )
        	{
        		cameraCtx->playing = XA_BOOLEAN_TRUE;
        	}

        	if ( mCtx->videoppBScrbin )
        	{
        		gst_element_set_state( GST_ELEMENT(mCtx->videoppBScrbin), GST_STATE_PAUSED);
        	}
        	if  (mCtx->isobjsrc && !mCtx->cameraSinkSynced && cameraCtx )
        	{ /* create videosink now */
				mCtx->cameraSinkSynced = XA_BOOLEAN_TRUE;
				if ( mCtx->videosink )
				{
					gst_element_unlink( mCtx->filter,mCtx->videosink );
					gst_element_set_state( GST_ELEMENT(mCtx->videosink), GST_STATE_NULL);
					gst_bin_remove( GST_BIN(mCtx->baseObj.bin), mCtx->videosink);
					mCtx->videosink = XAAdaptationGst_CreateGstSink( mCtx->xaVideoSink, "videosink", &(mCtx->isobjvsink) );
					gst_bin_add(GST_BIN(mCtx->baseObj.bin), mCtx->videosink);
					if (! gst_element_link(mCtx->filter, mCtx->videosink))
			               {
			                    DEBUG_ERR("Could not link Filter to videosink!!");
			                    return XA_RESULT_INTERNAL_ERROR;
			               }
				}
        	}

/*            if ( bCtx->pipeSrcThrCtx.state != CPStateNull )
            {
                XAresult retVal = XA_RESULT_SUCCESS;

                if ( bCtx->pipeSrcThrCtx.state == CPStateInitialized )
                {  Start thread if it's not running 
                    retVal = XAImpl_StartThread( &(bCtx->pipeSrcThr), NULL, &XAAdaptationGst_ContentPipeScrThrFunc, &(bCtx->pipeSrcThrCtx) );
                    if ( retVal != XA_RESULT_SUCCESS )
                    {
                        DEBUG_ERR("Could not start content pipe thread!");
                    }
                }
                if ( bCtx->pipeSrcThrCtx.state == CPStatePaused ||
                     bCtx->pipeSrcThrCtx.state == CPStateInitialized ||
                     bCtx->pipeSrcThrCtx.state == CPStateStarted )
                retVal = XAImpl_PostSemaphore( bCtx->pipeSrcThrCtx.stateSem );
                if ( retVal != XA_RESULT_SUCCESS )
                {
                    DEBUG_ERR("Could not post content pipe semaphore!");
                }
            }*/

            bCtx->binWantedState = GST_STATE_PLAYING;
            if(mCtx->playrate!=1 && !mCtx->isobjsrc)
            { /*set seek element for ff, rew and slow*/
                XAAdaptationGst_PrepareAsyncWait(bCtx);
                DEBUG_INFO_A1("Apply new playrate %f.", mCtx->playrate);
                if(!gst_element_seek( bCtx->bin, mCtx->playrate, GST_FORMAT_TIME,
                                    (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE),
                                    GST_SEEK_TYPE_NONE, 0,
                                    GST_SEEK_TYPE_NONE, -1))
                {
                    DEBUG_ERR("WARN: gst reports seek not handled");
                }
                /* flushed seeks always asynchronous */
                XAAdaptationGst_StartAsyncWait(bCtx);
                DEBUG_INFO("New playrate handled.");
                bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
            }

            playing = XA_BOOLEAN_TRUE;
            break;
        }
        default:
            ret = XA_RESULT_PARAMETER_INVALID;
            break;
    }
    /* launch Gstreamer state change only if necessary */
    if( GST_STATE_TARGET(bCtx->bin) == bCtx->binWantedState )
    {
        DEBUG_INFO("Gst already in or transitioning to wanted state");
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
        XAAdaptationGst_PrepareAsyncWait(bCtx);
        DEBUG_INFO_A1("Sending change state request to state %d", bCtx->binWantedState);
        gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
        switch ( gstRet )
        {
            case GST_STATE_CHANGE_FAILURE:
                DEBUG_ERR_A1("FAILED to change state (target %s)",
                              gst_element_state_get_name(bCtx->binWantedState));
                bCtx->binWantedState = GST_STATE(bCtx->bin);
                ret = XA_RESULT_INTERNAL_ERROR;
                break;
            case GST_STATE_CHANGE_ASYNC:
                DEBUG_INFO_A1("Change state will happen asyncronously (target %s)",
                               gst_element_state_get_name(bCtx->binWantedState));
                XAAdaptationGst_StartAsyncWait(bCtx);
                ret = XA_RESULT_SUCCESS;
                break;
            case GST_STATE_CHANGE_NO_PREROLL:
                DEBUG_INFO("GST_STATE_CHANGE_NO_PREROLL");
                /* deliberate fall-through */
            case GST_STATE_CHANGE_SUCCESS:
                DEBUG_INFO_A1("Successfully changed state (target %s)",
                               gst_element_state_get_name(bCtx->binWantedState));
                ret = XA_RESULT_SUCCESS;
                break;
            default:
                DEBUG_ERR_A1("Unhandled error (%d)",gstRet);
                ret = XA_RESULT_UNKNOWN_ERROR;
                break;
        }
        bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
    }

    if (playing && mCtx->isobjsrc && cameraCtx )
    {
    	GstPad* moSrc=NULL ;

    	playing = XA_BOOLEAN_FALSE;
    	moSrc = gst_element_get_static_pad( mCtx->source, "MPObjSrc");
		if( moSrc && gst_pad_is_linked(moSrc) )
		{
			DEBUG_INFO_A2("unblock element:%s pad:%s",
					gst_element_get_name( mCtx->source),
					gst_pad_get_name(moSrc));
			gst_pad_set_blocked_async(moSrc, FALSE, XAAdaptationGst_PadBlockCb, NULL);
		}

		if ( GST_STATE( GST_ELEMENT(mCtx->source)) != GST_STATE_PLAYING )
		{
			GstStateChangeReturn gret;
			DEBUG_INFO("Start camera source");
			gret = gst_element_set_state( GST_ELEMENT(mCtx->source), GST_STATE_PLAYING );
			if(gret == GST_STATE_CHANGE_SUCCESS)
			    gret = gst_element_get_state( GST_ELEMENT(mCtx->source), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC );
		}
    }
    DEBUG_API("<-XAPlayItfAdaptGST_SetPlayState");
    return ret;
}

/*
 * XAresult XAPlayItfAdaptGST_GetDuration(XAAdaptationGstCtx *bCtx, XAmillisecond *pMsec)
 * @param XAAdaptationGstCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAmillisecond *pMsec - Pointer where to store duration of stream.
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptGST_GetDuration(XAAdaptationGstCtx *bCtx, XAmillisecond *pMsec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    GstFormat format = GST_FORMAT_TIME;
    gint64 duration;
    DEBUG_API("->XAPlayItfAdaptGST_GetDuration");

    if(!bCtx || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation || !pMsec)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( gst_element_query_duration( GST_ELEMENT(bCtx->bin), &format, &duration ) )
    {
        DEBUG_INFO_A1("Duration: %"GST_TIME_FORMAT, GST_TIME_ARGS(duration));
        ret = XA_RESULT_SUCCESS;
        *pMsec = GST_TIME_AS_MSECONDS(duration);/*Warning ok due to used API specification*/
    }
    else
    {
        DEBUG_ERR("WARNING: Gst: could not get duration");
        *pMsec = XA_TIME_UNKNOWN;
        ret = XA_RESULT_SUCCESS;
    }

    DEBUG_API("<-XAPlayItfAdaptGST_GetDuration");
    return ret;
}

/*
 * XAresult XAPlayItfAdaptGST_GetPosition(XAAdaptationGstCtx *bCtx, XAmillisecond *pMsec)
 * @param XAAdaptationGstCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID value
 * XAmillisecond *pMsec - Pointer where to store current position in stream.
 * @return XAresult ret - Success value
 */
XAresult XAPlayItfAdaptGST_GetPosition(XAAdaptationGstCtx *bCtx, XAmillisecond *pMsec)
{
    XAresult ret = XA_RESULT_SUCCESS;
    gint64 position;
    GstFormat format = GST_FORMAT_TIME;
    DEBUG_API("->XAPlayItfAdapGSTt_GetPosition");

    if(!bCtx || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( gst_element_query_position( GST_ELEMENT(bCtx->bin), &format, &position  ) )
    {
        DEBUG_INFO_A1("Current position %"GST_TIME_FORMAT, GST_TIME_ARGS(position));
        ret = XA_RESULT_SUCCESS;
        *pMsec = GST_TIME_AS_MSECONDS(position);/*Warning ok due to used API specification*/
    }
    else
    {
        DEBUG_ERR("WARNING: Gst: could not get position");
        /* probably not fully prerolled - safe assumption for position = 0 */
        *pMsec = 0;
        ret = XA_RESULT_SUCCESS;
    }

    DEBUG_API("<-XAPlayItfAdaptGST_GetPosition");
    return ret;
}

/*
 * XAresult XAPlayItfAdapt_EnablePositionTracking
 * Enable/disable periodic position tracking callbacks
 */
XAresult XAPlayItfAdapt_EnablePositionTracking(XAAdaptationGstCtx *bCtx, XAboolean enable)
{
    XAMediaPlayerAdaptationCtx* mCtx;

    DEBUG_API_A1("->XAPlayItfAdapt_EnablePositionTracking (enable: %d)", (int)enable);
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
    if(enable && !(mCtx->trackpositionenabled))
    {
        mCtx->trackpositionenabled = XA_BOOLEAN_TRUE;
        XAMediaPlayerAdapt_UpdatePositionCbTimer(mCtx);
    }
    else if (!enable && (mCtx->trackpositionenabled))
    {
        mCtx->trackpositionenabled = XA_BOOLEAN_FALSE;
        XAMediaPlayerAdapt_UpdatePositionCbTimer(mCtx);
    }

    DEBUG_API("<-XAPlayItfAdapt_EnablePositionTracking");
    return XA_RESULT_SUCCESS;
}

