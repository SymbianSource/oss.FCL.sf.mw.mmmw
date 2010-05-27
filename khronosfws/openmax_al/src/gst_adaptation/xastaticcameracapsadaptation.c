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

#include <stdio.h>
#include <string.h>
#include <gst/gst.h>
#include <gst/interfaces/photography.h>
#include "xaglobals.h"
#include "xastaticcameracapsadaptation.h"
#include "xaadptbasectx.h"

#define XACAMERAIMPL_DEFAULT_MAX_ZOOM 16000 /* 16x */
#define XACAMERAIMPL_DEFAULT_MAX_ISO 1600
#define XACAMERAIMPL_DEFAULT_MIN_ISO 200
#define XACAMERAIMPL_DEFAULT_MAX_APERTURE 3200 /* f/32 */
#define XACAMERAIMPL_DEFAULT_MIN_APERTURE 75   /* f/0.75 */

static XAStaticCameraCapsData allCameraCaps[] =
{
#ifdef XA_USE_TEST_PLUGINS
        { XA_ADAPTID_VIDEOTESTSRC, { (XAchar*)"videotestsrc", 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
#else
        { XA_ADAPTID_V4L2SRC, { (XAchar*)"v4l2src", 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
#endif
};

/*forward declaration*/
XAuint32 XAStaticCameraCaps_IdxFromId(XAuint32 cameraDeviceID);

/*
 * void XAStaticCameraCaps_Init()
 */
void XAStaticCameraCaps_Init()
{
    GstElement* element=NULL;
    GstPhotoCaps photoCaps = GST_PHOTOGRAPHY_CAPS_NONE;
    GError* gerror=NULL;
    guint32 i=0;
    guint32 numDevices = sizeof(allCameraCaps)/sizeof(XAStaticCameraCapsData);
    gboolean detected = FALSE;


    if ( !gst_init_check( NULL, NULL, &gerror ) )
    {
        DEBUG_ERR("Gst Initalization failure.");
        return;
    }
    for(i=0;i<numDevices;i++)
    {
        /*init values*/
        allCameraCaps[i].XAcaps.orientation = XA_ORIENTATION_UNKNOWN;
        allCameraCaps[i].XAcaps.featuresSupported = 0;
        allCameraCaps[i].XAcaps.exposureModesSupported = 0;
        allCameraCaps[i].XAcaps.flashModesSupported = 0;
        allCameraCaps[i].XAcaps.focusModesSupported = 0;
        allCameraCaps[i].XAcaps.meteringModesSupported = 0;
        allCameraCaps[i].XAcaps.whiteBalanceModesSupported = 0;
        allCameraCaps[i].XAcaps.maxWidth = 0;
        allCameraCaps[i].XAcaps.maxHeight = 0;

        element = gst_element_factory_make((char*)allCameraCaps[i].XAcaps.name,NULL);
        if(element)
        {
            GstPad* srcPad = gst_element_get_static_pad( element, "src");
            if (srcPad)
            {
                guint32 j = 0, cnt = 0, width = 0, height = 0;
                GstCaps* srcCaps = gst_pad_get_caps( srcPad );

                /* search caps for absolute maximum values */
                if(srcCaps)
                {
                    cnt = gst_caps_get_size( srcCaps );
                    for( j = 0; j < cnt; j++ )
                    {
                        G_CONST_RETURN GValue* value = NULL;
                        GstStructure* capsStruct = gst_caps_get_structure( srcCaps, j );

                        if(capsStruct)
                        {
                            value = gst_structure_get_value( capsStruct, "width" );
                            if (GST_VALUE_HOLDS_INT_RANGE(value))
                            {
                                detected = TRUE;
                                width = gst_value_get_int_range_max( value );
                                if(width > allCameraCaps[i].XAcaps.maxWidth)
                                {
                                    allCameraCaps[i].XAcaps.maxWidth = width;
                                }
                            }

                            value = gst_structure_get_value( capsStruct, "height" );
                            if (GST_VALUE_HOLDS_INT_RANGE(value))
                            {
                                detected = TRUE;
                                height = gst_value_get_int_range_max( value );
                                if(height > allCameraCaps[i].XAcaps.maxHeight)
                                {
                                    allCameraCaps[i].XAcaps.maxHeight = height;
                                }
                            }
                        }
                    }
                }
            }

            if(!detected)
            {
                DEBUG_ERR("Warn: could not get maxWidth and maxHeight from GST!!");
                allCameraCaps[i].XAcaps.maxWidth = 0x7FFFFFFF;
                allCameraCaps[i].XAcaps.maxHeight =  0x7FFFFFFF;
            }

            if(GST_IS_PHOTOGRAPHY(element))
            {
                photoCaps = gst_photography_get_capabilities(GST_PHOTOGRAPHY(element));
                if( photoCaps & GST_PHOTOGRAPHY_CAPS_FLASH )
                {
                    allCameraCaps[i].XAcaps.featuresSupported |= XA_CAMERACAP_FLASH;
                    allCameraCaps[i].XAcaps.flashModesSupported = XA_CAMERA_FLASHMODE_OFF |
                                                       XA_CAMERA_FLASHMODE_ON |
                                                       XA_CAMERA_FLASHMODE_AUTO |
                                                       XA_CAMERA_FLASHMODE_REDEYEREDUCTION |
                                                       XA_CAMERA_FLASHMODE_FILLIN;
                }
                if( photoCaps & GST_PHOTOGRAPHY_CAPS_FOCUS )
                {
                    allCameraCaps[i].XAcaps.featuresSupported |= XA_CAMERACAP_AUTOFOCUS;
                    allCameraCaps[i].XAcaps.focusModesSupported = XA_CAMERA_FOCUSMODE_AUTO;
                }
                if( photoCaps & GST_PHOTOGRAPHY_CAPS_EXPOSURE )
                {
                    allCameraCaps[i].XAcaps.featuresSupported |= XA_CAMERACAP_MANUALEXPOSURE;
                    allCameraCaps[i].XAcaps.exposureModesSupported = XA_CAMERA_EXPOSUREMODE_AUTO |
                                                          XA_CAMERA_EXPOSUREMODE_PORTRAIT |
                                                          XA_CAMERA_EXPOSUREMODE_SPORTS |
                                                          XA_CAMERA_EXPOSUREMODE_NIGHT;
                }

                if( photoCaps & GST_PHOTOGRAPHY_CAPS_ISO_SPEED )
                {
                    allCameraCaps[i].XAcaps.featuresSupported |= XA_CAMERACAP_MANUALISOSENSITIVITY;
                }
                if( photoCaps & GST_PHOTOGRAPHY_CAPS_APERTURE )
                {
                    allCameraCaps[i].XAcaps.featuresSupported |= XA_CAMERACAP_MANUALAPERTURE;
                }
                if( photoCaps & GST_PHOTOGRAPHY_CAPS_WB_MODE )
                {
                    allCameraCaps[i].XAcaps.featuresSupported |= XA_CAMERACAP_AUTOWHITEBALANCE;
                    allCameraCaps[i].XAcaps.whiteBalanceModesSupported = XA_CAMERA_WHITEBALANCEMODE_AUTO |
                                                              XA_CAMERA_WHITEBALANCEMODE_SUNLIGHT |
                                                              XA_CAMERA_WHITEBALANCEMODE_CLOUDY |
                                                              XA_CAMERA_WHITEBALANCEMODE_TUNGSTEN |
                                                              XA_CAMERA_WHITEBALANCEMODE_FLUORESCENT |
                                                              XA_CAMERA_WHITEBALANCEMODE_SUNSET;
                }
                if( photoCaps & GST_PHOTOGRAPHY_CAPS_ZOOM )
                {
                    allCameraCaps[i].XAcaps.featuresSupported |= XA_CAMERACAP_DIGITALZOOM;
                }
            }
        }
    }
}

/*
 * XAresult XAStaticCameraCaps_GetCameraCapabilities(
 *                       XAuint32* pIndex,
 *                       XAuint32* pCameraDeviceID,
 *                       XACameraDescriptor* pDescriptor)
 *
 */
XAresult XAStaticCameraCaps_GetCameraCapabilities(
                        XAuint32* pIndex,
                        XAuint32* pCameraDeviceID,
                        XACameraDescriptor* pDescriptor)
{
    guint32 idx = 0;
    guint32 numDevices = sizeof(allCameraCaps)/sizeof(XAStaticCameraCapsData);

    if(!pDescriptor)
    {
        if(pIndex) /* query number of devices */
        {
            *pIndex = numDevices;
            DEBUG_API("<-XAStaticCameraCaps_GetCameraCapabilities");
            return XA_RESULT_SUCCESS;
        }
        else
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAStaticCameraCaps_GetCameraCapabilities");
            return XA_RESULT_PARAMETER_INVALID;
        }
    }

    if( !pIndex && pCameraDeviceID )
    {
        /* get index from ID */
        idx = XAStaticCameraCaps_IdxFromId(*pCameraDeviceID);
    }
    else if( pIndex && pCameraDeviceID )
    {
        idx = *pIndex;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAStaticCameraCaps_GetCameraCapabilities");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( idx >= numDevices )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAStaticCameraCaps_GetCameraCapabilities");
        return XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        /*fill capabilities*/
        memcpy(pDescriptor, &allCameraCaps[idx].XAcaps, sizeof(XACameraDescriptor) );
        *pCameraDeviceID = allCameraCaps[idx].deviceID;
    }

    DEBUG_API("<-XAStaticCameraCaps_GetCameraCapabilities");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAStaticCameraCaps_QueryFocusRegionPatterns(
 *                       XAuint32 cameraDeviceID,
 *                       XAuint32* pPatternID,
 *                       XAuint32* pFocusPattern,
 *                       XAuint32* pCustomPoints1,
 *                       XAuint32* pCustomPoints2)
 */
XAresult XAStaticCameraCaps_QueryFocusRegionPatterns(
                        XAuint32 cameraDeviceID,
                        XAuint32* pPatternID,
                        XAuint32* pFocusPattern,
                        XAuint32* pCustomPoints1,
                        XAuint32* pCustomPoints2)
{
    DEBUG_API("->XAStaticCameraCaps_QueryFocusRegionPatterns");
    if( !pFocusPattern )
    {   /* return number of focus patterns */
        if(!pPatternID)
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAStaticCameraCaps_QueryFocusRegionPatterns");
            return XA_RESULT_PARAMETER_INVALID;
        }
        /* no support in GST */
        DEBUG_INFO("No support in gst");
        *pPatternID = 0;
    }
    else
    {
        if( !(pFocusPattern&&pCustomPoints1&&pCustomPoints2) )
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAStaticCameraCaps_QueryFocusRegionPatterns");
            return XA_RESULT_PARAMETER_INVALID;
        }
        DEBUG_INFO("No support in gst");
        *pFocusPattern = 0;
        *pCustomPoints1 = 0;
        *pCustomPoints2 = 0;
        /*no focus patterns, index always out of range*/
        DEBUG_API("<-XAStaticCameraCaps_QueryFocusRegionPatterns");
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XAStaticCameraCaps_QueryFocusRegionPatterns");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAStaticCameraCaps_GetSupportedAutoLocks(
 *                       XAuint32 cameraDeviceID,
 *                       XAuint32* pNumCombinations,
 *                       XAuint32** ppLocks)
 */
XAresult XAStaticCameraCaps_GetSupportedAutoLocks(
                        XAuint32 cameraDeviceID,
                        XAuint32* pNumCombinations,
                        XAuint32** ppLocks)
{
    DEBUG_API("->XAStaticCameraCaps_GetSupportedAutoLocks");
    /* No support in gst.
     * Spec: "If no settings are supported,
     *        this method returns XA_RESULT_FEATURE_UNSUPPORTED."
     **/
    DEBUG_INFO("No support in gst");
    if(pNumCombinations)
    {
        *pNumCombinations = 0;
    }
    DEBUG_API("<-XAStaticCameraCaps_GetSupportedAutoLocks");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XAStaticCameraCaps_GetSupportedFocusManualSettings(
 *                       XAuint32 cameraDeviceID,
 *                       XAboolean macroEnabled,
 *                       XAmillimeter* pMinValue,
 *                       XAmillimeter* pMaxValue,
 *                       XAuint32* pNumSettings,
 *                       XAmillimeter** ppSettings)
 */
XAresult XAStaticCameraCaps_GetSupportedFocusManualSettings(
                        XAuint32 cameraDeviceID,
                        XAboolean macroEnabled,
                        XAmillimeter* pMinValue,
                        XAmillimeter* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmillimeter** ppSettings)
{
    DEBUG_API("->XAStaticCameraCaps_GetSupportedFocusManualSettings");
    /* No support in gst.
     * Spec: "If no settings are supported,
     *        this method returns XA_RESULT_FEATURE_UNSUPPORTED."
     **/
    DEBUG_INFO("No support in gst");
    if(pNumSettings)
    {
        *pNumSettings = 0;
    }
    DEBUG_API("<-XAStaticCameraCaps_GetSupportedFocusManualSettings");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XAStaticCameraCaps_GetSupportedISOSensitivitySettings(
 *                       XAuint32 cameraDeviceID,
 *                       XAuint32* pMinValue,
 *                       XAuint32* pMaxValue,
 *                       XAuint32* pNumSettings,
 *                       XAuint32** ppSettings)
 */
XAresult XAStaticCameraCaps_GetSupportedISOSensitivitySettings(
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings)
{
    DEBUG_API("->XAStaticCameraCaps_GetSupportedISOSensitivitySettings");
    if( !pNumSettings )
    {
        return XA_RESULT_PARAMETER_INVALID;
    }
    if( !ppSettings )
    {
        /* no items in ppSettings array*/
        *pNumSettings = 0;
    }
    else
    {
        if( !(pMinValue && pMaxValue) )
        {
            DEBUG_API("<-XAStaticCameraCaps_GetSupportedISOSensitivitySettings");
            return XA_RESULT_PARAMETER_INVALID;
        }
        /* continuous range */
        *pNumSettings = 0;
        /* NOTE: no way to query actual values from gstreamer, use hardcoded default values */
        *pMinValue = XACAMERAIMPL_DEFAULT_MIN_ISO;
        *pMaxValue = XACAMERAIMPL_DEFAULT_MAX_ISO;
    }
    DEBUG_API("<-XAStaticCameraCaps_GetSupportedISOSensitivitySettings");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAStaticCameraCaps_GetSupportedApertureManualSettings(
 *                       XAuint32 cameraDeviceID,
 *                       XAuint32* pMinValue,
 *                       XAuint32* pMaxValue,
 *                       XAuint32* pNumSettings,
 *                       XAuint32** ppSettings)
 */
XAresult XAStaticCameraCaps_GetSupportedApertureManualSettings(
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings)
{
    DEBUG_API("->XAStaticCameraCaps_GetSupportedApertureManualSettings");
    if( !pNumSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAStaticCameraCaps_GetSupportedApertureManualSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
    if( !ppSettings )
    {
        /* no items in ppSettings array*/
        *pNumSettings = 0;
    }
    else
    {
        /* continuous range */
        *pNumSettings = 0;
        /* NOTE: no way to query actual values from gstreamer, use hardcoded default values */
        *pMinValue = XACAMERAIMPL_DEFAULT_MIN_APERTURE;
        *pMaxValue = XACAMERAIMPL_DEFAULT_MAX_APERTURE;
    }
    DEBUG_API("<-XAStaticCameraCaps_GetSupportedApertureManualSettings");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAStaticCameraCaps_GetSupportedShutterSpeedManualSettings(
 *                        XAuint32 cameraDeviceID,
 *                       XAmicrosecond* pMinValue,
 *                       XAmicrosecond* pMaxValue,
 *                       XAuint32* pNumSettings,
 *                       XAmicrosecond** ppSettings)
 */
XAresult XAStaticCameraCaps_GetSupportedShutterSpeedManualSettings(
                        XAuint32 cameraDeviceID,
                        XAmicrosecond* pMinValue,
                        XAmicrosecond* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmicrosecond** ppSettings)
{
    DEBUG_API("->XAStaticCameraCaps_GetSupportedShutterSpeedManualSettings");
    /* No support in gst.
     * Spec: "If no settings are supported,
     *        this method returns XA_RESULT_FEATURE_UNSUPPORTED."
     **/
    DEBUG_INFO("No support in gst");
    if(pNumSettings)
    {
        *pNumSettings = 0;
    }
    DEBUG_API("<-XAStaticCameraCaps_GetSupportedShutterSpeedManualSettings");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XAStaticCameraCaps_GetSupportedWhiteBalanceManualSettings(
 *                       XAuint32 cameraDeviceID,
 *                       XAuint32* pMinValue,
 *                       XAuint32* pMaxValue,
 *                       XAuint32* pNumSettings,
 *                       XAuint32** ppSettings)
 */
XAresult XAStaticCameraCaps_GetSupportedWhiteBalanceManualSettings(
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings)
{
    DEBUG_API("->XAStaticCameraCaps_GetSupportedWhiteBalanceManualSettings");
    /* No support in gst.
     * Spec: "If no settings are supported,
     *        this method returns XA_RESULT_FEATURE_UNSUPPORTED."
     **/
    DEBUG_INFO("No support in gst");
    if(pNumSettings)
    {
        *pNumSettings = 0;
    }
    DEBUG_API("<-XAStaticCameraCaps_GetSupportedWhiteBalanceManualSettings");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

/*
 * XAresult XAStaticCameraCaps_GetSupportedZoomSettings(
 *                       XAuint32 cameraDeviceID,
 *                       XAboolean digitalEnabled,
 *                       XAboolean macroEnabled,
 *                       XApermille* pMaxValue,
 *                       XAuint32* pNumSettings,
 *                       XApermille** ppSettings,
 *                       XAboolean* pSpeedSupported)
 */
XAresult XAStaticCameraCaps_GetSupportedZoomSettings(
                        XAuint32 cameraDeviceID,
                        XAboolean digitalEnabled,
                        XAboolean macroEnabled,
                        XApermille* pMaxValue,
                        XAuint32* pNumSettings,
                        XApermille** ppSettings,
                        XAboolean* pSpeedSupported)
{
    DEBUG_API("->XAStaticCameraCaps_GetSupportedZoomSettings");
    if( !ppSettings )
    {
        if( !pNumSettings )
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAStaticCameraCaps_GetSupportedZoomSettings");
            return XA_RESULT_PARAMETER_INVALID;
        }
        *pNumSettings = 0;
    }
    else
    {
        if( !(pMaxValue && pSpeedSupported) )
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAStaticCameraCaps_GetSupportedZoomSettings");
            return XA_RESULT_PARAMETER_INVALID;
        }
        /*NOTE: no way to query actual max value*/
        *pMaxValue = XACAMERAIMPL_DEFAULT_MAX_ZOOM;
        /*no gst support for speed settings*/
        *pSpeedSupported = XA_BOOLEAN_FALSE;
        *pNumSettings = 0;
    }
    DEBUG_API("<-XAStaticCameraCaps_GetSupportedZoomSettings");
    return XA_RESULT_SUCCESS;
}

/*
 * XAuint32 XAStaticCameraCaps_IdxFromId(XAuint32 cameraDeviceID)
 */
XAuint32 XAStaticCameraCaps_IdxFromId(XAuint32 cameraDeviceID)
{
    XAuint32 devId = 0;
    XAuint32 idx = 0;
    guint32 numDevices = sizeof(allCameraCaps)/sizeof(XAStaticCameraCapsData);

    if(cameraDeviceID == XA_DEFAULTDEVICEID_CAMERA)
    {
        devId = XACAMERAIMPL_DEFAULT_DEVICE;
    }
    else
    {
        devId = cameraDeviceID;
    }

    /* query capas with id -> find index*/
    for(idx=0; idx<numDevices; idx++)
    {
        if( devId==allCameraCaps[idx].deviceID )
            break;
    }
    return idx;
}
