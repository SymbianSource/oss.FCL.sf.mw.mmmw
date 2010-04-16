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

#ifndef XASTATICCAMERACAPSADAPTATION_H
#define XASTATICCAMERACAPSADAPTATION_H

#include "openmaxalwrapper.h"
#include "XAGlobals.h"

#ifdef XA_USE_TEST_PLUGINS
#define XACAMERAIMPL_DEFAULT_DEVICE XA_ADAPTID_VIDEOTESTSRC
#else
#define XACAMERAIMPL_DEFAULT_DEVICE XA_ADAPTID_V4L2SRC
#endif

/* Structure to hold commonly used capabilities of codec/device.
 */
typedef struct XAStaticCameraCapsData_
{
    XAuint32              deviceID;
    XACameraDescriptor    XAcaps;
} XAStaticCameraCapsData;

void XAStaticCameraCaps_Init();

XAresult XAStaticCameraCaps_GetCameraCapabilities(
                        XAuint32* pIndex,
                        XAuint32* pCameraDeviceID,
                        XACameraDescriptor* pDescriptor);

XAresult XAStaticCameraCaps_QueryFocusRegionPatterns(
                        XAuint32 cameraDeviceID,
                        XAuint32* pPatternID,
                        XAuint32* pFocusPattern,
                        XAuint32* pCustomPoints1,
                        XAuint32* pCustomPoints2);

XAresult XAStaticCameraCaps_GetSupportedAutoLocks(
                        XAuint32 cameraDeviceID,
                        XAuint32* pNumCombinations,
                        XAuint32** ppLocks);

XAresult XAStaticCameraCaps_GetSupportedFocusManualSettings(
                        XAuint32 cameraDeviceID,
                        XAboolean macroEnabled,
                        XAmillimeter* pMinValue,
                        XAmillimeter* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmillimeter** ppSettings);

XAresult XAStaticCameraCaps_GetSupportedISOSensitivitySettings(
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings);

XAresult XAStaticCameraCaps_GetSupportedApertureManualSettings(
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings);

XAresult XAStaticCameraCaps_GetSupportedShutterSpeedManualSettings(
                        XAuint32 cameraDeviceID,
                        XAmicrosecond* pMinValue,
                        XAmicrosecond* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmicrosecond** ppSettings);

XAresult XAStaticCameraCaps_GetSupportedWhiteBalanceManualSettings(
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings);

XAresult XAStaticCameraCaps_GetSupportedZoomSettings(
                        XAuint32 cameraDeviceID,
                        XAboolean digitalEnabled,
                        XAboolean macroEnabled,
                        XApermille* pMaxValue,
                        XAuint32* pNumSettings,
                        XApermille** ppSettings,
                        XAboolean* pSpeedSupported);

#endif /* XASTATICCAMERACAPSADAPTATION_H */
