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
#include <stdlib.h>
#include <assert.h>
#include "xaglobals.h"
#include "xacameracapabilitiesitf.h"
  

#include "xastaticcameracapsadaptation.h"


static XACameraCapabilitiesItfImpl* GetImpl(XACameraCapabilitiesItf self)
{
    if( self )
    {
        XACameraCapabilitiesItfImpl* impl = (XACameraCapabilitiesItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XACameraCapabilitiesItf implementation
 */
XAresult XACameraCapabilitiesItfImpl_GetCameraCapabilities(
                        XACameraCapabilitiesItf self,
                        XAuint32* pIndex,
                        XAuint32* pCameraDeviceID,
                        XACameraDescriptor* pDescriptor)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_GetCameraCapabilities");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetCameraCapabilities");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetCameraCapabilities(pIndex,pCameraDeviceID,pDescriptor);

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetCameraCapabilities");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_QueryFocusRegionPatterns(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pPatternID,
                        XAuint32* pFocusPattern,
                        XAuint32* pCustomPoints1,
                        XAuint32* pCustomPoints2)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_QueryFocusRegionPatterns");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_QueryFocusRegionPatterns");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_QueryFocusRegionPatterns(
            cameraDeviceID, pPatternID, pFocusPattern,pCustomPoints1,pCustomPoints2);

    DEBUG_API("<-XACameraCapabilitiesItfImpl_QueryFocusRegionPatterns");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_GetSupportedAutoLocks(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pNumCombinations,
                        XAuint32** ppLocks)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraCapabilitiesItfImpl_GetSupportedAutoLocks");
    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedAutoLocks");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetSupportedAutoLocks(
                cameraDeviceID, pNumCombinations, ppLocks);

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedAutoLocks");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_GetSupportedFocusManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAboolean macroEnabled,
                        XAmillimeter* pMinValue,
                        XAmillimeter* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmillimeter** ppSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_GetSupportedFocusManualSettings");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedFocusManualSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetSupportedFocusManualSettings(
            cameraDeviceID, macroEnabled,pMinValue, pMaxValue, pNumSettings, ppSettings);

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedFocusManualSettings");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_GetSupportedISOSensitivitySettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_GetSupportedISOSensitivitySettings");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedISOSensitivitySettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetSupportedISOSensitivitySettings(
                cameraDeviceID, pMinValue, pMaxValue, pNumSettings, ppSettings );

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedISOSensitivitySettings");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_GetSupportedApertureManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_GetSupportedApertureManualSettings");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedApertureManualSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetSupportedApertureManualSettings(
                cameraDeviceID, pMinValue, pMaxValue, pNumSettings, ppSettings );

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedApertureManualSettings");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_GetSupportedShutterSpeedManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAmicrosecond* pMinValue,
                        XAmicrosecond* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmicrosecond** ppSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_GetSupportedShutterSpeedManualSettings");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedShutterSpeedManualSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetSupportedShutterSpeedManualSettings(
                cameraDeviceID, pMinValue, pMaxValue, pNumSettings, ppSettings);

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedShutterSpeedManualSettings");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_GetSupportedWhiteBalanceManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_GetSupportedWhiteBalanceManualSettings");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedWhiteBalanceManualSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetSupportedWhiteBalanceManualSettings(
                cameraDeviceID, pMinValue, pMaxValue, pNumSettings, ppSettings);

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedWhiteBalanceManualSettings");
    return ret;
}


XAresult XACameraCapabilitiesItfImpl_GetSupportedZoomSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAboolean digitalEnabled,
                        XAboolean macroEnabled,
                        XApermille* pMaxValue,
                        XAuint32* pNumSettings,
                        XApermille** ppSettings,
                        XAboolean* pSpeedSupported)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraCapabilitiesItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraCapabilitiesItfImpl_GetSupportedZoomSettings");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedZoomSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAStaticCameraCaps_GetSupportedZoomSettings(
                cameraDeviceID, digitalEnabled, macroEnabled, pMaxValue,
                pNumSettings, ppSettings, pSpeedSupported);

    DEBUG_API("<-XACameraCapabilitiesItfImpl_GetSupportedZoomSettings");
    return ret;
}

  
/**
 * XACameraCapabilitiesItfImpl -specific methods
 **/
XACameraCapabilitiesItfImpl* XACameraCapabilitiesItfImpl_Create(void)
{
    XACameraCapabilitiesItfImpl* self = (XACameraCapabilitiesItfImpl*)
        calloc(1,sizeof(XACameraCapabilitiesItfImpl));
    DEBUG_API("->XACameraCapabilitiesItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.GetCameraCapabilities =
            XACameraCapabilitiesItfImpl_GetCameraCapabilities;
        self->itf.QueryFocusRegionPatterns =
            XACameraCapabilitiesItfImpl_QueryFocusRegionPatterns;
        self->itf.GetSupportedAutoLocks =
            XACameraCapabilitiesItfImpl_GetSupportedAutoLocks;
        self->itf.GetSupportedFocusManualSettings =
            XACameraCapabilitiesItfImpl_GetSupportedFocusManualSettings;
        self->itf.GetSupportedISOSensitivitySettings =
            XACameraCapabilitiesItfImpl_GetSupportedISOSensitivitySettings;
        self->itf.GetSupportedApertureManualSettings =
            XACameraCapabilitiesItfImpl_GetSupportedApertureManualSettings;
        self->itf.GetSupportedShutterSpeedManualSettings =
            XACameraCapabilitiesItfImpl_GetSupportedShutterSpeedManualSettings;
        self->itf.GetSupportedWhiteBalanceManualSettings =
            XACameraCapabilitiesItfImpl_GetSupportedWhiteBalanceManualSettings;
        self->itf.GetSupportedZoomSettings =
            XACameraCapabilitiesItfImpl_GetSupportedZoomSettings;

        self->self = self;
    }
    DEBUG_API("<-XACameraCapabilitiesItfImpl_Create");
    return self;
}

void XACameraCapabilitiesItfImpl_Free(XACameraCapabilitiesItfImpl* self)
{
    DEBUG_API("->XACameraCapabilitiesItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XACameraCapabilitiesItfImpl_Free");
}

