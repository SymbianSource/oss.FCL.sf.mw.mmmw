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
#include "xacameraitf.h"
#ifdef _GSTREAMER_BACKEND_
#include "XACameraItfAdaptation.h"
#endif
#include "xathreadsafety.h"

/* XACameraItfImpl* GetImpl(XACameraItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XACameraItfImpl* GetImpl(XACameraItf self)
{
    if( self )
    {
        XACameraItfImpl* impl = (XACameraItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XACameraItf implementation
 *****************************************************************************/

/**
 * XAresult XACameraItfImpl_RegisterCallback( XACameraItf self,
 *                                            xaCameraCallback callback,
 *                                            void *pContext )
 * Description: Sets callback for camera event notification.
 **/
XAresult XACameraItfImpl_RegisterCallback( XACameraItf self,
                                           xaCameraCallback callback,
                                           void *pContext )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_RegisterCallback");
    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_RegisterCallback");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->callback = callback;
    impl->context = pContext;
    impl->cbPtrToSelf = self;

    DEBUG_API("<-XACameraItfImpl_RegisterCallback");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetFlashMode( XACameraItf self, XAuint32 flashMode )
 * Description: Sets the camera flash setting.
 **/
XAresult XACameraItfImpl_SetFlashMode( XACameraItf self, XAuint32 flashMode )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_SetFlashMode");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);

    if( !impl || flashMode > XA_CAMERA_FLASHMODE_TORCH || flashMode < XA_CAMERA_FLASHMODE_OFF )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetFlashMode");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is flash mode changed */
    if( impl->flashMode != flashMode )
    {
#ifdef _GSTREAMER_BACKEND_    
        ret = XACameraItfAdapt_SetFlashMode( impl->adapCtx, flashMode );
#endif        
        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->flashMode = flashMode;
        }
        /* wait for adaptation */
        impl->flashReady = XA_BOOLEAN_FALSE;
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetFlashMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetFlashMode( XACameraItf self, XAuint32 *pFlashMode )
 * Description: Gets the camera flash setting.
 **/
XAresult XACameraItfImpl_GetFlashMode( XACameraItf self, XAuint32 *pFlashMode )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_GetFlashMode");
    if( !impl || !pFlashMode )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetFlashMode");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pFlashMode = impl->flashMode;

    DEBUG_API("<-XACameraItfImpl_GetFlashMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_IsFlashReady( XACameraItf self, XAboolean *pReady )
 * Description: Queries whether the flash is ready for use.
 **/
XAresult XACameraItfImpl_IsFlashReady( XACameraItf self, XAboolean *pReady )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_IsFlashReady");
    if( !impl || !pReady )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_IsFlashReady");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    *pReady = impl->flashReady;

    DEBUG_API("<-XACameraItfImpl_IsFlashReady");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetFocusMode( XACameraItf self, XAuint32 focusMode,
 *                                        XAmillimeter manualSetting,
 *                                        XAboolean macroEnabled )
 * Description: Sets the camera focus mode.
 **/
XAresult XACameraItfImpl_SetFocusMode( XACameraItf self, XAuint32 focusMode,
                                       XAmillimeter manualSetting,
                                       XAboolean macroEnabled )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_SetFocusMode");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);

    if( !impl || focusMode < XA_CAMERA_FOCUSMODE_MANUAL || focusMode > XA_CAMERA_FOCUSMODE_CONTINUOUS_CENTROID )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetFocusMode");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is focus mode changed */
    if( impl->focusMode != focusMode )
    {
#ifdef _GSTREAMER_BACKEND_       
        ret = XACameraItfAdapt_SetFocusMode( impl->adapCtx, focusMode, manualSetting, macroEnabled  );
#endif        
        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->focusMode = focusMode;
            impl->focusManualSetting = manualSetting;
            impl->macroEnabled = macroEnabled;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetFocusMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetFocusMode( XACameraItf self,
 *                                        XAuint32 *pFocusMode,
 *                                        XAmillimeter *pManualSetting,
 *                                        XAboolean *pMacroEnabled )
 * Description: Gets the camera focus mode.
 **/
XAresult XACameraItfImpl_GetFocusMode( XACameraItf self,
                                       XAuint32 *pFocusMode,
                                       XAmillimeter *pManualSetting,
                                       XAboolean *pMacroEnabled )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_GetFocusMode");
    if( !impl || !pFocusMode || !pManualSetting || !pMacroEnabled )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetFocusMode");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pFocusMode = impl->focusMode;
    *pMacroEnabled = impl->macroEnabled;

    if( impl->focusMode & XA_CAMERA_FOCUSMODE_MANUAL )
    {
        *pManualSetting = impl->focusManualSetting;
    }

    DEBUG_API("<-XACameraItfImpl_GetFocusMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetFocusRegionPattern( XACameraItf self,
 *                                                 XAuint32 focusPattern,
 *                                                 XAuint32 activePoints1,
 *                                                 XAuint32 activePoints2 )
 * Description: Set the camera focus region pattern.
 **/
XAresult XACameraItfImpl_SetFocusRegionPattern( XACameraItf self,
                                                XAuint32 focusPattern,
                                                XAuint32 activePoints1,
                                                XAuint32 activePoints2 )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_SetFocusRegionPattern");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);

    if( !impl || focusPattern < XA_FOCUSPOINTS_ONE || focusPattern > XA_FOCUSPOINTS_CUSTOM )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetFocusRegionPattern");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is focus pattern changed */
    if( focusPattern == impl->focusPattern )
    {
#ifdef _GSTREAMER_BACKEND_   
        ret = XACameraItfAdapt_SetFocusRegionPattern( impl->adapCtx, focusPattern, activePoints1, activePoints2 );
#endif
        if (ret == XA_RESULT_SUCCESS)
        {
            impl->focusPattern = focusPattern;
            if( !(impl->focusMode & XA_CAMERA_FOCUSMODE_MANUAL) )
            {
                impl->activePoints1 = activePoints1;
                impl->activePoints2 = activePoints2;
            }
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetFocusRegionPattern");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetFocusRegionPattern( XACameraItf self,
 *                                                 XAuint32 *pFocusPattern,
 *                                                 XAuint32 *pActivePoints1,
 *                                                 XAuint32 *pActivePoints2 )
 * Description: Gets the camera focus region pattern.
 **/
XAresult XACameraItfImpl_GetFocusRegionPattern( XACameraItf self,
                                                XAuint32 *pFocusPattern,
                                                XAuint32 *pActivePoints1,
                                                XAuint32 *pActivePoints2 )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_GetFocusRegionPattern");
    if( !impl || !pFocusPattern || !pActivePoints1 || !pActivePoints2 )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetFocusRegionPattern");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pFocusPattern = impl->focusPattern;

    if( !(impl->focusMode & XA_CAMERA_FOCUSMODE_MANUAL) &&
        (impl->focusPattern == XA_FOCUSPOINTS_CUSTOM) )
    {
        *pActivePoints1 = impl->activePoints1;
        *pActivePoints2 = impl->activePoints2;
    }

    DEBUG_API("<-XACameraItfImpl_GetFocusRegionPattern");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetFocusRegionPositions( XACameraItf self,
 *                                                   XAuint32 *pNumPositionEntries,
 *                                                   XAFocusPointPosition *pFocusPosition )
 * Description: Get the camera focus region pattern's positioning and size for each
 * point in the active focus pattern.
 **/
XAresult XACameraItfImpl_GetFocusRegionPositions( XACameraItf self,
                                                  XAuint32 *pNumPositionEntries,
                                                  XAFocusPointPosition *pFocusPosition )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_GetFocusRegionPositions");
    if( !impl || !pNumPositionEntries || !pFocusPosition )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetFocusRegionPositions");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_       
    ret = XACameraItfAdapt_GetFocusRegionPositions( impl->adapCtx, pNumPositionEntries,
                                                    pFocusPosition );
#endif
    DEBUG_API("<-XACameraItfImpl_GetFocusRegionPositions");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetFocusModeStatus( XACameraItf self,
 *                                              XAuint32 *pFocusStatus,
 *                                              XAuint32 *pRegionStatus1,
 *                                              XAuint32 *pRegionStatus2 )
 * Description: Gets the camera focus status.
 **/
XAresult XACameraItfImpl_GetFocusModeStatus( XACameraItf self,
                                             XAuint32 *pFocusStatus,
                                             XAuint32 *pRegionStatus1,
                                             XAuint32 *pRegionStatus2 )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_GetFocusModeStatus");
    if( !impl || !*pFocusStatus )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetFocusModeStatus");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    *pFocusStatus = impl->focusStatus;

    DEBUG_API("<-XACameraItfImpl_GetFocusModeStatus");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetMeteringMode( XACameraItf self, XAuint32 meteringMode )
 * Description: Sets the camera metering mode for exposure.
 **/
XAresult XACameraItfImpl_SetMeteringMode( XACameraItf self, XAuint32 meteringMode )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);

    DEBUG_API("->XACameraItfImpl_SetMeteringMode");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);

    if( !impl || meteringMode < XA_CAMERA_METERINGMODE_AVERAGE || meteringMode >XA_CAMERA_METERINGMODE_MATRIX )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetMeteringMode");
        return XA_RESULT_PARAMETER_INVALID;
    }
#ifdef _GSTREAMER_BACKEND_   
    ret = XACameraItfAdapt_SetMeteringMode( impl->adapCtx, meteringMode );
#endif
    if ( ret == XA_RESULT_SUCCESS )
    {
        impl->meteringMode = meteringMode;
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetMeteringMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetMeteringMode( XACameraItf self, XAuint32 *pMeteringMode )
 * Description: Gets the camera metering mode for exposure.
 **/
XAresult XACameraItfImpl_GetMeteringMode( XACameraItf self, XAuint32 *pMeteringMode )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = GetImpl(self);
    DEBUG_API("->XACameraItfImpl_GetMeteringMode");

    if( !impl || !pMeteringMode )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetMeteringMode");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pMeteringMode = impl->meteringMode;

    DEBUG_API("<-XACameraItfImpl_GetMeteringMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetExposureMode( XACameraItf self, XAuint32 exposure,
 *                                           XAuint32 compensation )
 * Description: Sets the camera exposure mode.
 **/
XAresult XACameraItfImpl_SetExposureMode( XACameraItf self, XAuint32 exposure,
                                          XAuint32 compensation )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_SetExposureMode");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    impl = GetImpl(self);
    if( !impl || exposure < XA_CAMERA_EXPOSUREMODE_MANUAL || exposure > XA_CAMERA_EXPOSUREMODE_NIGHTPORTRAIT )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetExposureMode");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is focus mode changed */
    if( impl->exposureMode != exposure || impl->compensation != compensation )
    {
#ifdef _GSTREAMER_BACKEND_   
    ret = XACameraItfAdapt_SetExposureMode( impl->adapCtx, exposure, exposure );
#endif
        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->exposureMode = exposure;
            impl->compensation = compensation;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetExposureMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetExposureMode( XACameraItf self, XAuint32 *pExposure,
 *                                           XAuint32 *pCompensation )
 * Description: Gets the camera exposure mode.
 **/
XAresult XACameraItfImpl_GetExposureMode( XACameraItf self, XAuint32 *pExposure,
                                          XAuint32 *pCompensation )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_GetExposureMode");
    impl = GetImpl(self);
    if( !impl || !pExposure || !pCompensation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetExposureMode");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pExposure = impl->exposureMode;
    *pCompensation = impl->compensation;

    DEBUG_API("<-XACameraItfImpl_GetExposureMode");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetISOSensitivity( XACameraItf self, XAuint32 isoSensitivity,
 *                                             XAuint32 manualSetting )
 * Description: Sets the camera ISO sensitivity.
 **/
XAresult XACameraItfImpl_SetISOSensitivity( XACameraItf self, XAuint32 isoSensitivity,
                                            XAuint32 manualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_SetISOSensitivity");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    impl = GetImpl(self);
    if( !impl || isoSensitivity<XA_CAMERA_ISOSENSITIVITYMODE_MANUAL || isoSensitivity>XA_CAMERA_ISOSENSITIVITYMODE_AUTO )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetISOSensitivity");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is focus mode changed */
    if( impl->isoSensitivity != isoSensitivity || impl->isoManualSetting != manualSetting )
    {
#ifdef _GSTREAMER_BACKEND_   
    ret = XACameraItfAdapt_SetISOSensitivity( impl->adapCtx, isoSensitivity, manualSetting );
#endif
        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->isoSensitivity = isoSensitivity;
            impl->isoManualSetting = manualSetting;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetISOSensitivity");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetISOSensitivity( XACameraItf self, XAuint32 *pIsoSensitivity,
 *                                             XAuint32 *pManualSetting )
 * Description: Gets the camera ISO sensitivity.
 **/
XAresult XACameraItfImpl_GetISOSensitivity( XACameraItf self, XAuint32 *pIsoSensitivity,
                                            XAuint32 *pManualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_GetISOSensitivity");
    impl = GetImpl(self);
    if( !impl || !pIsoSensitivity || !pManualSetting )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetISOSensitivity");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pIsoSensitivity = impl->isoSensitivity;

    if( impl->isoSensitivity == XA_CAMERA_ISOSENSITIVITYMODE_MANUAL )
    {
        *pManualSetting = impl->isoManualSetting;
    }
    if( impl->isoSensitivity == XA_CAMERA_ISOSENSITIVITYMODE_AUTO )
    {
        *pManualSetting = 0;
    }

    DEBUG_API("<-XACameraItfImpl_GetISOSensitivity");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetAperture( XACameraItf self, XAuint32 aperture,
 *                                       XAuint32 manualSetting )
 * Description: Sets the camera aperture.
 **/
XAresult XACameraItfImpl_SetAperture( XACameraItf self, XAuint32 aperture,
                                      XAuint32 manualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_SetAperture");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    impl = GetImpl(self);
    if( !impl || aperture<XA_CAMERA_APERTUREMODE_MANUAL || aperture>XA_CAMERA_APERTUREMODE_AUTO )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetAperture");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is aperture mode or value changed */
    if( impl->aperture != aperture || impl->apertureManualSetting != manualSetting )
    {
#ifdef _GSTREAMER_BACKEND_   
        ret = XACameraItfAdapt_SetAperture( impl->adapCtx, aperture, manualSetting );
#endif
        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->aperture = aperture;
            impl->apertureManualSetting = manualSetting;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetAperture");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetAperture( XACameraItf self, XAuint32 *pAperture,
 *                                       XAuint32 *pManualSetting )
 * Description: Gets the camera aperture.
 **/
XAresult XACameraItfImpl_GetAperture( XACameraItf self, XAuint32 *pAperture,
                                      XAuint32 *pManualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_GetAperture");
    impl = GetImpl(self);
    if( !impl || !pAperture || !pManualSetting )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetAperture");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pAperture = impl->aperture;

    if( impl->aperture == XA_CAMERA_APERTUREMODE_MANUAL )
    {
        *pManualSetting = impl->apertureManualSetting;
    }
    if( impl->aperture == XA_CAMERA_APERTUREMODE_AUTO )
    {
          *pManualSetting = 0;
    }

    DEBUG_API("<-XACameraItfImpl_GetAperture");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetShutterSpeed( XACameraItf self, XAuint32 shutterSpeed,
 *                                           XAmicrosecond manualSetting )
 * Description: Sets the camera shutter speed.
 **/
XAresult XACameraItfImpl_SetShutterSpeed( XACameraItf self, XAuint32 shutterSpeed,
                                          XAmicrosecond manualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_SetShutterSpeed");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    impl = GetImpl(self);
    if( !impl || shutterSpeed < XA_CAMERA_SHUTTERSPEEDMODE_MANUAL || shutterSpeed > XA_CAMERA_SHUTTERSPEEDMODE_AUTO )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetShutterSpeed");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_   
    ret = XACameraItfAdapt_SetShutterSpeed( impl->adapCtx, shutterSpeed, manualSetting );
#endif
    if ( ret == XA_RESULT_SUCCESS )
    {
        impl->shutterManualSetting = manualSetting;
        impl->shutterSpeed = shutterSpeed;
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetShutterSpeed");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetShutterSpeed( XACameraItf self, XAuint32 *pShutterSpeed,
 *                                           XAmicrosecond *pManualSetting )
 * Description: Gets the camera shutter speed.
 **/
XAresult XACameraItfImpl_GetShutterSpeed( XACameraItf self, XAuint32 *pShutterSpeed,
                                          XAmicrosecond *pManualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_GetShutterSpeed");
    impl = GetImpl(self);
    if( !impl || !pShutterSpeed || !pManualSetting )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetShutterSpeed");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pShutterSpeed = impl->shutterSpeed;

    if( impl->shutterSpeed == XA_CAMERA_SHUTTERSPEEDMODE_MANUAL )
    {
        *pManualSetting = impl->shutterManualSetting;
    }
    if( impl->shutterSpeed == XA_CAMERA_SHUTTERSPEEDMODE_AUTO )
    {
        *pManualSetting = 0;
    }

    DEBUG_API("<-XACameraItfImpl_GetShutterSpeed");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetWhiteBalance( XACameraItf self, XAuint32 whiteBalance,
 *                                           XAuint32 manualSetting )
 * Description: Sets the camera white balance.
 **/
XAresult XACameraItfImpl_SetWhiteBalance( XACameraItf self, XAuint32 whiteBalance,
                                          XAuint32 manualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_SetWhiteBalance");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    impl = GetImpl(self);
    if( !impl )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetWhiteBalance");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is whitebalance mode or value changed */
    if( impl->whiteBalance != whiteBalance || impl->whiteBalManualSetting != manualSetting )
    {
#ifdef _GSTREAMER_BACKEND_   
        ret = XACameraItfAdapt_SetWhiteBalance( impl->adapCtx, whiteBalance, manualSetting );
#endif
        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->whiteBalance = whiteBalance;
            impl->whiteBalManualSetting = manualSetting;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetWhiteBalance");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetWhiteBalance( XACameraItf self, XAuint32 *pWhiteBalance,
 *                                           XAuint32 *pManualSetting )
 * Description: Gets the camera white balance.
 **/
XAresult XACameraItfImpl_GetWhiteBalance( XACameraItf self, XAuint32 *pWhiteBalance,
                                          XAuint32 *pManualSetting )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_GetWhiteBalance");
    impl = GetImpl(self);
    if( !impl || !pWhiteBalance || !pManualSetting )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetWhiteBalance");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pWhiteBalance = impl->whiteBalance;

    if( impl->whiteBalance == XA_CAMERA_WHITEBALANCEMODE_MANUAL )
    {
        *pManualSetting = impl->whiteBalManualSetting;
    }

    DEBUG_API("<-XACameraItfImpl_GetWhiteBalance");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetAutoLocks( XACameraItf self, XAuint32 locks )
 * Description: Locks the given automatic camera settings. This method is typically
 * called when the camera trigger is half-pressed.
 **/
XAresult XACameraItfImpl_SetAutoLocks( XACameraItf self, XAuint32 locks )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    DEBUG_API("->XACameraItfImpl_SetAutoLocks");
    impl = GetImpl(self);
    if( !impl || locks < XA_CAMERA_LOCK_AUTOFOCUS ||
        (locks > (  XA_CAMERA_LOCK_AUTOFOCUS & XA_CAMERA_LOCK_AUTOEXPOSURE & XA_CAMERA_LOCK_AUTOWHITEBALANCE )))
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetAutoLocks");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_   
    ret = XACameraItfAdapt_SetAutoLocks( impl->adapCtx, locks );
#endif
    if ( ret == XA_RESULT_SUCCESS )
    {
        impl->locks = locks;
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetAutoLocks");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetAutoLocks( XACameraItf self, XAuint32 *locks )
 * Description: Gets the current state of the automatic camera settings locks.
 **/
XAresult XACameraItfImpl_GetAutoLocks( XACameraItf self, XAuint32 *locks )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_GetAutoLocks");
    impl = GetImpl(self);
    if( !impl || !locks )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetAutoLocks");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *locks = impl->locks;

    DEBUG_API("<-XACameraItfImpl_GetAutoLocks");
    return ret;
}

/**
 * XAresult XACameraItfImpl_SetZoom( XACameraItf self, XAuint32 zoom,
 *                                   XAboolean digitalEnabled, XAuint32 speed,
 *                                   XAboolean async )
 * Description: Sets the new zoom factor.
 **/
XAresult XACameraItfImpl_SetZoom( XACameraItf self, XApermille zoom,
                                  XAboolean digitalEnabled, XAuint32 speed,
                                  XAboolean async )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_SetZoom");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    impl = GetImpl(self);
    if( !impl || !( speed == XA_CAMERA_ZOOM_FASTEST ||  speed == XA_CAMERA_ZOOM_FAST
        || speed == XA_CAMERA_ZOOM_NORMAL || speed == XA_CAMERA_ZOOM_SLOW ) )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_SetZoom");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_   
    ret = XACameraItfAdapt_SetZoom( impl->adapCtx, zoom, digitalEnabled, speed, async );
#endif
    if ( ret == XA_RESULT_SUCCESS )
    {
        impl->zoom = zoom;
        impl->digitalEnabled = digitalEnabled;
        impl->speed = speed;
        impl->async = async;
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraItfImpl_SetZoom");
    return ret;
}

/**
 * XAresult XACameraItfImpl_GetZoom( XACameraItf self, XAuint32 *pZoom,
 *                                   XAboolean *pDigital )
 * Description: Gets the current zoom factor.
 **/
XAresult XACameraItfImpl_GetZoom( XACameraItf self, XApermille *pZoom,
                                  XAboolean *pDigital )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XACameraItfImpl* impl = NULL;

    DEBUG_API("->XACameraItfImpl_GetZoom");
    impl = GetImpl(self);
    if( !impl || !pZoom || !pDigital )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraItfImpl_GetZoom");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pZoom = impl->zoom;
    *pDigital = impl->digitalEnabled;

    DEBUG_API("<-XACameraItfImpl_GetZoom");
    return ret;
}


/*****************************************************************************
 * XACameraItfImpl -specific methods
 *****************************************************************************/

/* XACameraItfImpl* XACameraItfImpl_Create()
 * Description: Allocate and initialize CameraItfImpl
 */
#ifdef _GSTREAMER_BACKEND_   

XACameraItfImpl* XACameraItfImpl_Create( XAAdaptationBaseCtx *adapCtx )
{
    XACameraItfImpl* self = (XACameraItfImpl*)
        calloc(1,sizeof(XACameraItfImpl));
    DEBUG_API("->XACameraItfImpl_Create");
    if( self )
    {
        /* init itf default implementation */
        self->itf.RegisterCallback = XACameraItfImpl_RegisterCallback;
        self->itf.SetFlashMode = XACameraItfImpl_SetFlashMode;
        self->itf.GetFlashMode = XACameraItfImpl_GetFlashMode;
        self->itf.IsFlashReady = XACameraItfImpl_IsFlashReady;
        self->itf.SetFocusMode = XACameraItfImpl_SetFocusMode;
        self->itf.GetFocusMode = XACameraItfImpl_GetFocusMode;
        self->itf.SetFocusRegionPattern = XACameraItfImpl_SetFocusRegionPattern;
        self->itf.GetFocusRegionPattern = XACameraItfImpl_GetFocusRegionPattern;
        self->itf.GetFocusRegionPositions = XACameraItfImpl_GetFocusRegionPositions;
        self->itf.GetFocusModeStatus = XACameraItfImpl_GetFocusModeStatus;
        self->itf.SetMeteringMode = XACameraItfImpl_SetMeteringMode;
        self->itf.GetMeteringMode = XACameraItfImpl_GetMeteringMode;
        self->itf.SetExposureMode = XACameraItfImpl_SetExposureMode;
        self->itf.GetExposureMode = XACameraItfImpl_GetExposureMode;
        self->itf.SetISOSensitivity = XACameraItfImpl_SetISOSensitivity;
        self->itf.GetISOSensitivity = XACameraItfImpl_GetISOSensitivity;
        self->itf.SetAperture = XACameraItfImpl_SetAperture;
        self->itf.GetAperture = XACameraItfImpl_GetAperture;
        self->itf.SetShutterSpeed = XACameraItfImpl_SetShutterSpeed;
        self->itf.GetShutterSpeed = XACameraItfImpl_GetShutterSpeed;
        self->itf.SetWhiteBalance = XACameraItfImpl_SetWhiteBalance;
        self->itf.GetWhiteBalance = XACameraItfImpl_GetWhiteBalance;
        self->itf.SetAutoLocks = XACameraItfImpl_SetAutoLocks;
        self->itf.GetAutoLocks = XACameraItfImpl_GetAutoLocks;
        self->itf.SetZoom = XACameraItfImpl_SetZoom;
        self->itf.GetZoom = XACameraItfImpl_GetZoom;


        /* init variables */
        self->flashReady = XA_BOOLEAN_TRUE;
        self->adapCtx = adapCtx;
        self->cbPtrToSelf = NULL;
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBase_AddEventHandler( adapCtx, &XACameraItfImp_AdaptCb, XA_CAMERAITFEVENTS, self );
#endif
        self->self = self;
    }

    DEBUG_API("<-XACameraItfImpl_Create");
    return self;
}

#endif
/* void XACameraItfImpl_Free(XACameraItfImpl* self)
 * Description: Free all resources reserved at XACameraItfImpl_Create
 */
void XACameraItfImpl_Free(XACameraItfImpl* self)
{
    DEBUG_API("->XACameraItfImpl_Free");
    assert( self==self->self );
#ifdef _GSTREAMER_BACKEND_   
    XAAdaptationBase_RemoveEventHandler( self->adapCtx, &XACameraItfImp_AdaptCb );
#endif
    free( self );
    DEBUG_API("<-XACameraItfImpl_Free");
}

#ifdef _GSTREAMER_BACKEND_
/* void XACameraItfAdapt_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event )
 * @param void *pHandlerCtx - pointer to cb context (XACameraItfImpl)
 * @param XAAdaptEvent *event  - Event
 * Description: Event handler for adaptation events
 */
void XACameraItfImp_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event )
{
    XACameraItfImpl* impl =(XACameraItfImpl*)pHandlerCtx;
    XAuint32 eventData=0;

    DEBUG_API("->XACameraItfImp_AdaptCb");
    if(!impl)
    {
        DEBUG_ERR("XACameraItfAdapt_AdaptCb, invalid context pointer!");
        DEBUG_API("<-XACameraItfImp_AdaptCb");
        return;
    }
    assert(event);

    if( event->data )
    {
        eventData = *(XAuint32*)event->data;
    }
    switch( event->eventid )
    {
        case XA_CAMERACBEVENT_FOCUSSTATUS:
            impl->focusStatus = eventData;
            break;
        case XA_CAMERACBEVENT_ZOOMSTATUS:
            impl->zoom = eventData;
            break;
        case XA_CAMERACBEVENT_EXPOSURESTATUS:
            impl->focusStatus = eventData;
            break;
        case XA_CAMERACBEVENT_FLASHREADY:
            impl->flashReady = XA_BOOLEAN_TRUE;
            break;
        case XA_CAMERACBEVENT_WHITEBALANCELOCKED:
        case XA_CAMERACBEVENT_ROTATION:
        default:
            break;
    }

    if( impl->callback )
    {
        impl->callback( impl->cbPtrToSelf, impl->context, event->eventid, eventData );
    }
    DEBUG_API("<-XACameraItfImp_AdaptCb");
}

#endif
