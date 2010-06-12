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
 * Description: Base GStreamer Adaptation Source
 *
 */

#include <assert.h>
#include <string.h>
#include <gst/gst.h>
#include <gst/gstbuffer.h>
#include "xaobjectitf.h"
#include "xacameradevice.h"
#include "xaradiodevice.h"
#include "xaoutputmix.h"
#include "xamediaplayeradaptctx.h"
#include "xacameraadaptctx.h"
#include <stdlib.h>

#ifdef OMAX_CAMERABIN
extern XAboolean cameraRealized;

extern XACameraAdaptationCtx_* cameraCtx;
#else
XAboolean cameraRealized = XA_BOOLEAN_FALSE;
XACameraAdaptationCtx_* cameraCtx = NULL;
#endif
/*
 * XAAdaptationGstCtx* XAAdaptationGst_Create()
 * 1st phase initialization function for Adaptation Base context structure.
 * Reserves memory for base context and initializes GStreamer FW.
 */
XAresult XAAdaptationGst_Init(XAAdaptationGstCtx* pSelf, XAuint32 ctxId)
    {
    DEBUG_API("->XAAdaptationGst_Init");

    if (pSelf)
        {
        if (XAAdaptationBase_Init(&(pSelf->baseObj), ctxId)
                != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
            }
        else
            {
            GError* gerror = 0;

            /*         Add default handler for Gst-bus messages */
            pSelf->busCb = XAAdaptationGst_GstBusCb;

            // VASU MOD BEGINS
            pSelf->cond_mutx_inited = XA_BOOLEAN_FALSE;
            // VASU MOD ENDS

            sem_init(&(pSelf->semAsyncWait), 0, 0);

            if (!gst_init_check(NULL, NULL, &gerror))
                {
                DEBUG_ERR("Gst Initalization failure.");
                return XA_RESULT_INTERNAL_ERROR;
                }

            }
        }
    else
        {
        DEBUG_ERR("Invalid Gst Base Context.")
        return XA_RESULT_PARAMETER_INVALID;
        }

    DEBUG_API("<-XAAdaptationGst_Init");
    return XA_RESULT_SUCCESS;
    }

/*
 * XAresult XAAdaptationGst_PostInit()
 * 2nd phase initialization for Adaptation Base.
 */
XAresult XAAdaptationGst_PostInit(XAAdaptationGstCtx* ctx)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAAdaptationGst_PostInit");
    // VASU MOD BEGINS
    XAAdaptationBase_PostInit(&ctx->baseObj);

    ctx->thread_launched = XA_BOOLEAN_FALSE;
    pthread_mutex_init(&(ctx->ds_mutex), NULL);
    pthread_cond_init(&(ctx->ds_condition), NULL);
    ctx->cond_mutx_inited = XA_BOOLEAN_TRUE;

    // VASU MOD ENDS
    DEBUG_API("<-XAAdaptationGst_PostInit");
    return ret;
    }

/*
 * void XAAdaptationGst_Free( XAAdaptationGstCtx* ctx )
 * Frees all Base context variables .
 */
void XAAdaptationGst_Free(XAAdaptationGstCtx* ctx)
    {
    GstElement* fakesink = NULL;
    DEBUG_API("->XAAdaptationGst_Free");

    if (ctx->bin)
        {
        fakesink = gst_bin_get_by_name(GST_BIN(ctx->bin), "fakesink");
        if (fakesink)
            {
            gst_element_set_state(GST_ELEMENT(fakesink), GST_STATE_NULL);
            gst_object_unref(fakesink);
            }

        if (gst_element_set_state(GST_ELEMENT(ctx->bin), GST_STATE_NULL)
                != GST_STATE_CHANGE_SUCCESS)
            { /*not much we can do*/
            DEBUG_ERR("WARNING: Failed to change to NULL state before deletion!!")
            }
        }

    if (ctx->asynctimer)
        { /*cancel timer*/
        g_source_remove(ctx->asynctimer);
        }
    sem_post(&(ctx->semAsyncWait));
    sem_destroy(&(ctx->semAsyncWait));

    XAAdaptationGst_StopGstListener(ctx);

    if (ctx->bin)
        {
        gst_object_unref(ctx->bin);
        }

    //g_array_free(ctx->evtHdlrs, TRUE);
    // VASU MOD BEGINS
    if (ctx->cond_mutx_inited == XA_BOOLEAN_TRUE)
        {
        ctx->thread_launched = XA_BOOLEAN_FALSE;
        pthread_mutex_destroy(&(ctx->ds_mutex));
        pthread_cond_destroy(&(ctx->ds_condition));
        ctx->cond_mutx_inited = XA_BOOLEAN_FALSE;
        }
    // VASU MOD ENDS
    XAAdaptationBase_Free(&ctx->baseObj);

    DEBUG_API("<-XAAdaptationGst_Free");
    }

/*
 * gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
 * Default Gst-bus message handler (Callback)
 */
gboolean XAAdaptationGst_GstBusCb(GstBus *bus, GstMessage *message,
        gpointer data)
    {
    GError *error;
    gchar *debug;
    DEBUG_API("->XAAdaptationGst_GstBusCb");DEBUG_INFO_A2("Received Gst callback \"%s\" from \"%s\"",
            GST_MESSAGE_TYPE_NAME(message),
            GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));

    switch (GST_MESSAGE_TYPE(message))
        {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(message, &error, &debug);
            DEBUG_INFO_A1("%s", debug)
            ;
            break;
        case GST_MESSAGE_EOS:
            break;
        case GST_MESSAGE_UNKNOWN:
            break;
        case GST_MESSAGE_WARNING:
            break;
        case GST_MESSAGE_INFO:
            break;
        case GST_MESSAGE_TAG:
            break;
        case GST_MESSAGE_BUFFERING:
            break;
        case GST_MESSAGE_STATE_CHANGED:
            break;
        case GST_MESSAGE_STATE_DIRTY:
            break;
        case GST_MESSAGE_STEP_DONE:
            break;
        case GST_MESSAGE_CLOCK_PROVIDE:
            break;
        case GST_MESSAGE_CLOCK_LOST:
            break;
        case GST_MESSAGE_NEW_CLOCK:
            break;
        case GST_MESSAGE_STRUCTURE_CHANGE:
            break;
        case GST_MESSAGE_STREAM_STATUS:
            break;
        case GST_MESSAGE_APPLICATION:
            break;
        case GST_MESSAGE_ELEMENT:
            break;
        case GST_MESSAGE_SEGMENT_START:
            break;
        case GST_MESSAGE_SEGMENT_DONE:
            break;
        case GST_MESSAGE_DURATION:
            break;
        case GST_MESSAGE_LATENCY:
            break;
        case GST_MESSAGE_ASYNC_START:
            break;
        case GST_MESSAGE_ASYNC_DONE:
            break;
        case GST_MESSAGE_ANY:
            break;
        default:
            DEBUG_INFO("Unhandled Gst-Bus message")
            ;
            break;
        }DEBUG_API("<-XAAdaptationGst_GstBusCb");
    return TRUE;
    }

XAresult XAAdaptationGst_InitGstListener(XAAdaptationGstCtx* ctx)
    {
    int ret;
    DEBUG_API("->XAAdaptationGst_InitGstListener");
    if (ctx->bin)
        {
        ctx->bus = gst_pipeline_get_bus(GST_PIPELINE( ctx->bin ));
        }
    if (!ctx->bus)
        {
        DEBUG_ERR("could not get gst bus!")
        return XA_RESULT_INTERNAL_ERROR;
        }
    ret = pthread_create(&(ctx->busloopThr), NULL,
            (XAAdaptationGst_LaunchGstListener), (void*) ctx);
    // VASU MOD BEGINS
    if (ctx->thread_launched == XA_BOOLEAN_FALSE)
        {
        // Wait until the thread is created
        pthread_mutex_lock(&(ctx->ds_mutex));
        pthread_cond_wait(&(ctx->ds_condition), &(ctx->ds_mutex));
        pthread_mutex_unlock(&(ctx->ds_mutex));
        // VASU MOD ENDS
        }
    if (ret)
        {
        DEBUG_ERR_A1("could not create thread!! (%d)",ret)
        return XA_RESULT_INTERNAL_ERROR;
        }DEBUG_API("<-XAAdaptationGst_InitGstListener");
    return XA_RESULT_SUCCESS;
    }

void * XAAdaptationGst_LaunchGstListener(void* args)
    {
    XAAdaptationGstCtx* ctx = (XAAdaptationGstCtx*) args;
    DEBUG_API("->XAAdaptationGst_LaunchGstListener");
    // VASU MOD BEGINS
    // Signal calling thread that this thread creation is completed
    ctx->thread_launched = XA_BOOLEAN_TRUE;
    pthread_mutex_lock(&(ctx->ds_mutex));
    pthread_cond_signal(&(ctx->ds_condition));
    pthread_mutex_unlock(&(ctx->ds_mutex));
    // VASU MOD ENDS

    ctx->busloop = g_main_loop_new(NULL, FALSE);
    if (!ctx->busloop)
        {
        DEBUG_ERR("Glib main loop failure.");
        DEBUG_API("<-XAAdaptationGst_LaunchGstListener");
        assert(0);
        }
    else
        {
        DEBUG_INFO("Start Glib main loop")
        g_main_loop_run(ctx->busloop);
        DEBUG_INFO("Glib main loop stopped - exiting thread");
        DEBUG_API("<-XAAdaptationGst_LaunchGstListener");
        pthread_exit(NULL);
        }
    return NULL;
    }

void XAAdaptationGst_StopGstListener(XAAdaptationGstCtx* ctx)
    {
    DEBUG_API("->XAAdaptationGst_StopGstListener");
    if (ctx->busloop)
        {
        g_main_loop_quit(ctx->busloop);
        g_main_loop_unref(ctx->busloop);
        }
    if (ctx->bus)
        {
        gst_object_unref(ctx->bus);
        ctx->bus = NULL;
        }DEBUG_API("<-XAAdaptationGst_StopGstListener");
    }

/*
 * ASynchronous operation managing
 **/

/* NOTE: This should NOT be called from gst callbacks - danger of deadlock!!
 */
void XAAdaptationGst_PrepareAsyncWait(XAAdaptationGstCtx* ctx)
    {
    DEBUG_API("->XAAdaptationGst_PrepareAsyncWait");

    if (ctx->waitingasyncop)
        { /*wait previous async op*/
        DEBUG_INFO("::WARNING:: previous asynch still ongoing!!!");
        DEBUG_INFO(">>>>  WAIT PREVIOUS");
        sem_wait(&(ctx->semAsyncWait));
        DEBUG_INFO("<<<<  PREVIOUS COMPLETED");
        }
    sem_init(&(ctx->semAsyncWait), 0, 0);

    ctx->waitingasyncop = XA_BOOLEAN_TRUE;
    DEBUG_API("<-XAAdaptationGst_PrepareAsyncWait");
    }

void XAAdaptationGst_StartAsyncWait(XAAdaptationGstCtx* ctx)
    {
    DEBUG_API("->XAAdaptationGst_StartAsyncWait");

    /* timeout to try to avoid gst freeze in rollup */
    ctx->asynctimer = g_timeout_add(XA_ADAPT_ASYNC_TIMEOUT,
            XAAdaptationGst_CancelAsyncWait, ctx);
    /* check flag once again if callback already happened before wait */
    if (ctx->waitingasyncop)
        {
        DEBUG_INFO(">>>>  ASYNC STARTS");
        sem_wait(&(ctx->semAsyncWait));
        DEBUG_INFO("<<<<  ASYNC COMPLETED");
        }
    else
        {
        DEBUG_INFO("<> async completed already");
        }
    /*cancel timer*/
    if (ctx->asynctimer)
        {
        g_source_remove(ctx->asynctimer);
        }
    ctx->waitingasyncop = XA_BOOLEAN_FALSE;

    DEBUG_API("<-XAAdaptationGst_StartAsyncWait");
    }

/* async operation timeout callback*/
gboolean XAAdaptationGst_CancelAsyncWait(gpointer ctx)
    {
    XAAdaptationGstCtx* bCtx = (XAAdaptationGstCtx*) ctx;
    DEBUG_API("->XAAdaptationGst_CancelAsyncWait");
    if (bCtx->waitingasyncop)
        {
        DEBUG_ERR_A3("ASYNC TIMED OUT : current %d, gsttarget %d, wanted %d",
                GST_STATE(bCtx->bin), GST_STATE_TARGET(bCtx->bin), bCtx->binWantedState);
        bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
        sem_post(&(bCtx->semAsyncWait));
        }DEBUG_API("<-XAAdaptationGst_CancelAsyncWait");
    /* return false to remove timer */
    return FALSE;
    }

void XAAdaptationGst_CompleteAsyncWait(XAAdaptationGstCtx* ctx)
    {
    DEBUG_API("->XAAdaptationGst_CompleteAsyncWait");
    if (ctx->waitingasyncop)
        {
        int i;
        ctx->waitingasyncop = XA_BOOLEAN_FALSE;
        sem_getvalue(&(ctx->semAsyncWait), &i);
        DEBUG_INFO_A1("Asynch operation succeeded, sem value %d",i);
        if (i <= 0)
            { /* only post if locked */
            sem_post(&(ctx->semAsyncWait));
            }
        else if (i > 0)
            { /* should not be, reset semaphore */
            sem_init(&(ctx->semAsyncWait), 0, 0);
            }
        }DEBUG_API("<-XAAdaptationGst_CompleteAsyncWait");
    }

/**
 * GstElement* XAAdaptationGst_CreateGstSource( XADataSource* xaSrc, const XAchar *name )
 * @param XADataSource* xaSnk - XADataSource defining gst source to create
 * @param const XAchar *name - string for naming the gst element
 * @param XAboolean *isobj - (out param) is source another XA object?
 * @return GstElement* - return newly created gst source element
 * Description: Create gst source element corresponding to XA source structure
 */
GstElement* XAAdaptationGst_CreateGstSource(XADataSource* xaSrc,
        const char *name, XAboolean *isobj, XAboolean *isPCM,
        XAboolean *isRawImage)
    {
    XAuint32 locType = 0;
    GstElement* gstSrc = NULL;
    char* fname = NULL;
    XADataLocator_URI* uri = NULL;
    XADataLocator_IODevice* ioDevice = NULL;
#ifdef OMAX_CAMERABIN
    XACameraDeviceImpl* cameraDevice = NULL;
    XAObjectItfImpl* pObj = NULL;
#endif
    XARadioDeviceImpl* radioDevice = NULL;

    DEBUG_API("->XAAdaptationGst_CreateGstSource");
    if (!xaSrc || !xaSrc->pLocator || !isobj)
        {
        return NULL;
        }
    *isobj = XA_BOOLEAN_FALSE;
    if (xaSrc && xaSrc->pFormat && *((XAuint32*) (xaSrc->pFormat))
            ==XA_DATAFORMAT_PCM && isPCM)
        {
        *isPCM = XA_BOOLEAN_TRUE;
        }
    if (xaSrc && xaSrc->pFormat && *((XAuint32*) (xaSrc->pFormat))
            ==XA_DATAFORMAT_RAWIMAGE && isRawImage)
        {
        *isRawImage = XA_BOOLEAN_TRUE;
        }
    locType = *((XAuint32*) (xaSrc->pLocator));
    switch (locType)
        {
        case XA_DATALOCATOR_URI:
            DEBUG_INFO("XA_DATALOCATOR_URI")
            ;
            uri = (XADataLocator_URI*) xaSrc->pLocator;
            gstSrc = gst_element_factory_make("filesrc", name);
            if (uri->URI != NULL)
                {
                DEBUG_INFO_A1("URI: %s", uri->URI);
                if (strncmp((char *) uri->URI, "file:///", 8) == 0)
                    {
                    fname = (char *) &((uri->URI)[8]);
                    }
                else
                    {
                    fname = (char *) uri->URI;
                    }DEBUG_INFO_A1("->filesystem path %s", fname);
                g_object_set(G_OBJECT(gstSrc), "location", fname, NULL);
                /*check for pcm - decodebin does not know how to handle raw PCM files */
                if (isPCM && strstr(fname, ".pcm"))
                    {
                    DEBUG_INFO("PCM file detected");
                    *isPCM = XA_BOOLEAN_TRUE;
                    }
                }
            else
                {
                DEBUG_ERR("No uri specified.");
                return NULL;
                }
            break; /* XA_DATALOCATOR_URI */

        case XA_DATALOCATOR_IODEVICE:
            DEBUG_INFO("XA_DATALOCATOR_IODEVICE")
            ;
            ioDevice = (XADataLocator_IODevice*) (xaSrc->pLocator);
            switch (ioDevice->deviceType)
                {
                case XA_IODEVICE_AUDIOINPUT:
                    {
                    DEBUG_INFO("XA_IODEVICE_AUDIOINPUT");
                    DEBUG_INFO_A1("ioDevice->deviceID: %x", ioDevice->deviceID);
                    switch (ioDevice->deviceID)
                        {
                        //case XA_ADAPTID_ALSASRC: //Krishna
                        case XA_ADAPTID_DEVSOUNDSRC:
                            //DEBUG_INFO("alsasrc"); //Krishna
                            DEBUG_INFO("devsoundsrc")
                            ;
                            gstSrc = gst_element_factory_make("devsoundsrc",
                                    name); //Krishna - changed to devsoundsrc
                            g_object_set(G_OBJECT (gstSrc), "num-buffers",
                                    80, NULL);
                            break;
                        case XA_ADAPTID_AUDIOTESTSRC:
                            /*fall through*/
                        default:
                            DEBUG_INFO("audiotestsrc")
                            ;
                            gstSrc = gst_element_factory_make("audiotestsrc",
                                    name);
                            break;
                        }
                    break;
                    }
#ifdef OMAX_CAMERABIN

                    case XA_IODEVICE_CAMERA:
                        {
                        DEBUG_INFO("XA_IODEVICE_CAMERA");
                        if ( ioDevice->device )
                            { /*source is camera object*/
                            DEBUG_INFO("Use camerabin as source.");
                            /* Get camerabin from source object */
                            pObj = (XAObjectItfImpl*)(*ioDevice->device);
                            cameraDevice = (XACameraDeviceImpl*)(pObj);
                            /*TODO we had to remove this line below since adaptationCtx
                             * was  not a member of structure*/

                            gstSrc = GST_ELEMENT(((XAAdaptationGstCtx*)(cameraDevice->adaptationCtx))->bin);
                            /* refcount increase is needed to keep this not being deleted after use */
                            gst_object_ref(GST_OBJECT(gstSrc));
                            *isobj = XA_BOOLEAN_TRUE;
                            }
                        else
                            {
                            DEBUG_INFO_A1("ioDevice->deviceID: %x", ioDevice->deviceID);
                            switch (ioDevice->deviceID )
                                {
                                case XA_ADAPTID_V4L2SRC:
                                DEBUG_INFO("Camera deviceID: v4l2src ");

                                break;
                                case XA_ADAPTID_VIDEOTESTSRC:
                                DEBUG_INFO("Camera deviceID: videotestsrc");

                                break;
                                default:
                                case XA_DEFAULTDEVICEID_CAMERA:
                                DEBUG_INFO("Camera deviceID:Default");

                                break;
                                }
                            if ( cameraCtx )
                                {
                                gstSrc = GST_ELEMENT(cameraCtx->baseObj.bin);
                                gst_object_ref(GST_OBJECT(gstSrc));
                                *isobj = XA_BOOLEAN_TRUE;
                                }
                            else
                                {
                                DEBUG_ERR("No camera object created!");
                                return NULL;
                                }
                            }
                        break;
                        }
#endif                
                case XA_IODEVICE_RADIO:
                    DEBUG_INFO("XA_IODEVICE_RADIO")
                    ;
                    if (ioDevice->device)
                        {
                        DEBUG_INFO("Use radio pipeline as source.");
                        /* Get radio_pipeline and set it to base context */
                        radioDevice = (XARadioDeviceImpl*) (*ioDevice->device);
                        /* radio does not have actual bin, only source element*/
                        gstSrc = GST_ELEMENT(((XAAdaptationGstCtx*)radioDevice->adaptationCtx)->bin);
                        /* refcount increase is needed to keep this not being deleted after use */
                        gst_object_ref(GST_OBJECT(gstSrc));
                        /**isobj = XA_BOOLEAN_TRUE;*/
                        }
                    break;
                default:
                    {
                    DEBUG_ERR("Unsupported IODevice.");
                    break;
                    }
                }
            break; /* XA_DATALOCATOR_IODEVICE */

        case XA_DATALOCATOR_ADDRESS:
            {
            XADataLocator_Address* address =
                    (XADataLocator_Address*) (xaSrc->pLocator);
            gstSrc = gst_element_factory_make("appsrc", name);
            /* init gst buffer from datalocator */
            if (gstSrc)
                {
                /* init GST buffer from XADataLocator*/
                GstBuffer* userBuf = gst_buffer_new();
                if (userBuf)
                    {
                    userBuf->size = address->length;
                    userBuf->data = address->pAddress;
                    /* push the whole buffer to appsrc so it is ready for preroll */
                    DEBUG_INFO("Pushing buffer");
                    gst_app_src_push_buffer(GST_APP_SRC(gstSrc), userBuf);
                    DEBUG_INFO_A1("Sent buffer at 0x%x to appsrc", userBuf );
                    gst_app_src_end_of_stream(GST_APP_SRC(gstSrc));
                    }
                else
                    {
                    DEBUG_ERR("Failure allocating buffer!");
                    }
                }
            else
                {
                DEBUG_ERR("Failure creating appsrc!");
                }
            }
            break;

        default:
            DEBUG_ERR("Incorrect data locator for source.")
            break;
        }

    if (gstSrc)
        {
        DEBUG_INFO_A1("Created gstreamer source element at %x", gstSrc);
        }

    DEBUG_API("<-XAAdaptationGst_CreateGstSource");
    return gstSrc;
    }

/**
 * GstElement* XAAdaptationGst_CreateGstSink( XADataSink* xaSnk, const XAchar *name )
 * @param XADataSink* xaSnk - XADataSink defining gst sink to create
 * @param const XAchar *name - string for naming the gst element
 * @return GstElement* - return newly created gst sink element
 * Description: Create gst sink element corresponding to XA sink structure
 */
GstElement* XAAdaptationGst_CreateGstSink(XADataSink* xaSnk,
        const char *name, XAboolean *isobj)
    {
    XAuint32 locType = 0;
    GstElement* gstSnk = NULL;
    XADataLocator_URI* uri = NULL;
    DEBUG_API("->XAAdaptationGst_CreateGstSink");
    if (!xaSnk || !xaSnk->pLocator)
        {
        DEBUG_INFO("Warning! No sink specified, use fakesink");
        gstSnk = gst_element_factory_make("fakesink", name);
        if (!gstSnk)
            {
            DEBUG_ERR("Cannot create sink!");
            return NULL;
            }
        g_object_set(G_OBJECT(gstSnk), "async", FALSE, NULL);
        }
    else
        {
        locType = *((XAuint32*) (xaSnk->pLocator));
        switch (locType)
            {
            case XA_DATALOCATOR_URI:
                DEBUG_INFO("XA_DATALOCATOR_URI")
                ;
                uri = (XADataLocator_URI*) xaSnk->pLocator;
                gstSnk = gst_element_factory_make("filesink", name);
                if (!gstSnk)
                    {
                    DEBUG_ERR("Cannot create sink!");
                    return NULL;
                    }
                if (uri->URI != NULL)
                    {
                    XAchar *fname;
                    DEBUG_INFO_A1("URI: %s", uri->URI);
                    if (strncmp((char *) uri->URI, "file:///", 8) == 0)
                        {
                        fname = &((uri->URI)[8]);
                        }
                    else
                        {
                        fname = uri->URI;
                        }DEBUG_INFO_A1("->filesystem path %s", fname);
                    g_object_set(G_OBJECT(gstSnk), "location", fname,
                            "async", FALSE, "qos", FALSE, "max-lateness",
                            (gint64) (-1), NULL);
                    }
                else
                    {
                    DEBUG_ERR("No recording output uri specified.");
                    return NULL;
                    }
                break;
            case XA_DATALOCATOR_NATIVEDISPLAY:
                DEBUG_INFO("Sink locator type - XA_DATALOCATOR_NATIVEDISPLAY")
                ;
#ifdef USE_NGA_SURFACES
                gstSnk = gst_element_factory_make("devvideosink",
                        "devvideosink");
#else
                gstSnk = gst_element_factory_make("ximagesink",name);
#endif /*USE_NGA_SURFACES*/
                if (!gstSnk)
                    {
                    DEBUG_ERR("Cannot create sink!");
                    return NULL;
                    }
                g_object_set(G_OBJECT(gstSnk), "force-aspect-ratio", TRUE,
                        "async", FALSE, "qos", FALSE, "handle-events", TRUE,
                        "handle-expose", TRUE, "max-lateness", (gint64) (-1),
                        NULL);
                break;
            case XA_DATALOCATOR_OUTPUTMIX:
                DEBUG_INFO("Sink locator type - XA_DATALOCATOR_OUTPUTMIX")
                ;
                    {
                    /* Get OutputMix adaptation from data locator */
                    XADataLocator_OutputMix* omix =
                            (XADataLocator_OutputMix*) (xaSnk->pLocator);
                    if (omix->outputMix)
                        {
                        XAOMixImpl* omixDevice =
                                (XAOMixImpl*) (*omix->outputMix);

                        if (omixDevice)
                            {
                            /*TODO we had to remove this line below since adaptationCtx
                             * was  not a member of structure*/

                            /*gstSnk = XAOutputMixAdapt_GetSink(omixDevice->adaptationCtx);*/
                            if (!gstSnk)
                                {
                                DEBUG_ERR("Cannot create sink!");
                                return NULL;
                                }
                            *isobj = XA_BOOLEAN_TRUE;
                            }
                        else
                            {
                            DEBUG_ERR("Warning - NULL outputmix object - default audio output used");
                            gstSnk = gst_element_factory_make("alsasink",
                                    name);
                            }
                        }
                    else
                        {
                        DEBUG_ERR("Warning - NULL outputmix object - default audio output used");
                        gstSnk = gst_element_factory_make("alsasink", name);
                        }

                    }
                break;

            case XA_DATALOCATOR_ADDRESS:
                {
                gstSnk = gst_element_factory_make("appsink", name);
                /* Not actually object sink, but attribute used to notify recorder
                 * about appsink (no object sinks applicable in this use case)
                 **/
                *isobj = TRUE;
                }
                break;
            case XA_DATALOCATOR_IODEVICE:
                /* when only valid IOdevice sinks vibra and LED sinks implemented
                 * at adaptation level, add handling here (in this implementation,
                 * no handling needed as only dummy implementations for those)
                 **/
            default:
                DEBUG_ERR("Incorrect data locator for sink.")
                break;
            }
        }
    if (gstSnk)
        {
        DEBUG_INFO_A1("Created gstreamer sink element at %x", gstSnk);
        }DEBUG_API("<-XAAdaptationGst_CreateGstSink");
    return gstSnk;
    }

/**
 * GstElement* XAAdaptationGst_CreateVideoPP( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline
 */
GstElement* XAAdaptationGst_CreateVideoPP()
    {
    GstElement *vpp;
    DEBUG_API("->XAAdaptationGst_CreateVideoPP");
    vpp = gst_pipeline_new("videopp");
    if (vpp)
        {
        GstPad *ghostsink, *ghostsrc;
        GstElement *col1, *col2, *rotate, *mirror, *box, *crop, *gamma,
                *balance, *scale, *scale2,
#ifdef USE_NGA_SURFACES
                *identity,
#endif /*USE_NGA_SURFACES*/
                *queue;

        /* Crete ffmpegcolorspace to convert stream to correct format */
        col1 = gst_element_factory_make("ffmpegcolorspace", "pp_colsp1");
        if (col1)
            {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col1);
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(col1, "sink");
            if (ghostsink)
                {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_sink",
                        ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
                }
            }

        /* create video crop, this will be sink for videoPP pipeline */
        crop = gst_element_factory_make("videocrop", "pp_crop");
        if (crop)
            {
            DEBUG_INFO("Created crop element");
            gst_bin_add(GST_BIN(vpp), crop);
            }

        /* create video rotate */
        rotate = gst_element_factory_make("videoflip", "pp_rotate");
        if (rotate)
            {
            DEBUG_INFO("Created rotate element");
            g_object_set(G_OBJECT(rotate), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), rotate);
            }

        /* create video mirror */
        mirror = gst_element_factory_make("videoflip", "pp_mirror");
        if (mirror)
            {
            DEBUG_INFO("Created mirror element");
            g_object_set(G_OBJECT(mirror), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), mirror);
            }

        /* create video box */
        box = gst_element_factory_make("videobox", "pp_box");
        if (box)
            {
            DEBUG_INFO("Created videobox element");
            gst_bin_add(GST_BIN(vpp), box);
            }

        /* create video balance */
        balance = gst_element_factory_make("videobalance", "pp_balance");
        if (balance)
            {
            DEBUG_INFO("Created balance element");
            gst_bin_add(GST_BIN(vpp), balance);
            }

        /* create video gamma */
        gamma = gst_element_factory_make("gamma", "pp_gamma");
        if (gamma)
            {
            DEBUG_INFO("Created gamma element");
            gst_bin_add(GST_BIN(vpp), gamma);
            }

        /* Create videoscale element to scale postprocessed output to correct size */
        scale = gst_element_factory_make("videoscale", "pp_scale");
        if (scale)
            {
            DEBUG_INFO("Created videoscale element");
            gst_bin_add(GST_BIN(vpp), scale);
            }
        scale2 = gst_element_factory_make("videoscale", "pp_scale2");
        if (scale2)
            {
            GstPad *pad = NULL;
            GstCaps *caps = NULL;
            DEBUG_INFO("Created videoscale element");
            pad = gst_element_get_static_pad(scale2, "src");
            caps = gst_caps_new_simple("video/x-raw-yuv", "width",
                    G_TYPE_INT, 0, "height", G_TYPE_INT, 0, NULL);
            gst_pad_set_caps(pad, caps);
            gst_bin_add(GST_BIN(vpp), scale2);
            }

        /* create video queue */
        queue = gst_element_factory_make("queue", "vpp_queue");
        if (queue)
            {
            DEBUG_INFO("Created queue element");
            gst_bin_add(GST_BIN(vpp), queue);
#ifdef USE_NGA_SURFACES
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(queue, "sink");
            if (ghostsink)
                {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_src",
                        ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
                }
#endif /*USE_NGA_SURFACES*/
            }

        /* Crete ffmpegcolorspace to convert stream to correct format */
        col2 = gst_element_factory_make("ffmpegcolorspace", "pp_colsp2");
        if (col2)
            {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col2);
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(col2, "src");
            if (ghostsrc)
                {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_src",
                        ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
                }
            }

#ifdef USE_NGA_SURFACES
        //shyward
        /* create identity element */
        identity = gst_element_factory_make("identity", "identity");
        if (identity)
            {
            DEBUG_INFO("Created identity element");
            gst_bin_add(GST_BIN(vpp), identity);
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(identity, "src");
            if (ghostsrc)
                {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_sink",
                        ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
                }
            }
        if (!(gst_element_link_many(queue, identity, NULL)))
#else
        //shyward - thins code assumes all the elements will have been created, which is not true
        if( !(gst_element_link_many(col1,
                                scale,
                                crop,
                                rotate,
                                mirror,
                                box,
                                balance,
                                gamma,
                                queue,
                                scale2,
#ifdef USE_NGA_SURFACES
                                identity,
#endif /*USE_NGA_SURFACES*/
                                col2,
                                NULL)
                ) )
#endif /*USE_NGA_SURFACES*/
            {
            DEBUG_ERR("Could not link videopp elements!!");
            gst_object_unref(vpp);
            vpp = NULL;
            }
        }DEBUG_API("<-XAAdaptationGst_CreateVideoPP");
    return vpp;
    }

/**
 * GstElement* XAAdaptationGst_CreateFixedSizeRecordVideoPP( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline with fixed output size
 *  to TEST_VIDEO_WIDTH x TEST_VIDEO_HEIGHT
 *              experimental implementation for changing recorder output size
 */
GstElement* XAAdaptationGst_CreateFixedSizeVideoPP()
    {
    GstElement *vpp;
    DEBUG_API("->XAAdaptationGst_CreateFixedSizeVideoPP");
    vpp = gst_pipeline_new("videopp");
    if (vpp)
        {
        GstPad *ghostsink, *ghostsrc;
        GstElement *col1, *col2, *rotate, *mirror, *box, *crop, *gamma,
                *balance, *scale, *scale2, *filter, *queue;

        /* Crete ffmpegcolorspace to convert stream to correct format */
        col1 = gst_element_factory_make("ffmpegcolorspace", "pp_colsp1");
        if (col1)
            {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col1);
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(col1, "sink");
            if (ghostsink)
                {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_sink",
                        ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
                }
            }

        /* create video crop, this will be sink for videoPP pipeline */
        crop = gst_element_factory_make("videocrop", "pp_crop");
        if (crop)
            {
            DEBUG_INFO("Created crop element");
            gst_bin_add(GST_BIN(vpp), crop);
            }

        /* create video rotate */
        rotate = gst_element_factory_make("videoflip", "pp_rotate");
        if (rotate)
            {
            DEBUG_INFO("Created rotate element");
            g_object_set(G_OBJECT(rotate), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), rotate);
            }

        /* create video mirror */
        mirror = gst_element_factory_make("videoflip", "pp_mirror");
        if (mirror)
            {
            DEBUG_INFO("Created mirror element");
            g_object_set(G_OBJECT(mirror), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), mirror);
            }

        /* create video box */
        box = gst_element_factory_make("videobox", "pp_box");
        if (box)
            {
            DEBUG_INFO("Created videobox element");
            gst_bin_add(GST_BIN(vpp), box);
            }

        /* create video balance */
        balance = gst_element_factory_make("videobalance", "pp_balance");
        if (balance)
            {
            DEBUG_INFO("Created balance element");
            gst_bin_add(GST_BIN(vpp), balance);
            }

        /* create video gamma */
        gamma = gst_element_factory_make("gamma", "pp_gamma");
        if (gamma)
            {
            DEBUG_INFO("Created gamma element");
            gst_bin_add(GST_BIN(vpp), gamma);
            }

        /* Create videoscale element to scale postprocessed output to correct size */
        scale = gst_element_factory_make("videoscale", "pp_scale");
        if (scale)
            {
            DEBUG_INFO("Created videoscale element");
            gst_bin_add(GST_BIN(vpp), scale);
            }
        scale2 = gst_element_factory_make("videoscale", "pp_scale2");
        if (scale2)
            {
            GstPad *pad = NULL;
            GstCaps *caps = NULL;
            DEBUG_INFO("Created videoscale element");
            pad = gst_element_get_static_pad(scale2, "src");
            caps = gst_caps_new_simple("video/x-raw-yuv", "width",
                    G_TYPE_INT, 0, "height", G_TYPE_INT, 0, NULL);
            gst_pad_set_caps(pad, caps);
            gst_bin_add(GST_BIN(vpp), scale2);
            }

        /* create capsfilter for fixed video size */
        filter = gst_element_factory_make("capsfilter", "pp_filter");
        if (filter)
            {

            g_object_set(G_OBJECT(filter), "caps", gst_caps_new_simple(
                    "video/x-raw-yuv", "width", G_TYPE_INT, TEST_VIDEO_WIDTH,
                    "height", G_TYPE_INT, TEST_VIDEO_HEIGHT, NULL), NULL);
            gst_bin_add(GST_BIN(vpp), filter);
            }

        /* create video queue */
        queue = gst_element_factory_make("queue", "vpp_queue");
        if (queue)
            {
            gst_bin_add(GST_BIN(vpp), queue);
            }

        /* Crete ffmpegcolorspace to convert stream to correct format */
        col2 = gst_element_factory_make("ffmpegcolorspace", "pp_colsp2");
        if (col2)
            {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col2);
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(col2, "src");
            if (ghostsrc)
                {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_src",
                        ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
                }
            }
        if (!(gst_element_link_many(col1, scale, crop, rotate, mirror, box,
                balance, gamma, queue, scale2, filter, col2, NULL)))
            {
            DEBUG_ERR("Could not link videopp elements!!");
            gst_object_unref(vpp);
            vpp = NULL;
            }
        }DEBUG_API("<-XAAdaptationGst_CreateFixedSizeVideoPP");
    return vpp;
    }

/**
 * GstElement* XAAdaptationGst_CreateVideoPPBlackScr( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline for black screen
 */
GstElement* XAAdaptationGst_CreateVideoPPBlackScr()
    {
    GstElement *vppBScr;
    DEBUG_API("->XAAdaptationGst_CreateVideoPPBlackScr");
    vppBScr = gst_pipeline_new("videoppBScr");
    if (vppBScr)
        {
        GstPad *ghostsrc = NULL;
        GstElement *testVideo = NULL, *scale = NULL;
        GstElement *ffmpegcolorspace = NULL;

        testVideo = gst_element_factory_make("videotestsrc", "videotest");
        if (testVideo)
            {
            DEBUG_INFO("Created videotestsrc element");

            g_object_set(G_OBJECT(testVideo), "pattern", (gint) 2,
                    "num-buffers", (gint) 1, NULL);
            gst_bin_add(GST_BIN(vppBScr), testVideo);
            }

        scale = gst_element_factory_make("videoscale", "BSrc_scale");
        if (scale)
            {
            DEBUG_INFO("Created videoscale element");
            gst_bin_add(GST_BIN(vppBScr), scale);
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(scale, "src");
            if (ghostsrc)
                {
                gst_element_add_pad(vppBScr, gst_ghost_pad_new(
                        "videoppBSrc_src", ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
                }
            }
        ffmpegcolorspace = gst_element_factory_make("ffmpegcolorspace",
                "BlackScrFfmpeg");
        gst_bin_add(GST_BIN(vppBScr), ffmpegcolorspace);
        if (!(gst_element_link_many(testVideo, ffmpegcolorspace, scale, NULL)))
            {
            DEBUG_ERR("Could not link videoppBSrc elements!!");
            gst_object_unref(vppBScr);
            vppBScr = NULL;
            }
        }DEBUG_API("<-XAAdaptationGst_CreateVideoPPBlackScr");
    return vppBScr;
    }

/**
 * GstElement* XAAdaptationGst_CreateInputSelector( )
 * @return GstElement* - return newly created input selector
 * Description: Create input selector to processing between black screen and video screen
 */
GstElement* XAAdaptationGst_CreateInputSelector()
    {
    GstElement *inputSelector;
    DEBUG_API("->XAAdaptationGst_CreateInputSelector");
    inputSelector = gst_element_factory_make("input-selector",
            "input-selector");
    if (inputSelector)
        {
        g_object_set(G_OBJECT(inputSelector), "select-all", TRUE, NULL);
        }DEBUG_API("<-XAAdaptationGst_CreateInputSelector");
    return inputSelector;
    }

/**
 * GstElement* XAAdaptationGst_CreateAudioPP( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline
 */
GstElement* XAAdaptationGst_CreateAudioPP()
    {
    GstElement *app;
    gboolean ok = TRUE;
    DEBUG_API("->XAAdaptationGst_CreateAudioPP");
    app = gst_pipeline_new("audiopp");
    if (app)
        {
        GstPad *ghostsink, *ghostsrc;
        GstElement *ac, *vol, *eq, *queue, *pan, *ac2;

        /* first and last elements should be audioconverts to
         *  match sink and encoder formats */
        ac = gst_element_factory_make("audioconvert", "pp_ac");
        if (ac)
            {
            ok = gst_bin_add(GST_BIN(app), ac);
            /* make this bin link point*/
            if (ok)
                {
                ghostsink = gst_element_get_static_pad(ac, "sink");
                ok = gst_element_add_pad(app, gst_ghost_pad_new("sink",
                        ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
                }
            }
        ac2 = gst_element_factory_make("audioconvert", "pp_ac2");
        if (ac2 && ok)
            {
            ok = gst_bin_add(GST_BIN(app), ac2);
            /* make this bin link point*/
            if (ok)
                {
                ghostsrc = gst_element_get_static_pad(ac2, "src");
                ok = gst_element_add_pad(app, gst_ghost_pad_new("src",
                        ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
                }
            }

        vol = gst_element_factory_make("volume", "pp_vol");
        /* create volume controller */
        if (vol && ok)
            {
            ok = gst_bin_add(GST_BIN(app), vol);
            g_object_set(G_OBJECT(vol), "volume", (gdouble) 1, NULL);
            }
        /* create 10-band equalizer */
        eq = gst_element_factory_make("equalizer-10bands", "pp_equ");
        if (eq && ok)
            {
            ok = gst_bin_add(GST_BIN(app), eq);
            }
        /* create audio queue */
        queue = gst_element_factory_make("queue", "app_queue");
        if (queue && ok)
            {
            ok = gst_bin_add(GST_BIN(app), queue);
            g_object_set(G_OBJECT (queue), "max-size-buffers", 2, NULL);
            }
        /* create audio pan effect */
        pan = gst_element_factory_make("audiopanorama", "pp_pan");
        if (pan && ok)
            {
            ok = gst_bin_add(GST_BIN(app), pan);
            }

        if (ac && ok)
            {
            if (queue)
                {
                ok = gst_element_link(ac, queue);
                }
            else if (vol)
                {
                ok = gst_element_link(ac, vol);
                }
            else if (pan)
                {
                ok = gst_element_link(ac, pan);
                }
            else if (eq)
                {
                ok = gst_element_link(ac, eq);
                }
            else if (ac2)
                {
                ok = gst_element_link(ac, ac2);
                }
            }
        if (queue && ok)
            {
            if (vol)
                {
                ok = gst_element_link(queue, vol);
                }
            else if (pan)
                {
                ok = gst_element_link(queue, pan);
                }
            else if (eq)
                {
                ok = gst_element_link(queue, eq);
                }
            else if (ac2)
                {
                ok = gst_element_link(queue, ac2);
                }
            }
        if (vol && ok)
            {
            if (pan)
                {
                ok = gst_element_link(vol, pan);
                }
            else if (eq)
                {
                ok = gst_element_link(vol, eq);
                }
            else if (ac2)
                {
                ok = gst_element_link(vol, ac2);
                }
            }
        if (pan && ok)
            {
            if (eq)
                {
                ok = gst_element_link(pan, eq);
                }
            else if (ac2)
                {
                ok = gst_element_link(pan, ac2);
                }
            }
        if (eq && ok)
            {
            if (ac2)
                {
                ok = gst_element_link(eq, ac2);
                }
            }

        if (ac)
            {
            // ghost sink above
            }
        else if (queue && ok)
            {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(queue, "sink");
            ok = gst_element_add_pad(app,
                    gst_ghost_pad_new("sink", ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
            }
        else if (vol && ok)
            {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(vol, "sink");
            ok = gst_element_add_pad(app,
                    gst_ghost_pad_new("sink", ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
            }
        else if (pan && ok)
            {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(pan, "sink");
            ok = gst_element_add_pad(app,
                    gst_ghost_pad_new("sink", ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
            }
        else if (eq && ok)
            {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(eq, "sink");
            ok = gst_element_add_pad(app,
                    gst_ghost_pad_new("sink", ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
            }
        else if (ac2 && ok)
            {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(ac2, "sink");
            ok = gst_element_add_pad(app,
                    gst_ghost_pad_new("sink", ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
            }

        if (ac2)
            {
            // ghost src above
            }
        else if (eq && ok)
            {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(eq, "src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src", ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
            }
        else if (pan && ok)
            {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(pan, "src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src", ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
            }
        else if (vol && ok)
            {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(vol, "src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src", ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
            }
        else if (queue && ok)
            {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(queue, "src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src", ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
            }
        else if (ac && ok)
            {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(ac, "src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src", ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
            }

        //        if( !(gst_element_link_many(ac, queue, vol, ac2, NULL)) )
        //        if( !(gst_element_link_many(ac, queue, vol, pan, eq, ac2, NULL)) )
        if (!ok)
            {
            DEBUG_ERR("Could not link audiopp elements!!");
            gst_object_unref(app);
            app = NULL;
            }
        }

    DEBUG_API("<-XAAdaptationGst_CreateAudioPP");
    return app;
    }

/* called when pad is actually blocking/ gets unblocked*/
void XAAdaptationGst_PadBlockCb(GstPad *pad, gboolean blocked,
        gpointer user_data)
    {
    DEBUG_API_A2("->XAAdaptationGst_PadBlockCb   pad \"%s\" of \"%s\" ",
            GST_OBJECT_NAME(pad),
            GST_OBJECT_NAME(gst_pad_get_parent_element(pad)) );
    DEBUG_API_A1("<-XAAdaptationGst_PadBlockCb   blocked:%d",blocked);
    }

/* utility to set same fields for all media types in caps */
void XAAdaptationGst_SetAllCaps(GstCaps * caps, char *field, ...)
    {
    GstStructure *structure;
    va_list var_args;
    int i;

    for (i = 0; i < gst_caps_get_size(caps); i++)
        {
        structure = gst_caps_get_structure(caps, i);
        va_start (var_args, field);
        gst_structure_set_valist(structure, field, var_args);
        va_end (var_args);
        }
    }

