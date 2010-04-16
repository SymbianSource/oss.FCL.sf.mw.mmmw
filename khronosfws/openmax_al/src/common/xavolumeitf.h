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

#ifndef XAVOLUMEITF_H
#define XAVOLUMEITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/* Max volume level is implementation-dependent but must be at least 0mB
 * now used max volume 10 mB
 */
#define MAX_VOLUME_LEVEL 10
#define MAX_VOLUME_AT_LEAST 0
/* Stereo position range is -1000 to 1000 permille. -1000 permille is fully left
 * and 1000 permille is fully right. 0 permille is center.
 */
#define STEREO_POSITION_RIGHT 1000
#define STEREO_POSITION_LEFT -1000
#define STEREO_POSITION_CENTER 0

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAVolumeItf implementation */
typedef struct XAVolumeItfImpl_
{
    /* parent interface */
    struct XAVolumeItf_ itf;
    /* pointer to self */
    struct XAVolumeItfImpl_* self;

    XAmillibel volumeLevel;
    XAboolean mute;
    XAboolean enableStereoPos;
    XApermille stereoPosition;
#ifdef _GSTREAMER_BACKEND_  
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
#endif
} XAVolumeItfImpl;

/** METHODS **/

/* Base interface XAVolumeItf implementation */
XAresult XAVolumeItfImpl_SetVolumeLevel(XAVolumeItf self, XAmillibel level);

XAresult XAVolumeItfImpl_GetVolumeLevel(XAVolumeItf self, XAmillibel *pLevel);

XAresult XAVolumeItfImpl_GetMaxVolumeLevel(XAVolumeItf self, XAmillibel *pMaxLevel);

XAresult XAVolumeItfImpl_SetMute(XAVolumeItf self, XAboolean mute);

XAresult XAVolumeItfImpl_GetMute(XAVolumeItf self, XAboolean *pMute);

XAresult XAVolumeItfImpl_EnableStereoPosition(XAVolumeItf self, XAboolean enable);

XAresult XAVolumeItfImpl_IsEnabledStereoPosition(XAVolumeItf self, XAboolean *pEnable);

XAresult XAVolumeItfImpl_SetStereoPosition(XAVolumeItf self, XApermille stereoPosition);

XAresult XAVolumeItfImpl_GetStereoPosition(XAVolumeItf self, XApermille *pStereoPosition);

#ifdef _GSTREAMER_BACKEND_  
/* XAVolumeItfImpl -specific methods */
XAVolumeItfImpl* XAVolumeItfImpl_Create(XAAdaptationBaseCtx *adapCtx);
#endif
void XAVolumeItfImpl_Free(XAVolumeItfImpl* self);

#endif /* XAVOLUMEITF_H */
