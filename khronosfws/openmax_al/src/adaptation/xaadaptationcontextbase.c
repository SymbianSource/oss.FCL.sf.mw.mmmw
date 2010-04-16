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
#include <string.h>
#include "gst/gst.h"
#include "gst/gstbuffer.h"
#include "gst/app/gstappsrc.h"
#include "XAAdaptationContextBase.h"
#include "XAAdaptation.h"
#include "XAObjectItf.h"
#include "XACameraDevice.h"
#include "XARadioDevice.h"
#include "XAOutputMix.h"
#include "XAStaticCapsAdaptation.h"
#include "XAMediaPlayerAdaptCtx.h"

extern XAboolean cameraRealized;
extern XACameraAdaptationCtx_* cameraCtx;
/*
 * XAAdaptationBaseCtx* XAAdaptationBase_Create()
 * 1st phase initialization function for Adaptation Base context structure.
 * Reserves memory for base context and initializes GStreamer FW.
 */
XAresult XAAdaptationBase_Init( XAAdaptationBaseCtx* pSelf, XAuint32 ctxId )
{
    DEBUG_API("->XAAdaptationBase_Init");

    if ( pSelf )
    {
        GError* gerror = 0;
		pSelf->pipeSrcThr = NULL;
		pSelf->pipeSinkThr = NULL;

        /* Set context id */
        pSelf->ctxId = ctxId;
        /* Add default handler for Gst-bus messages */
        pSelf->busCb = XAAdaptationBase_GstBusCb;

        // VASU MOD BEGINS
        pSelf->cond_mutx_inited = XA_BOOLEAN_FALSE;
        // VASU MOD ENDS

        pSelf->evtHdlrs = g_array_new (FALSE, FALSE, sizeof (XAAdaptEvtHdlr));

        sem_init(&(pSelf->semAsyncWait),0,0);


        if ( !gst_init_check( NULL, NULL, &gerror ) )
        {
            DEBUG_ERR("Gst Initalization failure.");
            return XA_RESULT_INTERNAL_ERROR;
        }
    }
    else
    {
        DEBUG_ERR("Invalid Adaptation Base Context.")
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XAAdaptationBase_Init");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAAdaptationBase_PostInit()
 * 2nd phase initialization for Adaptation Base.
 */
XAresult XAAdaptationBase_PostInit( XAAdaptationBaseCtx* ctx )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAAdaptationBase_PostInit");
    // VASU MOD BEGINS
    ctx->thread_launched = XA_BOOLEAN_FALSE;
    pthread_mutex_init(&(ctx->ds_mutex), NULL);
    pthread_cond_init(&(ctx->ds_condition), NULL);
    ctx->cond_mutx_inited = XA_BOOLEAN_TRUE;
    // VASU MOD ENDS
    DEBUG_API("<-XAAdaptationBase_PostInit");
    return ret;
}

/*
 * void XAAdaptationBase_Free( XAAdaptationBaseCtx* ctx )
 * Frees all Base context variables .
 */
void XAAdaptationBase_Free( XAAdaptationBaseCtx* ctx )
{
	GstElement* fakesink = NULL;
    DEBUG_API("->XAAdaptationBase_Free");

    if ( ctx->bin )
    {
    	fakesink = gst_bin_get_by_name(GST_BIN(ctx->bin), "fakesink");
		if ( fakesink )
		{
			gst_element_set_state( GST_ELEMENT(fakesink), GST_STATE_NULL);
			gst_object_unref(fakesink);
		}

        if ( gst_element_set_state(GST_ELEMENT(ctx->bin), GST_STATE_NULL )!=GST_STATE_CHANGE_SUCCESS )
        {   /*not much we can do*/
            DEBUG_ERR("WARNING: Failed to change to NULL state before deletion!!")
        }
    }

    if(ctx->asynctimer)
    {   /*cancel timer*/
        g_source_remove(ctx->asynctimer);
    }
    sem_post(&(ctx->semAsyncWait));
    sem_destroy(&(ctx->semAsyncWait));

    XAAdaptationBase_StopGstListener(ctx);

    if ( ctx->bin )
    {
        gst_object_unref(ctx->bin);
    }

    g_array_free(ctx->evtHdlrs, TRUE);
    // VASU MOD BEGINS
    if (ctx->cond_mutx_inited == XA_BOOLEAN_TRUE)
        {
        ctx->thread_launched = XA_BOOLEAN_FALSE;
        pthread_mutex_destroy(&(ctx->ds_mutex));
        pthread_cond_destroy(&(ctx->ds_condition));
        ctx->cond_mutx_inited = XA_BOOLEAN_FALSE;
        }
    // VASU MOD ENDS

    DEBUG_API("<-XAAdaptationBase_Free");
}

/*
 * XAresult XAAdaptationBase_AddEventHandler
 * Adds event handler for certain event types.
 */
XAresult XAAdaptationBase_AddEventHandler( XAAdaptationBaseCtx* ctx, xaAdaptEventHandler evtHandler,
                                    XAuint32 evtTypes, void *pHandlerCtx )
{
    XAuint32 i;
    XAAdaptEvtHdlr tmp;
    DEBUG_API("->XAAdaptationBase_AddEventHandler");
    if(!ctx)
    {
        DEBUG_ERR("no context");
        return XA_RESULT_PARAMETER_INVALID;
    }
    for(i=0; i<ctx->evtHdlrs->len; i++)
    {
        if( (g_array_index(ctx->evtHdlrs, XAAdaptEvtHdlr, i)).handlerfunc == evtHandler )
        {
            return XA_RESULT_PARAMETER_INVALID;
        }
    }
    tmp.handlerfunc = evtHandler;
    tmp.handlercontext = pHandlerCtx;
    tmp.eventtypes = evtTypes;
    g_array_append_val(ctx->evtHdlrs, tmp);
    DEBUG_API("<-XAAdaptationBase_AddEventHandler");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAAdaptationBase_RemoveEventHandler
 * Removes event handler for certain event types.
 */
XAresult XAAdaptationBase_RemoveEventHandler( XAAdaptationBaseCtx* ctx, xaAdaptEventHandler evtHandler)
{
    XAuint32 i;
    DEBUG_API("->XAAdaptationBase_RemoveEventHandler");
    if(!ctx)
    {
        DEBUG_ERR("no context");
        return XA_RESULT_PARAMETER_INVALID;
    }
    for(i=0; i<ctx->evtHdlrs->len; i++)
    {
        if( (g_array_index(ctx->evtHdlrs, XAAdaptEvtHdlr, i)).handlerfunc == evtHandler )
        {
            g_array_remove_index(ctx->evtHdlrs, i);
            return XA_RESULT_SUCCESS;
        }
    }
    DEBUG_API("<-XAAdaptationBase_RemoveEventHandler");
    /*did not find, return error*/
    return XA_RESULT_PARAMETER_INVALID;
}

/*
 * gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
 * Default Gst-bus message handler (Callback)
 */
gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
{
    GError *error;
    gchar *debug;
    DEBUG_API("->XAAdaptationBase_GstBusCb");
    DEBUG_INFO_A2("Received Gst callback \"%s\" from \"%s\"",
                    GST_MESSAGE_TYPE_NAME(message),
                    GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));

    switch( GST_MESSAGE_TYPE(message))
    {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error( message, &error, &debug );
            DEBUG_INFO_A1("%s", debug);
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
            DEBUG_INFO("Unhandled Gst-Bus message");
            break;
    }
    DEBUG_API("<-XAAdaptationBase_GstBusCb");
    return TRUE;
}

XAresult XAAdaptationBase_InitGstListener(XAAdaptationBaseCtx* ctx)
{
    int ret;
    DEBUG_API("->XAAdaptationBase_InitGstListener");
    if ( ctx->bin )
    {
        ctx->bus = gst_pipeline_get_bus( GST_PIPELINE( ctx->bin ));
    }
    if( !ctx->bus )
    {
        DEBUG_ERR("could not get gst bus!")
        return XA_RESULT_INTERNAL_ERROR;
    }
    ret = pthread_create(&(ctx->busloopThr), NULL, (XAAdaptationBase_LaunchGstListener),(void*)ctx);
    // VASU MOD BEGINS
    if ( ctx->thread_launched == XA_BOOLEAN_FALSE )
    {
        // Wait until the thread is created
        pthread_mutex_lock(&(ctx->ds_mutex));
        pthread_cond_wait(&(ctx->ds_condition), &(ctx->ds_mutex));
        pthread_mutex_unlock(&(ctx->ds_mutex));
        // VASU MOD ENDS
    }
    if(ret)
    {
        DEBUG_ERR_A1("could not create thread!! (%d)",ret)
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_API("<-XAAdaptationBase_InitGstListener");
    return XA_RESULT_SUCCESS;
}

void * XAAdaptationBase_LaunchGstListener(void* args)
{
    XAAdaptationBaseCtx* ctx = (XAAdaptationBaseCtx*)args;
    DEBUG_API("->XAAdaptationBase_LaunchGstListener");
    // VASU MOD BEGINS
    // Signal calling thread that this thread creation is completed
    ctx->thread_launched = XA_BOOLEAN_TRUE;
    pthread_mutex_lock(&(ctx->ds_mutex));
    pthread_cond_signal(&(ctx->ds_condition));
    pthread_mutex_unlock(&(ctx->ds_mutex));
    // VASU MOD ENDS

    ctx->busloop = g_main_loop_new( NULL, FALSE );
    if ( !ctx->busloop )
    {
        DEBUG_ERR("Glib main loop failure.")
        DEBUG_API("<-XAAdaptationBase_LaunchGstListener");
        assert(0);
    }
    else
    {
        DEBUG_INFO("Start Glib main loop")
        g_main_loop_run(ctx->busloop);
        DEBUG_INFO("Glib main loop stopped - exiting thread")
        DEBUG_API("<-XAAdaptationBase_LaunchGstListener");
        pthread_exit(NULL);
    }
   
}

void XAAdaptationBase_StopGstListener(XAAdaptationBaseCtx* ctx)
{
    DEBUG_API("->XAAdaptationBase_StopGstListener");
    if(ctx->busloop)
    {
        g_main_loop_quit (ctx->busloop);
        g_main_loop_unref(ctx->busloop);
    }
    if(ctx->bus)
    {
        gst_object_unref(ctx->bus);
        ctx->bus = NULL;
    }
    DEBUG_API("<-XAAdaptationBase_StopGstListener");
}

void XAAdaptationBase_SendAdaptEvents(XAAdaptationBaseCtx* ctx, XAAdaptEvent* event)
{
    XAuint32 i;
    XAAdaptEvtHdlr* tmp;
    for(i=0; i<ctx->evtHdlrs->len; i++)
    {
        tmp = &g_array_index(ctx->evtHdlrs, XAAdaptEvtHdlr, i);
        if( tmp->eventtypes & event->eventtype )
        {
            (tmp->handlerfunc)(tmp->handlercontext, event);
        }
    }
}

/*
 * ASynchronous operation managing
 **/

/* NOTE: This should NOT be called from gst callbacks - danger of deadlock!!
 */
void XAAdaptationBase_PrepareAsyncWait(XAAdaptationBaseCtx* ctx)
{
    DEBUG_API("->XAAdaptationBase_PrepareAsyncWait");

    if( ctx->waitingasyncop )
    {   /*wait previous async op*/
        DEBUG_INFO("::WARNING:: previous asynch still ongoing!!!");
        DEBUG_INFO(">>>>  WAIT PREVIOUS");
        sem_wait(&(ctx->semAsyncWait));
        DEBUG_INFO("<<<<  PREVIOUS COMPLETED");
    }
    sem_init(&(ctx->semAsyncWait),0,0);

    ctx->waitingasyncop = XA_BOOLEAN_TRUE;
    DEBUG_API("<-XAAdaptationBase_PrepareAsyncWait");
}

void XAAdaptationBase_StartAsyncWait(XAAdaptationBaseCtx* ctx)
{
    DEBUG_API("->XAAdaptationBase_StartAsyncWait");

    /* timeout to try to avoid gst freeze in rollup */
    ctx->asynctimer = g_timeout_add(XA_ADAPT_ASYNC_TIMEOUT,
                                    XAAdaptationBase_CancelAsyncWait, ctx);
    /* check flag once again if callback already happened before wait */
    if(ctx->waitingasyncop)
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
    if(ctx->asynctimer)
    {
        g_source_remove(ctx->asynctimer);
    }
    ctx->waitingasyncop = XA_BOOLEAN_FALSE;

    DEBUG_API("<-XAAdaptationBase_StartAsyncWait");
}

/* async operation timeout callback*/
gboolean XAAdaptationBase_CancelAsyncWait(gpointer ctx)
{
    XAAdaptationBaseCtx* bCtx = (XAAdaptationBaseCtx*)ctx;
    DEBUG_API("->XAAdaptationBase_CancelAsyncWait");
    if( bCtx->waitingasyncop )
    {
        DEBUG_ERR_A3("ASYNC TIMED OUT : current %d, gsttarget %d, wanted %d",
                      GST_STATE(bCtx->bin), GST_STATE_TARGET(bCtx->bin), bCtx->binWantedState);
        bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
        sem_post(&(bCtx->semAsyncWait));
    }
    DEBUG_API("<-XAAdaptationBase_CancelAsyncWait");
    /* return false to remove timer */
    return FALSE;
}

void XAAdaptationBase_CompleteAsyncWait(XAAdaptationBaseCtx* ctx)
{
    DEBUG_API("->XAAdaptationBase_CompleteAsyncWait");
    if( ctx->waitingasyncop )
    {
        int i;
        ctx->waitingasyncop = XA_BOOLEAN_FALSE;
        sem_getvalue(&(ctx->semAsyncWait),&i);
        DEBUG_INFO_A1("Asynch operation succeeded, sem value %d",i);
        if(i<=0)
        {   /* only post if locked */
            sem_post(&(ctx->semAsyncWait));
        }
        else if(i>0)
        {   /* should not be, reset semaphore */
            sem_init(&(ctx->semAsyncWait),0,0);
        }
    }
    DEBUG_API("<-XAAdaptationBase_CompleteAsyncWait");
}

XAresult XAAdaptationBase_SetCPConfiguration(XAAdaptationBaseCtx* ctx, XAConfigExtensionCpKey configValue)
{
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("-> XAAdaptationBase_SetConfiguration");
    if( ctx )
    {
		if ( configValue == XA_READ )
		{
			ctx->pipeSrcThrCtx.cpConfig = configValue;
			res = XA_RESULT_SUCCESS;
		}
		else if ( configValue == XA_READBUFFER )
		{
		    ctx->pipeSrcThrCtx.cpConfig = configValue;
			res = XA_RESULT_SUCCESS;
		}
		else if ( configValue == XA_WRITE )
        {
            ctx->pipeSinkThrCtx.cpConfig = configValue;
            res = XA_RESULT_SUCCESS;
        }
		else if ( configValue == XA_WRITEBUFFER )
        {
            ctx->pipeSinkThrCtx.cpConfig = configValue;
            res = XA_RESULT_SUCCESS;
        }
    }
    else
    {
        res = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("-> XAAdaptationBase_SetConfiguration");
    return res;
}

/**
 * GstElement* XAAdaptationBase_CreateGstSource( XADataSource* xaSrc, const XAchar *name )
 * @param XADataSource* xaSnk - XADataSource defining gst source to create
 * @param const XAchar *name - string for naming the gst element
 * @param XAboolean *isobj - (out param) is source another XA object?
 * @return GstElement* - return newly created gst source element
 * Description: Create gst source element corresponding to XA source structure
 */
GstElement* XAAdaptationBase_CreateGstSource( XADataSource* xaSrc, const char *name, XAboolean *isobj, XAboolean *isPCM, XAboolean *isRawImage  )
{
    XAuint32 locType = 0;
    GstElement* gstSrc = NULL;
    char* fname=NULL;
    XADataLocator_URI* uri = NULL;
    XADataLocator_IODevice* ioDevice = NULL;
    XACameraDeviceImpl* cameraDevice = NULL;
    XARadioDeviceImpl* radioDevice = NULL;
    XAObjectItfImpl* pObj = NULL;

    DEBUG_API("->XAAdaptationBase_CreateGstSource");
    if( !xaSrc || !xaSrc->pLocator || !isobj )
    {
        return NULL;
    }
    *isobj = XA_BOOLEAN_FALSE;
    if( xaSrc && xaSrc->pFormat && *((XAuint32*)(xaSrc->pFormat))==XA_DATAFORMAT_PCM && isPCM )
    {
        *isPCM = XA_BOOLEAN_TRUE;
    }
    if( xaSrc && xaSrc->pFormat && *((XAuint32*)(xaSrc->pFormat))==XA_DATAFORMAT_RAWIMAGE && isRawImage )
    {
        *isRawImage = XA_BOOLEAN_TRUE;
    }
    locType = *((XAuint32*)(xaSrc->pLocator));
    switch ( locType )
    {
        case XA_DATALOCATOR_URI:
            DEBUG_INFO("XA_DATALOCATOR_URI");
            uri = (XADataLocator_URI*)xaSrc->pLocator;
            gstSrc = gst_element_factory_make("filesrc",name);
            if ( uri->URI != NULL )
            {
                DEBUG_INFO_A1("URI: %s", uri->URI);
                if(strncmp((char *)uri->URI, "file://", 7) == 0)
                    {
                    fname = (char *)&((uri->URI)[7]);
                    }
                else
                    {
                    fname = (char *)uri->URI;
                    }
                DEBUG_INFO_A1("->filesystem path %s", fname);
                g_object_set( G_OBJECT(gstSrc), "location", fname, NULL );
                /*check for pcm - decodebin does not know how to handle raw PCM files */
                if( isPCM && strstr(fname, ".pcm") )
                {
                    DEBUG_INFO("PCM file detected");
                    *isPCM=XA_BOOLEAN_TRUE;
                }
            }
            else
            {
                DEBUG_ERR("No uri specified.");
                return NULL;
            }
            break; /* XA_DATALOCATOR_URI */


        case XA_DATALOCATOR_IODEVICE:
            DEBUG_INFO("XA_DATALOCATOR_IODEVICE");
            ioDevice = (XADataLocator_IODevice*)(xaSrc->pLocator);
            switch ( ioDevice->deviceType )
            {
                case XA_IODEVICE_AUDIOINPUT:
                {
                    DEBUG_INFO("XA_IODEVICE_AUDIOINPUT");
                    DEBUG_INFO_A1("ioDevice->deviceID: %x", ioDevice->deviceID);
                    switch (ioDevice->deviceID )
                    {
                    //case XA_ADAPTID_ALSASRC: //Krishna
                    case XA_ADAPTID_DEVSOUNDSRC:
                        //DEBUG_INFO("alsasrc"); //Krishna
                        DEBUG_INFO("devsoundsrc");
                        gstSrc = gst_element_factory_make("devsoundsrc",name); //Krishna - changed to devsoundsrc
                        g_object_set (G_OBJECT (gstSrc), "num-buffers", 80, NULL);
                        break;
                    case XA_ADAPTID_AUDIOTESTSRC:
                        /*fall through*/
                    default:
                        DEBUG_INFO("audiotestsrc");
                        gstSrc = gst_element_factory_make("audiotestsrc",name);
                        break;
                    }
                    break;
                }
                case XA_IODEVICE_CAMERA:
                {
                    DEBUG_INFO("XA_IODEVICE_CAMERA");
                    if ( ioDevice->device )
                    {   /*source is camera object*/
                        DEBUG_INFO("Use camerabin as source.");
                        /* Get camerabin from source object */
                        pObj = (XAObjectItfImpl*)(*ioDevice->device);
                        cameraDevice = (XACameraDeviceImpl*)(pObj);
                        gstSrc = GST_ELEMENT(cameraDevice->adaptationCtx->bin);
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
                case XA_IODEVICE_RADIO:
                    DEBUG_INFO("XA_IODEVICE_RADIO");
                    if ( ioDevice->device )
                    {
                        DEBUG_INFO("Use radio pipeline as source.");
                        /* Get radio_pipeline and set it to base context */
                        radioDevice = (XARadioDeviceImpl*)(*ioDevice->device);
                        /* radio does not have actual bin, only source element*/
                        gstSrc = GST_ELEMENT(radioDevice->adaptationCtx->bin);
                        /* refcount increase is needed to keep this not being deleted after use */
                        gst_object_ref(GST_OBJECT(gstSrc));
                        /**isobj = XA_BOOLEAN_TRUE;*/
                    }
                    break;
                default:
                {
                    DEBUG_ERR("Unsupported IODevice.");
                    return NULL;
                    break;
                }
            }
            break; /* XA_DATALOCATOR_IODEVICE */

        case XA_DATALOCATOR_CONTENTPIPE:
        {
        	DEBUG_INFO("XA_DATALOCATOR_CONTENTPIPE");
        	gstSrc = gst_element_factory_make("appsrc",name);
            break;
        }
        case XA_DATALOCATOR_ADDRESS:
            {
                XADataLocator_Address* address = (XADataLocator_Address*)(xaSrc->pLocator);
                gstSrc = gst_element_factory_make("appsrc", name);
                /* init gst buffer from datalocator */
                if( gstSrc )
                {
                    /* init GST buffer from XADataLocator*/
                    GstBuffer* userBuf = gst_buffer_new();
                    if( userBuf )
                    {
                        userBuf->size = address->length;
                        userBuf->data = address->pAddress;
                        /* push the whole buffer to appsrc so it is ready for preroll */
                        DEBUG_INFO("Pushing buffer");
                        gst_app_src_push_buffer( GST_APP_SRC(gstSrc), userBuf );
                        DEBUG_INFO_A1("Sent buffer at 0x%x to appsrc", userBuf );
                        gst_app_src_end_of_stream( GST_APP_SRC(gstSrc) );
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
            return NULL;
            break;
    }

    if ( gstSrc )
    {
        DEBUG_INFO_A1("Created gstreamer source element at %x", gstSrc);
    }

    DEBUG_API("<-XAAdaptationBase_CreateGstSource");
    return gstSrc;
}


/**
 * GstElement* XAAdaptationBase_CreateGstSink( XADataSink* xaSnk, const XAchar *name )
 * @param XADataSink* xaSnk - XADataSink defining gst sink to create
 * @param const XAchar *name - string for naming the gst element
 * @return GstElement* - return newly created gst sink element
 * Description: Create gst sink element corresponding to XA sink structure
 */
GstElement* XAAdaptationBase_CreateGstSink( XADataSink* xaSnk, const char *name, XAboolean *isobj )
{
    XAuint32 locType = 0;
    GstElement* gstSnk = NULL;
    XADataLocator_URI* uri = NULL;
    DEBUG_API("->XAAdaptationBase_CreateGstSink");
    if(!xaSnk || !xaSnk->pLocator)
    {
        DEBUG_INFO("Warning! No sink specified, use fakesink");
        gstSnk = gst_element_factory_make("fakesink",name);
        if(!gstSnk)
        {
            DEBUG_ERR("Cannot create sink!");
            return NULL;
        }
        g_object_set( G_OBJECT(gstSnk),"async", FALSE, NULL);
    }
    else
    {
        locType = *((XAuint32*)(xaSnk->pLocator));
        switch ( locType )
        {
            case XA_DATALOCATOR_URI:
                DEBUG_INFO("XA_DATALOCATOR_URI");
                uri = (XADataLocator_URI*)xaSnk->pLocator;
                gstSnk = gst_element_factory_make("filesink",name);
                if(!gstSnk)
                {
                    DEBUG_ERR("Cannot create sink!");
                    return NULL;
                }
                if ( uri->URI != NULL )
                {
                    XAchar *fname;
                    DEBUG_INFO_A1("URI: %s", uri->URI);
                    if(strncmp((char *)uri->URI, "file://", 7) == 0)
                    {
                        fname = &((uri->URI)[7]);
                    }
                    else
                    {
                        fname = uri->URI;
                    }
                    DEBUG_INFO_A1("->filesystem path %s", fname);
                    g_object_set( G_OBJECT(gstSnk),"location", fname,
                                                   "async", FALSE,
                                                   "qos", FALSE,
                                                   "max-lateness", (gint64)(-1),
                                                   NULL);
                }
                else
                {
                    DEBUG_ERR("No recording output uri specified.");
                    return NULL;
                }
                break;
            case XA_DATALOCATOR_NATIVEDISPLAY:
                DEBUG_INFO("Sink locator type - XA_DATALOCATOR_NATIVEDISPLAY");
#ifdef USE_NGA_SURFACES
                gstSnk = gst_element_factory_make("devvideosink","devvideosink");
#else
                gstSnk = gst_element_factory_make("ximagesink",name);
#endif /*USE_NGA_SURFACES*/
                if(!gstSnk)
                {
                    DEBUG_ERR("Cannot create sink!");
                    return NULL;
                }
                g_object_set( G_OBJECT(gstSnk), "force-aspect-ratio", TRUE,
												"async", FALSE,
                                               "qos", FALSE,
												"handle-events", TRUE,
												"handle-expose", TRUE,
                                               "max-lateness", (gint64)(-1),
                                               NULL);
                break;
            case XA_DATALOCATOR_OUTPUTMIX:
                DEBUG_INFO("Sink locator type - XA_DATALOCATOR_OUTPUTMIX");
                {
                    /* Get OutputMix adaptation from data locator */
                    XADataLocator_OutputMix* omix = (XADataLocator_OutputMix*)(xaSnk->pLocator);
                    if ( omix->outputMix )
                    {
                        XAOMixImpl* omixDevice = (XAOMixImpl*)(*omix->outputMix);

                        if(omixDevice)
                        {
                            gstSnk = XAOutputMixAdapt_GetSink(omixDevice->adaptationCtx);
                            if(!gstSnk)
                            {
                                DEBUG_ERR("Cannot create sink!");
                                return NULL;
                            }
                            *isobj = XA_BOOLEAN_TRUE;
                        }
                        else
                        {
                            DEBUG_ERR("Warning - NULL outputmix object - default audio output used");
                            gstSnk = gst_element_factory_make("alsasink",name);
                        }
                    }
                    else
                    {
                        DEBUG_ERR("Warning - NULL outputmix object - default audio output used");
                        gstSnk = gst_element_factory_make("alsasink",name);
                    }

                }
                break;
		 	case XA_DATALOCATOR_CONTENTPIPE:
				DEBUG_INFO("XA_DATALOCATOR_CONTENTPIPE");
				gstSnk = gst_element_factory_make("appsink",name);
				break;
            case XA_DATALOCATOR_ADDRESS:
                {
                    gstSnk = gst_element_factory_make("appsink", name);
                    /* Not actually object sink, but attribute used to notify recorder
                     * about appsink (no object sinks applicable in this use case)
                     **/
                    *isobj=TRUE;
                }
                break;
            case XA_DATALOCATOR_IODEVICE:
                /* when only valid IOdevice sinks vibra and LED sinks implemented
                 * at adaptation level, add handling here (in this implementation,
                 * no handling needed as only dummy implementations for those)
                 **/
            default:
                DEBUG_ERR("Incorrect data locator for sink.")
                return NULL;
                break;
        }
    }
    if (gstSnk )
    {
        DEBUG_INFO_A1("Created gstreamer sink element at %x", gstSnk);
    }
    DEBUG_API("<-XAAdaptationBase_CreateGstSink");
    return gstSnk;
}

/**
 * GstElement* XAAdaptationBase_CreateVideoPP( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline
 */
GstElement* XAAdaptationBase_CreateVideoPP( )
{
    GstElement *vpp;
    DEBUG_API("->XAAdaptationBase_CreateVideoPP");
    vpp = gst_pipeline_new("videopp");
    if( vpp )
    {
        GstPad *ghostsink, *ghostsrc;
        GstElement 	*col1,
					*col2,
					*rotate,
					*mirror,
					*box,
					*crop,
					*gamma,
					*balance,
					*scale,
					*scale2,
#ifdef USE_NGA_SURFACES
					*identity,
#endif /*USE_NGA_SURFACES*/
					*queue;


        /* Crete ffmpegcolorspace to convert stream to correct format */
        col1 = gst_element_factory_make( "ffmpegcolorspace", "pp_colsp1");
        if(col1)
        {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col1);
           /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(col1,"sink");
            if(ghostsink)
            {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_sink",ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
            }
        }

        /* create video crop, this will be sink for videoPP pipeline */
        crop = gst_element_factory_make( "videocrop", "pp_crop");
        if(crop)
        {
            DEBUG_INFO("Created crop element");
            gst_bin_add(GST_BIN(vpp), crop);
        }

        /* create video rotate */
        rotate = gst_element_factory_make( "videoflip", "pp_rotate");
        if(rotate)
        {
            DEBUG_INFO("Created rotate element");
            g_object_set(G_OBJECT(rotate), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), rotate);
        }

        /* create video mirror */
        mirror = gst_element_factory_make( "videoflip", "pp_mirror");
        if(mirror)
        {
            DEBUG_INFO("Created mirror element");
            g_object_set(G_OBJECT(mirror), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), mirror);
        }

        /* create video box */
        box = gst_element_factory_make( "videobox", "pp_box");
        if(box)
        {
            DEBUG_INFO("Created videobox element");
            gst_bin_add(GST_BIN(vpp), box);
        }

        /* create video balance */
        balance = gst_element_factory_make( "videobalance", "pp_balance");
        if(balance)
        {
            DEBUG_INFO("Created balance element");
            gst_bin_add(GST_BIN(vpp), balance);
        }

        /* create video gamma */
        gamma = gst_element_factory_make( "gamma", "pp_gamma");
        if(gamma)
        {
            DEBUG_INFO("Created gamma element");
            gst_bin_add(GST_BIN(vpp), gamma);
        }

        /* Create videoscale element to scale postprocessed output to correct size */
        scale = gst_element_factory_make("videoscale", "pp_scale");
        if ( scale )
        {
            DEBUG_INFO("Created videoscale element");
            gst_bin_add(GST_BIN(vpp), scale);
        }
        scale2 = gst_element_factory_make("videoscale", "pp_scale2");
		if ( scale2 )
		{
			GstPad *pad = NULL;
			GstCaps *caps = NULL;
			DEBUG_INFO("Created videoscale element");
			pad = gst_element_get_static_pad(scale2,"src");
			caps = gst_caps_new_simple("video/x-raw-yuv",
					 "width", G_TYPE_INT,0,
					 "height", G_TYPE_INT,0,
					 NULL);
			gst_pad_set_caps(pad, caps);
			gst_bin_add(GST_BIN(vpp), scale2);
		}

        /* create video queue */
        queue = gst_element_factory_make( "queue", "vpp_queue");
        if(queue)
        {
            DEBUG_INFO("Created queue element");
            gst_bin_add(GST_BIN(vpp), queue);
#ifdef USE_NGA_SURFACES
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(queue,"sink");
            if(ghostsink)
            {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_src",ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
            }            
#endif /*USE_NGA_SURFACES*/
        }


        /* Crete ffmpegcolorspace to convert stream to correct format */
        col2 = gst_element_factory_make( "ffmpegcolorspace", "pp_colsp2");
        if(col2)
        {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col2);
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(col2,"src");
            if(ghostsrc)
            {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_src",ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
            }
        }

#ifdef USE_NGA_SURFACES
        //shyward
        /* create identity element */
        identity  = gst_element_factory_make( "identity", "identity" );
        if(identity)
        {
           DEBUG_INFO("Created identity element");
           gst_bin_add(GST_BIN(vpp), identity);   
           /* make this bin link point*/
           ghostsrc = gst_element_get_static_pad(identity,"src");
           if(ghostsrc)
           {
               gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_sink",ghostsrc));
               gst_object_unref(GST_OBJECT(ghostsrc));
           }                
        }        
        if( !(gst_element_link_many(queue,identity,NULL)) )
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
    }
    DEBUG_API("<-XAAdaptationBase_CreateVideoPP");
    return vpp;
}

/**
 * GstElement* XAAdaptationBase_CreateFixedSizeRecordVideoPP( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline with fixed output size to TEST_VIDEO_WIDTH x TEST_VIDEO_HEIGHT
 *              experimental implementation for changing recorder output size
 */
GstElement* XAAdaptationBase_CreateFixedSizeVideoPP( )
{
    GstElement *vpp;
    DEBUG_API("->XAAdaptationBase_CreateFixedSizeVideoPP");
    vpp = gst_pipeline_new("videopp");
    if( vpp )
    {
        GstPad *ghostsink, *ghostsrc;
        GstElement  *col1,
                    *col2,
                    *rotate,
                    *mirror,
                    *box,
                    *crop,
                    *gamma,
                    *balance,
                    *scale,
                    *scale2,
                    *filter,
                    *queue;


        /* Crete ffmpegcolorspace to convert stream to correct format */
        col1 = gst_element_factory_make( "ffmpegcolorspace", "pp_colsp1");
        if(col1)
        {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col1);
           /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(col1,"sink");
            if(ghostsink)
            {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_sink",ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
            }
        }

        /* create video crop, this will be sink for videoPP pipeline */
        crop = gst_element_factory_make( "videocrop", "pp_crop");
        if(crop)
        {
            DEBUG_INFO("Created crop element");
            gst_bin_add(GST_BIN(vpp), crop);
        }

        /* create video rotate */
        rotate = gst_element_factory_make( "videoflip", "pp_rotate");
        if(rotate)
        {
            DEBUG_INFO("Created rotate element");
            g_object_set(G_OBJECT(rotate), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), rotate);
        }

        /* create video mirror */
        mirror = gst_element_factory_make( "videoflip", "pp_mirror");
        if(mirror)
        {
            DEBUG_INFO("Created mirror element");
            g_object_set(G_OBJECT(mirror), "method", FLIP_NONE, NULL);
            gst_bin_add(GST_BIN(vpp), mirror);
        }

        /* create video box */
        box = gst_element_factory_make( "videobox", "pp_box");
        if(box)
        {
            DEBUG_INFO("Created videobox element");
            gst_bin_add(GST_BIN(vpp), box);
        }

        /* create video balance */
        balance = gst_element_factory_make( "videobalance", "pp_balance");
        if(balance)
        {
            DEBUG_INFO("Created balance element");
            gst_bin_add(GST_BIN(vpp), balance);
        }

        /* create video gamma */
        gamma = gst_element_factory_make( "gamma", "pp_gamma");
        if(gamma)
        {
            DEBUG_INFO("Created gamma element");
            gst_bin_add(GST_BIN(vpp), gamma);
        }

        /* Create videoscale element to scale postprocessed output to correct size */
        scale = gst_element_factory_make("videoscale", "pp_scale");
        if ( scale )
        {
            DEBUG_INFO("Created videoscale element");
            gst_bin_add(GST_BIN(vpp), scale);
        }
        scale2 = gst_element_factory_make("videoscale", "pp_scale2");
        if ( scale2 )
        {
            GstPad *pad = NULL;
            GstCaps *caps = NULL;
            DEBUG_INFO("Created videoscale element");
            pad = gst_element_get_static_pad(scale2,"src");
            caps = gst_caps_new_simple("video/x-raw-yuv",
                     "width", G_TYPE_INT,0,
                     "height", G_TYPE_INT,0,
                     NULL);
            gst_pad_set_caps(pad, caps);
            gst_bin_add(GST_BIN(vpp), scale2);
        }

        /* create capsfilter for fixed video size */
        filter = gst_element_factory_make("capsfilter", "pp_filter");
        if ( filter )
        {

            g_object_set( G_OBJECT(filter), "caps",
                            gst_caps_new_simple("video/x-raw-yuv",
                                                "width", G_TYPE_INT, TEST_VIDEO_WIDTH,
                                                "height", G_TYPE_INT, TEST_VIDEO_HEIGHT, NULL)
                                                ,NULL );
            gst_bin_add(GST_BIN(vpp), filter);
        }

        /* create video queue */
        queue = gst_element_factory_make( "queue", "vpp_queue");
        if(queue)
        {
            gst_bin_add(GST_BIN(vpp), queue);
        }


        /* Crete ffmpegcolorspace to convert stream to correct format */
        col2 = gst_element_factory_make( "ffmpegcolorspace", "pp_colsp2");
        if(col2)
        {
            DEBUG_INFO("Created ffmpegcolorspace element");
            gst_bin_add(GST_BIN(vpp), col2);
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(col2,"src");
            if(ghostsrc)
            {
                gst_element_add_pad(vpp, gst_ghost_pad_new("videopp_src",ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
            }
        }
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
                                    filter,
                                    col2,
                                    NULL)
                                    ) )
        {
            DEBUG_ERR("Could not link videopp elements!!");
            gst_object_unref(vpp);
            vpp = NULL;
        }
    }
    DEBUG_API("<-XAAdaptationBase_CreateFixedSizeVideoPP");
    return vpp;
}



/**
 * GstElement* XAAdaptationBase_CreateVideoPPBlackScr( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline for black screen
 */
GstElement* XAAdaptationBase_CreateVideoPPBlackScr( )
{
    GstElement *vppBScr;
    DEBUG_API("->XAAdaptationBase_CreateVideoPPBlackScr");
    vppBScr = gst_pipeline_new("videoppBScr");
    if( vppBScr )
    {
        GstPad *ghostsrc=NULL;
        GstElement *testVideo=NULL, *scale=NULL;
        GstElement *ffmpegcolorspace=NULL;

        testVideo = gst_element_factory_make( "videotestsrc", "videotest");
        if(testVideo)
        {
            DEBUG_INFO("Created videotestsrc element");

            g_object_set(G_OBJECT(testVideo), "pattern", (gint)2, "num-buffers", (gint)1, NULL);
            gst_bin_add(GST_BIN(vppBScr), testVideo);
        }

        scale = gst_element_factory_make("videoscale", "BSrc_scale");
        if(scale)
        {
            DEBUG_INFO("Created videoscale element");
            gst_bin_add(GST_BIN(vppBScr), scale);
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(scale,"src");
            if(ghostsrc)
            {
                gst_element_add_pad(vppBScr, gst_ghost_pad_new("videoppBSrc_src",ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
            }
        }
        ffmpegcolorspace = gst_element_factory_make("ffmpegcolorspace", "BlackScrFfmpeg");
        gst_bin_add(GST_BIN(vppBScr), ffmpegcolorspace);
        if( !(gst_element_link_many(testVideo, ffmpegcolorspace, scale, NULL)) )
        {
            DEBUG_ERR("Could not link videoppBSrc elements!!");
            gst_object_unref(vppBScr);
            vppBScr = NULL;
        }
    }
    DEBUG_API("<-XAAdaptationBase_CreateVideoPPBlackScr");
    return vppBScr;
}

/**
 * GstElement* XAAdaptationBase_CreateInputSelector( )
 * @return GstElement* - return newly created input selector
 * Description: Create input selector to processing between black screen and video screen
 */
GstElement* XAAdaptationBase_CreateInputSelector( )
{
    GstElement *inputSelector;
    DEBUG_API("->XAAdaptationBase_CreateInputSelector");
    inputSelector = gst_element_factory_make("input-selector", "input-selector");
    if( inputSelector )
    {
        g_object_set(G_OBJECT(inputSelector), "select-all", TRUE, NULL);
    }
    DEBUG_API("<-XAAdaptationBase_CreateInputSelector");
    return inputSelector;
}

/**
 * GstElement* XAAdaptationBase_CreateAudioPP( )
 * @return GstElement* - return newly created gst pipeline element
 * Description: Create video processing pipeline
 */
GstElement* XAAdaptationBase_CreateAudioPP( )
{
    GstElement *app;
    gboolean ok = TRUE;
    DEBUG_API("->XAAdaptationBase_CreateAudioPP");
    app = gst_pipeline_new("audiopp");
    if( app )
    {
        GstPad *ghostsink, *ghostsrc;
        GstElement *ac,*vol,*eq,*queue,*pan, *ac2;

        /* first and last elements should be audioconverts to match sink and encoder formats */
        ac = gst_element_factory_make( "audioconvert", "pp_ac");
        if (ac)
        {
            ok = gst_bin_add(GST_BIN(app), ac);
            /* make this bin link point*/
            if (ok)
            {
                ghostsink = gst_element_get_static_pad(ac,"sink");
                ok = gst_element_add_pad(app, gst_ghost_pad_new("sink",ghostsink));
                gst_object_unref(GST_OBJECT(ghostsink));
            }
        }
        ac2 = gst_element_factory_make( "audioconvert", "pp_ac2");
        if (ac2 && ok)
        {
            ok = gst_bin_add(GST_BIN(app), ac2);
            /* make this bin link point*/
            if (ok)
            {
                ghostsrc = gst_element_get_static_pad(ac2,"src");
                ok = gst_element_add_pad(app, gst_ghost_pad_new("src",ghostsrc));
                gst_object_unref(GST_OBJECT(ghostsrc));
            }
        }

        vol = gst_element_factory_make( "volume", "pp_vol");
        /* create volume controller */
        if (vol && ok)
        {
            ok = gst_bin_add(GST_BIN(app), vol);
            g_object_set( G_OBJECT(vol), "volume", (gdouble)1, NULL );
        }
        /* create 10-band equalizer */
        eq = gst_element_factory_make( "equalizer-10bands", "pp_equ");
        if (eq && ok)
        {
            ok = gst_bin_add(GST_BIN(app), eq);
        }
        /* create audio queue */
        queue = gst_element_factory_make( "queue", "app_queue");
        if(queue && ok)
        {
            ok = gst_bin_add(GST_BIN(app), queue);
            g_object_set (G_OBJECT (queue), "max-size-buffers", 2, NULL);
        }
        /* create audio pan effect */
        pan = gst_element_factory_make( "audiopanorama", "pp_pan");
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
            ghostsink = gst_element_get_static_pad(queue,"sink");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("sink",ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
        }
        else if (vol && ok)
        {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(vol,"sink");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("sink",ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
        }
        else if (pan && ok)
        {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(pan,"sink");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("sink",ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
        }
        else if (eq && ok)
        {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(eq,"sink");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("sink",ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
        }
        else if (ac2 && ok)
        {
            /* make this bin link point*/
            ghostsink = gst_element_get_static_pad(ac2,"sink");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("sink",ghostsink));
            gst_object_unref(GST_OBJECT(ghostsink));
        }
        
        if (ac2)
        {
            // ghost src above
        }
        else if (eq && ok)
        {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(eq,"src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src",ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
        }
        else if (pan && ok)
        {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(pan,"src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src",ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
        }
        else if (vol && ok)
        {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(vol,"src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src",ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
        }
        else if (queue && ok)
        {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(queue,"src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src",ghostsrc));
            gst_object_unref(GST_OBJECT(ghostsrc));
        }
        else if (ac && ok)
        {
            /* make this bin link point*/
            ghostsrc = gst_element_get_static_pad(ac,"src");
            ok = gst_element_add_pad(app, gst_ghost_pad_new("src",ghostsrc));
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

    DEBUG_API("<-XAAdaptationBase_CreateAudioPP");
    return app;
}

/* called when pad is actually blocking/ gets unblocked*/
void XAAdaptationBase_PadBlockCb(GstPad *pad, gboolean blocked, gpointer user_data)
{
    DEBUG_API_A2("->XAAdaptationBase_PadBlockCb   pad \"%s\" of \"%s\" ",
                        GST_OBJECT_NAME(pad),
                        GST_OBJECT_NAME(gst_pad_get_parent_element(pad)) );
    DEBUG_API_A1("<-XAAdaptationBase_PadBlockCb   blocked:%d",blocked);
}

/* utility to set same fields for all media types in caps */
void XAAdaptationBase_SetAllCaps (GstCaps * caps, char *field, ...)
{
    GstStructure *structure;
    va_list var_args;
    int i;

    for (i = 0; i < gst_caps_get_size (caps); i++)
    {
        structure = gst_caps_get_structure (caps, i);
        va_start (var_args, field);
        gst_structure_set_valist (structure, field, var_args);
        va_end (var_args);
    }
}

/*
 * void* XAAdaptationBase_ContentPipeScrThrFunc( void* arg )
 */
void* XAAdaptationBase_ContentPipeScrThrFunc( void* arg )
{
	XAAdaptCpThrCtx* thrCtx = (XAAdaptCpThrCtx*)arg;
	XAresult ret;
	CPresult cpRet;
	XAuint32 requestedBytes = CONTENT_PIPE_BUFFER_SIZE;
	CP_CHECKBYTESRESULTTYPE eResult;

	DEBUG_API("->XAAdaptationBase_ContentPipeScrThrFunc");


	/* Wait until playstate is changed */
	ret = XAImpl_WaitSemaphore( thrCtx->stateSem );
	if ( ret != XA_RESULT_SUCCESS)
	{
	    DEBUG_ERR("Could not start semaphore");
	}

	thrCtx->state = CPStateRunning;

	do
	{
	    GstBuffer  *buffer = NULL;            /* Gstreamer buffer */

        if ( thrCtx->state == CPStatePaused )
        {
            /* Wait until playstate is changed */
            ret = XAImpl_WaitSemaphore( thrCtx->stateSem );
            if ( ret != XA_RESULT_SUCCESS)
            {
                DEBUG_ERR("Could not start semaphore");
                thrCtx->state = CPStateError;
                break;
            }
        }

        if ( thrCtx->state == CPStateWaitForData )
        {
        }
	    /* Check do we have enough bytes in pipe */
	    cpRet = thrCtx->pipe->pContentPipe->CheckAvailableBytes(&(thrCtx->dataHandle), requestedBytes, &eResult);
	    if ( cpRet != EXIT_SUCCESS )
	    {
	        thrCtx->state = CPStateError;
	    }

		if ( eResult == CP_CheckBytesOk)
		{ /* We have enough bytes in content pipe */
		    thrCtx->state = CPStateRunning;
		    DEBUG_API("CP_CheckBytesOk");

			if ( thrCtx->cpConfig == XA_READ )
			{ /* OMX-AL implementation allocates buffers */

			    /* Pointer to OMX-AL RI allocated data */
			    gpointer cpBuffer = NULL;

			    /* Allocate new buffer with preferred size */
			    cpBuffer = g_malloc0(requestedBytes );
			    cpRet = thrCtx->pipe->pContentPipe->Read( &(thrCtx->dataHandle), (CPbyte*)cpBuffer, requestedBytes );
			    if ( cpRet != EXIT_SUCCESS )
			    {
			        DEBUG_ERR("Could not read data from content pipe!");
			        thrCtx->state = CPStateError;
                    break;
			    }
			    else
			    {
			        DEBUG_INFO_A1("Readed %u bytes", requestedBytes );
                    /* Create gstBuffer, GStreamer frees data  */
			        buffer = gst_app_buffer_new( (void*)cpBuffer, requestedBytes, g_free, cpBuffer );
                    if ( !buffer )
                    {
                        DEBUG_ERR("Could not allocate buffer for content pipe source!");
                        thrCtx->state = CPStateError;
                        break;
                    }
			    }
			}
			else
			{ /* Content pipe implementation allocates buffers */
			    gpointer pipeBuffer = NULL;
			    gpointer omxBuffer = NULL;

			    cpRet = thrCtx->pipe->pContentPipe->ReadBuffer( &(thrCtx->dataHandle), (CPbyte**)&pipeBuffer, &requestedBytes, FALSE);
                if ( cpRet != EXIT_SUCCESS )
                {
                    DEBUG_ERR("Could not read data from content pipe!");
                    thrCtx->state = CPStateError;
                    break;
                }
                else
                {
                    DEBUG_INFO_A1("Readed %u bytes", requestedBytes );
                    /* Copy pipe allocated data into own buffer */
                    omxBuffer = g_malloc0(requestedBytes );
                    memcpy( omxBuffer, pipeBuffer, requestedBytes );

                    /* Create gstBuffer, GStreamer frees data  */
                    buffer = gst_app_buffer_new( omxBuffer, requestedBytes, g_free, omxBuffer );
                    if ( !buffer )
                    {
                        DEBUG_ERR("Could not allocate buffer for content pipe source!");
                        thrCtx->state = CPStateError;
                        break;
                    }
                    /* Release readbuffer */
                    cpRet = thrCtx->pipe->pContentPipe->ReleaseReadBuffer( &(thrCtx->dataHandle), (CPbyte*)pipeBuffer );
                    if ( cpRet != EXIT_SUCCESS )
                    {
                        DEBUG_ERR("Could not release readbuffer!");
                        thrCtx->state = CPStateError;
                        break;
                    }
                    pipeBuffer = NULL;

                }
			}
			if ( cpRet == EXIT_SUCCESS  )
            {
			    if( buffer )
			    {
			    DEBUG_INFO("Pushing buffer");
                gst_app_src_push_buffer( GST_APP_SRC(thrCtx->appSrc), GST_BUFFER(buffer) );
			    }
            }
		}
		else if ( eResult == CP_CheckBytesNotReady )
		{
			DEBUG_API("CP_CheckBytesNotReady");
			thrCtx->state = CPStateWaitForData;
		}
		else if ( eResult == CP_CheckBytesInsufficientBytes )
		{
			DEBUG_API("CP_CheckBytesInsufficientBytes");
			if ( requestedBytes > 0 )
			{
				requestedBytes = requestedBytes - 1 ;
			}
			else
			{
				thrCtx->state = CPStateError;
			}
		}
		else if ( eResult == CP_CheckBytesAtEndOfStream)
		{
			DEBUG_API("XAAdaptationBase_ContentPipeScrThrFunc-> CP_CheckBytesAtEndOfStream");
		    thrCtx->state = CPStateEOS;
		}
	}while (thrCtx->state == CPStateRunning || thrCtx->state == CPStateWaitForData || thrCtx->state == CPStatePaused );

	if ( thrCtx->state == CPStateError )
	{
	    /* If error goto beginning of content */
	    cpRet = thrCtx->pipe->pContentPipe->SetPosition( &(thrCtx->dataHandle),0, CP_OriginBegin);
	}
	else if ( thrCtx->state == CPStateEOS )
	{
	    /* Send EOS to appSrc */
	    gst_app_src_end_of_stream( GST_APP_SRC(thrCtx->appSrc) );
	    /* Set position to beginning */
	    cpRet = thrCtx->pipe->pContentPipe->SetPosition( &(thrCtx->dataHandle),0, CP_OriginBegin);
	}
	else if ( thrCtx->state == CPStateStopped )
	{
	    cpRet = thrCtx->pipe->pContentPipe->SetPosition( &(thrCtx->dataHandle),0, CP_OriginBegin);
	}

	thrCtx->state = CPStateInitialized;

   DEBUG_API("<-XAAdaptationBase_ContentPipeScrThrFunc");
   return NULL;
}

/*
 * void* XAAdaptationBase_ContentPipeSinkThrFunc( void* arg )
 */
void* XAAdaptationBase_ContentPipeSinkThrFunc( void* arg )
{
    XAAdaptCpThrCtx* thrCtx = (XAAdaptCpThrCtx*)arg;
    XAresult ret;
    CPresult cpRet;
    XAboolean paused;
    XAuint32 position = 0;

    DEBUG_API("->XAAdaptationBase_ContentPipeSinkThrFunc");

    thrCtx->state = CPStateStarted;

    /* Wait until recordstate is changed */
    ret = XAImpl_WaitSemaphore( thrCtx->stateSem );
    if ( ret != XA_RESULT_SUCCESS)
    {
        DEBUG_ERR("Could not start semaphore");
    }
    thrCtx->state = CPStateRunning;

    do
    {
        GstBuffer  *buffer;

        if ( thrCtx->state == CPStatePaused )
        {
        	/*If paused get position from end of the file*/
        	paused = XA_BOOLEAN_TRUE;
        	DEBUG_API("Get New position");
        	cpRet = thrCtx->pipe->pContentPipe->GetPosition(&(thrCtx->dataHandle), &position);
        	cpRet = thrCtx->pipe->pContentPipe->SetPosition(&(thrCtx->dataHandle), (CPint)position, CP_OriginEnd);
            /* Wait until playstate is changed */
            ret = XAImpl_WaitSemaphore( thrCtx->stateSem );
            if ( ret != XA_RESULT_SUCCESS)
            {
                DEBUG_ERR("Could not start waiting content pipe state semaphore");
                thrCtx->state = CPStateError;
                break;
            }
        }

        buffer = gst_app_sink_pull_buffer(GST_APP_SINK(thrCtx->appSink) );
        if ( !buffer )
        {
            DEBUG_INFO("No buffer in gstAppSink!")
            if ( gst_app_sink_is_eos( thrCtx->appSink) )
            {
				DEBUG_API("XAAdaptationBase_ContentPipeSinkThrFunc-> CPStateEOS")
                thrCtx->state = CPStateEOS;
            }
            continue;
        }

        if ( thrCtx->cpConfig == XA_WRITE )
        { /* OMX-AL implementation allocates buffers */

            /* Write data to content pipe */
            cpRet = thrCtx->pipe->pContentPipe->Write( &(thrCtx->dataHandle), (CPbyte *)GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE(buffer) );
            if ( cpRet != EXIT_SUCCESS )
            {
                DEBUG_ERR("Could not write data to content pipe!");
                thrCtx->state = CPStateError;
                break;
            }
        }
        else
        { /* Use content pipe allocated buffers */

            gpointer cpBuffer = NULL;

            /* Request buffer from content pipe with gst-buffer size */
            cpRet = thrCtx->pipe->pContentPipe->GetWriteBuffer( &(thrCtx->dataHandle), (CPbyte**)&cpBuffer, GST_BUFFER_SIZE(buffer) );
            if ( cpRet != EXIT_SUCCESS )
            {
                DEBUG_ERR("Could not get write buffer from content pipe!");
                thrCtx->state = CPStateError;
                break;
            }

            /* Copy data from gst-buffer to content pipe buffer */
            memcpy( cpBuffer,GST_BUFFER_DATA(buffer),GST_BUFFER_SIZE(buffer));

            /* Write buffer to content pipe */
            cpRet = thrCtx->pipe->pContentPipe->WriteBuffer( &(thrCtx->dataHandle), cpBuffer, GST_BUFFER_SIZE(buffer) );
            if ( cpRet != EXIT_SUCCESS )
            {
                DEBUG_ERR("Could not write buffer to content pipe!");
                thrCtx->state = CPStateError;
                break;
            }

            g_free(cpBuffer);
            cpBuffer = NULL;
        }

        if ( gst_app_sink_is_eos( thrCtx->appSink) )
        {
            thrCtx->state = CPStateEOS;
        }

    }while (thrCtx->state == CPStateRunning || thrCtx->state == CPStatePaused );

    if ( thrCtx->state == CPStateError )
    {
        /* If error goto beginning of content */
        cpRet = thrCtx->pipe->pContentPipe->SetPosition( &(thrCtx->dataHandle),0, CP_OriginBegin);
    }
    else if ( thrCtx->state == CPStateStopped )
    {
        cpRet = thrCtx->pipe->pContentPipe->SetPosition( &(thrCtx->dataHandle),0, CP_OriginBegin);
    }

    DEBUG_API("<-XAAdaptationBase_ContentPipeSinkThrFunc");
    return NULL;
}

/*
 * CPresult XAAdaptationBase_ContentPipeSrcCb(CP_EVENTTYPE eEvent, CPuint iParam)
 * Description: Callback function for content pipe source
 * @param: CP_EVENTTYPE eEvent - Callback event
 * @param: CPuint iParam - Param related to event
 *
 *  No actual functionality can be handled in callback, since current
 *  Content Pipe specification lacks methods of supplying client context and/or
 *  reference to context pipe handle with CP callback.
 *  Khronos group is in progress on proposals for extending this functionality.
 *
 */
CPresult XAAdaptationBase_ContentPipeSrcCb(CP_EVENTTYPE eEvent, CPuint iParam)
{
	XAresult ret = XA_RESULT_SUCCESS;
	switch (eEvent )
	{
	case CP_BytesAvailable:
		/* Restart reading thread */
		break;
	case CP_Overflow:
		/* Handle error */
		break;
	case CP_PipeDisconnected:
		/* Reconnect pipe */
	case CP_EventMax:
		break;
	default:
		break;
	}
	return ret;
}

/*
 * CPresult XAAdaptationBase_ContentPipeSinkCb(CP_EVENTTYPE eEvent, CPuint iParam)
 * Description: Callback function for content pipe sink
 * @param: CP_EVENTTYPE eEvent - Callback event
 * @param: CPuint iParam - Param related to event
 *
 *  No actual functionality can be handled in callback, since current
 *  Content Pipe specification lacks methods of supplying client context and/or
 *  reference to context pipe handle with CP callback.
 *  Khronos group is in progress on proposals for extending this functionality.
 *
 */
CPresult XAAdaptationBase_ContentPipeSinkCb(CP_EVENTTYPE eEvent, CPuint iParam)
{
	XAresult ret = XA_RESULT_SUCCESS;
	switch (eEvent )
	{
	case CP_BytesAvailable:
		/*Restart write thread.*/
		break;
	case CP_Overflow:
		/*Handle error */
		break;
	case CP_PipeDisconnected:
		/*Reconnect pipe */
	default:
		break;
	}
	return ret;

}
