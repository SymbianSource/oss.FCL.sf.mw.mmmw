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
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include "xaadaptationgst.h"
#include "xasnapshotitfadaptation.h"
#include "xamediarecorderadaptctx.h"
#include "xametadataadaptation.h"
#include "xacameraadaptctx.h"
#include "xacapabilitiesmgr.h"

#define SSMAXPIC 30
#define SSMAXFPS 30 /*technically, same as FPS of video stream*/
#define SSMINFPS 1

extern XAboolean cameraRealized;
extern XACameraAdaptationCtx_* cameraCtx;

/* Forward declarations for internal methods */
XAresult XASnapshotItfAdaptation_CreateSnapshotPipeline(XAAdaptationGstCtx* bCtx);
XAresult XASnapshotItfAdaptation_FreeSnapshot(XAAdaptationGstCtx* bCtx);
const char* XASnapshotItfAdaptation_GetFileSuffix(XADataFormat_MIME* format);
void XASnapshotItfAdaptation_AllocNextFilename(char** fname, const char* template);
void XASnapshotItfAdaptation_StopSnapshotting(XAAdaptationGstCtx* bCtx);

/* callbacks */
gboolean XASnapshotItfAdaptation_SnapshotBusCb( GstBus *bus, GstMessage *message, gpointer data );
void XASnapshotItfAdaptation_BufferHandoffCb( GstElement *extract, GstBuffer *buffer, gpointer data);

/*********************************
 * SnapshotItf adaptation methods
 *********************************/

/*
 * XAresult XASnapshotItfAdaptation_InitiateSnapshot(XAAdaptationGstCtx* bCtx,
 *                                                 XAuint32 numberOfPictures,
 *                                                 XAuint32 fps,
 *                                                 XAboolean freezeViewFinder,
 *                                                 XADataSink* sink)
 */
XAresult XASnapshotItfAdaptation_InitiateSnapshot(XAAdaptationGstCtx* bCtx,
                                                  XAuint32 numberOfPictures,
                                                  XAuint32 fps,
                                                  XAboolean freezeViewFinder,
                                                  XADataSink* sink)
{
    XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*)bCtx;
    XADataLocator_URI* uri=NULL;

    DEBUG_API("->XASnapshotItfAdaptation_InitiateSnapshot");
    /* Store initialization variables */
    mCtx->snapshotVars.numpics = numberOfPictures;
    mCtx->snapshotVars.fps = fps;
    mCtx->snapshotVars.freeze =freezeViewFinder;
    mCtx->snapshotVars.parsenegotiated = FALSE;
    mCtx->snapshotVars.snapshotbuffer = NULL;

    if( sink )
    {   /* parse file sink name*/
        if( sink->pLocator && *((XAuint32*)(sink->pLocator)) == XA_DATALOCATOR_URI )
        {
            uri = (XADataLocator_URI*)sink->pLocator;
            if ( uri->URI != NULL )
            {
                /* parse filename template: <prefix><nnnn><suffix> */
                mCtx->snapshotVars.fnametemplate = (char*)calloc(1,strlen((char*)uri->URI)+10);
                DEBUG_INFO_A1("URI: %s", uri->URI);
                if(strncmp((char*)uri->URI, "file://", 7) == 0)
                {
                    strcat(mCtx->snapshotVars.fnametemplate, (char*)&((uri->URI)[7]));
                }
                else
                {
                    strcat(mCtx->snapshotVars.fnametemplate, (char*)uri->URI);
                }
                strcat(mCtx->snapshotVars.fnametemplate, "%04d.");
                strcat(mCtx->snapshotVars.fnametemplate,
                        XASnapshotItfAdaptation_GetFileSuffix(sink->pFormat));
                DEBUG_INFO_A1("->template name %s", mCtx->snapshotVars.fnametemplate);
            }
            else
            {
                DEBUG_ERR("No uri specified.");
                return XA_RESULT_PARAMETER_INVALID;
            }
        }
        else
        { /* take snapshot to memory buffer */
            if( mCtx->snapshotVars.fnametemplate )
            {
                free( mCtx->snapshotVars.fnametemplate );
            }
            mCtx->snapshotVars.fnametemplate = NULL;
        }
        if( sink->pFormat && *((XAuint32*)(sink->pFormat)) == XA_DATAFORMAT_RAWIMAGE )
        {
            XADataFormat_RawImage* img = ((XADataFormat_RawImage*)sink->pFormat);
            /*set needed image settings from the sink structure*/
            mCtx->imageEncSettings.encoderId = XA_IMAGECODEC_RAW;
            mCtx->imageEncSettings.width = img->width;
            mCtx->imageEncSettings.height = img->height;
            mCtx->imageEncSettings.colorFormat = img->colorFormat;
        }
    }
    else
    {
        DEBUG_ERR("XASnapshotItfAdaptation_InitiateSnapshot, invalid data sink!");
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfAdaptation_InitiateSnapshot");
        return XA_RESULT_PARAMETER_INVALID;
    }


    if( mCtx->snapshotVars.sspipeline )
    {
        XASnapshotItfAdaptation_FreeSnapshot(bCtx);
    }
    if( XASnapshotItfAdaptation_CreateSnapshotPipeline(bCtx) != XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Failed to create pipeline!!");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XASnapshotItfAdaptation_InitiateSnapshot");
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_API("<-XASnapshotItfAdaptation_InitiateSnapshot");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XASnapshotItfAdaptation_TakeSnapshot(XAAdaptationGstCtx* bCtx)
 */
XAresult XASnapshotItfAdaptation_TakeSnapshot(XAAdaptationGstCtx* bCtx)
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;

    DEBUG_API("->XASnapshotItfAdaptation_TakeSnapshot");
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaRecorderAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfAdaptation_TakeSnapshot");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*)bCtx;
    /*to get buffer, base pipeline must be playing...*/
    if( GST_STATE(mCtx->baseObj.bin) != GST_STATE_PLAYING )
    {
        DEBUG_INFO_A1("Parent bin in state %d, set to PLAYING", GST_STATE(mCtx->baseObj.bin));
        gst_element_set_state( GST_ELEMENT(mCtx->baseObj.bin), GST_STATE_PLAYING );
        gst_element_get_state( GST_ELEMENT(mCtx->baseObj.bin), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
    }
    if ( mCtx->isobjvsrc && mCtx->videosource && cameraCtx && cameraRealized )
    {
    	GstPad *pad=NULL;
    	GstPad *pad1=NULL;

    	cameraCtx->snapshotting = XA_BOOLEAN_TRUE;
    	pad = gst_element_get_static_pad( GST_ELEMENT(cameraCtx->baseObj.bin), "MRObjSrc");
		if( pad && gst_pad_is_linked(pad) )
		{
			DEBUG_INFO_A2("unblock element:%s pad:%s",
					gst_element_get_name(cameraCtx->baseObj.bin),
					gst_pad_get_name(pad));
			gst_pad_set_blocked_async(pad, FALSE, XAAdaptationGst_PadBlockCb, NULL);
		}

    	pad1 = gst_element_get_static_pad( GST_ELEMENT(cameraCtx->baseObj.bin), "MPObjSrc");
		if( pad1 && gst_pad_is_linked(pad1) )
		{
			DEBUG_INFO_A2("unblock element:%s pad:%s",
					gst_element_get_name(cameraCtx->baseObj.bin),
					gst_pad_get_name(pad1));
			gst_pad_set_blocked_async(pad, FALSE, XAAdaptationGst_PadBlockCb, NULL);
		}

    	DEBUG_INFO_A1("Using camera from global pointer %x", cameraCtx);
    	if ( GST_STATE( GST_ELEMENT(cameraCtx->baseObj.bin)) != GST_STATE_PLAYING )
		{
			cameraCtx->baseObj.binWantedState = GST_STATE(cameraCtx->baseObj.bin);
			DEBUG_INFO_A1("Camerabin state %d, set to PLAYING", GST_STATE(GST_ELEMENT(cameraCtx->baseObj.bin)));
			gst_element_set_state( GST_ELEMENT(cameraCtx->baseObj.bin), GST_STATE_PLAYING );
			gst_element_get_state( GST_ELEMENT(cameraCtx->baseObj.bin), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
		}
	}

    /* Connect signal for getting current buffer from video pipeline*/
    mCtx->snapshotVars.numpicstaken = 0;
    mCtx->snapshotVars.waitforbuffer = TRUE;
    if(mCtx->videoextract)
    {
        mCtx->snapshotVars.sighandler = g_signal_connect(mCtx->videoextract, "handoff",
                                G_CALLBACK (XASnapshotItfAdaptation_BufferHandoffCb),mCtx);
    }
    if( mCtx->snapshotVars.sighandler==0 )
    {
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XASnapshotItfAdaptation_TakeSnapshot");
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_API("<-XASnapshotItfAdaptation_TakeSnapshot");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XASnapshotItfAdaptation_CancelSnapshot(XAAdaptationGstCtx* bCtx)
 */
XAresult XASnapshotItfAdaptation_CancelSnapshot(XAAdaptationGstCtx* bCtx)
{
    DEBUG_API("->XASnapshotItfAdaptation_CancelSnapshot");
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaRecorderAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfAdaptation_CancelSnapshot");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    XASnapshotItfAdaptation_FreeSnapshot(bCtx);

    DEBUG_API("<-XASnapshotItfAdaptation_CancelSnapshot");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XASnapshotItfAdaptation_GetMaxPicsPerBurst(XAAdaptationGstCtx* bCtx,
 *                                              XAuint32 *maxNumberOfPictures)
 */
XAresult XASnapshotItfAdaptation_GetMaxPicsPerBurst(XAAdaptationGstCtx* bCtx,
                                              XAuint32 *maxNumberOfPictures)
{
    DEBUG_API("->XASnapshotItfAdaptation_GetMaxPicsPerBurst");
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaRecorderAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfAdaptation_GetMaxPicsPerBurst");
        return XA_RESULT_PARAMETER_INVALID;
    }
    *maxNumberOfPictures = SSMAXPIC;
    DEBUG_API("<-XASnapshotItfAdaptation_GetMaxPicsPerBurst");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XASnapshotItfAdaptation_GetBurstFPSRange(XAAdaptationGstCtx* bCtx,
 *                                            XAuint32 *minFPS,
 *                                           XAuint32 *maxFPS)
 */
XAresult XASnapshotItfAdaptation_GetBurstFPSRange(XAAdaptationGstCtx* bCtx,
                                            XAuint32 *minFPS,
                                            XAuint32 *maxFPS)
{
    DEBUG_API("->XASnapshotItfAdaptation_GetBurstFPSRange");
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaRecorderAdaptation )
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XASnapshotItfAdaptation_GetBurstFPSRange");
            return XA_RESULT_PARAMETER_INVALID;
        }
    *minFPS = SSMINFPS;
    *maxFPS = SSMAXFPS;

    DEBUG_API("<-XASnapshotItfAdaptation_GetBurstFPSRange");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XASnapshotItfAdaptation_SetShutterFeedback(XAAdaptationGstCtx* bCtx,
 *                                              XAboolean enabled)
 */
XAresult XASnapshotItfAdaptation_SetShutterFeedback(XAAdaptationGstCtx* bCtx,
                                              XAboolean enabled)
{
    DEBUG_API("->XASnapshotItfAdaptation_SetShutterFeedback");
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaRecorderAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfAdaptation_SetShutterFeedback");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    /* stubbed, no implementation */
    DEBUG_API("<-XASnapshotItfAdaptation_SetShutterFeedback");
    return XA_RESULT_SUCCESS;
}

/********************
 * Internal methods
 ********************/

/*
 * void XASnapshotItfAdaptation_StopSnapshotting(XAAdaptationGstCtx* bCtx)
 */
void XASnapshotItfAdaptation_StopSnapshotting(XAAdaptationGstCtx* bCtx)
{
    XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    DEBUG_API("->XASnapshotItfAdaptation_StopSnapshotting");
    mCtx->snapshotVars.waitforbuffer = FALSE;
    mCtx->snapshotVars.snapshotbuffer = NULL;
    if(mCtx->snapshotVars.sighandler)
    {
        if(mCtx->videoextract)
        {
            g_signal_handler_disconnect(mCtx->videoextract,mCtx->snapshotVars.sighandler);
        }
        mCtx->snapshotVars.sighandler = 0;
    }
    /* did we change the state of parent pipeline?*/
    if( mCtx->baseObj.bin && (GST_STATE(mCtx->baseObj.bin) != mCtx->baseObj.binWantedState) )
    {
        gst_element_set_state( GST_ELEMENT(mCtx->baseObj.bin), mCtx->baseObj.binWantedState );
    }

    if ( cameraCtx && (GST_STATE(cameraCtx->baseObj.bin) != cameraCtx->baseObj.binWantedState) )
	{
    	cameraCtx->snapshotting = XA_BOOLEAN_FALSE;
		DEBUG_INFO_A2("Camerabin state %d, restored to %d", GST_STATE(cameraCtx->baseObj.bin), cameraCtx->baseObj.binWantedState );
		gst_element_set_state( GST_ELEMENT(cameraCtx->baseObj.bin), cameraCtx->baseObj.binWantedState );
		gst_element_get_state( GST_ELEMENT(cameraCtx->baseObj.bin), NULL,NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
	}
    DEBUG_API("<-XASnapshotItfAdaptation_StopSnapshotting");
}

/*
 * XAresult XASnapshotItfAdaptation_FreeSnapshot(XAAdaptationGstCtx* bCtx)
 */
XAresult XASnapshotItfAdaptation_FreeSnapshot(XAAdaptationGstCtx* bCtx)
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XASnapshotItfAdaptation_FreeSnapshot");
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaRecorderAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfAdaptation_FreeSnapshot");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    XASnapshotItfAdaptation_StopSnapshotting(bCtx);
    /* Clean up pipeline and set current pipeline state to null*/
    if( mCtx->snapshotVars.sspipeline )
    {
        gst_element_set_state( GST_ELEMENT(mCtx->snapshotVars.sspipeline), GST_STATE_NULL );
        gst_object_unref( GST_OBJECT(mCtx->snapshotVars.sspipeline) );
        mCtx->snapshotVars.sspipeline = NULL;
    }
    if( mCtx->snapshotVars.ssbus )
    {
        gst_object_unref( GST_OBJECT(mCtx->snapshotVars.ssbus) );
        mCtx->snapshotVars.ssbus = NULL;
    }
    if( mCtx->snapshotVars.fnametemplate )
    {
        free(mCtx->snapshotVars.fnametemplate);
        mCtx->snapshotVars.fnametemplate=NULL;
    }
    DEBUG_API("<-XASnapshotItfAdaptation_FreeSnapshot");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XASnapshotItfAdaptation_CreateSnapshotPipeline(XAAdaptationGstCtx* bCtx)
 */
XAresult XASnapshotItfAdaptation_CreateSnapshotPipeline(XAAdaptationGstCtx* bCtx)
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;

    XACapabilities temp;
    GstCaps *imageCaps=NULL;

    DEBUG_API("->XASnapshotItfAdaptation_CreateSnapshotPipeline");
    if(!bCtx || bCtx->baseObj.ctxId != XAMediaRecorderAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;

    /*Create snapshotpipeline*/
    mCtx->snapshotVars.sspipeline = gst_pipeline_new ("sspipeline");
    if( mCtx->snapshotVars.sspipeline  )
    {
        /*add listener*/
        mCtx->snapshotVars.ssbus = gst_pipeline_get_bus(GST_PIPELINE( mCtx->snapshotVars.sspipeline ) );
        if( ! mCtx->snapshotVars.ssbus )
        {
            DEBUG_API("Cannot create snapshotbus");
            DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }
        gst_bus_add_signal_watch( mCtx->snapshotVars.ssbus );
        g_signal_connect(mCtx->snapshotVars.ssbus, "message::eos",
                G_CALLBACK(XASnapshotItfAdaptation_SnapshotBusCb), bCtx );
        g_signal_connect(mCtx->snapshotVars.ssbus, "message::state-changed",
                G_CALLBACK(XASnapshotItfAdaptation_SnapshotBusCb), bCtx );
        g_signal_connect(mCtx->snapshotVars.ssbus, "message::async-done",
                G_CALLBACK(XASnapshotItfAdaptation_SnapshotBusCb), bCtx );

        /*Create snapshotsource element*/
        mCtx->snapshotVars.ssbuffersrc = gst_element_factory_make("appsrc", "ssbuffersrc");
        if( !mCtx->snapshotVars.ssbuffersrc )
        {
            DEBUG_ERR("Cannot create ssbuffersrc!");
            DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }
        /*Frame parser*/
        mCtx->snapshotVars.ssparser =
            gst_element_factory_make("videoparse","ssparser");
        if( !mCtx->snapshotVars.ssparser )
        {
            DEBUG_ERR("Could not create snapshotparse");
            DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }

        /*Scaler and filter for XAImageSettings width&height*/
        mCtx->snapshotVars.ssscaler =
            gst_element_factory_make("videoscale","ssscaler");
        if( !mCtx->snapshotVars.ssscaler )
        {
            DEBUG_ERR("Could not create ssscaler");
            DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }
        mCtx->snapshotVars.ssfilter =
            gst_element_factory_make("capsfilter","ssfilter");
        if( !mCtx->snapshotVars.ssfilter )
        {
            DEBUG_ERR("Could not create ssfilter");
            DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }

        /*Create imageencoder */
        if(XACapabilitiesMgr_GetCapsById(NULL, (XACapsType)(XACAP_ENCODER|XACAP_IMAGE), mCtx->imageEncSettings.encoderId, &temp) == XA_RESULT_SUCCESS)
        {
            if(temp.adaptId != NULL)
            {
                mCtx->snapshotVars.ssencoder = gst_element_factory_make((char*)temp.adaptId, "ssencoder");
            }
            else if(mCtx->imageEncSettings.encoderId == XA_IMAGECODEC_RAW)
            {
                /* raw frames are internal format, so no codec needed. just insert identity for linking*/
               mCtx->snapshotVars.ssencoder = gst_element_factory_make("identity", "ssencoder");
            }
        }
        if( !mCtx->snapshotVars.ssencoder )
        {
            DEBUG_API("Cannot create image encoder");
            DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }

        /* Create also tag setter for JPG */
        if(mCtx->imageEncSettings.encoderId == XA_IMAGECODEC_JPEG)
        {
            mCtx->snapshotVars.sstagger = gst_element_factory_make("metadatamux", "sstagger");
            if( !mCtx->snapshotVars.sstagger || !gst_bin_add(GST_BIN(mCtx->snapshotVars.sspipeline),mCtx->snapshotVars.sstagger))
            {
                DEBUG_API("Cannot create metadatamux");
                DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            }
            g_object_set( G_OBJECT(mCtx->snapshotVars.sstagger), "xmp", TRUE, "exif", TRUE, "iptc", TRUE, NULL );
        }

        /*Create sink*/
        if(mCtx->snapshotVars.fnametemplate)
        {
            DEBUG_INFO("RECORD SNAPSHOT TO FILE");
            mCtx->snapshotVars.sssink = gst_element_factory_make("filesink","ssfilesink");
            g_object_set( G_OBJECT(mCtx->snapshotVars.sssink), "location", "temp",
                                                                "async", FALSE,
                                                                "qos", FALSE,
                                                                "max-lateness", (gint64)(-1),
                                                                "buffer-mode", 2,
                                                                NULL );
        }
        else
        {
            DEBUG_INFO("RECORD SNAPSHOT TO MEMORY");
            mCtx->snapshotVars.sssink = gst_element_factory_make("appsink","ssbuffersink");
        }
        if( !mCtx->snapshotVars.sssink )
        {
            DEBUG_ERR("Could not create sssink!!");
            DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }
        g_object_set( G_OBJECT(mCtx->snapshotVars.sssink), "async", FALSE, NULL );

        /*Add elements to bin*/
        gst_bin_add_many (GST_BIN (mCtx->snapshotVars.sspipeline),
                mCtx->snapshotVars.ssbuffersrc,
                mCtx->snapshotVars.ssparser,
                mCtx->snapshotVars.ssscaler,
                mCtx->snapshotVars.ssfilter,
                mCtx->snapshotVars.ssencoder,
                mCtx->snapshotVars.sssink,
                NULL);

        /* set needed XAImageSettings properties*/
        /* set caps from imagesettings */
        imageCaps = gst_caps_new_simple("video/x-raw-yuv",
                                        "width", G_TYPE_INT, mCtx->imageEncSettings.width,
                                        "height", G_TYPE_INT, mCtx->imageEncSettings.height, NULL);
        g_object_set( G_OBJECT(mCtx->snapshotVars.ssfilter), "caps", imageCaps, NULL );
        DEBUG_INFO_A1("new caps: %s",gst_caps_to_string(imageCaps));
        gst_caps_unref(imageCaps);

        /* set compression level */
        if(mCtx->imageEncSettings.encoderId == XA_IMAGECODEC_JPEG)
        {
            g_object_set( G_OBJECT(mCtx->snapshotVars.ssencoder), "quality", (gint)(1000 - mCtx->imageEncSettings.compressionLevel)/10, NULL );
        }

        /*Chain elements together*/
        if(mCtx->snapshotVars.sstagger)
        {
            if( !gst_element_link_many(
                    mCtx->snapshotVars.ssbuffersrc,
                    mCtx->snapshotVars.ssparser,
                    mCtx->snapshotVars.ssscaler,
                    mCtx->snapshotVars.ssfilter,
                    mCtx->snapshotVars.ssencoder,
                    mCtx->snapshotVars.sstagger,
                    mCtx->snapshotVars.sssink,
                    NULL) )
            {
                DEBUG_ERR("Could not link pipeline")
                return XA_RESULT_INTERNAL_ERROR;
            }
        }
        else
        {
            if( !gst_element_link_many(
                    mCtx->snapshotVars.ssbuffersrc,
                    mCtx->snapshotVars.ssparser,
                    mCtx->snapshotVars.ssscaler,
                    mCtx->snapshotVars.ssfilter,
                    mCtx->snapshotVars.ssencoder,
                    mCtx->snapshotVars.sssink,
                    NULL) )
            {
                DEBUG_ERR("Could not link pipeline")
                return XA_RESULT_INTERNAL_ERROR;
            }
        }
        gst_element_set_state(GST_ELEMENT(mCtx->snapshotVars.sspipeline), GST_STATE_READY);
        DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline");
        return XA_RESULT_SUCCESS;

    }
    else
    {
        DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED");
        DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline");
        return  XA_RESULT_PRECONDITIONS_VIOLATED;
    }
}

/*
 * gboolean XASnapshotItfAdaptation_SnapshotBusCb( GstBus *bus, GstMessage *message, gpointer data )
 */
gboolean XASnapshotItfAdaptation_SnapshotBusCb( GstBus *bus, GstMessage *message, gpointer data )
{
    XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*)data;

    GstState oldstate = GST_STATE_NULL , newstate = GST_STATE_NULL , pendingstate = GST_STATE_NULL;

    /* only interested in messages from snapshot pipeline */
    if( GST_MESSAGE_SRC(message) == GST_OBJECT(mCtx->snapshotVars.sspipeline) )
    {
        DEBUG_API_A2("->XASnapshotItfAdaptation_SnapshotBusCb:\"%s\" from object \"%s\"",
                GST_MESSAGE_TYPE_NAME(message), GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));

        switch( GST_MESSAGE_TYPE(message))
        {
            case GST_MESSAGE_EOS:
            {
                if( gst_element_set_state(GST_ELEMENT(mCtx->snapshotVars.sspipeline), GST_STATE_READY)
                        == GST_STATE_CHANGE_FAILURE )
                {
                    DEBUG_ERR("Error stopping snapshot pipeline!!!");
                }
                gst_element_get_state(mCtx->snapshotVars.sspipeline, NULL, NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);

                DEBUG_INFO_A2("Requested %lu snapshots, taked %lu",mCtx->snapshotVars.numpics,mCtx->snapshotVars.numpicstaken+1);
                if( ++mCtx->snapshotVars.numpicstaken >= mCtx->snapshotVars.numpics )
                {
                    XAAdaptEvent takenevent = {XA_SNAPSHOTITFEVENTS, XA_ADAPT_SNAPSHOT_TAKEN, 0, NULL };
                    DEBUG_INFO("Snapshot burst finished");
                    XASnapshotItfAdaptation_StopSnapshotting(&(mCtx->baseObj));
                    if( mCtx->snapshotVars.fnametemplate == NULL )
                    {   /* non-datasink snapshot, return buffer */
                        GstBuffer* pullBuffer = NULL;
                        XADataSink* pullSink = NULL;
                        XADataLocator_Address* pullSinkLoc = NULL;
                        DEBUG_INFO("Get buffer from appsink");
                        pullBuffer = gst_app_sink_pull_preroll( GST_APP_SINK(mCtx->snapshotVars.sssink) );
                        /* allocate XADataSink, client should release this later*/
                        pullSink = (XADataSink*)calloc(1, sizeof(XADataSink));
                        pullSinkLoc = (XADataLocator_Address*)calloc(1, sizeof(XADataLocator_Address));
                        pullSinkLoc->length = pullBuffer->size;
                        pullSinkLoc->pAddress = (XADataLocator_Address*)calloc(1, pullBuffer->size);
                        memcpy(pullSinkLoc->pAddress, pullBuffer->data, pullBuffer->size);
                        pullSinkLoc->locatorType = XA_DATALOCATOR_ADDRESS;
                        pullSink->pLocator = pullSinkLoc;
                        pullSink->pFormat = NULL;
                        takenevent.data = pullSink;
                    }
                    /* send needed events */
                    takenevent.datasize = mCtx->snapshotVars.numpicstaken;
                    XAAdaptationBase_SendAdaptEvents(&(mCtx->baseObj.baseObj), &takenevent );
                }
                else
                {
                    /* videoparse element can not handle renegotiation of stream for new buffer
                     * after EOS, so recreate it */
                    gst_element_unlink_many(
                            mCtx->snapshotVars.ssbuffersrc,
                            mCtx->snapshotVars.ssparser,
                            mCtx->snapshotVars.ssscaler,
                            NULL);
                    gst_element_set_state(GST_ELEMENT(mCtx->snapshotVars.ssparser), GST_STATE_NULL);
                    gst_bin_remove(GST_BIN (mCtx->snapshotVars.sspipeline),mCtx->snapshotVars.ssparser);
                    mCtx->snapshotVars.ssparser = gst_element_factory_make("videoparse", "ssparser");
                    if( !mCtx->snapshotVars.ssparser )
                    {
                        DEBUG_ERR("Cannot create ssparser!");
                        DEBUG_API("<-XASnapshotItfAdaptation_CreateSnapshotPipeline - XA_RESULT_INTERNAL_ERROR");
                        return XA_RESULT_INTERNAL_ERROR;
                    }
                    gst_bin_add(GST_BIN (mCtx->snapshotVars.sspipeline),mCtx->snapshotVars.ssparser);
                    if( !gst_element_link_many(
                            mCtx->snapshotVars.ssbuffersrc,
                            mCtx->snapshotVars.ssparser,
                            mCtx->snapshotVars.ssscaler,
                            NULL) )
                    {
                        DEBUG_ERR("Could not link pipeline")
                        return XA_RESULT_INTERNAL_ERROR;
                    }
                    mCtx->snapshotVars.parsenegotiated = FALSE;

                    /*now, wait for new buffer to arrive*/
                    DEBUG_INFO("Wait for more pictures");
                    mCtx->snapshotVars.waitforbuffer = TRUE;
                    mCtx->snapshotVars.sighandler = g_signal_connect(mCtx->videoextract, "handoff",
                                            G_CALLBACK (XASnapshotItfAdaptation_BufferHandoffCb),mCtx);
                }
                break;
            }
            case GST_MESSAGE_STATE_CHANGED:
            {
                gst_message_parse_state_changed(message, &oldstate, &newstate, &pendingstate);
                DEBUG_INFO_A4("old %s -> new %s ( pending %s, gsttarget %s )",
                        gst_element_state_get_name(oldstate),
                        gst_element_state_get_name(newstate),
                        gst_element_state_get_name(pendingstate),
                        gst_element_state_get_name(GST_STATE_TARGET(mCtx->snapshotVars.sspipeline)) );
                if( newstate==GST_STATE_READY && oldstate==GST_STATE_NULL )
                {
                    XAAdaptEvent initevent = {XA_SNAPSHOTITFEVENTS, XA_ADAPT_SNAPSHOT_INITIATED,0, NULL };
                    DEBUG_INFO("Init complete");
                    /* send needed events */
                    XAAdaptationBase_SendAdaptEvents( &(mCtx->baseObj.baseObj), &initevent);
                }
                else if( newstate==GST_STATE_PLAYING && oldstate==GST_STATE_PAUSED && mCtx->snapshotVars.snapshotbuffer )
                {
                    DEBUG_INFO("Pushing buffer");
                    gst_app_src_push_buffer( GST_APP_SRC(mCtx->snapshotVars.ssbuffersrc),
                                             mCtx->snapshotVars.snapshotbuffer );
                    DEBUG_INFO_A1("Sent buffer at 0x%x to  ssbuffersrc", (int)mCtx->snapshotVars.snapshotbuffer );
                    gst_app_src_end_of_stream( GST_APP_SRC(mCtx->snapshotVars.ssbuffersrc) );
                    mCtx->snapshotVars.snapshotbuffer = NULL;
                    DEBUG_INFO("Sent EOS ssbuffersrc");
                }

                break;
            }
            default:
                break;
        }
        DEBUG_API("<-XASnapshotItfAdaptation_SnapshotBusCb");
    }
    return TRUE;
}

/*
 * void XASnapshotItfAdaptation_BufferHandoffCb( GstElement *extract, GstBuffer  *buffer, gpointer data )
 */
void XASnapshotItfAdaptation_BufferHandoffCb( GstElement *extract, GstBuffer  *buffer, gpointer data )
{
    XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*)data;
    gint32 width=0, height=0;
    guint32  fourcc=0, formatnum=0;
    GstPad* srcPad=NULL;
    GstCaps* srcPadCaps=NULL;
    GstStructure* capS=NULL;
    XAAdaptEvent event = {XA_SNAPSHOTITFEVENTS, XA_ADAPT_SNAPSHOT_TAKEN, 0, NULL };
    char* fname=NULL;

    DEBUG_API("->XASnapshotItfAdaptation_BufferHandoffCb");
    if( !mCtx->snapshotVars.waitforbuffer ||
        !GST_IS_BUFFER(buffer) )
    {   /* pass on... */
        DEBUG_API("<-XASnapshotItfAdaptation_BufferHandoffCb");
        return;
    }

    if(mCtx->snapshotVars.snapshotbuffer)
    {
        DEBUG_INFO("WARNING: snapshotbuffer already exists!!");
        gst_buffer_unref(GST_BUFFER(mCtx->snapshotVars.snapshotbuffer));
    }
    DEBUG_INFO("Receiced snapshotbuffer");
    mCtx->snapshotVars.snapshotbuffer = gst_buffer_copy(buffer);
    mCtx->snapshotVars.waitforbuffer = FALSE;
    g_signal_handler_disconnect(mCtx->videoextract,mCtx->snapshotVars.sighandler);
    mCtx->snapshotVars.sighandler = 0;

    if( GST_STATE(mCtx->snapshotVars.sspipeline)==GST_STATE_READY )
    {
        if( !(mCtx->snapshotVars.parsenegotiated) )
        {
            /*read relevant caps of extraction source and set them to videoparse*/
            srcPad = gst_element_get_pad( GST_ELEMENT(extract), "src");
            srcPadCaps = gst_pad_get_negotiated_caps( GST_PAD(srcPad) );
            capS = gst_caps_get_structure(srcPadCaps,0);
            DEBUG_INFO_A1("buffer caps from extraction source: %s",gst_caps_to_string(srcPadCaps));
            if( !gst_structure_get_int(capS,"width",&width) ||
                !gst_structure_get_int(capS,"height",&height) ||
                !gst_structure_get_fourcc(capS,"format",&fourcc) )
            {
                DEBUG_ERR("ERROR! Missing crucial capabilities for buffer!!");
                DEBUG_API("<-XASnapshotItfAdaptation_BufferHandoffCb");
                return;
            }
            /* convert fourcc to videoparse enumeration */
            switch(fourcc)
            {
                case GST_MAKE_FOURCC('I','4','2','0'):
                    formatnum = 0;
                    break;
                case GST_MAKE_FOURCC('Y','V','1','2'):
                    formatnum = 1;
                    break;
                case GST_MAKE_FOURCC('Y','U','Y','2'):
                    formatnum = 2;
                    break;
                case GST_MAKE_FOURCC('U','Y','V','Y'):
                    formatnum = 3;
                    break;
                case GST_MAKE_FOURCC('R','G','B',' '):
                    formatnum = 10;
                    break;
                case GST_MAKE_FOURCC('G','R','A','Y'):
                    formatnum = 11;
                    break;
                default:
                    formatnum = 0;
                    break;
            }
            /* set source width and height for parser */
            g_object_set(mCtx->snapshotVars.ssparser,"width",width,"height",height,"format",formatnum,NULL);
            mCtx->snapshotVars.parsenegotiated = TRUE;
        }
        if(mCtx->snapshotVars.fnametemplate)
        {   /* get actual filename from template */
            XASnapshotItfAdaptation_AllocNextFilename(&fname, mCtx->snapshotVars.fnametemplate);
            DEBUG_INFO_A1("start taking snapshot (%s)", fname);
            gst_element_set_state(GST_ELEMENT(mCtx->snapshotVars.sssink), GST_STATE_NULL);
            g_object_set( G_OBJECT(mCtx->snapshotVars.sssink), "location", fname,
                                                                "async", FALSE,
                                                                "qos", FALSE,
                                                                "max-lateness", (gint64)(-1),
                                                                NULL );
            gst_element_sync_state_with_parent(mCtx->snapshotVars.sssink);
            gst_element_get_state(mCtx->snapshotVars.sssink, NULL, NULL, XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC);
            free(fname);
        }
        else
        {   /* take snapshot to buffer */
            DEBUG_INFO("start taking snapshot (memory buffer used)");
        }
        /* write metadata, if any */
        XAMetadataAdapt_TryWriteTags(&(mCtx->baseObj), GST_BIN(mCtx->snapshotVars.sspipeline));
        /* start buffering */
        if( gst_element_set_state(GST_ELEMENT(mCtx->snapshotVars.sspipeline), GST_STATE_PLAYING )
                == GST_STATE_CHANGE_FAILURE )
        {
            DEBUG_ERR("Error taking picture!!!");
            /* NOTE: no event for errors in snapshotitf!!! */
            event.datasize = mCtx->snapshotVars.numpicstaken;
            XAAdaptationBase_SendAdaptEvents(&(mCtx->baseObj.baseObj), &event );
        }
    }
    else
    {
        DEBUG_INFO_A1("warning: sspipeline in wrong state (%d)",
                        GST_STATE(mCtx->snapshotVars.sspipeline));
    }
    DEBUG_API("<-XASnapshotItfAdaptation_BufferHandoffCb");
}

/*
 * const char* XASnapshotItfAdaptation_GetFileSuffix(XADataFormat_MIME* format)
 */
const char* XASnapshotItfAdaptation_GetFileSuffix(XADataFormat_MIME* format)
{
    const char* ret=NULL;
    if( format )
    {
        if( *(XAuint32*)format == XA_DATAFORMAT_MIME )
        {
            switch (format->containerType)
            {
                case XA_CONTAINERTYPE_JPG:
                    ret = "jpg";
                    break;
                case XA_CONTAINERTYPE_RAW:
                    ret = "raw";
                    break;
                case XA_CONTAINERTYPE_BMP:
                    ret = "bmp";
                    break;
                default:
                    break;
            }
            if(!ret)
            { /*parse from mimetype*/
                if(format->mimeType)
                {
                    ret = strrchr((char*)format->mimeType,'/');
                    if (ret)
                    {
                        ret++;
                    }
                }
            }
        }
        else if( *(XAuint32*)format == XA_DATAFORMAT_RAWIMAGE )
        {
            ret = "raw";
        }
    }
    if(!ret)
    {
        ret="jpg"; /*default*/
    }
    return ret;
}

/*
 * void XASnapshotItfAdaptation_AllocNextFilename(char** fname, const char* template)
 */
void XASnapshotItfAdaptation_AllocNextFilename(char** fname, const char* template)
{
    XAuint32 idx=0;
    XAboolean found=XA_BOOLEAN_FALSE;
    FILE* file=NULL;
    *fname = (char*)calloc(1,strlen(template)+10);
    while(!found)
    {
        sprintf(*fname, template, idx++ );
        strcat(*fname, "\0");
        file = fopen(*fname, "r");
        if(file==NULL)
        {
            found = XA_BOOLEAN_TRUE;
            break;
        }
        fclose(file);
    }
}
