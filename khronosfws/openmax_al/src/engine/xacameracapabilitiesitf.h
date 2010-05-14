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

#ifndef XACAMERACAPABILITIESITF_H
#define XACAMERACAPABILITIESITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"

/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XACameraCapabilitiesItf implementation */
typedef struct XACameraCapabilitiesItfImpl_
{
    /* parent interface */
    struct XACameraCapabilitiesItf_ itf;
    /* pointer to self */
    struct XACameraCapabilitiesItfImpl_* self;
    /* variables */
} XACameraCapabilitiesItfImpl;

/* Base interface XACameraCapabilitiesItf implementation */
XAresult XACameraCapabilitiesItfImpl_GetCameraCapabilities(
                        XACameraCapabilitiesItf self,
                        XAuint32* pIndex,
                        XAuint32* pCameraDeviceID,
                        XACameraDescriptor* pDescriptor);

XAresult XACameraCapabilitiesItfImpl_QueryFocusRegionPatterns(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pPatternID,
                        XAuint32* pFocusPattern,
                        XAuint32* pCustomPoints1,
                        XAuint32* pCustomPoints2);

XAresult XACameraCapabilitiesItfImpl_GetSupportedAutoLocks(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pNumCombinations,
                        XAuint32** ppLocks);

XAresult XACameraCapabilitiesItfImpl_GetSupportedFocusManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAboolean macroEnabled,
                        XAmillimeter* pMinValue,
                        XAmillimeter* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmillimeter** ppSettings);

XAresult XACameraCapabilitiesItfImpl_GetSupportedISOSensitivitySettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings);

XAresult XACameraCapabilitiesItfImpl_GetSupportedApertureManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings);

XAresult XACameraCapabilitiesItfImpl_GetSupportedShutterSpeedManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAmicrosecond* pMinValue,
                        XAmicrosecond* pMaxValue,
                        XAuint32* pNumSettings,
                        XAmicrosecond** ppSettings);

XAresult XACameraCapabilitiesItfImpl_GetSupportedWhiteBalanceManualSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAuint32* pMinValue,
                        XAuint32* pMaxValue,
                        XAuint32* pNumSettings,
                        XAuint32** ppSettings);

XAresult XACameraCapabilitiesItfImpl_GetSupportedZoomSettings(
                        XACameraCapabilitiesItf self,
                        XAuint32 cameraDeviceID,
                        XAboolean digitalEnabled,
                        XAboolean macroEnabled,
                        XApermille* pMaxValue,
                        XAuint32* pNumSettings,
                        XApermille** ppSettings,
                        XAboolean* pSpeedSupported);
 
/* XACameraCapabilitiesItfImpl -specific methods */
XACameraCapabilitiesItfImpl* XACameraCapabilitiesItfImpl_Create(void);
void XACameraCapabilitiesItfImpl_Free(XACameraCapabilitiesItfImpl* self);

#endif /* XACAMERACAPABILITIESITF_H */
