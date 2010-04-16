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

#ifndef XAVIBRADEVICE_H
#define XAVIBRADEVICE_H

#include "openmaxalwrapper.h"
#include "xaobjectitf.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "../adaptation/XAVibraAdaptCtx.h"
#endif
/** MACROS **/


/** TYPES **/


/** ENUMERATIONS **/
/* Enumeration for interfaces that CameraDevice supports.  */
typedef enum
{
    VIBRA_OBJECTITF,
    VIBRA_CONFIGEXTENSIONITF,
    VIBRA_VIBRAITF,
    VIBRA_DIMITF,
    VIBRA_ITFCOUNT
} XAVibraDeviceInterfaces;

/** STRUCTURES **/
/* Specification for VibraDeviceImpl. */
typedef struct XAVibraDeviceImpl_
{
    /* Parent for XACameraDeviceImpl */
    XAObjectItfImpl baseObj;

    /* variables */
    XAuint32 deviceID;

#ifdef _GSTREAMER_BACKEND_
    XAAdaptationBaseCtx* adaptationCtx;
#endif
} XAVibraDeviceImpl;

/** METHODS **/

/* base object XAObjectItfImpl methods */
XAresult    XAVibraDeviceImpl_DoRealize(XAObjectItf self);
XAresult    XAVibraDeviceImpl_DoResume(XAObjectItf self);
void        XAVibraDeviceImpl_FreeResources(XAObjectItf self);

#endif /* XAVIBRADEVICE_H */
