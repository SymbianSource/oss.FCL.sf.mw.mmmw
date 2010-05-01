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
#include "xaadaptationgst.h"
#include "xamediaplayeradaptctx.h"
#include "xamediarecorderadaptctx.h"
#include "xacameraadaptctx.h"
#include "xaimagecontrolsitfadaptation.h"

/*
 * XAresult XAImageControlsItfAdapt_SetBrightness(XAAdaptationGstCtx *bCtx,
 *                                                XAuint32 brightness)
 */
XAresult XAImageControlsItfAdapt_SetBrightness(XAAdaptationGstCtx *bCtx,
                                               XAuint32 brightness)
{
    XAresult ret = XA_RESULT_SUCCESS;

    gdouble gstBrightness = 0;
    XAint32 tempBrightness = 0;
    GstElement *balanceElement = NULL;
    DEBUG_API("->XAImageControlsItfAdapt_SetBrightness")

    if(!bCtx || (bCtx->baseObj.ctxId != XAMediaPlayerAdaptation && bCtx->baseObj.ctxId != XAMediaRecorderAdaptation
       && bCtx->baseObj.ctxId != XACameraAdaptation))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageControlsItfAdapt_SetBrightness")
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
    {
        XAMediaRecorderAdaptationCtx* ctx = (XAMediaRecorderAdaptationCtx*) bCtx;

        balanceElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_balance");
    }
    if( bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
    {
        XAMediaPlayerAdaptationCtx* ctx = (XAMediaPlayerAdaptationCtx*) bCtx;

        balanceElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_balance");
    }
    if( bCtx->baseObj.ctxId == XACameraAdaptation )
    {
        XACameraAdaptationCtx* ctx = (XACameraAdaptationCtx*) bCtx;

        GstElement *videoPP = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "videopp_camera");
        if ( !videoPP )
        {
            DEBUG_ERR("Could not receive videopp from camerabin!");
        }
        else
        {
            /* Get camera balance element */
            balanceElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_balance");
        }
    }

    /* count gstBrightness from XA contrast. Allowed values for gst is -1 to 1
     * and allowed value for XA is 0 to 100 */
    tempBrightness = brightness;

    gstBrightness = ( ((gdouble)tempBrightness - SCALE_VALUE_BRIGHTNESS) / SCALE_VALUE_BRIGHTNESS );

    if(balanceElement)
    {
        g_object_set(G_OBJECT(balanceElement), "brightness", gstBrightness, NULL);

        if(bCtx->baseObj.ctxId == XAMediaPlayerAdaptation)
        {
            ret = XAImageControlsItfAdapt_HandleImageType(bCtx);
        }
    }

	if ( balanceElement )
	{
        gst_object_unref(balanceElement);
	}

    DEBUG_API("<-XAImageControlsItfAdapt_SetBrightness")
    return ret;
}

/*
 * XAresult XAImageControlsItfAdapt_SetContrast(XAAdaptationGstCtx *bCtx,
 *                                              XAuint32 contrast)
 */
XAresult XAImageControlsItfAdapt_SetContrast(XAAdaptationGstCtx *bCtx,
                                             XAint32 contrast)
{
    XAresult ret = XA_RESULT_SUCCESS;

    gdouble gstContrast = 1;
    GstElement *balanceElement = NULL;
    DEBUG_API("->XAImageControlsItfAdapt_SetContrast")

    if(!bCtx || (bCtx->baseObj.ctxId != XAMediaPlayerAdaptation && bCtx->baseObj.ctxId != XAMediaRecorderAdaptation
       && bCtx->baseObj.ctxId != XACameraAdaptation))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageControlsItfAdapt_SetContrast")
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
    {
        XAMediaPlayerAdaptationCtx* ctx = (XAMediaPlayerAdaptationCtx*) bCtx;

        balanceElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_balance");
    }
    if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
    {
        XAMediaRecorderAdaptationCtx* ctx = (XAMediaRecorderAdaptationCtx*) bCtx;

        balanceElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_balance");
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
            /* Get camera balance element */
            balanceElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_balance");
        }
    }

    /* count gstContrast from XA contrast. Allowed values for gst is 0 to 2
     * and allowed value for XA is -100 to 100 */
    gstContrast = (( (gdouble)contrast + SCALE_VALUE_CONTRAST ) / SCALE_VALUE_CONTRAST );

    if(balanceElement)
    {
        g_object_set(G_OBJECT(balanceElement), "contrast", gstContrast, NULL);

        if(bCtx->baseObj.ctxId == XAMediaPlayerAdaptation)
        {
            ret = XAImageControlsItfAdapt_HandleImageType(bCtx);
        }
    }

	if ( balanceElement )
	{
        gst_object_unref(balanceElement);
	}

    DEBUG_API("<-XAImageControlsItfAdapt_SetContrast")
    return ret;
}

/*
 * XAresult XAImageControlsItfAdapt_SetGamma(XAAdaptationGstCtx *bCtx,
 *                                           XApermille gamma)
 */
XAresult XAImageControlsItfAdapt_SetGamma(XAAdaptationGstCtx *bCtx,
                                          XApermille gamma)
{
    XAresult ret = XA_RESULT_SUCCESS;

    gdouble gstGamma=0;
    GstElement *gammaElement = NULL;
    DEBUG_API("->XAImageControlsItfAdapt_SetGamma")

    if(!bCtx || (bCtx->baseObj.ctxId != XAMediaPlayerAdaptation && bCtx->baseObj.ctxId != XAMediaRecorderAdaptation
       && bCtx->baseObj.ctxId != XACameraAdaptation))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageControlsItfAdapt_SetGamma")
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
    {
        XAMediaPlayerAdaptationCtx* ctx = (XAMediaPlayerAdaptationCtx*) bCtx;

        gammaElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_gamma");
    }

    if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
    {
        XAMediaRecorderAdaptationCtx* ctx = (XAMediaRecorderAdaptationCtx*) bCtx;

        gammaElement = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "pp_gamma");
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
            /* Get camera gamma element */
            gammaElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_gamma");
        }
    }

    /* count gstGamma from XA gamma */
    gstGamma = ( (gdouble)gamma / SCALE_VALUE_GAMMA );

    if(gammaElement)
    {
        g_object_set(G_OBJECT(gammaElement), "gamma", gstGamma, NULL);

        if(bCtx->baseObj.ctxId == XAMediaPlayerAdaptation)
        {
            ret = XAImageControlsItfAdapt_HandleImageType(bCtx);
        }
    }

	if ( gammaElement )
	{
        gst_object_unref(gammaElement);
	}

    DEBUG_API("<-XAImageControlsItfAdapt_SetGamma")
    return ret;
}

/*
 * XAresult XAImageControlsItfAdapt_GetSupportedGammaSettings(XAAdaptationGstCtx *bCtx,
 *                                                            XApermille *pMinValue,
 *                                                            XApermille *pMaxValue,
 *                                                            XAuint32 *pNumSettings,
 *                                                            XApermille **ppSettings)
 */
XAresult XAImageControlsItfAdapt_GetSupportedGammaSettings(XAAdaptationGstCtx *bCtx,
                                                           XApermille *pMinValue,
                                                           XApermille *pMaxValue,
                                                           XAuint32 *pNumSettings,
                                                           XApermille **ppSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAImageControlsItfAdapt_GetSupportedGammaSettings")

    if(!bCtx || (bCtx->baseObj.ctxId != XAMediaPlayerAdaptation && bCtx->baseObj.ctxId != XAMediaRecorderAdaptation
       && bCtx->baseObj.ctxId != XACameraAdaptation) || !pMinValue || !pMaxValue || !pNumSettings)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfAdapt_GetSupportedGammaSettings")
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pMinValue = MIN_GAMMA_VALUE;
    *pMaxValue = MAX_GAMMA_VALUE;

    /* manual settings is continuous from min to max then pNumSetting return 0. */
    *pNumSettings = 0;

    DEBUG_API("<-XAImageControlsItfAdapt_GetSupportedGammaSettings")
    return ret;
}
/*
 * XAresult XAImageControlsItfAdapt_HandleImageType(XAAdaptationGstCtx *bCtx)
 * Description: this run gstreamer for image type
 *
 */
XAresult XAImageControlsItfAdapt_HandleImageType(XAAdaptationGstCtx *bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XADataSource *dataSrc = NULL;
    GstStateChangeReturn gstRet = GST_STATE_CHANGE_SUCCESS;
    GstState gstOrigState = GST_STATE_PLAYING;
    GstState gstTmpState = GST_STATE_PLAYING;
    XAMediaPlayerAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XAImageControlsItfAdapt_HandleImageType");

    if( !bCtx || (bCtx->baseObj.ctxId != XAMediaPlayerAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageControlsItfAdapt_HandleImageType")
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;

    dataSrc = mCtx->xaSource;

    if( dataSrc )
    {
        XAMediaType mediaType;
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
    DEBUG_API("<-XAImageControlsItfAdapt_HandleImageType");
    return ret;
}
