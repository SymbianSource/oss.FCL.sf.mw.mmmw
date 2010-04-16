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

#ifndef XALEDARRAYDEVICE_H
#define XALEDARRAYDEVICE_H

#include "openmaxalwrapper.h"
#include "xaobjectitf.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_  
#include "../adaptation/XALEDArrayAdaptCtx.h"
#endif
/** MACROS **/


/** TYPES **/


/** ENUMERATIONS **/
/* Enumeration for interfaces that LEDArrayDevice supports.  */
typedef enum
{
    LEDARRAY_OBJECTITF,
    LEDARRAY_LEDARRAYITF,
    LEDARRAY_CONFIGEXTENSIONITF,
    LEDARRAY_DIMITF,
    LEDARRAY_ITFCOUNT
} XALEDArrayDeviceInterfaces;

/** STRUCTURES **/
/* Specification for LEDArrayDeviceImpl. */
typedef struct XALEDArrayDeviceImpl_
{
    /* Parent for XALEDArrayDeviceImpl */
    XAObjectItfImpl baseObj;

    /* variables */
    XAuint32 deviceID;

#ifdef _GSTREAMER_BACKEND_
    XAAdaptationBaseCtx* adaptationCtx;
#endif
} XALEDArrayDeviceImpl;


/** METHODS **/

/* base object XAObjectItfImpl methods */
XAresult    XALEDArrayDeviceImpl_DoRealize(XAObjectItf self);
XAresult    XALEDArrayDeviceImpl_DoResume(XAObjectItf self);
void        XALEDArrayDeviceImpl_FreeResources(XAObjectItf self);

#endif /* XALEDARRAYDEVICE_H */
