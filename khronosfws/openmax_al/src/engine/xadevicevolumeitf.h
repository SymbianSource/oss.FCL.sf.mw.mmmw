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

#ifndef XADEVICEVOLUMEITF_H
#define XADEVICEVOLUMEITF_H

#include "xaadptbasectx.h"

/** MACROS **/
#define DEFAULTDEVICEVOLUME 5
#define DEFAULTDEVICEMINVOLUME 0
#define DEFAULTDEVICEMAXVOLUME 10
#define DEFAULTSCALEISMILLIBEL XA_BOOLEAN_FALSE
/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XADeviceVolumeItf implementation */
typedef struct XADeviceVolumeItfImpl_
{
    /* parent interface */
    struct XADeviceVolumeItf_ itf;
    /* pointer to self */
    struct XADeviceVolumeItfImpl_* self;

    /* variables */

    XAint32 curVolume;
    XAint32 minVolume;
    XAint32 maxVolume;
    XAboolean ismBscale;
 
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;

} XADeviceVolumeItfImpl;

/* Base interface XADeviceVolumeItf implementation */
XAresult XADeviceVolumeItfImpl_GetVolumeScale(
                        XADeviceVolumeItf self,
                        XAuint32 deviceID,
                        XAint32 * pMinValue,
                        XAint32 * pMaxValue,
                        XAboolean * pIsMillibelScale);
XAresult XADeviceVolumeItfImpl_SetVolume(
                        XADeviceVolumeItf self,
                        XAuint32 deviceID,
                        XAint32 volume);
XAresult XADeviceVolumeItfImpl_GetVolume(
                        XADeviceVolumeItf self,
                        XAuint32 deviceID,
                        XAint32 * pVolume);
  
/* XADeviceVolumeItfImpl -specific methods */
XADeviceVolumeItfImpl* XADeviceVolumeItfImpl_Create(XAAdaptationBaseCtx *adapCtx);

void XADeviceVolumeItfImpl_Free(XADeviceVolumeItfImpl* self);

#endif /* XADEVICEVOLUMEITF_H */
