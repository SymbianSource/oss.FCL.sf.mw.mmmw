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

#include <gst/gst.h>
#include <gst/video/video.h>
#include "xaadaptationgst.h"
#include "xamediaplayeradaptctx.h"
#include "xamediarecorderadaptctx.h"
#include "xacameraadaptctx.h"
#include "xavideopostprosessingitfadaptation.h"
#include "xastaticcameracapsadaptation.h"

/*
 * XAresult XAVideoPostProcessingItfAdapt_IsArbitraryRotationSupported(XAAdaptationGstCtx *bCtx,
 *                                                                     XAboolean *pSupported)
 */
XAresult XAVideoPostProcessingItfAdapt_IsArbitraryRotationSupported(XAAdaptationGstCtx *bCtx,
                                                                    XAboolean *pSupported)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAVideoPostProcessingItfAdapt_IsArbitraryRotationSupported");

    if(!bCtx || (bCtx->baseObj.ctxId != XAMediaPlayerAdaptation && bCtx->baseObj.ctxId != XAMediaRecorderAdaptation
            && bCtx->baseObj.ctxId != XACameraAdaptation) || !pSupported)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVideoPostProcessingItfAdapt_IsArbitraryRotationSupported");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* This feature is not supported, return boolean false */
    *pSupported = XA_BOOLEAN_FALSE;

    DEBUG_API("<-XAVideoPostProcessingItfAdapt_IsArbitraryRotationSupported");
    return ret;
}

/*
 * XAresult XAVideoPostProcessingItfAdapt_Commit(XAAdaptationGstCtx *bCtx)
 */
XAresult XAVideoPostProcessingItfAdapt_Commit(XAAdaptationGstCtx *bCtx,
                                              XAmillidegree rotation,
                                              XAuint32 scaleOptions,
                                              XAuint32 backgroundColor,
                                              XAuint32 renderingHints,
                                              const XARectangle *pSrcRect,
                                              const XARectangle *pDestRect,
                                              XAuint32 mirror,
                                              XAboolean isMirror,
                                              XAboolean isRotate,
                                              XAboolean isDestRect,
                                              XAboolean isSrcRect,
                                              XAboolean isScaleOptions
                                              )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAmillidegree tempRotation = 0;
    XAmillidegree newRotation = 0;
    XAuint32 tempMirror = XA_VIDEOMIRROR_NONE;
    XAuint32 newMirror = XA_VIDEOMIRROR_NONE;
    XADataSource *dataSrc = NULL;
    GstElement *cropElement = NULL;
    GstElement *rotateElement = NULL;
    GstElement *mirrorElement = NULL;
    GstElement *boxElement = NULL;
/*
    GstElement *balanceElement = NULL;
    GstElement *gammaElement = NULL;
*/
    GstElement *sink = NULL;
    GstElement *col1 = NULL;
    GstElement *scale = NULL;
/*
    gdouble     alphaValue = 1;
    gint        videoBackgroundColor = 0;
*/
    gint            cropscaleRight = 0, cropscaleBottom = 0,
                cropscaleLeft = 0,
                cropscaleTop = 0,
                videoscaleHeight = 0;
    GstStateChangeReturn gstRet = GST_STATE_CHANGE_SUCCESS;
    GstState gstOrigState = GST_STATE_PLAYING;
    GstState gstTmpState = GST_STATE_PLAYING;

    DEBUG_API("->XAVideoPostProcessingItfAdapt_Commit");

    if( !bCtx || (bCtx->baseObj.ctxId != XAMediaPlayerAdaptation && bCtx->baseObj.ctxId != XAMediaRecorderAdaptation
        && bCtx->baseObj.ctxId != XACameraAdaptation))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
    {
        XAMediaPlayerAdaptationCtx* ctx = (XAMediaPlayerAdaptationCtx*) bCtx;


        /* Get video pp elements */
        GstPad *pad = NULL;
        GstCaps *caps = NULL;
        col1 = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_colsp1");
        cropElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_crop");
        rotateElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_rotate");
        mirrorElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_mirror");
        boxElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_box");
/*
        balanceElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_balance");
        gammaElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_gamma");
*/
        sink = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "videosink");
        pad = gst_element_get_static_pad(GST_ELEMENT(sink),"sink");
        caps = gst_caps_new_simple("video/x-raw-yuv",
                 "width", G_TYPE_INT,0,
                 "height", G_TYPE_INT,0,
                 NULL);
        gst_pad_set_caps(pad, caps);

        /* get current mirror state and rotate value */
        tempMirror = ctx->curMirror;
        tempRotation = ctx->curRotation;
        dataSrc = ctx->xaSource;
    }

    if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
    {
        XAMediaRecorderAdaptationCtx* ctx = (XAMediaRecorderAdaptationCtx*) bCtx;

        /* Get video pp elements */
        GstPad *pad = NULL;
        GstCaps *caps = NULL;
        scale = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_scale2");
        col1 = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_colsp1");
        cropElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_crop");
        rotateElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_rotate");
        mirrorElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_mirror");
        boxElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_box");
/*        balanceElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_balance");
        gammaElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_gamma");*/
        sink = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "datasink");
        pad = gst_element_get_static_pad(GST_ELEMENT(sink),"sink");
        caps = gst_caps_new_simple("video/x-raw-yuv",
                 "width", G_TYPE_INT,0,
                 "height", G_TYPE_INT,0,
                 NULL);
        gst_pad_set_caps(pad, caps);

        /* get current mirror state and rotate value */
        tempMirror = ctx->curMirror;
        tempRotation = ctx->curRotation;
    }

    if( bCtx->baseObj.ctxId == XACameraAdaptation )
    {
        XACameraAdaptationCtx* ctx = (XACameraAdaptationCtx*) bCtx;

        GstElement *videoPP = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "videopp_camera");
        if( !videoPP )
        {
            DEBUG_ERR("Could not receive videopp from camerabin!");
        }
        else
        {
            /* Get camera pp elements */
            GstPad *pad = NULL;
            GstCaps *caps = NULL;
            rotateElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_rotate");
            col1 = gst_bin_get_by_name( GST_BIN(videoPP), "pp_colsp1");
            cropElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_crop");
            mirrorElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_mirror");
            boxElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_box");
/*            balanceElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_balance");
            gammaElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_gamma");*/
            sink = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_crop");
            pad = gst_element_get_static_pad(GST_ELEMENT(sink),"sink");
            caps = gst_caps_new_simple("video/x-raw-yuv",
                     "width", G_TYPE_INT,0,
                     "height", G_TYPE_INT,0,
                     NULL);
            gst_pad_set_caps(pad, caps);

            /* get current mirror state and rotate value */
            tempMirror = ctx->curMirror;
            tempRotation = ctx->curRotation;
        }
    }

    /* Cropping */
    if( isSrcRect && pSrcRect && cropElement && col1 )
    {

        gint        cropRight = 0;
        gint        cropBottom = 0;
        gint        cropLeft = 0;
        gint        cropTop = 0;
        GstPad      *videoPad = NULL;
        gint        videoWidth = 0;
        gint        videoHeight = 0;
        DEBUG_INFO("Start cropping!");

        DEBUG_INFO_A1("pSrcRect->left:%d",(int)pSrcRect->left);
        DEBUG_INFO_A1("pSrcRect->top:%d",(int)pSrcRect->top);
        DEBUG_INFO_A1("pSrcRect->width:%d",(int)pSrcRect->width);
        DEBUG_INFO_A1("pSrcRect->height:%d", (int)pSrcRect->height);

        if( bCtx->baseObj.ctxId == XACameraAdaptation )
        {
            GstCaps *caps = NULL;
            XACameraAdaptationCtx* ctx = (XACameraAdaptationCtx*) bCtx;
            gint capsCount = 0;
            gint iterator = 0;
            GstStructure *capsStruct = NULL;

            g_object_get( G_OBJECT(ctx->baseObj.bin), "filter-caps", &caps, NULL );

            if( !caps )
            {
                DEBUG_ERR("Cannot receive caps (filter-caps) from camerabin!");
                DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }

            capsCount = gst_caps_get_size( caps );

            for( iterator= 0; iterator < capsCount; iterator++ )
            {
                capsStruct = gst_caps_get_structure( caps, iterator );
                if( capsStruct )
                {
                    if( !gst_structure_get_int( capsStruct, "width", &videoWidth ) )
                    {
                        DEBUG_ERR("Could not get width from filter-caps");
                        videoWidth = TEST_VIDEO_WIDTH;
                    }
                    if( !gst_structure_get_int( capsStruct, "height", &videoHeight) )
                    {
                        DEBUG_ERR("Could not get height from filter-caps");
                        videoHeight = TEST_VIDEO_HEIGHT;
                    }
                    DEBUG_INFO_A2("videoWidth:%d, videoHeight:%d",videoWidth,videoHeight);
                }
            }
        }
        else
        {
            GstCaps* negcapas=NULL;
            videoPad = gst_element_get_pad( col1, "src" );
            negcapas = gst_pad_get_negotiated_caps( GST_PAD(videoPad) );
            if ( negcapas )
            {
                if( !gst_video_get_size( videoPad, &videoWidth, &videoHeight ) )
                {
                    DEBUG_ERR("WARN: Cannot receive video size, using defaults");
                    videoWidth = TEST_VIDEO_WIDTH;
                    videoHeight = TEST_VIDEO_HEIGHT;
                }
            }
            else
            {
				videoWidth = TEST_VIDEO_WIDTH;
				videoHeight = TEST_VIDEO_HEIGHT;
            }
        }

        DEBUG_INFO_A2("Received video frame info, videoWidth:%d, videoHeight:%d",videoWidth,videoHeight);
        cropLeft = (gint)pSrcRect->left;
        cropTop = (gint)pSrcRect->top;
        cropRight = videoWidth - ((gint)pSrcRect->left + (gint)pSrcRect->width);
        cropBottom = videoHeight - ((gint)pSrcRect->top + (gint)pSrcRect->height);
        DEBUG_INFO_A4("Crop values - cropLeft:%d ,cropTop:%d,cropRight:%d,cropBottom:%d", cropLeft,cropTop,cropRight,cropBottom);

        if( cropBottom >= 0 && cropLeft >=0 && cropRight >= 0  && cropTop >= 0 )
        {
            g_object_set(G_OBJECT(cropElement), "bottom",cropBottom, NULL);
            g_object_set(G_OBJECT(cropElement), "left", cropLeft, NULL);
            g_object_set(G_OBJECT(cropElement), "right", cropRight, NULL);
            g_object_set(G_OBJECT(cropElement), "top", cropTop, NULL);
        }
        else
        {
            if( cropLeft > videoWidth || cropLeft < 0 ||
                cropRight > videoWidth || cropRight < 0 ||
                cropBottom > videoHeight || cropBottom < 0 ||
                cropTop > videoHeight || cropTop < 0)
            {
                DEBUG_INFO("Cropped params out of original frame.");
            }
        }
    }

    if(rotateElement && isRotate)
    {
        DEBUG_INFO("Start rotating!");

        /* calculate rotation */
        newRotation = tempRotation + rotation;

        if( newRotation > ROTATION_RATIO || newRotation < ROTATION_RATIO_NEG )
        {
            newRotation = newRotation % ROTATION_RATIO;
        }

        /* Set rotation */
        switch(newRotation)
        {
            case 0:
            {
                /* no rotation */
                DEBUG_INFO("Set rotation FLIP_NONE");
                g_object_set(G_OBJECT(rotateElement), "method", FLIP_NONE, NULL);
                break;
            }
            case 90000:
            case -270000:
            {
                /* rotation 90 degree */
                DEBUG_INFO("Set rotation 90 degrees");
                g_object_set(G_OBJECT(rotateElement), "method", FLIP_CLOCKWISE, NULL);
                break;
            }
            case 180000:
            case -180000:
            {
                /* rotation 180 degree */
                DEBUG_INFO("Set rotation 180 degrees");
                g_object_set(G_OBJECT(rotateElement), "method", FLIP_ROTATE_180, NULL);
                break;
            }
            case 270000:
            case -90000:
            {
                /* rotation 270 degree */
                DEBUG_INFO("Set rotation 270 degrees");
                g_object_set(G_OBJECT(rotateElement), "method", FLIP_COUNTERCLOCKWISE, NULL);
                break;
            }
            case 360000:
            case -360000:
            {
                /* rotation 360 degree */
                DEBUG_INFO("Set rotation 360 degrees");
                g_object_set(G_OBJECT(rotateElement), "method", FLIP_NONE, NULL);
                break;
            }
            default:
            {
                /* no rotation */
                DEBUG_INFO("Set rotation default (FLIP_NONE) degree");
                g_object_set(G_OBJECT(rotateElement), "method", FLIP_NONE, NULL);
                break;
            }
        }

        /* Store current rotate value */
        if( bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
        {
            XAMediaPlayerAdaptationCtx* ctx = (XAMediaPlayerAdaptationCtx*) bCtx;
            ctx->curRotation = newRotation;
        }
        if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
        {
            XAMediaRecorderAdaptationCtx* ctx = (XAMediaRecorderAdaptationCtx*) bCtx;
            ctx->curRotation = newRotation;
        }
        if( bCtx->baseObj.ctxId == XACameraAdaptation )
        {
            XACameraAdaptationCtx* ctx = (XACameraAdaptationCtx*) bCtx;
            ctx->curRotation = newRotation;
        }
    }

    if(mirrorElement && isMirror)
    {
        /* solve new mirror state */
        switch(mirror)
        {
            case XA_VIDEOMIRROR_NONE:
            {
                newMirror = tempMirror;
                break;
            }
            case XA_VIDEOMIRROR_VERTICAL:
            {
                if( tempMirror == XA_VIDEOMIRROR_VERTICAL )
                {
                    newMirror = XA_VIDEOMIRROR_NONE;
                }
                else if( tempMirror == XA_VIDEOMIRROR_HORIZONTAL )
                {
                    newMirror = XA_VIDEOMIRROR_BOTH;
                }
                else if( tempMirror == XA_VIDEOMIRROR_BOTH )
                {
                    newMirror = XA_VIDEOMIRROR_HORIZONTAL;
                }
                else
                {
                    newMirror = XA_VIDEOMIRROR_VERTICAL;
                }
                break;
            }
            case XA_VIDEOMIRROR_HORIZONTAL:
            {
                if( tempMirror == XA_VIDEOMIRROR_VERTICAL )
                {
                    newMirror = XA_VIDEOMIRROR_BOTH;
                }
                else if( tempMirror == XA_VIDEOMIRROR_HORIZONTAL )
                {
                    newMirror = XA_VIDEOMIRROR_NONE;
                }
                else if( tempMirror == XA_VIDEOMIRROR_BOTH )
                {
                    newMirror = XA_VIDEOMIRROR_VERTICAL;
                }
                else
                {
                    newMirror = XA_VIDEOMIRROR_HORIZONTAL;
                }
                break;
            }
            case XA_VIDEOMIRROR_BOTH:
            {
                if( tempMirror == XA_VIDEOMIRROR_VERTICAL )
                {
                    newMirror = XA_VIDEOMIRROR_HORIZONTAL;
                }
                else if( tempMirror == XA_VIDEOMIRROR_HORIZONTAL )
                {
                    newMirror = XA_VIDEOMIRROR_VERTICAL;
                }
                else if( tempMirror == XA_VIDEOMIRROR_BOTH )
                {
                    newMirror = XA_VIDEOMIRROR_NONE;
                }
                else
                {
                    newMirror = XA_VIDEOMIRROR_BOTH;
                }
                break;
            }
            default:
                break;
        }


        DEBUG_INFO("Start mirroring!");
        /* Set mirror */
        switch(newMirror)
        {
            case XA_VIDEOMIRROR_NONE:
            {
                /* none */
                DEBUG_INFO("Set mirror none");
                g_object_set(G_OBJECT(mirrorElement), "method", FLIP_NONE, NULL);
                break;
            }
            case XA_VIDEOMIRROR_VERTICAL:
            {
                /* vertical mirror */
                DEBUG_INFO("Set mirror vertical");
                g_object_set(G_OBJECT(mirrorElement), "method", FLIP_VERTICAL, NULL);
                break;
            }
            case XA_VIDEOMIRROR_HORIZONTAL:
            {
                /* horizontal mirror */
                DEBUG_INFO("Set mirror horizontal");
                g_object_set(G_OBJECT(mirrorElement), "method", FLIP_HORIZONTAL, NULL);
                break;
            }
            case XA_VIDEOMIRROR_BOTH:
            {
                /* both mirror */
                DEBUG_INFO("Set mirror vertical and horizontal");
                g_object_set(G_OBJECT(mirrorElement), "method", FLIP_ROTATE_180, NULL);
                break;
            }
            default:
            {
                /* Default no mirroring */
                g_object_set(G_OBJECT(mirrorElement), "method", FLIP_NONE, NULL);
                break;
            }
        }

        /* Store current mirror state */
        if( bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
        {
            XAMediaPlayerAdaptationCtx* ctx = (XAMediaPlayerAdaptationCtx*) bCtx;
            ctx->curMirror = newMirror;
        }
        if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
        {
            XAMediaRecorderAdaptationCtx* ctx = (XAMediaRecorderAdaptationCtx*) bCtx;
            ctx->curMirror = newMirror;
        }
        if( bCtx->baseObj.ctxId == XACameraAdaptation )
        {
            XACameraAdaptationCtx* ctx = (XACameraAdaptationCtx*) bCtx;
            ctx->curMirror = newMirror;
        }
    }

    /* Set scale */
    if ( isScaleOptions || isDestRect || isSrcRect )
    {
		switch( scaleOptions )
		{

			case XA_VIDEOSCALE_STRETCH:
			{
				DEBUG_INFO("XA_VIDEOSCALE_STRETCH");
				/* The source and destination rectangle's width and height params are used to calculate
				 * the scaling factors independently. Aspect ratio is ignored. */
				if (pDestRect)
				{
					if (bCtx->baseObj.ctxId != XAMediaRecorderAdaptation)
					{
						GstPad *pad = NULL;
						GstCaps* simplecaps = NULL;
						g_object_set ( G_OBJECT(sink), "force-aspect-ratio", FALSE, NULL);
						simplecaps = gst_caps_new_simple("video/x-raw-rgb",
											"width", G_TYPE_INT, pDestRect->width,
											"height", G_TYPE_INT, pDestRect->height,
											"framerate", GST_TYPE_FRACTION, 0,1,
											NULL);
						DEBUG_API_A1("caps: %s",gst_caps_to_string(simplecaps));
						pad = gst_element_get_static_pad(GST_ELEMENT(sink),"sink");
						if (!gst_pad_set_caps(pad, simplecaps))
						{
							DEBUG_INFO("Stubbed at this point");
							DEBUG_INFO("Cannot set destrect size during XA_VIDEOSCALE_STRETCH!");
							DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit - XA_VIDEOSCALE_STRETCH");
							return XA_RESULT_SUCCESS;
						}
						DEBUG_API_A1("ret: %lu",ret);
					}
					else
					{
						GstCaps* simplecaps = NULL;
						GstPad *pad = NULL;
						if ( !scale )
						{
							DEBUG_ERR("Could not get scaling element from pipeline!");
							DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit - XA_RESULT_INTERNAL_ERROR");
							return XA_RESULT_INTERNAL_ERROR;
						}
						simplecaps = gst_caps_new_simple("video/x-raw-yuv",
											"width", G_TYPE_INT, pDestRect->width,
											"height", G_TYPE_INT, pDestRect->height,
											"framerate", GST_TYPE_FRACTION, 0,1,
											NULL);
						DEBUG_API_A1("caps: %s",gst_caps_to_string(simplecaps));
						pad = gst_element_get_static_pad(GST_ELEMENT(scale),"src");
						if (!gst_pad_set_caps(pad, simplecaps))
						{
							DEBUG_INFO("Stubbed at this point");
							DEBUG_INFO("Cannot set destrect size during XA_VIDEOSCALE_STRETCH!");
							DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit - XA_VIDEOSCALE_STRETCH");
							return XA_RESULT_SUCCESS;
						}

					}
				}
				DEBUG_INFO("XA_VIDEOSCALE_STRETCH Done");

				break;
			}
			case XA_VIDEOSCALE_FIT:
			{
				DEBUG_INFO("XA_VIDEOSCALE_FIT");
				/* The miminum scale factor between the destination rectangle's width over the
				 * source rectangle's source rectangle's width and the destination rectangle's height over
				 * the source rectangle's height is used. Aspect ratio is maintained. Frame is centered */
				if (pDestRect)
				{
					if (bCtx->baseObj.ctxId != XAMediaRecorderAdaptation)
					{
						GstPad *pad = NULL;
						GstCaps* simplecaps = NULL;
						g_object_set ( G_OBJECT(sink), "force-aspect-ratio", TRUE, NULL);
						simplecaps = gst_caps_new_simple("video/x-raw-rgb",
											"width", G_TYPE_INT, pDestRect->width,
											"height", G_TYPE_INT, pDestRect->height,
											"framerate", GST_TYPE_FRACTION, 0,1,
											NULL);
						DEBUG_API_A1("caps: %s",gst_caps_to_string(simplecaps));
						pad = gst_element_get_static_pad(GST_ELEMENT(sink),"sink");
						if (!gst_pad_set_caps(pad, simplecaps))
						{
							DEBUG_INFO("Stubbed at this point");
							DEBUG_INFO("Cannot set destrect size during XA_VIDEOSCALE_FIT!");
							DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit - XA_VIDEOSCALE_FIT");
							return XA_RESULT_SUCCESS;
						}
					}
					else
					{
						GstPad *videoScalePad = NULL;
						GstCaps *negcaps = NULL;
						gint videoScalePadHeight = 0, videoScalePadWidth = 0;
						gfloat scaleFactorWidth = 0;
						gfloat scaleFactorHeight = 0;
						gfloat scaleFactor = 0;
						videoScalePad = gst_element_get_pad( col1, "src" );
						negcaps = gst_pad_get_negotiated_caps( GST_PAD(videoScalePad) );
						if ( negcaps )
						{
							if( !gst_video_get_size( videoScalePad, &videoScalePadWidth, &videoScalePadHeight ) )
							{
								DEBUG_ERR("Cannot receive current cropscalevideo size!");
								DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit - XA_RESULT_INTERNAL_ERROR");
								return XA_RESULT_INTERNAL_ERROR;
							}
						}
						else
						{
							DEBUG_ERR("No negotiated caps in col1:src!");
							DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit");
							return XA_RESULT_SUCCESS;
						}
						if (pSrcRect->width != 0 && pSrcRect->height != 0)
						{
							scaleFactorWidth = (gfloat)videoScalePadWidth / (gfloat)pSrcRect->width;
							scaleFactorHeight = (gfloat)videoScalePadHeight / (gfloat)pSrcRect->height;
							if (scaleFactorWidth < scaleFactorHeight)
							{
								scaleFactor = scaleFactorWidth;
							}
							else
							{
								scaleFactor = scaleFactorHeight;
							}

							cropscaleBottom = (gint)(pSrcRect->height*scaleFactor - videoScalePadHeight)/2;
							cropscaleLeft = (gint)(pSrcRect->width*scaleFactor - videoScalePadWidth)/2;
							if (cropscaleLeft > 0){
								cropscaleLeft *= -1;
							}
							cropscaleRight = cropscaleLeft;
							if (cropscaleBottom > 0){
								cropscaleBottom *= -1;
							}
							cropscaleTop = cropscaleBottom;
							g_object_set (G_OBJECT (boxElement), "bottom", cropscaleBottom , NULL);
							g_object_set (G_OBJECT (boxElement), "right", cropscaleRight, NULL);
							g_object_set (G_OBJECT (boxElement), "left", cropscaleLeft, NULL);
							g_object_set (G_OBJECT (boxElement), "top", cropscaleTop, NULL);
						}
					}
				}
				DEBUG_INFO("XA_VIDEOSCALE_FIT done");

				break;
			}
			case XA_VIDEOSCALE_CROP:
			{
				DEBUG_INFO("XA_VIDEOSCALE_CROP");
				/* The maximum scale factor between the destination rectangle's width over the source
				 * rectangle's width and destination rectangle's height over the source rectangle's
				 * height is used. Aspect ratio is maintained. Frame is centered. */
				if( pDestRect && pSrcRect )
				{
					GstPad *videoScalePad = NULL;
					GstCaps *negcaps = NULL;
					gint videoScalePadHeight = 0, videoScalePadWidth = 0;
					videoScalePad = gst_element_get_pad( col1, "src" );
					negcaps = gst_pad_get_negotiated_caps( GST_PAD(videoScalePad) );
					if ( negcaps )
					{
						if( !gst_video_get_size( videoScalePad, &videoScalePadWidth, &videoScalePadHeight ) )
						{
							DEBUG_ERR("Cannot receive current cropscalevideo size!");
							DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit - XA_RESULT_INTERNAL_ERROR");
							return XA_RESULT_INTERNAL_ERROR;
						}
					}
					else
					{
						DEBUG_ERR("No negotiated caps in col1:src!");
						DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit");
						return XA_RESULT_SUCCESS;
					}

					DEBUG_INFO_A2( "Received video scale frame info, videoScalePadWidth:%d, "
									"videoScalePadHeight:%d",videoScalePadWidth,videoScalePadHeight);

					if( pSrcRect->height > 0 && pSrcRect->width > 0 )
					{
						if( pSrcRect->height > pDestRect->height )
						{
							videoscaleHeight = pSrcRect->height;
							if( pDestRect->top == 0)
							{
								cropscaleTop = ((videoscaleHeight - pDestRect->height)/2);
								cropscaleBottom = ((videoscaleHeight - pDestRect->height)/2);
							}
							else
							{
								cropscaleTop = (pDestRect->top/2);
								cropscaleBottom = (pDestRect->top/2);
							}
						}
						else if( pDestRect->height > pSrcRect->height )
						{
							videoscaleHeight = pDestRect->height;
							if( pDestRect->top == 0)
							{
								cropscaleTop = -((videoscaleHeight - pSrcRect->height)/2);
								cropscaleBottom = -((videoscaleHeight - pSrcRect->height)/2);
							}
							else
							{
								cropscaleTop = -(pDestRect->top/2);
								cropscaleBottom = -(pDestRect->top/2);
							}
						}
						else if( pDestRect->height == pSrcRect->height )
						{
						}
						else
						{
						}
						if( pSrcRect->width > pDestRect->width )
						{
							if( pDestRect->left == 0 )
							{
								cropscaleLeft = ((gint)(pSrcRect->width -pDestRect->width)/2);
								cropscaleRight = ((gint)(pSrcRect->width -pDestRect->width)/2);
							}
							else
							{
								cropscaleLeft = (pDestRect->left/2);
								cropscaleRight = (pDestRect->left/2);
							}
						}
						else if( pDestRect->width > pSrcRect->width )
						{
							if( pDestRect->left == 0 )
							{
								cropscaleLeft =-((gint)(pDestRect->width -pSrcRect->width)/2);
								cropscaleRight =-((gint)(pDestRect->width -pSrcRect->width)/2);
							}
							else
							{
								cropscaleLeft = -(pDestRect->left/2);
								cropscaleRight = -(pDestRect->left/2);
							}
						}
						else if( pDestRect->width == pSrcRect->width )
						{
						}
						else
						{
						}
					}
					else
					{
						DEBUG_ERR("Invalid rectangle values in source rectangles");
						DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit, Exit with invalid source rectangle values");
						return XA_RESULT_PARAMETER_INVALID;
					}
					if( pDestRect->width != pSrcRect->width && pDestRect->height != pSrcRect->height )
					{
						  DEBUG_INFO_A4("Crop values - cropscaleLeft:%d "
										",cropscaleTop:%d,"
										"cropscaleRight:%d,"
										"cropscaleBottom:%d",
										cropscaleLeft,
										cropscaleTop,
										cropscaleRight,
										cropscaleBottom);
						g_object_set (G_OBJECT (boxElement), "bottom",cropscaleBottom , NULL);
						g_object_set (G_OBJECT (boxElement), "right", cropscaleRight, NULL);
						g_object_set (G_OBJECT (boxElement), "left", cropscaleLeft, NULL);
						g_object_set (G_OBJECT (boxElement), "top",cropscaleTop, NULL);
					}
				}
				break;
			}
			default:
				DEBUG_INFO("no scale options!");
				break;
		}
    }

    
    /*TODO The Code below does nothing. just set the variable which are never used.
     * commenting the code below. */
/*    if(pDestRect && boxElement)
    {
         is background color black 
        if((backgroundColor >> 8) & BLACK_BG_COLOR_MASK)
        {
            videoBackgroundColor = 0;
        }
         is background color green 
        else if((backgroundColor >> 8) & GREEN_BG_COLOR_MASK)
        {
            videoBackgroundColor = 1;
        }
         is background color blue 
        else if((backgroundColor >> 8) & BLUE_BG_COLOR_MASK)
        {
            videoBackgroundColor = 2;
        }
        else
        {
             by default black 
            videoBackgroundColor = 0;
        }

         check alpha value. Gst support values 0 to 1 and XA 0 to 255 
        {
            XAuint32 tempColor = 0;
            tempColor = backgroundColor & ALPHA_VALUE_MASK;

            alphaValue = (gdouble)(tempColor/ALPHA_VALUE_MASK);
        }
    
    
    }*/

    if( dataSrc )
    {
        XAMediaType mediaType = XA_MEDIATYPE_UNKNOWN;
        ret = XACommon_CheckDataSource(dataSrc, &mediaType);
        if( ret == XA_RESULT_SUCCESS && mediaType == XA_MEDIATYPE_IMAGE )
        {
            gstOrigState = GST_STATE(bCtx->bin);

            DEBUG_INFO_A1("Sending change state request to state %d", GST_STATE_READY);
            gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), GST_STATE_READY);
            gstTmpState = GST_STATE(bCtx->bin);
            
            if(gstRet == GST_STATE_CHANGE_SUCCESS && gstTmpState == GST_STATE_READY)
            {
            DEBUG_INFO_A1("Sending change state request to state %d", gstOrigState);
            gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), gstOrigState);
            }
        }
    }

    DEBUG_API("<-XAVideoPostProcessingItfAdapt_Commit");
    return ret;
}

