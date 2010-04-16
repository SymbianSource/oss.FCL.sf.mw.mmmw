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

#ifndef XACAMERAITF_H
#define XACAMERAITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XACameraItf implementation */
typedef struct XACameraItfImpl_
{
    /* parent interface */
    struct XACameraItf_ itf;
    /* pointer to self */
    struct XACameraItfImpl_* self;

    /* variables */
    XAuint32            flashMode;
    XAboolean           flashReady;
    XAuint32            focusMode;
    XAuint32            focusStatus;
    XAmillimeter        focusManualSetting;
    XAboolean           macroEnabled;
    XAuint32            focusPattern;
    XAuint32            activePoints1;
    XAuint32            activePoints2;
    XAuint32            meteringMode;
    XAuint32            exposureMode;
    XAuint32            compensation;
    XAuint32            isoSensitivity;
    XAuint32            isoManualSetting;
    XAuint32            aperture;
    XAuint32            apertureManualSetting;
    XAuint32            shutterSpeed;
    XAmicrosecond       shutterManualSetting;
    XAuint32            whiteBalance;
    XAuint32            whiteBalManualSetting;
    XAuint32            locks;
    XApermille          zoom;
    XAboolean           digitalEnabled;
    XAuint32            speed;
    XAuint32            async;

    xaCameraCallback    callback;
    void                *context;
    XACameraItf         cbPtrToSelf;

#ifdef _GSTREAMER_BACKEND_
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
#endif
} XACameraItfImpl;

/** METHODS **/

/* Base interface XACameraItf implementation
 *See API Specification for method documentation
 */
XAresult XACameraItfImpl_RegisterCallback( XACameraItf self,
                                           xaCameraCallback callback,
                                           void *pContext );

XAresult XACameraItfImpl_SetFlashMode( XACameraItf self, XAuint32 flashMode );

XAresult XACameraItfImpl_GetFlashMode( XACameraItf self, XAuint32 *pFlashMode );

XAresult XACameraItfImpl_IsFlashReady( XACameraItf self, XAboolean *pReady );

XAresult XACameraItfImpl_SetFocusMode( XACameraItf self, XAuint32 focusMode,
                                       XAmillimeter manualSetting,
                                       XAboolean macroEnabled );

XAresult XACameraItfImpl_GetFocusMode( XACameraItf self,
                                       XAuint32 *pFocusMode,
                                       XAmillimeter *pManualSetting,
                                       XAboolean *pMacroEnabled );

XAresult XACameraItfImpl_SetFocusRegionPattern( XACameraItf self,
                                                XAuint32 focusPattern,
                                                XAuint32 activePoints1,
                                                XAuint32 activePoints2 );

XAresult XACameraItfImpl_GetFocusRegionPattern( XACameraItf self,
                                                XAuint32 *pFocusPattern,
                                                XAuint32 *pActivePoints1,
                                                XAuint32 *pActivePoints2 );

XAresult XACameraItfImpl_GetFocusRegionPositions( XACameraItf self,
                                                  XAuint32 *pNumPositionEntries,
                                                  XAFocusPointPosition *pFocusPosition );

XAresult XACameraItfImpl_GetFocusModeStatus( XACameraItf self,
                                             XAuint32 *pFocusStatus,
                                             XAuint32 *pRegionStatus1,
                                             XAuint32 *pRegionStatus2 );

XAresult XACameraItfImpl_SetMeteringMode( XACameraItf self, XAuint32 meteringMode );

XAresult XACameraItfImpl_GetMeteringMode( XACameraItf self, XAuint32 *pMeteringMode );

XAresult XACameraItfImpl_SetExposureMode( XACameraItf self, XAuint32 exposure,
                                          XAuint32 compensation );

XAresult XACameraItfImpl_GetExposureMode( XACameraItf self, XAuint32 *pExposure,
                                          XAuint32 *pCompensation );

XAresult XACameraItfImpl_SetISOSensitivity( XACameraItf self, XAuint32 isoSensitivity,
                                            XAuint32 manualSetting );

XAresult XACameraItfImpl_GetISOSensitivity( XACameraItf self, XAuint32 *pIsoSensitivity,
                                            XAuint32 *pManualSetting );

XAresult XACameraItfImpl_SetAperture( XACameraItf self, XAuint32 aperture,
                                      XAuint32 manualSetting );

XAresult XACameraItfImpl_GetAperture( XACameraItf self, XAuint32 *pAperture,
                                      XAuint32 *pManualSetting );

XAresult XACameraItfImpl_SetShutterSpeed( XACameraItf self, XAuint32 shutterSpeed,
                                          XAmicrosecond manualSetting );

XAresult XACameraItfImpl_GetShutterSpeed( XACameraItf self, XAuint32 *pShutterSpeed,
                                          XAmicrosecond *pManualSetting );

XAresult XACameraItfImpl_SetWhiteBalance( XACameraItf self, XAuint32 whiteBalance,
                                          XAuint32 manualSetting );

XAresult XACameraItfImpl_GetWhiteBalance( XACameraItf self, XAuint32 *pWhiteBalance,
                                          XAuint32 *pManualSetting );

XAresult XACameraItfImpl_SetAutoLocks( XACameraItf self, XAuint32 locks );

XAresult XACameraItfImpl_GetAutoLocks( XACameraItf self, XAuint32 *locks );

XAresult XACameraItfImpl_SetZoom( XACameraItf self, XApermille zoom,
                                  XAboolean digitalEnabled, XAuint32 speed,
                                  XAboolean async );

XAresult XACameraItfImpl_GetZoom( XACameraItf self, XApermille *pZoom,
                                  XAboolean *pDigital );

#ifdef _GSTREAMER_BACKEND_
/* XACameraItfImpl -specific methods */
XACameraItfImpl* XACameraItfImpl_Create( XAAdaptationBaseCtx *adapCtx );
void XACameraItfImp_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event );
#endif
void XACameraItfImpl_Free(XACameraItfImpl* self);

#endif /* XACAMERAITF_H */
