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

#include <string.h>
#include <assert.h>
#include <gst.h>
#include <gstappsink.h>
#include "XAMediaRecorderAdaptCtx.h"
#include "XAAdaptation.h"
#include "XAMetadataAdaptation.h"
#include "XAStaticCapsAdaptation.h"

#define XA_ADAPTID_UNINITED 0

extern XAboolean cameraRealized;
extern XACameraAdaptationCtx_* cameraCtx;

/*forward declarations*/
GstElement* XAMediaRecorderAdapt_CreateEncodeBin(
        XAMediaRecorderAdaptationCtx* ctx);
XAresult XAMediaRecorderAdapt_CreatePipeline(
        XAMediaRecorderAdaptationCtx* ctx);
XAresult XAMediaRecorderAdapt_CreatePipeline_New(
        XAMediaRecorderAdaptationCtx* ctx);
void XAMediaRecorderAdapt_BufferAvailable(GstElement* sink,
        gpointer user_data);

/*
 * gboolean XAMediaRecorderAdapt_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
 * MediaPlayer Gst-bus message handler (Callback)
 */
gboolean XAMediaRecorderAdapt_GstBusCb(GstBus *bus, GstMessage *message,
        gpointer data)
    {
    XAAdaptationBaseCtx* bCtx = (XAAdaptationBaseCtx*) data;
    /* only listen to pipeline messages */
    if (GST_MESSAGE_SRC(message)==GST_OBJECT(bCtx->bin))
        {
        XAMediaRecorderAdaptationCtx* mCtx = NULL;
        DEBUG_API_A2("->XAMediaRecorderAdapt_GstBusCb:\"%s\" from object \"%s\"",
                        GST_MESSAGE_TYPE_NAME(message), GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));
        mCtx = (XAMediaRecorderAdaptationCtx*)data;

        switch( GST_MESSAGE_TYPE(message))
        {
            case GST_MESSAGE_EOS:
            {
                /* stop position tracking */
                if(mCtx->runpositiontimer > 0)
                {
                    g_source_remove(mCtx->runpositiontimer);
                    mCtx->runpositiontimer=0;
                }

                /* complete any ongoing client async operations */
                XAAdaptationBase_CompleteAsyncWait(bCtx);

                /* send needed events */
                {
                    XAAdaptEvent event = {XA_RECORDITFEVENTS, XA_RECORDEVENT_HEADATLIMIT, 0, NULL };
                    XAAdaptationBase_SendAdaptEvents(bCtx, &event );
                }
                if(mCtx->positionCb)
                {
                    mCtx->positionCb(bCtx);
                }
                bCtx->binWantedState = GST_STATE_PAUSED;
                break;
            }
            case GST_MESSAGE_STATE_CHANGED:
            {
                GstState oldstate, newstate, pendingstate, gsttargetstate;
                gst_message_parse_state_changed(message, &oldstate, &newstate, &pendingstate);
                gsttargetstate = GST_STATE_TARGET(bCtx->bin);
                DEBUG_INFO_A4("old %d -> new %d (-> pending %d -> gsttarget %d)",
                               oldstate, newstate, pendingstate, gsttargetstate);
                if(gsttargetstate!=bCtx->binWantedState)
                {
                    DEBUG_ERR_A1("WARNING: Gst target is not wanted target [%d]!!!", bCtx->binWantedState);
                }
                /* print out some more info */
                if( pendingstate == GST_STATE_VOID_PENDING )
                {
                    if( newstate != bCtx->binWantedState )
                    {
                        DEBUG_INFO_A2("Gst in intermediate state transition (curr %d, target %d)",
                                    newstate,bCtx->binWantedState);
                    }
                    else
                    {
                        DEBUG_INFO_A1("Gst in wanted target state (%d)",newstate);
                    }
                }
                if( oldstate!=GST_STATE_PLAYING && newstate==GST_STATE_PLAYING )
                {
                    XAAdaptEvent event = {XA_RECORDITFEVENTS, XA_RECORDEVENT_HEADMOVING, 0, NULL };
                    /* send needed events */
                    XAAdaptationBase_SendAdaptEvents(bCtx, &event );
                    /* enable position tracking if needed */
                    if( mCtx->runpositiontimer==0 && mCtx->trackpositionenabled && mCtx->positionCb )
                    {
                        mCtx->runpositiontimer = g_timeout_add(XA_ADAPT_PU_INTERVAL, mCtx->positionCb, mCtx);
                    }
                }
                break;
            }

            case GST_MESSAGE_ASYNC_DONE:
            {
                /* some async sequence ended */
                XAAdaptationBase_CompleteAsyncWait(bCtx);
                break;
            }

            case GST_MESSAGE_ERROR:
            {
                GError* error;
                gchar* debug;
                gst_message_parse_error(message, &error, &debug);
                DEBUG_ERR_A1("Gst reports error \"%s\"", debug);
                /* stop waiting any ongoing async operations */
                XAAdaptationBase_CompleteAsyncWait(bCtx);
                break;
            }

            default:
                break;
        }
    DEBUG_API("<-XAMediaRecorderAdapt_GstBusCb");
    }
    return TRUE;
    }

/*
 * XAAdaptationBaseCtx* XAMediaRecorderAdapt_Create()
 * Allocates memory for Media Recorder Adaptation Context and makes 1st phase initialization
 * @param XADataSource *pAudioSrc - pointer to OMX-AL audio source
 * @param XADataSource *pImageVideoSrc - pointer image/video source
 * @param XADataSink *pDataSnk - pointer to OMX-AL sink
 * @returns XAMediaRecorderAdaptationCtx* - Pointer to created context, NULL if error occurs.
 */
XAAdaptationBaseCtx* XAMediaRecorderAdapt_Create(XADataSource* pAudioSrc,
        XADataSource* pImageVideoSrc, XADataSink* pDataSnk, XAuint8 recModes)
    {
    XAMediaRecorderAdaptationCtx *pSelf = NULL;
    XAuint32 locType = 0;
    XADataLocator_IODevice *ioDevice;
    DEBUG_API("->XAMediaRecorderAdapt_Create");

    pSelf = calloc(1, sizeof(XAMediaRecorderAdaptationCtx));
    if (pSelf)
        {
        if (XAAdaptationBase_Init(&(pSelf->baseObj),
                XAMediaRecorderAdaptation) != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
            }
        else
            {
            pSelf->xaAudioSource = pAudioSrc;
            pSelf->xaVideoSource = pImageVideoSrc;
            pSelf->xaSink = pDataSnk;
            pSelf->baseObj.pipeSinkThrCtx.state = CPStateNull;
            pSelf->xaRecordState = XA_RECORDSTATE_STOPPED;
            pSelf->curMirror = XA_VIDEOMIRROR_NONE;
            pSelf->curRotation = 0;
            pSelf->recModes = recModes;
            pSelf->isRecord = XA_BOOLEAN_FALSE;

            /* defaults from API spec */
            pSelf->imageEncSettings.width = 640;
            pSelf->imageEncSettings.height = 480;
            pSelf->imageEncSettings.compressionLevel = 0;
            pSelf->imageEncSettings.encoderId = XA_IMAGECODEC_JPEG;
            pSelf->imageEncSettings.colorFormat = XA_COLORFORMAT_UNUSED;
            /* no specified defaults for rest, determined later from container type */
            pSelf->videoEncSettings.encoderId = XA_ADAPTID_UNINITED;
            pSelf->videoEncSettings.width = 640;
            pSelf->videoEncSettings.height = 480;
            pSelf->videoEncSettings.frameRate = 15;
            pSelf->audioEncSettings.encoderId = XA_ADAPTID_UNINITED;
            pSelf->audioEncSettings.channelsIn = 2;
            pSelf->audioEncSettings.channelsOut = 2;
            pSelf->audioEncSettings.bitsPerSample = 8;
            pSelf->audioEncSettings.bitRate = 128;
            pSelf->audioEncSettings.sampleRate = 44100;
            }

        if (pImageVideoSrc)
            {
            locType = *((XAuint32*) (pImageVideoSrc->pLocator));
            if (locType == XA_DATALOCATOR_IODEVICE)
                {
                ioDevice
                        = (XADataLocator_IODevice*) (pImageVideoSrc->pLocator);
                if (ioDevice->deviceType == XA_IODEVICE_CAMERA
                        && !cameraRealized)
                    {
                    DEBUG_ERR("Preconditions violated - Camera object not realized");
                    XAAdaptationBase_Free(&pSelf->baseObj);
                    free(pSelf);
                    pSelf = NULL;
                    }
                }
            }
        }

    DEBUG_API("<-XAMediaRecorderAdapt_Create");
    return (XAAdaptationBaseCtx*) pSelf;
    }

/*
 * XAresult XAMediaRecorderAdapt_PostInit()
 * 2nd phase initialization of Media Recorder Adaptation Context
 * @param XAAdaptationBaseCtx* ctx - pointer to Media Recorder adaptation context
 * @return XAresult - Success value
 */
XAresult XAMediaRecorderAdapt_PostInit(XAAdaptationBaseCtx* bCtx)
    {
    GstStateChangeReturn gret;

    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaRecorderAdaptationCtx* ctx = NULL;
    DEBUG_API("->XAMediaRecorderAdapt_PostInit");
    if (bCtx == NULL || bCtx->ctxId != XAMediaRecorderAdaptation)
        {
        DEBUG_ERR("Invalid parameter!!");DEBUG_API("<-XAMediaRecorderAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
        }
    ctx = (XAMediaRecorderAdaptationCtx*) bCtx;

    ret = XAAdaptationBase_PostInit(bCtx);
    if (ret != XA_RESULT_SUCCESS)
        {
        DEBUG_ERR("Base context postinit failed!!");
        return ret;
        }

    /* top level bin for media recorder */
    bCtx->bin = gst_pipeline_new("media_recorder");

    /* Create Gst bus listener. */
    ret = XAAdaptationBase_InitGstListener(bCtx);
    if (ret != XA_RESULT_SUCCESS)
        {
        DEBUG_ERR("Bus listener creation failed!!");
        return ret;
        }
    /* Add Media Recorder specific handler */
    if (bCtx->bus)
        {
        bCtx->busCb = XAMediaRecorderAdapt_GstBusCb;
        gst_bus_add_signal_watch(bCtx->bus);
        g_signal_connect(bCtx->bus, "message::eos", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(bCtx->bus, "message::error", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(bCtx->bus, "message::warning", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(bCtx->bus, "message::state-changed", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(bCtx->bus, "message::segment-done", G_CALLBACK(bCtx->busCb), ctx );
        g_signal_connect(bCtx->bus, "message::async-done", G_CALLBACK(bCtx->busCb), ctx );
        }
    else
        {
        DEBUG_ERR("Failed to create message bus");
        return XA_RESULT_INTERNAL_ERROR;
        }

    XAMetadataAdapt_PreInit(bCtx);

    /* create pipeline */
    ret = XAMediaRecorderAdapt_CreatePipeline(ctx);
    //ret = XAMediaRecorderAdapt_CreatePipeline_New(ctx);
    if (ret != XA_RESULT_SUCCESS)
        {
        DEBUG_ERR("Failed to create recorder pipeline");
        return XA_RESULT_INTERNAL_ERROR;
        }

    /* Init content pipe if used */
    if (ctx->xaSink && ctx->xaSink->pLocator
            && *(XAuint32*) (ctx->xaSink->pLocator)
                    == XA_DATALOCATOR_CONTENTPIPE)
        {
        XAMediaRecorderAdapt_InitContentPipeSink(ctx);
        }

#ifdef XA_IMPL_MEASURE_GST_DELAY
    bCtx->startTime = clock();
#endif /* XA_IMPL_MEASURE_GST_DELAY */
    /* roll up bin */
    bCtx->binWantedState = GST_STATE_PAUSED;
    XAAdaptationBase_PrepareAsyncWait(bCtx);
    gret = gst_element_set_state(GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
    if (gret == GST_STATE_CHANGE_ASYNC)
        {
        DEBUG_INFO("Wait for preroll");
        XAAdaptationBase_StartAsyncWait(bCtx);DEBUG_INFO("Preroll ready");
        }
    else if (gret == GST_STATE_CHANGE_FAILURE)
        {
        DEBUG_ERR("Preroll FAILED");
        /*ret = XA_RESULT_INTERNAL_ERROR;*/
        }
#ifdef XA_IMPL_MEASURE_GST_DELAY
    bCtx->endTime = clock();
    double diff = bCtx->endTime - bCtx->startTime;
    diff = diff / CLOCKS_PER_SEC;
    DEBUG_API_A1( "Starting up bin took %.4lf secs",diff);
#endif /* XA_IMPL_MEASURE_GST_DELAY */
    XAMetadataAdapt_PostInit(bCtx);

    bCtx->waitingasyncop = XA_BOOLEAN_FALSE;

    DEBUG_API("<-XAMediaRecorderAdapt_PostInit");
    return ret;
    }

/*
 * void XAMediaRecorderAdapt_Destroy( XAAdaptationBaseCtx* bCtx )
 * Destroys Media Recorder Adaptation Context
 * @param ctx - Media Recorder Adaptation context to be destroyed
 */
void XAMediaRecorderAdapt_Destroy(XAAdaptationBaseCtx* bCtx)
    {
    XAMediaRecorderAdaptationCtx* ctx = NULL;
    char* fname = NULL;
    DEBUG_API("->XAMediaRecorderAdapt_Destroy");

    if (bCtx == NULL || bCtx->ctxId != XAMediaRecorderAdaptation)
        {
        DEBUG_ERR("Invalid parameter!!");DEBUG_API("<-XAMediaRecorderAdapt_Destroy");
        return;
        }
    ctx = (XAMediaRecorderAdaptationCtx*) bCtx;

    if (ctx->isRecord == XA_BOOLEAN_FALSE)
        {
        DEBUG_INFO("Removing unnecessary file.");

        if (ctx->xaSink && *((XAuint32*) (ctx->xaSink->pLocator))
                == XA_DATALOCATOR_URI)
            {
            if (strncmp(
                    (char *) ((XADataLocator_URI*) (ctx->xaSink->pLocator))->URI,
                    "file://", 7) == 0)
                {
                fname
                        = (char *) &(((XADataLocator_URI*) (ctx->xaSink->pLocator))->URI[7]);
                }
            else
                {
                fname
                        = (char *) ((XADataLocator_URI*) (ctx->xaSink->pLocator))->URI;
                }

            if (remove(fname) != 0)
                {
                DEBUG_ERR_A1("Cannot remove file %s", fname);
                }
            }
        }

    if (ctx->isobjvsrc && ctx->videosource)
        { /* external source, unlink now */
        gst_element_unlink(ctx->videosource, ctx->codecbin);
        GST_OBJECT_FLAG_SET(GST_OBJECT(ctx->videosource),GST_OBJECT_FLOATING);
        }
    if (bCtx->bus)
        {
        gst_bus_remove_signal_watch(bCtx->bus);
        }
    XAAdaptationBase_CancelAsyncWait(bCtx);

    if (ctx->runpositiontimer)
        {
        g_source_remove(ctx->runpositiontimer);
        }
    XAAdaptationBase_Free(bCtx);

    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XAMediaRecorderAdapt_Destroy");
    }

/***************** INTERNAL FUNCTIONS *******************************/

XAresult XAMediaRecorderAdapt_CreatePipeline_New(
        XAMediaRecorderAdaptationCtx* ctx)
    {
    XADataLocator_URI* uri;
    GstCaps* caps;
    gboolean ok;

    //Create gst source
    ctx->audiosource
            = gst_element_factory_make("devsoundsrc", "record_audio");
    if (!ctx->audiosource)
        {
        g_print("could not create \"record\" element!");
        return XA_RESULT_INTERNAL_ERROR;
        }

    ctx->datasink = gst_element_factory_make("filesink", "filesink");
    if (!ctx->datasink)
        {
        g_print("could not create \"filesink\" element!");
        return XA_RESULT_INTERNAL_ERROR;
        }

    ctx->codecbin = gst_element_factory_make("wavenc", "wavencoder");
    if (!ctx->codecbin)
        {
        g_print("could not create \"wavenc\" element!");
        return XA_RESULT_INTERNAL_ERROR;
        }
    uri = (XADataLocator_URI*) ctx->xaSink->pLocator;

    if (uri->URI != NULL)
        {
        XAchar *fname;
        DEBUG_INFO_A1("URI: %s", uri->URI);
        if (strncmp((char *) uri->URI, "file://", 7) == 0)
            {
            fname = &((uri->URI)[7]);
            }
        else
            {
            fname = uri->URI;
            }DEBUG_INFO_A1("->filesystem path %s", fname);
        g_object_set( G_OBJECT(ctx->datasink),"location", fname,
         "async", FALSE,
         "qos", FALSE,
         "max-lateness", (gint64)(-1),
         NULL);
        
        //g_object_set(G_OBJECT(ctx->datasink), "location", fname, NULL);
        }
    
   // ctx->audioppbin = XAAdaptationBase_CreateAudioPP( );
    //gst_bin_add_many(GST_BIN (ctx->baseObj.bin), ctx->audiosource,  ctx->audioppbin, ctx->codecbin,ctx->datasink, NULL);
    
    gst_bin_add_many(GST_BIN (ctx->baseObj.bin), ctx->audiosource, ctx->codecbin,ctx->datasink, NULL);
    
    caps = gst_caps_new_simple ("audio/x-raw-int",
              "width", G_TYPE_INT, 16,
              "depth", G_TYPE_INT, 16,
              "signed",G_TYPE_BOOLEAN, TRUE,
              "endianness",G_TYPE_INT, G_BYTE_ORDER,
              "rate", G_TYPE_INT, 16000,
              "channels", G_TYPE_INT, 1, NULL);
    
    ok = gst_element_link_filtered (ctx->audiosource, ctx->codecbin, caps);
    if(!ok)
        {
        DEBUG_ERR("Could not link audiosrc to codecbin!!");
        }
    ok = gst_element_link (ctx->codecbin, ctx->datasink);
    if(!ok)
        {
        DEBUG_ERR("Could not link codecbin to datasink!!");
        }
    gst_caps_unref (caps);
    
    return XA_RESULT_SUCCESS;
    }
    /*
     * void XAMediaRecorderAdapt_CreatePipeline( XAMediaRecorderAdaptationCtx* ctx );
     */
    XAresult XAMediaRecorderAdapt_CreatePipeline(
            XAMediaRecorderAdaptationCtx* ctx)
        {
        XAresult ret = XA_RESULT_SUCCESS;
        GstCaps* encSrcCaps = NULL;
        DEBUG_API("->XAMediaRecorderAdapt_CreatePipeline");

        /* create and add video pipeline */
        ctx->codecbin = XAMediaRecorderAdapt_CreateEncodeBin(ctx);
        if (ctx->codecbin)
            {
            if (gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->codecbin))
                {
                DEBUG_API("->XAMediaRecorderAdapt_CreatePipeline: gst_bin_add success");
                }
            else
                {
                DEBUG_ERR("Could not add codec bin");
                return XA_RESULT_INTERNAL_ERROR;
                }
            }
        else
            {
            DEBUG_ERR("Could not create encoding bin!!!");
            return XA_RESULT_INTERNAL_ERROR;
            }

        /* create and add video pipeline if video source available and codec supports video */
        if (ctx->xaVideoSource
                && gst_element_get_pad(ctx->codecbin, "v_sink"))
            {
            /* create video pipe source */
            ctx->videosource = XAAdaptationBase_CreateGstSource(
                    ctx->xaVideoSource, "videosource", &(ctx->isobjvsrc),
                    NULL, NULL );
            if (!ctx->videosource)
                {
                DEBUG_ERR("Could not create video source!!!!");
                return XA_RESULT_INTERNAL_ERROR;
                }

            if (!ctx->isobjvsrc)
                { /* Add other than camera sources to media recorder bin */
                gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->videosource);
                }
            else
                { /* Don't add camera source to media recorder bin */
                GstCaps * encSrcCaps;
                encSrcCaps = gst_caps_new_simple( "video/x-raw-yuv", 
                "format", GST_TYPE_FOURCC,GST_MAKE_FOURCC('I','4','2','0'),
                "framerate", GST_TYPE_FRACTION, ctx->videoEncSettings.frameRate, 1,
                        NULL);
                DEBUG_INFO_A1("new camera encoding filter: %s",gst_caps_to_string(encSrcCaps));
                g_object_set( G_OBJECT(ctx->videosource), "filter-caps",encSrcCaps,NULL);
                gst_caps_unref(encSrcCaps);
                }
            /* create video filter for video encoder settings */
            ctx->videofilter = gst_element_factory_make("capsfilter", "videofilter");
            if( ctx->videofilter )
                {
                GstCaps* encSrcCaps;
                gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->videofilter);
                encSrcCaps = gst_caps_new_simple("video/x-raw-yuv",
                        "width", G_TYPE_INT, ctx->videoEncSettings.width,
                        "height", G_TYPE_INT, ctx->videoEncSettings.height,
                        "framerate", GST_TYPE_FRACTION, ctx->videoEncSettings.frameRate, 1,
                        NULL);
                DEBUG_INFO_A1("video encoder config from settings: %s",gst_caps_to_string(encSrcCaps));
                g_object_set( G_OBJECT(ctx->videofilter), "caps",encSrcCaps,NULL);
                gst_caps_unref(encSrcCaps);
                if ( ! ctx->isobjvsrc )
                    {
                    if(!gst_element_link(ctx->videosource, ctx->videofilter))
                        {
                        DEBUG_ERR("Could not link videopp to videofilter!!");
                        return XA_RESULT_INTERNAL_ERROR;
                        }
                    }
                else
                    { /* For camera source used ghost-pads for linking, because elements are in different bins */
                    GstStateChangeReturn gret;
                    GstElement *camTee=NULL;
                    GstPad *cameraBinGhostPad=NULL;
                    GstPad *ghost=NULL;
                    GstPad *mrGhostSink=NULL;

                    DEBUG_INFO("Set ext-source PAUSED for pipeline manipulation");
                    gret = gst_element_set_state( GST_ELEMENT(ctx->videosource), GST_STATE_READY);
                    gret = gst_element_get_state( GST_ELEMENT(ctx->videosource), NULL,NULL,XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);

                    /* Add new ghost-pad to external camera source */
                    camTee = gst_bin_get_by_name( GST_BIN(ctx->videosource), "CamTee");
                    if ( !camTee )
                        {
                        DEBUG_ERR("Could not get tee-element from camera");
                        }
                    cameraBinGhostPad = gst_element_get_request_pad( camTee, "src%d" );
                    if ( !cameraBinGhostPad )
                        {
                        DEBUG_ERR("Could not get new src-pad from CamTee element");
                        }
                    gst_element_add_pad(ctx->videosource, gst_ghost_pad_new("MRObjSrc",cameraBinGhostPad));
                    ghost = gst_element_get_static_pad( GST_ELEMENT(ctx->videosource), "MRObjSrc" );
                    DEBUG_INFO_A2("Setting element:%s pad:%s to blocking.",
                            gst_element_get_name(ctx->baseObj.bin),
                            gst_pad_get_name(ghost));
                    /* Set newly created pad to blocking */
                    gst_pad_set_blocked_async(ghost, TRUE, XAAdaptationBase_PadBlockCb, NULL);

                    mrGhostSink = gst_element_get_static_pad( GST_ELEMENT(ctx->videofilter), "sink");
                    gst_element_add_pad(ctx->baseObj.bin, gst_ghost_pad_new("MRObjSink",mrGhostSink));
                    if ( !gst_element_link_pads( GST_ELEMENT(ctx->videosource), "MRObjSrc",
                                    GST_ELEMENT(ctx->baseObj.bin), "MRObjSink") )
                        {
                        DEBUG_ERR("Could not link camera:MRObjSrc to videofilter:MRObjSink");
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
                    if ( mrGhostSink )
                        {
                        gst_object_unref( mrGhostSink );
                        }
                    if ( camTee )
                        {
                        gst_object_unref( camTee );
                        }
                    }
                }
            /* create video processing pipeline */
#ifdef XA_IMPL_FIXED_VIDEO_SIZE
            ctx->videoppbin = XAAdaptationBase_CreateFixedSizeVideoPP( );
#else
            ctx->videoppbin = XAAdaptationBase_CreateVideoPP( );
#endif
            if( ctx->videoppbin )
                {
                gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->videoppbin);
                if(!gst_element_link(ctx->videofilter, ctx->videoppbin))
                    {
                    DEBUG_ERR("Could not link videofilter to videopp!!");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
                }
            else
                {
                DEBUG_ERR("Could not create video pp bin!!!!");
                return XA_RESULT_INTERNAL_ERROR;
                }
            /* create identity to extract buffers from */
            ctx->videoextract = gst_element_factory_make("identity", "videoextract");
            if( ctx->videoextract )
                {
                gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->videoextract);
                if(!gst_element_link(ctx->videoppbin, ctx->videoextract))
                    {
                    DEBUG_ERR("Could not link videopp to videoextract!!");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
                }
            else
                {
                DEBUG_ERR("Could not create videoextract!!!!");
                return XA_RESULT_INTERNAL_ERROR;
                }
            if( ctx->videoextract )
                {
                if( !gst_element_link_pads(ctx->videoextract, "src", ctx->codecbin, "v_sink") )
                    {
                    DEBUG_INFO("Warning: could not link videoextract to codec!!");
                    }
                }
            }
        else
            {
            DEBUG_INFO("No video input");
            }

        /* create and add audio pipeline */
        if ( ctx->xaAudioSource && gst_element_get_pad(ctx->codecbin, "sink") )
            {
            /* create audio pipe source */
            ctx->audiosource = XAAdaptationBase_CreateGstSource( ctx->xaAudioSource, "audiosource",
                    &(ctx->isobjasrc), NULL, NULL );
            if( ctx->audiosource )
                {
                if (gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->audiosource))
                    {
                    DEBUG_INFO("Added audiosource to bin");
                    }
                else
                    {
                    DEBUG_ERR("Could not add audiosource to bin");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
                }
            else
                {
                DEBUG_ERR("Could not create audio source!!!!");
                return XA_RESULT_INTERNAL_ERROR;
                }
            /* create audio processing pipeline */
            ctx->audioppbin = XAAdaptationBase_CreateAudioPP( );
            if( ctx->audioppbin )
                {
                if (gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->audioppbin))
                    {
                    DEBUG_INFO("Added audioppbin to bin");
                    }
                else
                    {
                    DEBUG_ERR("Could not add audioppbin to bin");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
/*                if(!gst_element_link(ctx->audiosource, ctx->audioppbin))
                    {
                    DEBUG_ERR("Could not link audiofilter to audiopp!!");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
*/                    
                    
                }
            else
                {
                DEBUG_ERR("Could not create audio pp bin!!!!");
                return XA_RESULT_INTERNAL_ERROR;
                }
            /* create audio filter for audio encoder settings */
            ctx->audiofilter = gst_element_factory_make("capsfilter", "audiofilter");
            if( ctx->audiofilter )
                {
                //   GstCaps* encSrcCaps; TL
                if (gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->audiofilter))
                    {
                    DEBUG_INFO("Added audiofilter to bin");
                    }
                else
                    {
                    DEBUG_ERR("Could not add audio filter to bin");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
                encSrcCaps = gst_caps_new_full(
                        gst_structure_new("audio/x-raw-int",
                                "width", G_TYPE_INT, 16,
                                "depth", G_TYPE_INT, 16,
                                "signed",G_TYPE_BOOLEAN, 1,
                                "endianness",G_TYPE_INT, 1234,
                                "rate", G_TYPE_INT, 16000,
                                "bitrate", G_TYPE_INT, ctx->audioEncSettings.bitRate,
                                "channels", G_TYPE_INT, 1, NULL),
                        /*gst_structure_new("audio/x-raw-int",
                         "channels", G_TYPE_INT, ctx->audioEncSettings.channelsOut,
                         "rate", G_TYPE_INT, ctx->audioEncSettings.sampleRate,
                         "bitrate", G_TYPE_INT, ctx->audioEncSettings.bitRate,
                         NULL),*/
                        gst_structure_new("audio/x-raw-float",
                                "channels", G_TYPE_INT, ctx->audioEncSettings.channelsOut,
                                "width", G_TYPE_INT, ctx->audioEncSettings.bitsPerSample,
                                "rate", G_TYPE_INT, ctx->audioEncSettings.sampleRate,
                                "bitrate", G_TYPE_INT, ctx->audioEncSettings.bitRate,
                                NULL),
                        NULL);
                DEBUG_INFO_A1("audio encoder config from settings: %s",gst_caps_to_string(encSrcCaps));
                g_object_set( G_OBJECT(ctx->audiofilter), "caps",encSrcCaps,NULL);

                if(!gst_element_link(ctx->audiosource, ctx->audiofilter))
                    {
                    DEBUG_ERR("Could not link audiosource to audiofilter!!");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
                    
                }
/*            if( !gst_element_link_pads_filtered(ctx->audiofilter, "src", ctx->codecbin, "sink", encSrcCaps) )
                {
                DEBUG_INFO("Warning: could not link audiopp to codec!!");
                return XA_RESULT_INTERNAL_ERROR;
                }
*/
                if (!gst_element_link_filtered( ctx->audiofilter , ctx->audioppbin ,encSrcCaps))
                    {
                    DEBUG_INFO("Warning: could not link audiosource to audiopp!!");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
                if(!gst_element_link(ctx->audioppbin, ctx->codecbin))
                    {
                    DEBUG_INFO("Warning: could not link audioppbin to codecbin!!");
                    return XA_RESULT_INTERNAL_ERROR;
                    }
            }
        else
            {
            DEBUG_INFO("No audio input");
            }
        gst_caps_unref(encSrcCaps);

        /* create and add data sink */
        ctx->datasink = XAAdaptationBase_CreateGstSink( ctx->xaSink, "datasink", &(ctx->isobjsink) );
        if( ctx->datasink )
            {
            if ( GST_IS_APP_SINK(ctx->datasink) )
                {
                gst_app_sink_set_emit_signals( GST_APP_SINK(ctx->datasink), TRUE );
                }
            if (gst_bin_add(GST_BIN(ctx->baseObj.bin), ctx->datasink))
                {
                DEBUG_INFO("Added datasink to bin");
                }
            else
                {
                DEBUG_ERR("Could not add datasink to bin");
                return XA_RESULT_INTERNAL_ERROR;
                }
            if(!gst_element_link(ctx->codecbin, ctx->datasink))
                {
                DEBUG_ERR("Could not link codec to sink!!");
                return XA_RESULT_INTERNAL_ERROR;
                }
            /* NOTE: no actual object sinks applicable, variable used to imply appsrc (recording to memory)*/
            if(ctx->isobjsink)
                {
                g_signal_connect(ctx->datasink, "new-buffer",
                        G_CALLBACK (XAMediaRecorderAdapt_BufferAvailable),ctx);

                ret = XAImpl_CreateSemaphore( &(ctx->recThrCtx.bufInsufficientSem));
                if ( ret != XA_RESULT_SUCCESS )
                    {
                    DEBUG_ERR("WARN: Could not create semaphore for recorder event handler!");
                    }
                XAImpl_CreateThreadHandle( &(ctx->recordingEventThr) );
                }
            }
        else
            {
            DEBUG_ERR("Could not create data sink!!!");
            return XA_RESULT_INTERNAL_ERROR;
            }
        DEBUG_API("<-XAMediaRecorderAdapt_CreatePipeline");
        return ret;
        }

    /*
     * XAresult XAMediaRecorderAdapt_CheckCodec( XAMediaRecorderAdaptationCtx_* mCtx );
     * Check codec compatibility and support with initiated datasink content
     */
    XAresult XAMediaRecorderAdapt_CheckCodec( XAMediaRecorderAdaptationCtx_* mCtx, XACapsType encType, XAuint32 encoderId )
        {
        XAresult ret = XA_RESULT_SUCCESS;
        XAuint32 format;
        XAStaticCapsData temp;

        DEBUG_API("->XAMediaRecorderAdapt_CheckCodec");

        /*first, check if codec supported at all*/
        ret = XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|encType, encoderId, &temp);

        if( ret==XA_RESULT_SUCCESS )
            {
            if(encType & (XACAP_VIDEO|XACAP_AUDIO))
                {
                if(mCtx->xaSink && mCtx->xaSink->pFormat)
                    {
                    format = *(XAuint32*)(mCtx->xaSink->pFormat);
                    }
                else
                    {
                    ret=XA_RESULT_FEATURE_UNSUPPORTED;
                    }
                }
            else
                {
                if(mCtx->snapshotVars.xaSink && mCtx->snapshotVars.xaSink->pFormat)
                    {
                    format = *(XAuint32*)(mCtx->snapshotVars.xaSink->pFormat);
                    }
                else
                    {
                    ret=XA_RESULT_FEATURE_UNSUPPORTED;
                    }
                }
            if(ret==XA_RESULT_SUCCESS) switch ( format )
                {
                case XA_DATAFORMAT_PCM:
                if ( (encType == XACAP_AUDIO) && (encoderId == XA_AUDIOCODEC_PCM) )
                    {
                    ret=XA_RESULT_SUCCESS;
                    }
                else
                    {
                    ret=XA_RESULT_FEATURE_UNSUPPORTED;
                    }
                break;

                case XA_DATAFORMAT_RAWIMAGE:
                if ( (encType == XACAP_IMAGE) && (encoderId == XA_IMAGECODEC_RAW) )
                    {
                    ret=XA_RESULT_SUCCESS;
                    }
                else
                    {
                    ret=XA_RESULT_FEATURE_UNSUPPORTED;
                    }
                break;

                case XA_DATAFORMAT_MIME:
                DEBUG_INFO("XA_DATAFORMAT_MIME ");
                    {
                    XADataFormat_MIME* mime = ((XADataFormat_MIME*)mCtx->xaSink->pFormat);
                    DEBUG_INFO_A1("mime->containerType:%u",(int)mime->containerType);
                    DEBUG_INFO_A1("mime->mimeType:%s",mime->mimeType);
                    switch ( mime->containerType )
                        {
                        case XA_CONTAINERTYPE_RAW:
                        if( ((encType == XACAP_AUDIO) && (encoderId == XA_AUDIOCODEC_PCM)) ||
                                ((encType == XACAP_VIDEO) && (encoderId == XA_ADAPTID_RAWVIDEO)) ||
                                ((encType == XACAP_IMAGE) && (encoderId == XA_IMAGECODEC_RAW)) )
                            {
                            ret=XA_RESULT_SUCCESS;
                            }
                        else
                            {
                            ret=XA_RESULT_FEATURE_UNSUPPORTED;
                            }
                        break;

                        case XA_CONTAINERTYPE_AVI:
                        if(encType == XACAP_VIDEO)
                            {
                            switch(encoderId)
                                {
                                case XA_ADAPTID_MOTIONJPEG:
                                case XA_ADAPTID_RAWVIDEO:
                                ret=XA_RESULT_SUCCESS;
                                break;
                                default:
                                ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                break;
                                }
                            }
                        else if(encType == XACAP_AUDIO)
                            {
                            switch(encoderId)
                                {
                                case XA_AUDIOCODEC_PCM:
                                ret=XA_RESULT_SUCCESS;
                                break;
                                default:
                                ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                break;
                                }
                            }
                        else
                            {
                            ret=XA_RESULT_FEATURE_UNSUPPORTED;
                            }
                        break;

                        case XA_CONTAINERTYPE_WAV:
                        if(encType == XACAP_AUDIO)
                            {
                            switch(encoderId)
                                {
                                case XA_AUDIOCODEC_PCM:
                                ret=XA_RESULT_SUCCESS;
                                break;
                                default:
                                ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                break;
                                }
                            }
                        else
                            {
                            ret=XA_RESULT_FEATURE_UNSUPPORTED;
                            }
                        break;

                        case XA_CONTAINERTYPE_JPG:
                        if(encType == XACAP_VIDEO)
                            {
                            switch(encoderId)
                                {
                                case XA_ADAPTID_MOTIONJPEG:
                                ret=XA_RESULT_SUCCESS;
                                break;
                                default:
                                ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                break;
                                }
                            }
                        else if(encType == XACAP_IMAGE)
                            {
                            switch(encoderId)
                                {
                                case XA_IMAGECODEC_JPEG:
                                ret=XA_RESULT_SUCCESS;
                                break;
                                default:
                                ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                break;
                                }
                            }
                        else
                            {
                            ret=XA_RESULT_FEATURE_UNSUPPORTED;
                            }
                        break;

                        case XA_CONTAINERTYPE_UNSPECIFIED:
                        if(strstr( (char *) mime->mimeType, "/ogg") != 0)
                            {
                            if(encType == XACAP_VIDEO)
                                {
                                switch(encoderId)
                                    {
                                    case XA_ADAPTID_THEORA:
                                    ret=XA_RESULT_SUCCESS;
                                    break;
                                    default:
                                    ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                    break;
                                    }
                                }
                            else if(encType == XACAP_AUDIO)
                                {
                                switch(encoderId)
                                    {
                                    case XA_ADAPTID_VORBIS:
                                    ret=XA_RESULT_SUCCESS;
                                    break;
                                    default:
                                    ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                    break;
                                    }
                                }
                            else
                                {
                                ret=XA_RESULT_FEATURE_UNSUPPORTED;
                                }
                            }
                        else
                            {
                            ret=XA_RESULT_FEATURE_UNSUPPORTED;
                            }
                        break;

                        default: /*switch (containertype)*/
                        ret = XA_RESULT_FEATURE_UNSUPPORTED;
                        break;
                        }
                    break;
                    }
                default: /*switch (format)*/
                ret = XA_RESULT_CONTENT_UNSUPPORTED;
                break;
                }
            }
        if( ret!=XA_RESULT_SUCCESS )
            {
            DEBUG_ERR("cannot accommodate given codec & datasink pair!!!");
            }
        DEBUG_API("<-XAMediaRecorderAdapt_CheckCodec");
        return ret;
        }
    /*
     * XAresult XAMediaRecorderAdapt_ChangeEncoders( XAMediaRecorderAdaptationCtx* mCtx );
     * re-create encodebin based on new encoder settings
     */
    XAresult XAMediaRecorderAdapt_ChangeEncoders( XAMediaRecorderAdaptationCtx* mCtx )
        {
        XAresult ret = XA_RESULT_SUCCESS;
        XAAdaptationBaseCtx* bCtx = &(mCtx->baseObj);

        DEBUG_API("->XAMediaRecorderAdapt_ChangeEncoders");
        /* check state */
        if(GST_STATE(mCtx->baseObj.bin)<GST_STATE_PLAYING)
            {
            GstElement* newBin = XAMediaRecorderAdapt_CreateEncodeBin(mCtx);
            if(newBin)
                { /* replace old bin with new */
                GstStateChangeReturn gret;
                GstPad *asink=NULL, *linkedasrc=NULL;
                GstPad *vsink=NULL, *linkedvsrc=NULL;
                GstPad *src=NULL, *linkedsink=NULL;
                GstPad *moSrc=NULL, *moSink=NULL;
                GstCaps* encSrcCaps = NULL;

                /* pipeline must be unrolled for renegotiation */
                DEBUG_INFO("Unroll pipeline");
                bCtx->binWantedState = GST_STATE_READY;
                gret = gst_element_set_state( GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
                gret = gst_element_get_state( GST_ELEMENT(bCtx->bin), NULL, NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);

                /*set new stream settings*/
                if( mCtx->videofilter )
                    {
                    encSrcCaps = gst_caps_new_simple("video/x-raw-yuv",
                            "format", GST_TYPE_FOURCC,GST_MAKE_FOURCC('I','4','2','0'),
                            "width", G_TYPE_INT, mCtx->videoEncSettings.width,
                            "height", G_TYPE_INT, mCtx->videoEncSettings.height,
                            "framerate", GST_TYPE_FRACTION, mCtx->videoEncSettings.frameRate, 1,
                            NULL);
                    DEBUG_INFO_A1("new video encoder config from settings: %s",gst_caps_to_string(encSrcCaps));
                    g_object_set( G_OBJECT(mCtx->videofilter), "caps",encSrcCaps,NULL);
                    gst_caps_unref(encSrcCaps);
                    }
                if( mCtx->audiofilter )
                    {
                    encSrcCaps = gst_caps_new_full(
                            gst_structure_new("audio/x-raw-int",
                                    "channels", G_TYPE_INT, mCtx->audioEncSettings.channelsOut,
                                    "rate", G_TYPE_INT, mCtx->audioEncSettings.sampleRate,
                                    "bitrate", G_TYPE_INT, mCtx->audioEncSettings.bitRate,
                                    NULL),
                            gst_structure_new("audio/x-raw-float",
                                    "channels", G_TYPE_INT, mCtx->audioEncSettings.channelsOut,
                                    "width", G_TYPE_INT, mCtx->audioEncSettings.bitsPerSample,
                                    "rate", G_TYPE_INT, mCtx->audioEncSettings.sampleRate,
                                    "bitrate", G_TYPE_INT, mCtx->audioEncSettings.bitRate,
                                    NULL),
                            NULL);
                    DEBUG_INFO_A1("new audio encoder config from settings: %s",gst_caps_to_string(encSrcCaps));
                    g_object_set( G_OBJECT(mCtx->audiofilter), "caps",encSrcCaps,NULL);
                    gst_caps_unref(encSrcCaps);
                    }

                if(mCtx->isobjvsrc)
                    {
                    moSrc = gst_element_get_static_pad(mCtx->videosource,"MRObjSrc");
                    moSink = gst_pad_get_peer(moSrc);
                    if(moSink)
                        {
                        gst_pad_unlink(moSrc,moSink);
                        }
                    moSrc = gst_element_get_static_pad(mCtx->videosource,"mediaobjectsrc");
                    encSrcCaps = gst_caps_new_simple("video/x-raw-yuv",
                            "format", GST_TYPE_FOURCC,GST_MAKE_FOURCC('I','4','2','0'),
                            "framerate", GST_TYPE_FRACTION, mCtx->videoEncSettings.frameRate, 1,
                            NULL);
                    DEBUG_INFO_A1("new camera encoding filter: %s",gst_caps_to_string(encSrcCaps));
                    g_object_set( G_OBJECT(mCtx->videosource), "filter-caps",encSrcCaps,NULL);
                    gst_caps_unref(encSrcCaps);
                    }

                DEBUG_INFO("Unlink and remove old encodebin");
                asink = gst_element_get_static_pad(mCtx->codecbin,"sink");
                if(asink)
                    {
                    linkedasrc = gst_pad_get_peer(asink);
                    if(linkedasrc)
                        {
                        gst_pad_unlink(linkedasrc,asink);
                        }
                    }
                vsink = gst_element_get_static_pad(mCtx->codecbin,"v_sink");
                if(vsink)
                    {
                    linkedvsrc = gst_pad_get_peer(vsink);
                    if(linkedvsrc)
                        {
                        gst_pad_unlink(linkedvsrc,vsink);
                        }
                    }
                src = gst_element_get_static_pad(mCtx->codecbin,"src");
                if(src)
                    {
                    linkedsink = gst_pad_get_peer(src);
                    if(linkedsink)
                        {
                        gst_pad_unlink(src,linkedsink);
                        }
                    }

                gst_element_set_state( GST_ELEMENT(mCtx->codecbin), GST_STATE_NULL );
                gst_element_get_state( GST_ELEMENT(mCtx->codecbin),NULL,NULL,1000 );
                gst_bin_remove( GST_BIN(bCtx->bin), mCtx->codecbin );
                /*reset filesink too*/
                gst_element_set_state(GST_ELEMENT(mCtx->datasink),GST_STATE_NULL);
                gst_element_sync_state_with_parent(mCtx->datasink);
                gst_element_get_state(mCtx->datasink,NULL,NULL,XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);

                DEBUG_INFO("Link new encodebin");
                mCtx->codecbin = newBin;
                gst_bin_add(GST_BIN(bCtx->bin), mCtx->codecbin);
                asink = gst_element_get_static_pad(mCtx->codecbin,"sink");
                if(asink && linkedasrc)
                    {
                    gst_pad_link(linkedasrc,asink);
                    }
                vsink = gst_element_get_static_pad(mCtx->codecbin,"v_sink");
                if(vsink && linkedvsrc)
                    {
                    gst_pad_link(linkedvsrc,vsink);
                    }
                src = gst_element_get_static_pad(mCtx->codecbin,"src");
                if(src && linkedsink)
                    {
                    gst_pad_link(src,linkedsink);
                    }

                if(mCtx->isobjvsrc)
                    {
                    moSrc = gst_element_get_static_pad(mCtx->videosource,"MRObjSrc");
                    if(moSink&&moSrc)
                        {
                        gst_pad_link(moSrc,moSink);
                        }
                    }

                /*re-roll*/
                DEBUG_INFO("Reroll pipeline");
                bCtx->binWantedState = GST_STATE_PAUSED;
                gret = gst_element_set_state( GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
                if( gret == GST_STATE_CHANGE_ASYNC )
                    {
                    DEBUG_INFO("Wait for reroll");
                    XAAdaptationBase_StartAsyncWait(bCtx);
                    }
                else if( gret == GST_STATE_CHANGE_FAILURE )
                    {
                    DEBUG_ERR("reroll FAILED");
                    ret = XA_RESULT_INTERNAL_ERROR;
                    }
                bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
                DEBUG_INFO_A1("Pipeline in state %s",gst_element_state_get_name(GST_STATE(bCtx->bin)));
                }
            else
                {
                /* could not support wanted encoders */
                DEBUG_ERR("Failed to create encodebin with new settings, using old one!");
                ret = XA_RESULT_FEATURE_UNSUPPORTED;
                }
            }
        else
            { /* n/a while playing */
            DEBUG_ERR("Cannot change encoder when recording ongoing!");
            ret = XA_RESULT_PRECONDITIONS_VIOLATED;
            }
        DEBUG_API("<-XAMediaRecorderAdapt_ChangeEncoders");
        return ret;
        }

    /**
     * GstElement* XAMediaRecorderAdapt_CreateEncodeBin( XAMediaRecorderAdaptationCtx* ctx )
     * @return GstElement* - pointer to created bin element
     * Decription: Create encoder/muxer element based on given format and encoder settings
     */
    GstElement* XAMediaRecorderAdapt_CreateEncodeBin( XAMediaRecorderAdaptationCtx* ctx )
        {
        XAresult ret = XA_RESULT_SUCCESS;
        GstElement *audioenc = NULL, *videoenc=NULL, *mux=NULL;
        GstElement *codecbin = gst_bin_new( "mrcodecbin" );
        GstPad *ghostsrc = NULL, *ghostaudiosink = NULL, *ghostvideosink = NULL;
        XAuint32 format;
        XAStaticCapsData temp;

        DEBUG_API("->XAMediaRecorderAdapt_CreateEncodeBin");
        if(ctx->recModes & XA_RECMODE_STREAM)
            {
            if(ctx->xaSink && ctx->xaSink->pFormat)
                {
                format = *(XAuint32*)(ctx->xaSink->pFormat);
                switch ( format )
                    {
                    case XA_DATAFORMAT_PCM:
                    DEBUG_INFO("XA_DATAFORMAT_PCM");
                        {
                        XADataFormat_PCM* pcm = ((XADataFormat_PCM*)ctx->xaSink->pFormat);
                        if(!ctx->xaAudioSource)
                            {
                            DEBUG_ERR("Unsupported dataformat for given data sources");
                            return NULL;
                            }
                        /* no need for codec, just pass data on */
                        mux = gst_element_factory_make("identity", "mux");
                        gst_bin_add(GST_BIN(codecbin), mux);
                        ghostsrc = gst_element_get_static_pad(mux,"src");
                        ghostaudiosink = gst_element_get_static_pad(mux,"sink");
                        /*set usable audio settings from the sink structure*/
                        ctx->audioEncSettings.encoderId = XA_AUDIOCODEC_PCM;
                        ctx->audioEncSettings.channelsOut = pcm->numChannels;
                        ctx->audioEncSettings.bitsPerSample = pcm->bitsPerSample;
                        }
                    break;

                    case XA_DATAFORMAT_RAWIMAGE:
                    DEBUG_INFO("XA_DATAFORMAT_RAWIMAGE");
                        {
                        XADataFormat_RawImage* img = ((XADataFormat_RawImage*)ctx->xaSink->pFormat);
                        if(!ctx->xaVideoSource)
                            {
                            DEBUG_ERR("Unsupported dataformat for given data sources");
                            return NULL;
                            }
                        /* no need for codec, just pass data on */
                        mux = gst_element_factory_make("identity", "mux");
                        gst_bin_add(GST_BIN(codecbin), mux);
                        ghostsrc = gst_element_get_static_pad(mux,"src");
                        ghostvideosink = gst_element_get_static_pad(mux,"sink");
                        /*set needed image settings from the sink structure*/
                        ctx->imageEncSettings.encoderId = XA_IMAGECODEC_RAW;
                        ctx->imageEncSettings.width = img->width;
                        ctx->imageEncSettings.height = img->height;
                        ctx->imageEncSettings.colorFormat = img->colorFormat;
                        }
                    break;

                    case XA_DATAFORMAT_MIME:
                    DEBUG_INFO("XA_DATAFORMAT_MIME ");
                        {
                        XADataFormat_MIME* mime = ((XADataFormat_MIME*)ctx->xaSink->pFormat);
                        DEBUG_INFO_A1("mime->containerType:%u",(int)mime->containerType);
                        DEBUG_INFO_A1("mime->mimeType:%s",mime->mimeType);
                        switch ( mime->containerType )
                            {
                            case XA_CONTAINERTYPE_AVI:
                            DEBUG_INFO("XA_CONTAINERTYPE_AVI");
                            mux = gst_element_factory_make("avimux", "mux");
                            if(mux)
                                {
                                if (gst_bin_add(GST_BIN(codecbin), mux))
                                    {
                                    DEBUG_INFO("Added mux to codecbin");
                                    }
                                else
                                    {
                                    DEBUG_ERR("Could not add mux to codecbin");
                                    return NULL;
                                    }
                                /*use this as linkpoint*/
                                ghostsrc = gst_element_get_static_pad(mux,"src");
                                }
                            /* Add and link audio/video codecs */
                            /*set video defaults*/
                            if(ctx->videoEncSettings.encoderId == XA_ADAPTID_UNINITED)
                            ctx->videoEncSettings.encoderId = XA_ADAPTID_MOTIONJPEG;
                            if(ctx->xaVideoSource)
                                {
                                if(XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_VIDEO, ctx->videoEncSettings.encoderId, &temp) == XA_RESULT_SUCCESS)
                                    {
                                    if(temp.adaptId)
                                        {
                                        videoenc = gst_element_factory_make((char*)temp.adaptId, "videoenc");
                                        }
                                    }
                                if(videoenc)
                                    {
                                    gst_bin_add(GST_BIN(codecbin), videoenc);
                                    if(!gst_element_link(videoenc, mux))
                                        {
                                        DEBUG_ERR("Could not link videoenc to mux!!");
                                        DEBUG_API("<-XAMediaRecorderAdapt_CreateEncodeBin");
                                        return NULL;
                                        }
                                    ghostvideosink = gst_element_get_static_pad(videoenc,"sink");
                                    }
                                else
                                    {
                                    /*no video codec but video source = raw video case, request video pad directly from mux*/
                                    ghostvideosink = gst_element_get_request_pad(mux,"video_%d");
                                    }
                                }
                            /*set audio defaults*/
                            if(ctx->audioEncSettings.encoderId == XA_ADAPTID_UNINITED)
                            ctx->audioEncSettings.encoderId = XA_AUDIOCODEC_PCM;
                            if(ctx->xaAudioSource)
                                {
                                if(XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_AUDIO, ctx->audioEncSettings.encoderId, &temp) == XA_RESULT_SUCCESS)
                                    {
                                    if(temp.adaptId)
                                        {
                                        audioenc = gst_element_factory_make((char*)temp.adaptId, "audioenc");
                                        }
                                    }
                                if(audioenc)
                                    {
                                    gst_bin_add(GST_BIN(codecbin), audioenc);
                                    if(!gst_element_link(audioenc, mux))
                                        {
                                        DEBUG_ERR("Could not link audioenc to mux!!");
                                        DEBUG_API("<-XAMediaRecorderAdapt_CreateEncodeBin");
                                        return NULL;
                                        }
                                    ghostaudiosink = gst_element_get_static_pad(audioenc,"sink");
                                    }
                                else
                                    {
                                    /*no audio codec but audio source = PCM case, explicity request audio pad*/
                                    ghostaudiosink = gst_element_get_request_pad(mux,"audio_%d");
                                    }
                                }
                            break;

                            case XA_CONTAINERTYPE_WAV:
                            DEBUG_INFO("XA_CONTAINERTYPE_WAV");
                            audioenc = gst_element_factory_make("wavenc", "audioenc");
                            if(audioenc)
                                {
                                if (gst_bin_add(GST_BIN(codecbin), audioenc))
                                    {
                                    DEBUG_INFO("added audioenc to codecbin");
                                    }
                                else
                                    {
                                    DEBUG_ERR("Could not add audioenc to codecbin");
                                    return NULL;
                                    }
                                /*use this as linkpoint*/
                                ghostsrc = gst_element_get_static_pad(audioenc,"src");
                                ghostaudiosink = gst_element_get_static_pad(audioenc,"sink");
                                if ( ghostsrc == NULL || ghostaudiosink == NULL)
                                    {
                                    DEBUG_ERR("Could not get src or sink ghoset element(s)");
                                    return NULL;
                                    }
                                }
                            /* no other codecs needed */
                            break;
                            case XA_CONTAINERTYPE_JPG:
                            /*motion jpeg*/
                            DEBUG_INFO("XA_CONTAINERTYPE_JPG");
                            /*set defaults*/
                            if(ctx->videoEncSettings.encoderId == XA_ADAPTID_UNINITED)
                            ctx->videoEncSettings.encoderId = XA_ADAPTID_MOTIONJPEG;
                            if(XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_VIDEO, ctx->videoEncSettings.encoderId, &temp) == XA_RESULT_SUCCESS)
                                {
                                videoenc = gst_element_factory_make((char*)temp.adaptId, "videoenc");
                                }
                            if(videoenc)
                                {
                                gst_bin_add(GST_BIN(codecbin), videoenc);
                                /*use this as linkpoint*/
                                ghostsrc = gst_element_get_static_pad(videoenc,"src");
                                ghostvideosink = gst_element_get_static_pad(videoenc,"sink");
                                }
                            /* no other codecs needed */
                            break;
                            case XA_CONTAINERTYPE_RAW:
                            DEBUG_INFO("XA_CONTAINERTYPE_RAW");
                            /* no need for codec, just pass data on */
                            if(strncmp((char *)mime->mimeType, "video", 5) == 0 && ctx->xaVideoSource)
                                {
                                mux = gst_element_factory_make("identity", "mux");
                                gst_bin_add(GST_BIN(codecbin), mux);
                                ghostvideosink = gst_element_get_static_pad(mux,"sink");
                                }
                            else if (strncmp((char *)mime->mimeType, "audio", 5) == 0 && ctx->xaAudioSource)
                                {
                                mux = gst_element_factory_make("identity", "mux");
                                gst_bin_add(GST_BIN(codecbin), mux);
                                ghostaudiosink = gst_element_get_static_pad(mux,"sink");
                                }
                            else
                                {
                                ret = XA_RESULT_CONTENT_UNSUPPORTED;
                                DEBUG_ERR("Content mismatch with given sources!!!")
                                }
                            ghostsrc = gst_element_get_static_pad(mux,"src");
                            break;
                            case XA_CONTAINERTYPE_UNSPECIFIED:
                            DEBUG_INFO("No support for requested encoder...try to select encoder from mime string");
                            if(strstr( (char *) mime->mimeType, "/ogg") != 0)
                                {
                                DEBUG_INFO("XA_CONTAINERTYPE_UNSPECIFIED - mimetype ogg detected");
                                mux = gst_element_factory_make("oggmux", "mux");
                                if(mux)
                                    {
                                    gst_bin_add(GST_BIN(codecbin), mux);
                                    /*use this as linkpoint*/
                                    ghostsrc = gst_element_get_static_pad(mux,"src");
                                    /*set defaults*/
                                    if(ctx->audioEncSettings.encoderId == XA_ADAPTID_UNINITED)
                                        {
                                        ctx->audioEncSettings.encoderId = XA_ADAPTID_VORBIS;
                                        ctx->audioEncSettings.bitsPerSample=32;
                                        }
                                    if(ctx->videoEncSettings.encoderId == XA_ADAPTID_UNINITED)
                                        {
                                        ctx->videoEncSettings.encoderId = XA_ADAPTID_THEORA;
                                        }
                                    if(ctx->xaAudioSource)
                                        {
                                        if(XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_AUDIO, ctx->audioEncSettings.encoderId, &temp) == XA_RESULT_SUCCESS)
                                            {
                                            audioenc = gst_element_factory_make((char*)temp.adaptId, "audioenc");
                                            }
                                        if(audioenc)
                                            {
                                            gst_bin_add(GST_BIN(codecbin), audioenc);
                                            gst_element_link(audioenc, mux);
                                            ghostaudiosink = gst_element_get_static_pad(audioenc,"sink");
                                            }
                                        }
                                    if(strncmp((char *)mime->mimeType, "video", 5) == 0 && ctx->xaVideoSource)
                                        {
                                        if(XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_VIDEO, ctx->videoEncSettings.encoderId, &temp) == XA_RESULT_SUCCESS)
                                            {
                                            videoenc = gst_element_factory_make((char*)temp.adaptId, "videoenc");
                                            }
                                        if(videoenc)
                                            {
                                            gst_bin_add(GST_BIN(codecbin), videoenc);
                                            gst_element_link(videoenc, mux);
                                            ghostvideosink = gst_element_get_static_pad(videoenc,"sink");
                                            }
                                        }
                                    }
                                }
                            else
                                {
                                DEBUG_INFO("No support for requested mime/container type.");
                                ret = XA_RESULT_CONTENT_UNSUPPORTED;
                                }
                            break;
                            case XA_CONTAINERTYPE_MOBILE_DLS:
                            case XA_CONTAINERTYPE_MP4:
                            case XA_CONTAINERTYPE_AMR:
                            case XA_CONTAINERTYPE_3GPP:
                            case XA_CONTAINERTYPE_BMP:
                            case XA_CONTAINERTYPE_ASF:
                            case XA_CONTAINERTYPE_M4A:
                            case XA_CONTAINERTYPE_MP3:
                            case XA_CONTAINERTYPE_JPG2000:
                            case XA_CONTAINERTYPE_MPEG_ES:
                            case XA_CONTAINERTYPE_MPEG_PS:
                            case XA_CONTAINERTYPE_MPEG_TS:
                            case XA_CONTAINERTYPE_QT:
                            case XA_CONTAINERTYPE_XMF_0:
                            case XA_CONTAINERTYPE_XMF_1:
                            case XA_CONTAINERTYPE_XMF_2:
                            case XA_CONTAINERTYPE_XMF_3:
                            case XA_CONTAINERTYPE_XMF_GENERIC:
                            case XA_CONTAINERTYPE_AAC:
                            case XA_CONTAINERTYPE_3GA:
                            case XA_CONTAINERTYPE_RM:
                            case XA_CONTAINERTYPE_DMF:
                            default:
                            DEBUG_INFO("No support for requested container type.");
                            ret = XA_RESULT_CONTENT_UNSUPPORTED;
                            break;
                            }
                        break;
                        }
                    default:
                    DEBUG_ERR("Incorrect data format type.");
                    ret = XA_RESULT_PARAMETER_INVALID;
                    break;
                    }
                }
            else
                {
                DEBUG_ERR("Invalid data sink for stream recording!!");
                ret = XA_RESULT_PARAMETER_INVALID;
                }
            }
        else
            {/* stream recording not requested, datasink ignored, use uncoded recordstream*/
            mux = gst_element_factory_make("identity", "mux");
            gst_bin_add(GST_BIN(codecbin), mux);
            ghostsrc = gst_element_get_static_pad(mux,"src");
            ghostvideosink = gst_element_get_static_pad(mux,"sink");
            }

        /*set default codecs for unrecognized*/
        if(ctx->audioEncSettings.encoderId == XA_ADAPTID_UNINITED)
        ctx->audioEncSettings.encoderId = XA_AUDIOCODEC_PCM;
        if(ctx->imageEncSettings.encoderId == XA_ADAPTID_UNINITED)
        ctx->imageEncSettings.encoderId = XA_IMAGECODEC_RAW;
        if(ctx->videoEncSettings.encoderId == XA_ADAPTID_UNINITED)
        ctx->videoEncSettings.encoderId = XA_ADAPTID_RAWVIDEO;

        if ( ret != XA_RESULT_SUCCESS )
            {
            gst_object_unref(codecbin);
            codecbin=NULL;
            }
        else
            {
            /*add ghost pad(s) to link to*/
            if(ghostsrc)
                {
                gst_element_add_pad(codecbin, gst_ghost_pad_new("src",ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
                }
            if(ghostaudiosink)
                {
                gst_element_add_pad(codecbin, gst_ghost_pad_new("sink",ghostaudiosink));
                gst_object_unref(GST_OBJECT(ghostaudiosink));
                }
            if(ghostvideosink)
                {
                gst_element_add_pad(codecbin, gst_ghost_pad_new("v_sink",ghostvideosink));
                gst_object_unref(GST_OBJECT(ghostvideosink));
                }
            DEBUG_INFO_A1("Created encoder bin at %x", (int)codecbin);
            }

        DEBUG_API("<-XAMediaRecorderAdapt_CreateEncodeBin");
        return codecbin;

        }

    /*
     * void XAMediaRecorderAdapt_BufferAvailable(GstElement* sink, gpointer user_data)
     * called when new buffer is available at appsink
     */
    void XAMediaRecorderAdapt_BufferAvailable(GstElement* sink, gpointer user_data)
        {
        GstBuffer *buffer=NULL;
        XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*)user_data;
        DEBUG_API("->XAMediaRecorderAdapt_BufferAvailable");
        if(!mCtx || !mCtx->xaSink)
            {
            DEBUG_ERR("Invalid context")
            return;
            }
        /* get the buffer */
        buffer = gst_app_sink_pull_buffer(GST_APP_SINK(sink));
        if(buffer)
            {
            guint size;
            XADataLocator_Address* address;
            size = GST_BUFFER_SIZE(buffer);
            DEBUG_INFO_A1("Pulled new buffer of size %d", size);
            address = (XADataLocator_Address*)(mCtx->xaSink->pLocator);
            if( !address || *(XAuint32*)address != XA_DATALOCATOR_ADDRESS )
                {
                DEBUG_ERR("Invalid address datalocator")
                return;
                }

            if(mCtx->writepos + size < address->length )
                { /*enough room in buffer*/
                memcpy(((char*)(address->pAddress) + mCtx->writepos),
                        GST_BUFFER_DATA (buffer), size);
                mCtx->writepos+=size;
                }
            else
                { /*not enough room in buffer*/
                XAAdaptEvent event =
                    {XA_RECORDITFEVENTS, XA_RECORDEVENT_BUFFER_FULL, 0, NULL};

                size = address->length - mCtx->writepos;
                memcpy(((char*)(address->pAddress) + mCtx->writepos),
                        GST_BUFFER_DATA (buffer), size);
                DEBUG_INFO_A1("Buffer insufficient, wrote %d bytes", size);
                /* send event */
                XAAdaptationBase_SendAdaptEvents(&(mCtx->baseObj), &event);
                /* "recordhead to start" i.e. reset write position */
                mCtx->writepos=0;
                mCtx->recThrCtx.buffer_insufficient = XA_BOOLEAN_TRUE;
                if ( XAImpl_PostSemaphore( mCtx->recThrCtx.bufInsufficientSem ) != XA_RESULT_SUCCESS)
                    {
                    DEBUG_ERR("Posting buffer-insufficien semaphore FAILED!");
                    }
                }
            gst_buffer_unref (buffer);
            }
        else
            {
            DEBUG_ERR("Could not pull buffer from appsink!");
            }
        DEBUG_API("<-XAMediaRecorderAdapt_BufferAvailable");
        }

    /*
     * XAresult XAMediaRecorderAdapt_InitContentPipeSink(ctx);
     */
    XAresult XAMediaRecorderAdapt_InitContentPipeSink(XAMediaRecorderAdaptationCtx* ctx)
        {
        XAresult ret = XA_RESULT_SUCCESS;
        CPresult res;
        GstStateChangeReturn gstRet = GST_STATE_CHANGE_SUCCESS;
        DEBUG_API("->XAMediaRecorderAdapt_InitContentPipeSink");
        ctx->baseObj.pipeSinkThrCtx.appSink = GST_APP_SINK(ctx->datasink);
        ctx->baseObj.pipeSinkThrCtx.pipe = (XADataLocator_ContentPipe*)(ctx->xaSink->pLocator);

        /* Create thread for content pipe source */
        ret = XAImpl_CreateThreadHandle( &(ctx->baseObj.pipeSinkThr) );
        if ( ret != XA_RESULT_SUCCESS )
            {
            DEBUG_ERR("Could not create thread for content pipe sink!");
            DEBUG_API("<-XAMediaRecorderAdapt_InitContentPipeSink");
            return ret;
            }

        /* Create semaphore for content pipe source */
        ret = XAImpl_CreateSemaphore( &(ctx->baseObj.pipeSinkThrCtx.stateSem));
        if ( ret != XA_RESULT_SUCCESS )
            {
            DEBUG_ERR("Could not create semaphore for content pipe source!");
            DEBUG_API("<-XAMediaRecorderAdapt_InitContentPipeSink");
            return ret;
            }

        /* Open content pipe */
        res = ctx->baseObj.pipeSinkThrCtx.pipe->pContentPipe->Open(&(ctx->baseObj.pipeSinkThrCtx.dataHandle),
                (CPstring)(ctx->baseObj.pipeSinkThrCtx.pipe->URI),
                CP_AccessWrite );
        if ( res == EXIT_FAILURE )
            { /* Could not open content pipe data handle, try to create new one */
            res = ctx->baseObj.pipeSinkThrCtx.pipe->pContentPipe->Create(&(ctx->baseObj.pipeSinkThrCtx.dataHandle),
                    (CPstring)(ctx->baseObj.pipeSinkThrCtx.pipe->URI));
            if ( res == EXIT_FAILURE)
                {
                DEBUG_ERR("Could not create Content Pipe data handle!")
                return XA_RESULT_INTERNAL_ERROR;
                }
            res = ctx->baseObj.pipeSinkThrCtx.pipe->pContentPipe->Open(&(ctx->baseObj.pipeSinkThrCtx.dataHandle),
                    (CPstring)(ctx->baseObj.pipeSinkThrCtx.pipe->URI),
                    CP_AccessWrite );
            if ( res == EXIT_FAILURE)
                {
                DEBUG_ERR("Could not open newly created Content Pipe data handle!")
                return XA_RESULT_INTERNAL_ERROR;
                }
            }

        res = ctx->baseObj.pipeSinkThrCtx.pipe->pContentPipe->RegisterCallback( &(ctx->baseObj.pipeSinkThrCtx.dataHandle), &XAAdaptationBase_ContentPipeSinkCb);
        if ( res == EXIT_FAILURE )
            {
            DEBUG_ERR("Could not register content pipe callbacks!")
            return XA_RESULT_INTERNAL_ERROR;
            }

        gstRet = gst_element_set_state( GST_ELEMENT(ctx->datasink), GST_STATE_PAUSED);

        ctx->baseObj.pipeSinkThrCtx.state = CPStateStarted;
        XAImpl_StartThread( &(ctx->baseObj.pipeSrcThr), NULL, &XAAdaptationBase_ContentPipeSinkThrFunc, &(ctx->baseObj.pipeSinkThrCtx) );

        DEBUG_API("<-XAMediaRecorderAdapt_InitContentPipeSink");
        return ret;
        }

    /*
     * void* XAMediaRecorderAdapt_RecordEventThr( void* ctx )
     */
    void* XAMediaRecorderAdapt_RecordEventThr( void* ctx )
        {
        XAMediaRecorderAdaptationCtx* mrCtx = (XAMediaRecorderAdaptationCtx*)ctx;
        GstStateChangeReturn gret;
        XAresult ret;
        DEBUG_API("->XAMediaRecorderAdapt_RecordEventThr");

        /* Wait semaphore here */
        ret = XAImpl_WaitSemaphore( mrCtx->recThrCtx.bufInsufficientSem );
        if ( ret != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("Could not start semaphore");
            }

        if(mrCtx->recThrCtx.buffer_insufficient)
            {
            mrCtx->baseObj.binWantedState = GST_STATE_PAUSED;
            XAAdaptationBase_PrepareAsyncWait(&(mrCtx->baseObj));
            gret = gst_element_set_state( GST_ELEMENT(mrCtx->baseObj.bin), mrCtx->baseObj.binWantedState);
            if( gret == GST_STATE_CHANGE_ASYNC )
                {
                DEBUG_INFO("Start to wait recoder state change.");
                XAAdaptationBase_StartAsyncWait(&(mrCtx->baseObj));
                DEBUG_INFO("Recorder state change async. SUCCESFULL.");
                }
            else if( gret == GST_STATE_CHANGE_FAILURE )
                {
                DEBUG_INFO("Recorder state change FAILED");
                /*ret = XA_RESULT_INTERNAL_ERROR;*/
                }
            else
                {
                DEBUG_INFO("Recorder state change SUCCESFULL")
                }

            mrCtx->baseObj.waitingasyncop= XA_BOOLEAN_FALSE;
            }
        DEBUG_API("<-XAMediaRecorderAdapt_RecordEventThr");
        return NULL;
        }
