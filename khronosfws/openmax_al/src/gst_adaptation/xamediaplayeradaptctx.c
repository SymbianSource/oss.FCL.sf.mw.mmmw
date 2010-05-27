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

#include <assert.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include "xamediaplayeradaptctx.h"
#include "xamediaplayeradaptctxmmf.h"
#include "xaadaptationgst.h"
#include "xaobjectitf.h"
#include "xacameradevice.h"
#include "xaoutputmix.h"
#include "xametadataadaptation.h"
/*#include "xangavideosink.h"*/
#include "xacameraadaptctx.h"
#include "xaoutputmixadaptctx.h"

/* forward declarations */
XAresult XAMediaPlayerAdapt_CreatePipeline( XAMediaPlayerAdaptationCtx* ctx );

void* ngaVideoSinkPtr = NULL;


extern XAboolean cameraRealized;
extern XACameraAdaptationCtx_* cameraCtx;

/*
 * static void XAMediaPlayerAdapt_NewPadCb (GstElement *element, GstPad *pad,  gpointer data)
 * Listen to codec bin dynamic pads
 */
static void XAMediaPlayerAdapt_NewPadCb (GstElement *element, GstPad *pad,  gpointer data)
{
  XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*)data;
  gchar *name = gst_pad_get_name (pad);
  DEBUG_API_A1("->XAMediaPlayerAdapt_NewPadCb: A new pad %s was created", name);
  /*try first newly created pad to video pipe*/
  if( mCtx->videoppbin && !(gst_pad_is_linked(gst_element_get_static_pad(mCtx->videoppbin, "videopp_sink"))) )
  {
      if(gst_element_link_pads (mCtx->codecbin, name, mCtx->videoppbin, "videopp_sink"))
      {
          DEBUG_INFO_A1("Pads linked, codecbin:%s to videopp:sink", name);
          g_free (name);
          DEBUG_API("<-XAMediaPlayerAdapt_NewPadCb");
          return;
      }
  }
  /*..and then to audio pipe*/
  if( mCtx->audioppbin && !(gst_pad_is_linked(gst_element_get_static_pad(mCtx->audioppbin, "sink"))) )
  {
      if(gst_element_link_pads (mCtx->codecbin, name, mCtx->audioppbin, "sink"))
      {
          DEBUG_INFO_A1("Pads linked, codecbin:%s to audiopp:sink", name);
          g_free (name);
          DEBUG_API("<-XAMediaPlayerAdapt_NewPadCb");
          return;
      }
  }

  g_free (name);
  DEBUG_INFO("Warning: Could not find anything to link to new pad.");
  DEBUG_API("<-XAMediaPlayerAdapt_NewPadCb");
}

/*
 * void  push_data_for_prerolling (GstElement * pipeline, GstBuffer *buffer, XAMediaPlayerAdaptationCtx* ctx)
 * Called when "push-buffer" signal is emitted
 */
void  push_data_for_prerolling (GstElement * pipeline, GstBuffer *buffer, XAMediaPlayerAdaptationCtx* ctx)
{
	DEBUG_API("->push_data_for_prerolling");
	gst_app_src_push_buffer( GST_APP_SRC(ctx->source), GST_BUFFER(buffer) );
	/*GstPad* prerollPad = NULL;
	prerollPad = gst_element_get_static_pad(GST_ELEMENT(ctx->source),"src");
	gst_pad_push (prerollPad, buffer);
	gst_element_send_event(GST_ELEMENT(ctx->source),gst_event_new_flush_start());
	gst_element_send_event(GST_ELEMENT(ctx->source),gst_event_new_flush_stop());*/
	DEBUG_API("<-push_data_for_prerolling");
}


/*
 * gboolean XAMediaPlayerAdapt_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
 * MediaPlayer Gst-bus message handler (Callback)
 */
gboolean XAMediaPlayerAdapt_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
{
    XAAdaptationGstCtx* bCtx = (XAAdaptationGstCtx*)data;
    /* only listen to bin messages */
    if(GST_MESSAGE_SRC(message)==(GstObject*)(bCtx->bin))
    {
        XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*)data;
        DEBUG_API_A2("->XAMediaPlayerAdapt_GstBusCb:\"%s\" from object \"%s\"",
                        GST_MESSAGE_TYPE_NAME(message), GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));

        switch( GST_MESSAGE_TYPE(message))
        {
            case GST_MESSAGE_EOS:
            {
                if( mCtx && mCtx->loopingenabled && mCtx->loopend == GST_CLOCK_TIME_NONE)
                {
                    DEBUG_INFO_A2("Restart loop from %"GST_TIME_FORMAT" to %"GST_TIME_FORMAT,
                                  GST_TIME_ARGS(mCtx->loopstart), GST_TIME_ARGS(mCtx->loopend));
                    gst_element_seek( bCtx->bin, mCtx->playrate, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE),
                                      GST_SEEK_TYPE_SET, mCtx->loopstart,
                                      GST_SEEK_TYPE_NONE, (gint64)GST_CLOCK_TIME_NONE );
                    gst_element_get_state(bCtx->bin,NULL,NULL,XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
                    mCtx->lastpos = mCtx->loopstart;
                    if( mCtx && mCtx->trackpositionenabled )
                    {
                        XAmillisecond posMsec = GST_TIME_AS_MSECONDS(mCtx->lastpos);/*Warning ok due to used API specification*/
                        XAAdaptEvent event = {XA_PLAYITFEVENTS, XA_ADAPT_POSITION_UPDATE_EVT, 1, NULL};
                        event.data = &posMsec;
                        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
                    }
                }
                else
                {
                /* stop position tracking */
                if(mCtx->runpositiontimer > 0)
                {
                    g_source_remove(mCtx->runpositiontimer);
                    mCtx->runpositiontimer=0;
                }

                /* complete any ongoing client async operations */
                XAAdaptationGst_CompleteAsyncWait(bCtx);

                /* send needed events */
                {
					XAMediaType mediatype;
/*
					if( mCtx->baseObj.pipeSrcThrCtx.pipe )
					{
						XACommon_CheckDataSource((XADataSource*)mCtx->xaAudioSink, &mediatype);
					}
					else
*/
					{
						XACommon_CheckDataSource(mCtx->xaSource, &mediatype);
					}
					if(mediatype!=XA_MEDIATYPE_IMAGE)
					{
						XAAdaptEvent event = { XA_PLAYITFEVENTS, XA_PLAYEVENT_HEADATEND, 0, NULL };
						XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
					}
                }
                if(mCtx->positionCb)
                {
                    mCtx->positionCb(bCtx);
                }
                bCtx->binWantedState = GST_STATE_PAUSED;
                }
                break;
            }

            case GST_MESSAGE_STATE_CHANGED:
            {
                GstState oldstate, newstate, pendingstate, gsttargetstate;
                gst_message_parse_state_changed(message, &oldstate, &newstate, &pendingstate);
                gsttargetstate = GST_STATE_TARGET(bCtx->bin);
                DEBUG_INFO_A4("old %s -> new %s ( pending %s, gsttarget %s )",
                               gst_element_state_get_name(oldstate),
                               gst_element_state_get_name(newstate),
                               gst_element_state_get_name(pendingstate),
                               gst_element_state_get_name(gsttargetstate) );
                if(gsttargetstate!=bCtx->binWantedState)
                {
                    DEBUG_ERR_A1("WARNING: Gst target is not wanted target [%s]!!!",
                                    gst_element_state_get_name(bCtx->binWantedState));
                }
                /* print out some more info */
                if( pendingstate == GST_STATE_VOID_PENDING )
                {
                    if( newstate != bCtx->binWantedState )
                    {
                        DEBUG_INFO_A2("Gst in intermediate state transition (curr %s, target %s)",
                                        gst_element_state_get_name(newstate),
                                        gst_element_state_get_name(bCtx->binWantedState));
                       break; // TL: add to avoid extra event sent below in this case...                
                    }
                    else
                    {
                        DEBUG_INFO_A1("Gst in wanted target state (%s)",
                                        gst_element_state_get_name(newstate));
                    }
                }
                if( oldstate!=GST_STATE_PLAYING && newstate==GST_STATE_PLAYING )
                {
                    /* send needed events */
                    XAAdaptEvent event = {XA_PLAYITFEVENTS, XA_PLAYEVENT_HEADMOVING, 0, NULL };
                    XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
                    /* enable position tracking if needed */
                    XAMediaPlayerAdapt_UpdatePositionCbTimer(mCtx);
                }
                if ( oldstate== GST_STATE_READY && newstate==GST_STATE_PAUSED)
                {
/*                    if ( mCtx->baseObj.pipeSinkThrCtx.dataHandle )
                    {
                        mCtx->baseObj.pipeSrcThrCtx.state = CPStateInitialized;
                    }*/
                }
                break;
            }

            case GST_MESSAGE_ASYNC_DONE:
            {
                /* some async sequence ended */
                XAAdaptationGst_CompleteAsyncWait(bCtx);
                break;
            }

            case GST_MESSAGE_ERROR:
            {
                GError* error;
                gchar* debug;
                gst_message_parse_error(message, &error, &debug);
                DEBUG_ERR_A1("Gst reports error \"%s\"", debug);
                /* stop waiting any ongoing async operations */
                XAAdaptationGst_CompleteAsyncWait(bCtx);
                break;
            }
            case GST_MESSAGE_BUFFERING:
            {
                gint percent;
                gst_message_parse_buffering(message, &percent);
                DEBUG_INFO_A1("Gst message buffering %d", percent);
                mCtx->buffering = percent;
                {
                    XAAdaptEvent event = {XA_PREFETCHITFEVENTS, XA_ADAPT_BUFFERING, 1, NULL };
                    event.data = &mCtx->buffering;
                XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
                }
                break;
            }
            case GST_MESSAGE_ELEMENT:
            {
                DEBUG_INFO("GST_MESSAGE_ELEMENT");
                if ((gst_structure_has_name(message->structure, "graphics-surface-created")) || 
                    (gst_structure_has_name(message->structure, "graphics-surface-updated")))
                {
                     DEBUG_INFO("graphics-surface-created message recived");
                }
                break;
            }
            default:
                break;
        }
    }
    else //if (GST_MESSAGE_SRC(message)==(GstObject*)(bCtx->videosink))
    {
        switch( GST_MESSAGE_TYPE(message))
        {
            case GST_MESSAGE_ELEMENT:
            {
                DEBUG_INFO("GST_MESSAGE_ELEMENT");
                if ((gst_structure_has_name(message->structure, "graphics-surface-created")) || 
                    (gst_structure_has_name(message->structure, "graphics-surface-updated")))
                {
                     guint32 surfaceid0, surfaceid1, surfaceid2, surfaceid3;
                     gint crop_rect_tl_x, crop_rect_tl_y, crop_rect_br_x, crop_rect_br_y;
                     gint aspect_ratio_num, aspect_ratio_denom;
                     
                     GstObject *sink= GST_MESSAGE_SRC(message);
     
                     g_object_get(sink, "surfaceid0", &surfaceid0, NULL);
                     g_object_get(sink, "surfaceid1", &surfaceid1,NULL);
                     g_object_get(sink, "surfaceid2", &surfaceid2,NULL);
                     g_object_get(sink, "surfaceid3", &surfaceid3,NULL);
                     g_object_get(sink, "croprect_tl_x", &crop_rect_tl_x, NULL);
                     g_object_get(sink, "croprect_tl_y", &crop_rect_tl_y, NULL);
                     g_object_get(sink, "croprect_br_x", &crop_rect_br_x, NULL);
                     g_object_get(sink, "croprect_br_y", &crop_rect_br_y, NULL);                
                     g_object_get(sink, "aspectratio_num", &aspect_ratio_num, NULL);
                     g_object_get(sink, "aspectratio_denom", &aspect_ratio_denom, NULL);                     
/*
                     surface_created(ngaVideoSinkPtr, surfaceid0,surfaceid1,surfaceid2,surfaceid3,crop_rect_tl_x,
                     									crop_rect_tl_y,crop_rect_br_x,crop_rect_br_y,aspect_ratio_num,aspect_ratio_denom);                     
*/
                }
                break;
            }
            default:
                break;
        }   
    }
    DEBUG_API("<-XAMediaPlayerAdapt_GstBusCb");    
    return TRUE;
}

/*
 * XAMediaPlayerAdaptationCtx* XAMediaPlayerAdapt_Create()
 * Allocates memory for Media Player Adaptation Context and makes 1st phase initialization
 * @param XADataSource *pDataSrc - pointer to OMX-AL data source
 * @param XADataSource *pBankSrc - pointer to instrument bank structure in Mobile DLS, if NULL default will be used.
 * @param XADataSink *pAudioSnk - pointer to OMX-AL audio sink definition
 * @param XADataSink *pImageVideoSnk - pointer to OMX-AL image and video sink definition
 * @returns XAMediaPlayerAdaptationCtx* - Pointer to created context, NULL if error occurs.
 */
XAAdaptationBaseCtx* XAMediaPlayerAdapt_Create(XADataSource *pDataSrc, XADataSource *pBankSrc,
                                                      XADataSink *pAudioSnk, XADataSink *pImageVideoSnk,
                                                      XADataSink *pVibra, XADataSink *pLEDArray)
{
    XAMediaPlayerAdaptationCtx *pSelf = NULL;
    XAuint32 locType = 0;
    XADataLocator_IODevice *ioDevice;
    
    DEBUG_API("->XAMediaPlayerAdapt_Create");
    
    //Create NGA Video Sink class
/*    if(pImageVideoSnk)
    {
       ngaVideoSinkPtr = nga_video_sink_init();
    }*/

    pSelf = calloc(1, sizeof(XAMediaPlayerAdaptationCtx));
    if ( pSelf)
    {
        if( XAAdaptationBase_Init(&(pSelf->baseObj.baseObj),XAMediaPlayerAdaptation)
            != XA_RESULT_SUCCESS )
        {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
            return NULL;
        }
        else
        {
            pSelf->xaSource = pDataSrc;
            pSelf->xaBankSrc = pBankSrc;
            pSelf->xaAudioSink = pAudioSnk;
            pSelf->xaVideoSink = pImageVideoSnk;
            pSelf->xaLEDArray = pLEDArray;
            pSelf->xaVibra = pVibra;
            pSelf->loopstart = 0;
            pSelf->loopend = (gint64)GST_CLOCK_TIME_NONE;
            pSelf->playrate = 1.0;
            pSelf->rateprops = (XA_RATEPROP_SMOOTHVIDEO | XA_RATEPROP_SILENTAUDIO);
            pSelf->curMirror = XA_VIDEOMIRROR_NONE;
            pSelf->curRotation = 0;
            pSelf->isobjsrc = XA_BOOLEAN_FALSE;
            pSelf->cameraSinkSynced = XA_BOOLEAN_FALSE;
/*            if(pImageVideoSnk && ngaVideoSinkPtr)
            {
               setup_native_display(ngaVideoSinkPtr, pImageVideoSnk);
            }*/
        }

        if ( pDataSrc )
		{
			locType = *((XAuint32*)(pDataSrc->pLocator));
			if ( locType == XA_DATALOCATOR_IODEVICE  )
			{
				ioDevice = (XADataLocator_IODevice*)(pDataSrc->pLocator);
				if ( ioDevice->deviceType == XA_IODEVICE_CAMERA && !cameraRealized )
				{
					DEBUG_ERR("Preconditions violated - Camera object not realized");
					XAAdaptationBase_Free(&pSelf->baseObj.baseObj);
					free(pSelf);
					pSelf = NULL;
				}
			}
		}
    }

    DEBUG_API("<-XAMediaPlayerAdapt_Create");
    return (XAAdaptationBaseCtx*)(&pSelf->baseObj);
}



/*
 * XAresult XAMediaPlayerAdapt_PostInit()
 * 2nd phase initialization of Media Player Adaptation Context
 * @param XAMediaPlayerAdaptationCtx* ctx - pointer to Media Player adaptation context
 * @return XAresult - Success value
 */
XAresult XAMediaPlayerAdapt_PostInit( XAAdaptationGstCtx* bCtx )
{
    XAresult ret = XA_RESULT_SUCCESS;
    GstStateChangeReturn gret;

	XAMediaPlayerAdaptationCtx* ctx = NULL;
	GstElement *videotest=NULL;

    DEBUG_API("->XAMediaPlayerAdapt_PostInit");
    if(bCtx == NULL || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XAMediaPlayerAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
    }
    ctx = (XAMediaPlayerAdaptationCtx*)bCtx;
    assert(ctx);
    ret = XAAdaptationBase_PostInit( &bCtx->baseObj );
    if( ret!=XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Base context postinit failed!!");
        return ret;
    }

    /* top level bin for media player */
    ctx->baseObj.bin = gst_pipeline_new("media_player");
    /* Create Gst bus listener. */
    ret = XAAdaptationGst_InitGstListener(bCtx);
    if( ret!=XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Bus listener creation failed!!");
        return ret;
    }
    /* Add Media Player specific handler */
    if(ctx->baseObj.bus)
    {
        ctx->baseObj.busCb = XAMediaPlayerAdapt_GstBusCb;
        gst_bus_add_signal_watch( ctx->baseObj.bus );
        gst_bus_enable_sync_message_emission( ctx->baseObj.bus );
        g_signal_connect(ctx->baseObj.bus, "message::eos", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::error", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::warning", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::state-changed", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::segment-done", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::async-done", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::element", G_CALLBACK(bCtx->busCb), ctx );
    }
    else
    {
        DEBUG_ERR("Failed to create message bus");
        return XA_RESULT_INTERNAL_ERROR;
    }

    XAMetadataAdapt_PreInit(bCtx);

    /* create pipeline */
    ret = XAMediaPlayerAdapt_CreatePipeline(ctx);
    if ( ret != XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Failed to create Media Player pipeline");
        return ret;
    }

#ifdef XA_IMPL_MEASURE_GST_DELAY
    ctx->baseObj.startTime = clock();
#endif /* XA_IMPL_MEASURE_GST_DELAY */
    /* roll up bin */
    ctx->baseObj.binWantedState = GST_STATE_PAUSED;

    XAAdaptationGst_PrepareAsyncWait(bCtx);
    gret = gst_element_set_state( GST_ELEMENT(ctx->baseObj.bin), bCtx->binWantedState);
    if( gret == GST_STATE_CHANGE_ASYNC )
    {
        DEBUG_INFO("Wait for preroll");
        XAAdaptationGst_StartAsyncWait(bCtx);
        DEBUG_INFO("Preroll ready");
    }
    else if( gret == GST_STATE_CHANGE_FAILURE )
    {
        DEBUG_ERR("Preroll FAILED");
        ret = XA_RESULT_INTERNAL_ERROR;
    }

    bCtx->waitingasyncop = XA_BOOLEAN_FALSE;

    gret = gst_element_get_state( GST_ELEMENT(bCtx->bin), NULL, NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
    if(GST_STATE(bCtx->bin)<GST_STATE_PAUSED)
    {
        DEBUG_INFO("Warning! Preroll not ready");
        if( ctx->audioppbin && !(gst_pad_is_linked(gst_element_get_static_pad(ctx->audioppbin, "sink"))) )
        {/*could not find suitable pad for audiopipeline - remove it*/
            DEBUG_INFO("Warning! No suitable decodebin pad for audio pipeline!");
            gst_element_set_state( GST_ELEMENT(ctx->audioppbin), GST_STATE_NULL);
            gst_bin_remove(GST_BIN(bCtx->bin), ctx->audioppbin);
            gst_element_set_state( GST_ELEMENT(ctx->audiosink), GST_STATE_NULL);
            gst_bin_remove(GST_BIN(bCtx->bin), ctx->audiosink);
        }
        else if( ctx->videoppbin && !(gst_pad_is_linked(gst_element_get_static_pad(ctx->videoppbin, "videopp_sink"))) )
        {/*could not find suitable pad for videopipeline - remove it*/
            DEBUG_INFO("Warning! No suitable decodebin pad for video pipeline!");
            gst_element_set_state( GST_ELEMENT(ctx->videoppbin), GST_STATE_NULL);
            gst_bin_remove(GST_BIN(bCtx->bin), ctx->videoppbin);
            gst_element_set_state( GST_ELEMENT(ctx->videosink), GST_STATE_NULL);
            gst_bin_remove(GST_BIN(bCtx->bin), ctx->videosink);
        }
		gst_element_set_state( GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
		gst_element_get_state( GST_ELEMENT(bCtx->bin), NULL, NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
		if(GST_STATE(bCtx->bin)==GST_STATE_PAUSED)
		{
			DEBUG_INFO("Retry preroll successful!")
			ret = XA_RESULT_SUCCESS;
		}
    }
    else
    {
        DEBUG_INFO("Preroll ready");
    }

#ifdef XA_IMPL_MEASURE_GST_DELAY
    bCtx->endTime = clock();
    double diff = bCtx->endTime - bCtx->startTime ;
    diff = diff / CLOCKS_PER_SEC;
    DEBUG_API_A1( "Starting up bin took %.4lf secs",diff);
#endif /* XA_IMPL_MEASURE_GST_DELAY */
    videotest = gst_bin_get_by_name(GST_BIN(bCtx->bin), "videotest");
    if ( videotest && !ctx->isobjsrc )
    {
    	gst_element_set_state( GST_ELEMENT(videotest),GST_STATE_PLAYING);
    }

    XAMetadataAdapt_PostInit(bCtx);

    if ( videotest )
    {
        gst_object_unref(videotest);
    }

    DEBUG_API("<-XAMediaPlayerAdapt_PostInit");
    return ret;
}

/*
 * void XAMediaPlayerAdapt_Destroy( XAMediaPlayerAdaptationCtx* ctx )
 * Destroys Media Player Adaptation Context
 * @param ctx - Media Player Adaptation context to be destroyed
 */
void XAMediaPlayerAdapt_Destroy( XAAdaptationGstCtx* bCtx )
{
    XAMediaPlayerAdaptationCtx* ctx = NULL;

    DEBUG_API("->XAMediaPlayerAdapt_Destroy");
    if(bCtx == NULL || bCtx->baseObj.ctxId != XAMediaPlayerAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XAMediaPlayerAdapt_Destroy");
        return;
    }
    ctx = (XAMediaPlayerAdaptationCtx*)bCtx;

    if( ctx->isobjsrc )
    {   /* external source, unlink and remove now */
        /*gst_object_unparent( GST_OBJECT(ctx->source) );*/
        gst_element_unlink( ctx->source, ctx->codecbin );
        /*gst_bin_remove( GST_BIN(bCtx->bin), ctx->source );*/
        GST_OBJECT_FLAG_SET(GST_OBJECT(ctx->source),GST_OBJECT_FLOATING);
    }
#if 0
    if ( ctx->xaSource )
    {
    	XAuint32 locType = *(XAuint32*)(ctx->xaSource->pLocator);
    	switch (locType )
    	{
		case XA_DATALOCATOR_IODEVICE:
		{
			XADataLocator_IODevice* ioDevice = (XADataLocator_IODevice*)(ctx->xaSource->pLocator);
			if ( ioDevice->deviceType == XA_IODEVICE_RADIO )
			{
				gst_object_unparent( GST_OBJECT(ctx->source) );
				gst_element_unlink( ctx->source, ctx->codecbin );
				gst_bin_remove( GST_BIN(bCtx->bin), ctx->source );
				GST_OBJECT_FLAG_SET(GST_OBJECT(ctx->source),GST_OBJECT_FLOATING);
			}
		}
		default:
			break;
    	}
    }
#endif

    if( ctx->isobjasink && ctx->xaAudioSink && ctx->xaAudioSink->pLocator )
    {
        XAuint32 locType = *(XAuint32*)(ctx->xaAudioSink->pLocator);
        switch ( locType )
        {
            case XA_DATALOCATOR_OUTPUTMIX:
            {
                XADataLocator_OutputMix* omix = (XADataLocator_OutputMix*)(ctx->xaAudioSink->pLocator);
                XAOMixImpl* omixDevice = (XAOMixImpl*)(*omix->outputMix);
                if(omixDevice)
                {
                    XAOutputMixAdapt_DisconnectObject((XAAdaptationGstCtx*)omixDevice->adaptationCtx, bCtx);
                }
                break;
            }
            default:
                /* Vibra and LED need no handling */
                break;
        }

    }

    if( bCtx->bus )
    {
        gst_bus_remove_signal_watch( bCtx->bus );
        gst_bus_disable_sync_message_emission ( bCtx->bus );
    }
    XAAdaptationGst_CancelAsyncWait(bCtx);

    if( ctx->runpositiontimer )
    {
        g_source_remove(ctx->runpositiontimer);
    }
    XAMetadataAdapt_FreeVars(ctx->metadatavars);
    XAAdaptationBase_Free( &bCtx->baseObj );
    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XAMediaPlayerAdapt_Destroy");
}


/*
 * void XAMediaPlayerAdapt_CreatePipeline( XAMediaPlayerAdaptationCtx* ctx );
 */
XAresult XAMediaPlayerAdapt_CreatePipeline( XAMediaPlayerAdaptationCtx* ctx )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAboolean delayedlink = XA_BOOLEAN_FALSE;
    XAboolean isPCM = XA_BOOLEAN_FALSE;
    XAboolean isRawImage = XA_BOOLEAN_FALSE;
    XAMediaType mediatype;
    XAuint32 locType = 0;
    GstCaps* encSrcCaps = NULL;
    DEBUG_API("->XAMediaPlayerAdapt_CreatePipeline");

    /* create and add data source */
    XACommon_CheckDataSource(ctx->xaSource, &mediatype);
    ctx->source = XAAdaptationGst_CreateGstSource( ctx->xaSource, "datasrc", &(ctx->isobjsrc), &isPCM, &isRawImage);
    if( !(ctx->source) )
    {
        DEBUG_ERR("Could not create data source!!!");
        return XA_RESULT_INTERNAL_ERROR;
    }
    else
    {
        if(mediatype != XA_MEDIATYPE_AUDIO)
        {
          //temporary work around for video
    	  encSrcCaps = gst_caps_new_simple ("video/h263-2000",
                "framerate", GST_TYPE_FRACTION, 25, 1,
                "pixel-aspect-ratio", GST_TYPE_FRACTION, 16, 9,
                "width", G_TYPE_INT, 176,
                "height", G_TYPE_INT, 144,
                NULL);
          g_object_set(G_OBJECT(ctx->source), "caps", encSrcCaps, NULL);
        }

        //boolRetVal = gst_bin_add(GST_BIN(pipeline), appsrc);
    	
    	
    }

    if ( !ctx->isobjsrc )
    { /* Add other than camera source to media player bin */
    	DEBUG_INFO("No camera source");
    	gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->source);
    }
    else
    {
       GstCaps* encSrcCaps;
       encSrcCaps = gst_caps_new_simple("video/x-raw-yuv",
                   "format", GST_TYPE_FOURCC,GST_MAKE_FOURCC('I','4','2','0'),
                   "framerate", GST_TYPE_FRACTION, 30, 1,
                   NULL);
       DEBUG_INFO_A1("new camera encoding filter: %s",gst_caps_to_string(encSrcCaps));
       g_object_set( G_OBJECT(ctx->source), "filter-caps",encSrcCaps,NULL);
       gst_caps_unref(encSrcCaps);
    }

    /* create and add codec bin */
    if( !(ctx->isobjsrc || isPCM) )
    {
    	DEBUG_INFO("Create decodebin");
        if(mediatype == XA_MEDIATYPE_AUDIO)
        {
           ctx->codecbin = gst_element_factory_make( "decodebin" , "mpcodecbin" );
        }
        else
        {
           ctx->codecbin = gst_element_factory_make( "identity" , "mpcodecbin" );
        }
    }
    else if(ctx->isobjsrc )
    { /* object sources produce framed raw data, decodebin only causes trouble */ //shyward
    	DEBUG_INFO("Create identity")
        ctx->codecbin = gst_element_factory_make( "identity" , "mpcodecbin" );
    }
    else if(isPCM)
    { /* decodebin does not know how to handle PCM files */
    	DEBUG_INFO("Create audioparse")
        ctx->codecbin = gst_element_factory_make( "audioparse" , "mpcodecbin" );
    }
    else if ( isRawImage)
    { /* decodebin does not know how to handle raw unframed video data */
    	DEBUG_INFO("Create videoparse")
        ctx->codecbin = gst_element_factory_make( "videoparse", "mpcodecbin" );
    }

    if( ctx->codecbin )
    {
        gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->codecbin);
        if ( !ctx->isobjsrc )
        {
            if(mediatype == XA_MEDIATYPE_AUDIO)
            {
               if( !gst_element_link(ctx->source, ctx->codecbin) )
               {
                    DEBUG_ERR("Could not link source to decodebin!!");
                    return XA_RESULT_INTERNAL_ERROR;
               }
            }
            else
            {
			   if( !gst_element_link_filtered(ctx->source, ctx->codecbin, encSrcCaps ) )
			   {
			    	DEBUG_ERR("Could not link source to decodebin!!");
				    return XA_RESULT_INTERNAL_ERROR;
			   }
            }
        }
        else
        { /* Link camera source by using ghost-pads, because elements are in different bins */

        	GstPad *cameraBinGhostPad=NULL;
        	GstPad* ghost=NULL;
        	GstElement *camTee=NULL;
        	GstStateChangeReturn gret;
        	GstPad *mpGhostSink=NULL;

        	/* Set external camera source to ready for pipeline manipulation */
        	DEBUG_INFO("Set ext-source PAUSED for pipeline manipulation");
			gret = gst_element_set_state( GST_ELEMENT(ctx->source), GST_STATE_READY);
			if(gret == GST_STATE_CHANGE_SUCCESS)
			    {
                gret = gst_element_get_state( GST_ELEMENT(ctx->source), NULL,NULL,XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
			    }

			/* Add new ghost-pad to external camera source */
        	camTee = gst_bin_get_by_name( GST_BIN(ctx->source), "CamTee");
        	if ( !camTee )
        	{
        		DEBUG_ERR("Could not get tee-element from camera");
        	}
        	cameraBinGhostPad = gst_element_get_request_pad( camTee, "src%d" );
        	if ( !cameraBinGhostPad )
        	{
        		DEBUG_ERR("Could not get new src-pad from CamTee element");
        	}
			gst_element_add_pad(ctx->source, gst_ghost_pad_new("MPObjSrc",cameraBinGhostPad));
			ghost = gst_element_get_static_pad( GST_ELEMENT(ctx->source), "MPObjSrc" );
			DEBUG_INFO_A2("Setting element:%s pad:%s to blocking.",
							gst_element_get_name(ctx->baseObj.bin),
							gst_pad_get_name(ghost));
			/* Set newly created pad to blocking */
			gst_pad_set_blocked_async(ghost, TRUE, XAAdaptationGst_PadBlockCb, NULL);


			/* Create new ghost-pad to media player pipeline where external camera is connected */
        	mpGhostSink = gst_element_get_static_pad( GST_ELEMENT(ctx->codecbin), "sink");
			gst_element_add_pad(ctx->baseObj.bin, gst_ghost_pad_new("MPObjSink",mpGhostSink));

			if ( !gst_element_link_pads( GST_ELEMENT(ctx->source), "MPObjSrc",
										GST_ELEMENT(ctx->baseObj.bin), "MPObjSink") )
			{
				DEBUG_ERR("Could not link camera:MPObjSrc to videofilter:MPObjSink");
				return XA_RESULT_INTERNAL_ERROR;
			}

			if ( cameraBinGhostPad )
			{
				gst_object_unref( cameraBinGhostPad );
			}
			if ( ghost )
			{
				gst_object_unref( ghost );
			}
			if ( mpGhostSink )
			{
				gst_object_unref( mpGhostSink );
			}
			if ( camTee )
			{
				gst_object_unref( camTee );
			}
        }
    }
    else
    {
        DEBUG_ERR("Could not create decoder bin!!!");
        return XA_RESULT_INTERNAL_ERROR;
    }

    /* create and add video stream pipeline */
    if(!ctx->xaLEDArray && !ctx->xaVibra && mediatype!=XA_MEDIATYPE_AUDIO) /*no video for these*/
    {
        /* create video processing pipeline */
        ctx->videoppbin = XAAdaptationGst_CreateVideoPP( );
        if( ctx->videoppbin )
        {
            gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->videoppbin);
            //shyward ---link filtered???
            // boolRetVal = gst_element_link_filtered(appsrc, videosink, caps);
			      //if(!gst_element_link(ctx->codecbin, ctx->videoppbin))
            if(!gst_element_link_filtered(ctx->codecbin, ctx->videoppbin,encSrcCaps))
            {
                /* probably dynamic pads in codecbin */
                DEBUG_INFO("Could not link codec to videopp, trying delayed link");
                delayedlink = XA_BOOLEAN_TRUE;
            }
            ctx->videoScrSrcPad = gst_element_get_static_pad(ctx->videoppbin, "videopp_src");
        }
        else
        {
            DEBUG_ERR("Could not create video pp bin!!!!");
            return XA_RESULT_INTERNAL_ERROR;
        }
        //shyward
        /* Black screen pipeline not needed under Symbian. May need to revist for acceptance testing
        ctx->videoppBScrbin = XAAdaptationBase_CreateVideoPPBlackScr( );
        if( ctx->videoppBScrbin )
        {
            gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->videoppBScrbin);
            ctx->blackScrSrcPad = gst_element_get_static_pad(ctx->videoppBScrbin, "videoppBSrc_src");
        }
        else
        {
            DEBUG_ERR("Could not create video pp bin for black screen!!!!");
            return XA_RESULT_INTERNAL_ERROR;
        }
        */
        ctx->inputSelector = XAAdaptationGst_CreateInputSelector( );
        if( ctx->inputSelector )
        {
            gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->inputSelector);
            ctx->videoScrSinkPad = gst_element_get_request_pad(ctx->inputSelector, "sink%d");
            ctx->blackScrSinkPad = gst_element_get_request_pad(ctx->inputSelector, "sink%d");
            gst_pad_link(ctx->blackScrSrcPad, ctx->blackScrSinkPad);
            gst_pad_link(ctx->videoScrSrcPad, ctx->videoScrSinkPad);
        }

        //shyward - We have no video filter at this time
        /*
        ctx->filter = gst_element_factory_make("ffmpegcolorspace", "videofilter");
        gst_bin_add( GST_BIN(ctx->baseObj.bin), ctx->filter);
        if ( !gst_element_link( ctx->inputSelector, ctx->filter ) )
        {
		   DEBUG_ERR("Could not link ctx->filter <-> ctx->inputSelector");
		   return XA_RESULT_INTERNAL_ERROR;
	   }
	   */
        /* create video pipe sink */
        ctx->videosink = XAAdaptationGst_CreateGstSink( ctx->xaVideoSink, "videosink", &(ctx->isobjvsink) );
        /* NOTE: no valid object sinks for video output available */
        if( ctx->videosink )
        {
            gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->videosink);

            //shyward
            //if(!gst_element_link(ctx->filter, ctx->videosink))
            if(!gst_element_link_filtered(ctx->videoppbin, ctx->videosink,encSrcCaps))
            {
                DEBUG_ERR("Could not link videopp to videosink!!");
                return XA_RESULT_INTERNAL_ERROR;
            }
            else
            {
            	gst_caps_unref(encSrcCaps);
            }
        }
        else
        {
            DEBUG_ERR("Could not create video sink!!!!");
            return XA_RESULT_INTERNAL_ERROR;
        }
    }
    else
    {
        DEBUG_INFO("Media does not contain video!");
    }

    /* create and add audio stream pipeline */

    if(!ctx->xaLEDArray && !ctx->xaVibra && mediatype!=XA_MEDIATYPE_IMAGE) /*no audio for these*/
    {
    /* create audio post processing pipeline */
    ctx->audioppbin = XAAdaptationGst_CreateAudioPP( );
    if( ctx->audioppbin )
    {
        gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->audioppbin);
        if(!gst_element_link(ctx->codecbin, ctx->audioppbin))
        {
            DEBUG_INFO("Could not link codec to audiopp, trying delayed link");
            delayedlink = XA_BOOLEAN_TRUE;
        }
    }
    else
    {
        DEBUG_ERR("Could not create audio pp bin!!!!");
        return XA_RESULT_INTERNAL_ERROR;
    }
    /* create audio pipe sink */
    ctx->audiosink = XAAdaptationGst_CreateGstSink( ctx->xaAudioSink, "audiosink", &(ctx->isobjasink) );
    if( ctx->audiosink )
    {
        if( ctx->isobjasink && ctx->xaAudioSink && ctx->xaAudioSink->pLocator )
        {
            locType = *(XAuint32*)(ctx->xaAudioSink->pLocator);
            switch ( locType )
            {
                case XA_DATALOCATOR_OUTPUTMIX:
                {
                    XADataLocator_OutputMix* omix = (XADataLocator_OutputMix*)(ctx->xaAudioSink->pLocator);
                    XAOMixImpl* omixDevice = (XAOMixImpl*)(*omix->outputMix);
                    if(omixDevice)
                    {
                        XAOutputMixAdapt_ConnectObject((XAAdaptationGstCtx*)omixDevice->adaptationCtx, &(ctx->baseObj), ctx->audiosink);
                    }
                    break;
                }
                default:
                    /* Vibra and LED need no handling */
                    break;
            }
        }
        gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->audiosink);
        if(!gst_element_link(ctx->audioppbin, ctx->audiosink))
        {
            DEBUG_ERR("Could not link audiopp to audiosink!!");
            return XA_RESULT_INTERNAL_ERROR;
        }
    }
    else
    {
        DEBUG_ERR("Could not create audio sink!!!!");
        return XA_RESULT_INTERNAL_ERROR;
        }
    }
    else
    {
        DEBUG_INFO("Media does not contain audio!");
    }

    if(delayedlink)
    {
        /* listen for dynamically created pads */
        g_signal_connect (ctx->codecbin, "pad-added", G_CALLBACK (XAMediaPlayerAdapt_NewPadCb), ctx);
    }
    locType = *((XAuint32*)(ctx->xaSource->pLocator));

    DEBUG_API("<-XAMediaPlayerAdapt_CreatePipeline");
    return ret;
}

/*
 * gboolean XAMediaPlayerAdapt_PositionUpdate(gpointer ctx)
 * callback.
 * If position tracking enabled, periodic timer calls this method every XA_ADAPT_PU_INTERVAL msecs
 * @return false to stop periodic calls
 */
gboolean XAMediaPlayerAdapt_PositionUpdate(gpointer ctx)
{
    XAAdaptationGstCtx *bCtx = (XAAdaptationGstCtx*) ctx;
    XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) ctx;
    gint64 position;
    XAmillisecond posMsec;
    GstFormat format = GST_FORMAT_TIME;
    XAAdaptEvent event = {XA_PLAYITFEVENTS, XA_ADAPT_POSITION_UPDATE_EVT, 1, NULL};

    DEBUG_API("->XAMediaPlayerAdapt_PositionUpdate");
    if ( !gst_element_query_position( GST_ELEMENT(bCtx->bin), &format, &position ) )
    {
        DEBUG_ERR("Gst: Failed to get position");
        return( mCtx->runpositiontimer );
    }
    DEBUG_INFO_A1("Current position %"GST_TIME_FORMAT, GST_TIME_ARGS(position));
    if( mCtx && mCtx->trackpositionenabled )
    {
        posMsec = GST_TIME_AS_MSECONDS(position);/*Warning ok due to used API specification*/
        DEBUG_INFO_A1("mCtx->trackpositionenabled sending update, position:&ld ", posMsec);
        /* send needed events */
        event.data=&posMsec;
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }
    if( mCtx && mCtx->loopingenabled)
    {
    	DEBUG_INFO_A2("mCtx->loopingenabled, current position:%lu, loopend:%lu ", position, mCtx->loopend);
        if( (position >= mCtx->loopend) &&
            (mCtx->lastpos < mCtx->loopend) )
        {
            DEBUG_INFO_A2("Restart loop from %"GST_TIME_FORMAT" to %"GST_TIME_FORMAT,
                          GST_TIME_ARGS(mCtx->loopstart), GST_TIME_ARGS(mCtx->loopend));
            gst_element_seek( bCtx->bin, mCtx->playrate, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE),
                              GST_SEEK_TYPE_SET, mCtx->loopstart,
                              GST_SEEK_TYPE_NONE, (gint64)GST_CLOCK_TIME_NONE );
            mCtx->lastpos = mCtx->loopstart;
            if( mCtx && mCtx->trackpositionenabled )
            {
            	DEBUG_INFO_A1("mCtx->trackpositionenabled sending looping update, position:%&u ", posMsec);
                posMsec = GST_TIME_AS_MSECONDS(mCtx->lastpos);/*Warning ok due to used API specification*/
                /* send needed events */
                event.data=&posMsec;
                XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
            }
        }
        else
        {
            mCtx->lastpos = position;
        }
    }
    DEBUG_API("<-XAMediaPlayerAdapt_PositionUpdate");
    /* return false to stop timer */
    return( mCtx->runpositiontimer );
}

/*
 * XAresult XAMediaPlayerAdapt_UpdatePositionCbTimer
 * Enable/disable periodic position tracking callback timer
 */
XAresult XAMediaPlayerAdapt_UpdatePositionCbTimer(XAMediaPlayerAdaptationCtx* mCtx)
{
    DEBUG_API_A2("->XAMediaPlayerAdapt_UpdatePositionCbTimer: trackposition %u, tracklooping %u",
                mCtx->trackpositionenabled, mCtx->loopingenabled);

    if(mCtx->runpositiontimer==0 && (mCtx->trackpositionenabled || mCtx->loopingenabled))
    {
        DEBUG_INFO("Start position tracking timer");
        mCtx->positionCb = &XAMediaPlayerAdapt_PositionUpdate;
        /* if play is already on, create a timer to track position of playback */
        if( GST_STATE(mCtx->baseObj.bin) == GST_STATE_PLAYING )
        {
            mCtx->runpositiontimer = g_timeout_add(XA_ADAPT_PU_INTERVAL, mCtx->positionCb, mCtx);
        }
    }
    else if (mCtx->runpositiontimer!=0 && !(mCtx->trackpositionenabled || mCtx->loopingenabled))
    {
        DEBUG_INFO("Stop position tracking timer");
        mCtx->trackpositionenabled = XA_BOOLEAN_FALSE;
        if(mCtx->runpositiontimer > 0)
        {
            g_source_remove(mCtx->runpositiontimer);
            mCtx->runpositiontimer=0;
        }
    }
    DEBUG_API("<-XAMediaPlayerAdapt_UpdatePositionCbTimer");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAMediaPlayerAdapt_InitContentPipeSrc(ctx)
 * CP code: can be moved to context base
 */
/*XAresult XAMediaPlayerAdapt_InitContentPipeSrc(XAMediaPlayerAdaptationCtx* ctx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    CPresult res;
    GstStateChangeReturn gstRet = GST_STATE_CHANGE_SUCCESS;
    DEBUG_API("->XAMediaPlayerAdapt_InitContentPipeSrc");
    ctx->baseObj.pipeSrcThrCtx.appSrc = GST_APP_SRC(ctx->source);
    ctx->baseObj.pipeSrcThrCtx.pipe = (XADataLocator_ContentPipe*)(ctx->xaSource->pLocator);

     Create thread for content pipe source
    ret = XAImpl_CreateThreadHandle( &(ctx->baseObj.pipeSrcThr) );
    if ( ret != XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Could not create thread for content pipe source!");
        DEBUG_API("<-XAMediaPlayerAdapt_InitContentPipeSrc");
        return ret;
    }

     Create semaphore for content pipe source 
    ret = XAImpl_CreateSemaphore( &(ctx->baseObj.pipeSrcThrCtx.stateSem));
    if ( ret != XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Could not create semaphore for content pipe source!");
        DEBUG_API("<-XAMediaPlayerAdapt_InitContentPipeSrc");
        return ret;
    }

     Open content pipe 
    res = ctx->baseObj.pipeSrcThrCtx.pipe->pContentPipe->Open(&(ctx->baseObj.pipeSrcThrCtx.dataHandle),
                                                (CPstring)(ctx->baseObj.pipeSrcThrCtx.pipe->URI),
                                                CP_AccessRead );
    if ( res ==  EXIT_FAILURE )
    {
        DEBUG_ERR("Could not open Content Pipe!")
        return XA_RESULT_INTERNAL_ERROR;
    }

    res = ctx->baseObj.pipeSrcThrCtx.pipe->pContentPipe->RegisterCallback( &(ctx->baseObj.pipeSrcThrCtx.dataHandle), &XAAdaptationBase_ContentPipeSrcCb);
    if ( res == EXIT_FAILURE )
    {
        DEBUG_ERR("Could not register content pipe callbacks!")
        return XA_RESULT_INTERNAL_ERROR;
    }

    gstRet = gst_element_set_state( GST_ELEMENT(ctx->source), GST_STATE_PAUSED);
    gst_element_sync_state_with_parent( GST_ELEMENT( ctx->source));

    XAImpl_StartThread( &(ctx->baseObj.pipeSrcThr), NULL, &XAAdaptationBase_ContentPipeScrThrFunc, &(ctx->baseObj.pipeSrcThrCtx) );

    DEBUG_API("<-XAMediaPlayerAdapt_InitContentPipeSrc");
    return ret;
}*/
