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

#ifndef XACAMERAITFADAPTATION_H_
#define XACAMERAITFADAPTATION_H_

#include "xaadaptationgst.h"

#define MAX_ZOOM 1000;

/* FUNCTIONS */

XAresult XACameraItfAdapt_SetFlashMode( XAAdaptationGstCtx *bCtx, XAuint32 flashMode );
XAresult XACameraItfAdapt_SetFocusMode( XAAdaptationGstCtx *bCtx, XAuint32 focusMode, XAmillimeter manualSetting, XAboolean macroEnabled );
XAresult XACameraItfAdapt_SetFocusRegionPattern( XAAdaptationGstCtx *bCtx, XAuint32 focusPattern, XAuint32 activePoints1, XAuint32 activePoints2 );
XAresult XACameraItfAdapt_GetFocusRegionPositions( XAAdaptationGstCtx *bCtx, XAuint32 *pNumPositionEntries, XAFocusPointPosition *pFocusPosition );
XAresult XACameraItfAdapt_SetMeteringMode( XAAdaptationGstCtx *bCtx, XAuint32 meteringMode );
XAresult XACameraItfAdapt_SetExposureMode( XAAdaptationGstCtx *bCtx, XAuint32 exposure, XAuint32 compensation );
XAresult XACameraItfAdapt_SetISOSensitivity( XAAdaptationGstCtx *bCtx, XAuint32 isoSensitivity, XAuint32 manualSetting );
XAresult XACameraItfAdapt_SetAperture( XAAdaptationGstCtx *bCtx, XAuint32 aperture, XAuint32 manualSetting );
XAresult XACameraItfAdapt_SetShutterSpeed( XAAdaptationGstCtx *bCtx, XAuint32 shutterSpeed, XAmicrosecond manualSetting );
XAresult XACameraItfAdapt_SetWhiteBalance( XAAdaptationGstCtx *bCtx, XAuint32 whiteBalance, XAuint32 manualSetting );
XAresult XACameraItfAdapt_SetAutoLocks( XAAdaptationGstCtx *bCtx, XAuint32 locks );
XAresult XACameraItfAdapt_SetZoom( XAAdaptationGstCtx *bCtx, XAuint32 zoom, XAboolean digitalEnabled, XAuint32 speed, XAboolean async );

#endif /* XACAMERAITFADAPTATION_H_ */