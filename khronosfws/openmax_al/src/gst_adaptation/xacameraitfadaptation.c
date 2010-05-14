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
#include <gst/interfaces/photography.h>
#include "xacameraadaptctx.h"
#include "xacameraitfadaptation.h"
#include "xaadaptationgst.h"


/*
 * XAresult XACameraItfAdapt_SetFlashMode( XAAdaptationGstCtx *bCtx, XAuint32 flashMode );
 */
XAresult XACameraItfAdapt_SetFlashMode( XAAdaptationGstCtx *bCtx, XAuint32 flashMode )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAAdaptEvent event = {XA_CAMERAITFEVENTS,XA_CAMERACBEVENT_FLASHREADY,0,0};
    GstFlashMode gstFlashmode;

    DEBUG_API_A1("->XACameraItfAdapt_SetFlashMode 0x%x",(int)flashMode);
    if(!bCtx || bCtx->baseObj.ctxId != XACameraAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }
    switch( flashMode )
    {
         case XA_CAMERA_FLASHMODE_OFF:
             gstFlashmode = GST_PHOTOGRAPHY_FLASH_MODE_OFF;
             break;
         case XA_CAMERA_FLASHMODE_ON:
             gstFlashmode = GST_PHOTOGRAPHY_FLASH_MODE_ON;
             break;
         case XA_CAMERA_FLASHMODE_AUTO:
             gstFlashmode = GST_PHOTOGRAPHY_FLASH_MODE_AUTO;
             break;
         case XA_CAMERA_FLASHMODE_REDEYEREDUCTION:
             gstFlashmode = GST_PHOTOGRAPHY_FLASH_MODE_RED_EYE;
             break;
         case XA_CAMERA_FLASHMODE_FILLIN:
             gstFlashmode = GST_PHOTOGRAPHY_FLASH_MODE_FILL_IN;
             break;
         case XA_CAMERA_FLASHMODE_TORCH:
         case XA_CAMERA_FLASHMODE_REDEYEREDUCTION_AUTO:
         default:
             DEBUG_INFO("Mode not supported in GstPhotography!");
             ret = XA_RESULT_PARAMETER_INVALID;
             break;
    }

    if(ret == XA_RESULT_SUCCESS && GST_IS_PHOTOGRAPHY(bCtx->bin))
    {
        if ( !gst_photography_set_flash_mode( GST_PHOTOGRAPHY(bCtx->bin), gstFlashmode ) )
        {
            ret = XA_RESULT_INTERNAL_ERROR;
        }
    }
    if(ret == XA_RESULT_SUCCESS)
    {
        /* no gst event, senc cb now */
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }

    DEBUG_API("<-XACameraItfAdapt_SetFlashMode");
    return ret;
}

/*
 * XAresult XACameraItfAdapt_SetFocusMode( XAAdaptationGstCtx *bCtx, XAuint32 focusMode,
 *                                              XAmillimeter manualSetting,
 *                                              XAboolean macroEnabled )
 */
XAresult XACameraItfAdapt_SetFocusMode( XAAdaptationGstCtx *bCtx, XAuint32 focusMode,
                                        XAmillimeter manualSetting, XAboolean macroEnabled )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAAdaptEvent event = {XA_CAMERAITFEVENTS,XA_CAMERACBEVENT_FOCUSSTATUS,1,NULL};
    XAuint32 status;

    DEBUG_API_A3("->XACameraItfAdapt_SetFocusMode - focusMode:%d, manualSetting:%d, macroEnabled:%d",
                (int)focusMode, (int)manualSetting, (int)macroEnabled);
    if(!bCtx || bCtx->baseObj.ctxId != XACameraAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(GST_IS_PHOTOGRAPHY(bCtx->bin))
    {
        if ( focusMode == XA_CAMERA_FOCUSMODE_AUTO )
        {
            gst_photography_set_autofocus( GST_PHOTOGRAPHY(bCtx->bin), TRUE );
            status = XA_CAMERA_FOCUSMODESTATUS_REQUEST;
            event.data = &status;
        }
        else
        {
            /* Only auto focus supported in GstPhotography, other modes sets auto focus off */
            DEBUG_INFO("No support in GstPhotography");
            gst_photography_set_autofocus( GST_PHOTOGRAPHY(bCtx->bin), FALSE );
            status = XA_CAMERA_FOCUSMODESTATUS_OFF;
            event.data = &status;
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
        }
    }
    else
    {
        status = XA_CAMERA_FOCUSMODESTATUS_OFF;
        event.data = &status;
    }

    if(event.data)
    {
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }

    DEBUG_API("<-XACameraItfAdapt_SetFocusMode");
    return ret;
}

/*
 * XAresult XACameraItfAdapt_SetFocusRegionPattern( XAAdaptationGstCtx *bCtx,
 *                                                  XAuint32 focusPattern,
 *                                                  XAuint32 activePoints1,
 *                                                  XAuint32 activePoints2 )
 */
XAresult XACameraItfAdapt_SetFocusRegionPattern( XAAdaptationGstCtx *bCtx, XAuint32 focusPattern,
                                                    XAuint32 activePoints1, XAuint32 activePoints2 )
{
    DEBUG_API_A3("->XACameraItfAdapt_SetFocusRegionPattern - focusPattern:%d, activePoints1:%d, activePoints2:%d",
                (int)focusPattern,(int)activePoints1,(int)activePoints2);
    DEBUG_INFO("No support for focus region pattern in GstPhotography ");
    DEBUG_API("<-XACameraItfAdapt_SetFocusRegionPattern");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/* XAresult XACameraItfAdapt_GetFocusRegionPositions( XAAdaptationGstCtx *bCtx, XAuint32 *pNumPositionEntries,
 *                                                    XAFocusPointPosition *pFocusPosition )
 */
XAresult XACameraItfAdapt_GetFocusRegionPositions( XAAdaptationGstCtx *bCtx, XAuint32 *pNumPositionEntries,
                                                   XAFocusPointPosition *pFocusPosition )
{
    DEBUG_API("->XACameraItfAdapt_GetFocusRegionPositions");
    DEBUG_INFO("No focus region support in GstPhotography");
    DEBUG_API("<-XACameraItfAdapt_GetFocusRegionPositions");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XACameraItfAdapt_SetMeteringMode( XAAdaptationGstCtx *bCtx, XAuint32 meteringMode )
 */
XAresult XACameraItfAdapt_SetMeteringMode( XAAdaptationGstCtx *bCtx, XAuint32 meteringMode )
{
    DEBUG_API_A1("->XACameraItfAdapt_SetMeteringMode - meteringMode:%d",(int)meteringMode);
    DEBUG_INFO("No metering modes support in GstPhotography");
    DEBUG_API("<-XACameraItfAdapt_SetMeteringMode");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XACameraItfAdapt_SetExposureMode( XAAdaptationGstCtx *bCtx, XAuint32 exposure, XAuint32 compensation )
 */
XAresult XACameraItfAdapt_SetExposureMode( XAAdaptationGstCtx *bCtx, XAuint32 exposure, XAuint32 compensation )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAAdaptEvent event = {XA_CAMERAITFEVENTS,XA_CAMERACBEVENT_EXPOSURESTATUS,1,NULL};
    GstSceneMode expmode = GST_PHOTOGRAPHY_SCENE_MODE_AUTO;

    DEBUG_API_A2("->XACameraItfAdapt_SetExposureMode - exposure:%d, compensation:%d",
                (int)exposure, (int)compensation);
    if(!bCtx || bCtx->baseObj.ctxId != XACameraAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    switch(exposure)
    {
        case XA_CAMERA_EXPOSUREMODE_AUTO:
            expmode = GST_PHOTOGRAPHY_SCENE_MODE_AUTO;
            break;
        case XA_CAMERA_EXPOSUREMODE_SPORTS:
            expmode = GST_PHOTOGRAPHY_SCENE_MODE_SPORT;
            break;
        case XA_CAMERA_EXPOSUREMODE_PORTRAIT:
            expmode = GST_PHOTOGRAPHY_SCENE_MODE_PORTRAIT;
            break;
        case XA_CAMERA_EXPOSUREMODE_NIGHT:
            expmode = GST_PHOTOGRAPHY_SCENE_MODE_NIGHT;
            break;
        default:
            DEBUG_INFO("GstPhotography doesn't support other than manual settings");
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
    }

    if(GST_IS_PHOTOGRAPHY(bCtx->bin)&&ret==XA_RESULT_SUCCESS)
    {
        if (!gst_photography_set_scene_mode( GST_PHOTOGRAPHY(bCtx->bin), expmode) )
        {
            ret = XA_RESULT_INTERNAL_ERROR;
        }
        else if (exposure == XA_CAMERA_EXPOSUREMODE_AUTO)
        {
            if (!gst_photography_set_ev_compensation( GST_PHOTOGRAPHY(bCtx->bin), compensation) )
            {
                ret = XA_RESULT_INTERNAL_ERROR;
            }
        }
    }
    if(ret == XA_RESULT_SUCCESS)
    {
        event.data = &exposure;
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }

    DEBUG_API("<-XACameraItfAdapt_SetExposureMode");
    return ret;
}

/*
 * XAresult XACameraItfAdapt_SetISOSensitivity( XAAdaptationGstCtx *bCtx, XAuint32 isoSensitivity, XAuint32 manualSetting )
 */
XAresult XACameraItfAdapt_SetISOSensitivity( XAAdaptationGstCtx *bCtx, XAuint32 isoSensitivity, XAuint32 manualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API_A2("->XACameraItfAdapt_SetISOSensitivity - isoSensitivity:%d, manualSetting:%d",
                (int)isoSensitivity, (int)manualSetting);
    if(!bCtx || bCtx->baseObj.ctxId != XACameraAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( isoSensitivity ==  XA_CAMERA_ISOSENSITIVITYMODE_MANUAL)
    {
        if(GST_IS_PHOTOGRAPHY(bCtx->bin))
        {
            if ( !gst_photography_set_iso_speed (GST_PHOTOGRAPHY(bCtx->bin), manualSetting ) )
            {
                ret = XA_RESULT_INTERNAL_ERROR;
            }
        }
    }
    else
    {
        DEBUG_INFO("Iso sensitivity auto not supported in GstPhotography, stubbed value");
        ret = XA_RESULT_SUCCESS;
    }

    DEBUG_API("<-XACameraItfAdapt_SetISOSensitivity");
    return ret;
}

/*
 * XAresult XACameraItfAdapt_SetAperture( XAAdaptationGstCtx *bCtx, XAuint32 aperture, XAuint32 manualSetting )
 */
XAresult XACameraItfAdapt_SetAperture( XAAdaptationGstCtx *bCtx, XAuint32 aperture, XAuint32 manualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API_A2("->XACameraItfAdapt_SetAperture - aperture:%d, manualSetting:%d",
                (int)aperture, (int)manualSetting);
    if(!bCtx || bCtx->baseObj.ctxId != XACameraAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( aperture == XA_CAMERA_APERTUREMODE_MANUAL )
    {
        if(GST_IS_PHOTOGRAPHY(bCtx->bin))
        {
            if (!gst_photography_set_aperture ( GST_PHOTOGRAPHY(bCtx->bin), manualSetting) )
            {
                ret = XA_RESULT_INTERNAL_ERROR;
            }
        }
    }
    else
    {
        DEBUG_INFO("GstPhotography doesn't support auto aperture settings");
        ret = XA_RESULT_FEATURE_UNSUPPORTED;
    }

    DEBUG_API("<-XACameraItfAdapt_SetAperture");
    return ret;
}

/*
 * XAresult XACameraItfAdapt_SetShutterSpeed( XAAdaptationGstCtx *bCtx, XAuint32 shutterSpeed, XAmicrosecond manualSetting )
 */
XAresult XACameraItfAdapt_SetShutterSpeed( XAAdaptationGstCtx *bCtx, XAuint32 shutterSpeed, XAmicrosecond manualSetting )
{
    DEBUG_API_A2("->XACameraItfAdapt_SetShutterSpeed - shutterSpeed:%d, manualSetting:%d",
                (int)shutterSpeed, (int)manualSetting);
    DEBUG_INFO("Shutter speed setting not supported in GstPhotography");
    DEBUG_API("<-XACameraItfAdapt_SetShutterSpeed");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XACameraItfAdapt_SetWhiteBalance( XAAdaptationGstCtx *bCtx, XAuint32 whiteBalance, XAuint32 manualSetting )
 */
XAresult XACameraItfAdapt_SetWhiteBalance( XAAdaptationGstCtx *bCtx, XAuint32 whiteBalance, XAuint32 manualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    GstWhiteBalanceMode gstWbMode = GST_PHOTOGRAPHY_WB_MODE_AUTO;

    DEBUG_API_A2("->XACameraItfAdapt_SetWhiteBalance - whiteBalance:%d, manualSetting:%d",
                (int)whiteBalance, (int)manualSetting);
    if(!bCtx || bCtx->baseObj.ctxId != XACameraAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    switch ( whiteBalance )
    {
    case XA_CAMERA_WHITEBALANCEMODE_AUTO:
        gstWbMode = GST_PHOTOGRAPHY_WB_MODE_AUTO;
        break;
    case XA_CAMERA_WHITEBALANCEMODE_SUNLIGHT:
        gstWbMode = GST_PHOTOGRAPHY_WB_MODE_DAYLIGHT;
        break;
    case XA_CAMERA_WHITEBALANCEMODE_CLOUDY:
        gstWbMode = GST_PHOTOGRAPHY_WB_MODE_CLOUDY;
        break;
    case XA_CAMERA_WHITEBALANCEMODE_TUNGSTEN:
        gstWbMode = GST_PHOTOGRAPHY_WB_MODE_TUNGSTEN;
        break;
    case XA_CAMERA_WHITEBALANCEMODE_FLUORESCENT:
        gstWbMode = GST_PHOTOGRAPHY_WB_MODE_FLUORESCENT;
        break;
    case XA_CAMERA_WHITEBALANCEMODE_SUNSET:
        gstWbMode = GST_PHOTOGRAPHY_WB_MODE_SUNSET;
        break;
    case XA_CAMERA_WHITEBALANCEMODE_INCANDESCENT:
    case XA_CAMERA_WHITEBALANCEMODE_FLASH:
    case XA_CAMERA_WHITEBALANCEMODE_MANUAL:
    case XA_CAMERA_WHITEBALANCEMODE_SHADE:
        DEBUG_INFO("Wanted white balance mode not supported!");
        ret = XA_RESULT_FEATURE_UNSUPPORTED;
        break;
    default:
        break;
    }

    if(ret == XA_RESULT_SUCCESS && GST_IS_PHOTOGRAPHY(bCtx->bin))
    {
        if ( !gst_photography_set_white_balance_mode( GST_PHOTOGRAPHY(bCtx->bin), gstWbMode ) )
        {
            ret = XA_RESULT_INTERNAL_ERROR;
        }
    }

    DEBUG_API("<-XACameraItfAdapt_SetWhiteBalance");
    return ret;
}

/*
 * XAresult XACameraItfAdapt_SetAutoLocks( XAAdaptationGstCtx *bCtx, XAuint32 locks )
 */
XAresult XACameraItfAdapt_SetAutoLocks( XAAdaptationGstCtx *bCtx, XAuint32 locks )
{
    DEBUG_API_A1("->XACameraItfAdapt_SetAutoLocks - locks:%d", (int)locks);
    DEBUG_INFO("No autolocks support in GstPhotography")
    DEBUG_API("<-XACameraItfAdapt_SetAutoLocks");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XACameraItfAdapt_SetZoom( XAAdaptationGstCtx *bCtx, XAuint32 zoom, XAboolean digitalEnabled, XAuint32 speed, XAboolean async )
 */
XAresult XACameraItfAdapt_SetZoom( XAAdaptationGstCtx *bCtx, XAuint32 zoom, XAboolean digitalEnabled, XAuint32 speed, XAboolean async )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAAdaptEvent event = {XA_CAMERAITFEVENTS,XA_CAMERACBEVENT_ZOOMSTATUS,1,NULL};
    gint gstZoom;

    DEBUG_API_A4("->XACameraItfAdapt_SetZoom - zoom:%d,digitalEnabled:%d,speed:%d,async:%d ",
                    (int)zoom,(int)digitalEnabled,(int)speed,(int)async);
    if(!bCtx || bCtx->baseObj.ctxId != XACameraAdaptation || zoom < 1 )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( zoom > 10 )
    {
        gstZoom = MAX_ZOOM;
    }
    else
    {
        /* Gst Zoom range 100 - 1000, 100 = 1x, 200 = 2x, ...*/
        gstZoom = zoom * 100;
    }
    if(GST_IS_PHOTOGRAPHY(bCtx->bin))
    {
        if ( !gst_photography_set_zoom( GST_PHOTOGRAPHY(bCtx->bin), gstZoom ) )
        {
            ret = XA_RESULT_INTERNAL_ERROR;
        }
    }
    if( ret == XA_RESULT_SUCCESS && async )
    {
        /* no gst event, send cb now*/
        event.data = &zoom;
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }

    DEBUG_API("<-XACameraItfAdapt_SetZoom");
    return ret;
}
