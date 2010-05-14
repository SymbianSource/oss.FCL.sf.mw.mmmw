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
#include <gst.h>
#include "xacameraadaptctx.h"
#include "xaadaptationgst.h"
#include <photography.h>
//#include "XAStaticCapsAdaptation.h"


XAboolean cameraRealized = XA_BOOLEAN_FALSE;
XACameraAdaptationCtx_* cameraCtx = NULL;

/*
 * gboolean XACameraAdapt_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
 * MediaPlayer Gst-bus message handler (Callback)
 */
gboolean XACameraAdapt_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
{
    XACameraAdaptationCtx* mCtx = (XACameraAdaptationCtx*)data;
    XAAdaptEvent event = {XA_CAMERAITFEVENTS,XA_CAMERACBEVENT_FOCUSSTATUS,1,NULL};
    XAuint32 status;

    /* only listen to pipeline messages */
    if(GST_MESSAGE_SRC(message)==(GstObject*)(mCtx->baseObj.bin) )
    {
        DEBUG_API_A2("->XACameraAdapt_GstBusCb:\"%s\" from object \"%s\"",
                        GST_MESSAGE_TYPE_NAME(message), GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));

        if ( GST_MESSAGE_TYPE(message)== GST_MESSAGE_ASYNC_DONE )
        {
            /* some async sequence ended */
            XAAdaptationGst_CompleteAsyncWait((&mCtx->baseObj));
        }
        else if (strncmp(GST_MESSAGE_TYPE_NAME(message), GST_PHOTOGRAPHY_AUTOFOCUS_DONE,
                            strlen(GST_PHOTOGRAPHY_AUTOFOCUS_DONE))==0 )
        {
            DEBUG_INFO("Autofocus done!.")
            status = XA_CAMERA_FOCUSMODESTATUS_REACHED;
            event.data = &status;
            XAAdaptationBase_SendAdaptEvents(&(mCtx->baseObj.baseObj), &event );
        }
        else if ( strncmp(GST_MESSAGE_TYPE_NAME(message), GST_PHOTOGRAPHY_SHAKE_RISK,
                            strlen(GST_PHOTOGRAPHY_SHAKE_RISK)) ==0 )
        {
            DEBUG_INFO("Camera shake risk!")
        }
        DEBUG_API("<-XACameraAdapt_GstBusCb");
    }
    return TRUE;
}

/*
 * XAAdaptationGstCtx* XACameraAdapt_Create()
 * Allocates memory for Camera Adaptation Context and makes 1st phase initialization
 * @returns XACameraAdaptationCtx* - Pointer to created context
 */
XAAdaptationBaseCtx* XACameraAdapt_Create(XAuint32 deviceID)
{
    XACameraAdaptationCtx *pSelf = (XACameraAdaptationCtx*) calloc(1, sizeof(XACameraAdaptationCtx));
    DEBUG_API("->XACameraAdapt_Create");
    if ( pSelf)
    {
        if( XAAdaptationGst_Init(&(pSelf->baseObj),XACameraAdaptation)
                    != XA_RESULT_SUCCESS )
        {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
        }
        else
        {
            pSelf->deviceID = deviceID;
            pSelf->curMirror = XA_VIDEOMIRROR_NONE;
            pSelf->curRotation = 0;
            pSelf->recording = XA_BOOLEAN_FALSE;
            pSelf->playing = XA_BOOLEAN_FALSE;
            pSelf->snapshotting = XA_BOOLEAN_FALSE;
            cameraCtx = pSelf; /* Store global pointer */
            DEBUG_INFO_A1("Stored global camera ponter to %x", cameraCtx);
            cameraRealized = XA_BOOLEAN_FALSE;
        }
    }

    DEBUG_API("<-XACameraAdapt_Create");
    return (XAAdaptationBaseCtx*)&pSelf->baseObj;
}

/*
 * XAresult XACameraAdapt_PostInit()
 * 2nd phase initialization of Camera Adaptation Context
 */
XAresult XACameraAdapt_PostInit(XAAdaptationBaseCtx* bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;

    XACameraAdaptationCtx* ctx = NULL;
    DEBUG_API("->XACameraAdapt_PostInit");
    if(bCtx == NULL || bCtx->ctxId != XACameraAdaptation)
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XACameraAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
    }
    ctx = (XACameraAdaptationCtx*)bCtx;

    if ( !ctx )
    {
        return XA_RESULT_INTERNAL_ERROR;
    }

    XAAdaptationGst_PostInit( &(ctx->baseObj) );

    ctx->baseObj.bin = gst_element_factory_make( "camerabin", "camera");
    if ( !ctx->baseObj.bin )
    {
        DEBUG_ERR("Failed to create CameraBin");
        return XA_RESULT_INTERNAL_ERROR;
    }

    /* Create Gst bus listener. */
    ret = XAAdaptationGst_InitGstListener(&(ctx->baseObj));
    if( ret!=XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Bus listener creation failed!!");
        return ret;
    }
    /* Add Camera specific handler */
    if(ctx->baseObj.bus)
    {
        ctx->baseObj.busCb = XACameraAdapt_GstBusCb;
        gst_bus_add_signal_watch( ctx->baseObj.bus);
        g_signal_connect(ctx->baseObj.bus, "message::autofocus-done", G_CALLBACK(ctx->baseObj.busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::shake-risk", G_CALLBACK(ctx->baseObj.busCb), ctx );
        g_signal_connect(ctx->baseObj.bus, "message::async-done", G_CALLBACK(ctx->baseObj.busCb), ctx );
    }
    else
    {
        DEBUG_ERR("Failed to create message bus");
        return XA_RESULT_INTERNAL_ERROR;
    }


    /* SET UP CAMERABIN */

    /* use test video source if set, camerabin default (v4l2src) otherwise */
#ifdef XA_USE_TEST_PLUGINS
    if(ctx->deviceID == XA_ADAPTID_VIDEOTESTSRC || ctx->deviceID == XA_DEFAULTDEVICEID_CAMERA )
#else
    if(ctx->deviceID == XA_ADAPTID_VIDEOTESTSRC )
#endif
    {
        g_object_set( G_OBJECT(ctx->baseObj.bin), "videosrc", gst_element_factory_make("videotestsrc", "videotestsrc"), NULL );
    }

    /* set viewfinder element to be fake for the time of preroll.. if ghost pad added now,
     * stupid camerabin makes circular linking...
     */
    g_object_set( G_OBJECT(ctx->baseObj.bin), "vfsink" ,gst_element_factory_make("fakesink", "fakevfsink"), NULL );

    /* Setup camerabin to produce raw video */
    g_object_set( G_OBJECT(ctx->baseObj.bin), "videomux",NULL, NULL );
    g_object_set( G_OBJECT(ctx->baseObj.bin), "videoenc",NULL, NULL );
    g_object_set( G_OBJECT(ctx->baseObj.bin), "mute", TRUE, NULL );
    g_object_set( G_OBJECT(ctx->baseObj.bin), "async-handling", FALSE, NULL);
	g_object_set( G_OBJECT(ctx->baseObj.bin), "mode",(gint)1, NULL);


    /* drive camerabin to READY to create the elements inside bin */
    gst_element_set_state( GST_ELEMENT(ctx->baseObj.bin), GST_STATE_READY);

    if(ctx->deviceID == XA_ADAPTID_VIDEOTESTSRC)
    {   /* set properties for videotestsrc */
        GstElement *testsrc = gst_bin_get_by_name(GST_BIN(ctx->baseObj.bin), "videotestsrc");
        g_object_set( G_OBJECT(testsrc),"is-live", TRUE, NULL);
        g_object_set( G_OBJECT(testsrc),"do-timestamp", TRUE, NULL);
        gst_object_unref(G_OBJECT(testsrc));
    }

    /* do some filter optimization */
#ifdef XA_USE_TEST_PLUGINS
    g_object_set( G_OBJECT(ctx->baseObj.bin), "filter-caps",
                    gst_caps_new_simple("video/x-raw-yuv",
                                        "format", GST_TYPE_FOURCC,GST_MAKE_FOURCC('I','4','2','0'),
                                        "framerate",GST_TYPE_FRACTION_RANGE,0,1,30,1, NULL)
                ,NULL );
#else
    g_object_set( G_OBJECT(ctx->baseObj.bin), "filter-caps",
                    gst_caps_new_simple("video/x-raw-yuv",
                                        "format", GST_TYPE_FOURCC,GST_MAKE_FOURCC('Y','U','Y','2'),
                                        "framerate",GST_TYPE_FRACTION_RANGE,0,1,30,1, NULL)
                ,NULL );

#endif

    /* now, unlink fake sink, create camera post processing pipeline and create ghost pad from it */
    {
        GstElement *fakesink = gst_bin_get_by_name(GST_BIN(ctx->baseObj.bin),"fakevfsink");
        GstPad *fakepad = gst_element_get_static_pad(fakesink,"sink");
        GstPad *linkedpad = gst_pad_get_peer(fakepad);
        GstElement *linkedelement = gst_pad_get_parent_element(linkedpad);
        GstElement * cameraPP = NULL;
        GstElement * camfilter = NULL;
        GstElement *tee = NULL;

        /* Unlink fakesink */
        gst_element_unlink(linkedelement,fakesink);
        /* Create VideoPP pipeline for Camera object */
        cameraPP = XAAdaptationGst_CreateVideoPP();
        g_object_set( G_OBJECT(cameraPP),"name", "videopp_camera", NULL);
        gst_bin_add( GST_BIN(ctx->baseObj.bin), cameraPP );
        /*Link videoPP into camera bin last element */
        if (! gst_element_link( linkedelement, cameraPP ))
            {
                 DEBUG_ERR("Could not link VideoPP to Camera bin!!");
                 return XA_RESULT_INTERNAL_ERROR;
            }

        /* Add extra filter for caps negotiable after post processing*/
        camfilter = gst_element_factory_make("capsfilter", "camfilter");
        gst_bin_add( GST_BIN(ctx->baseObj.bin), camfilter );
        if(! gst_element_link( cameraPP, camfilter ))
            {
                 DEBUG_ERR("Could not link camera bin  to camerafilter!!");
                 return XA_RESULT_INTERNAL_ERROR;
            }

        /* Add tee element into camerabin */
        tee = gst_element_factory_make( "tee", "CamTee");
        gst_bin_add( GST_BIN(ctx->baseObj.bin), tee);
        if (! gst_element_link( camfilter, tee ))
            {
                 DEBUG_ERR("Could not link camera filter  to tee element!!");
                 return XA_RESULT_INTERNAL_ERROR;
            }
		/* Unref */
        gst_object_unref(linkedelement);
        gst_object_unref(linkedpad);
        gst_object_unref(fakepad);
        gst_bin_remove(GST_BIN(ctx->baseObj.bin),fakesink);
        gst_object_unparent(GST_OBJECT(fakesink));
    }

    if ( ret == XA_RESULT_SUCCESS )
    {
    	cameraRealized = XA_BOOLEAN_TRUE;
    }

    DEBUG_API("<-XACameraAdapt_PostInit");
    return ret;
}

/*
 * void XACameraAdapt_Destroy(XACameraAdaptationCtx* ctx)
 * Destroys Camera Adaptation Context
 * @param ctx - Camera Adaptation context to be destroyed
 */
void XACameraAdapt_Destroy(XAAdaptationBaseCtx* bCtx)
{
	XACameraAdaptationCtx* ctx = NULL;

	DEBUG_API("->XACameraAdapt_Destroy");
	if(bCtx == NULL || bCtx->ctxId != XACameraAdaptation )
	{
		DEBUG_ERR("Invalid parameter!!");
		DEBUG_API("<-XACameraAdapt_Destroy");
		return;
	}
	ctx = (XACameraAdaptationCtx*)bCtx;

    XAAdaptationGst_Free(&(ctx->baseObj));

    free(ctx);
    ctx = NULL;
    cameraCtx = NULL;
    cameraRealized = XA_BOOLEAN_FALSE;
    DEBUG_API("<-XACameraAdapt_Destroy");
}

