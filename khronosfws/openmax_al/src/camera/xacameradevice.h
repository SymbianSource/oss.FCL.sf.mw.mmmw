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

#ifndef XACAMERADEVICE_H
#define XACAMERADEVICE_H

#include "openmaxalwrapper.h"
#include "xaobjectitf.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "../adaptation/XACameraAdaptCtx.h"
#endif
/** MACROS **/


/** TYPES **/


/** ENUMERATIONS **/
/* Enumeration for interfaces that CameraDevice supports.  */
typedef enum
{
    CAMERA_OBJECTITF,
    CAMERA_CAMERAITF,
    CAMERA_CONFIGEXTENSIONITF,
    CAMERA_DIMITF,
    CAMERA_IMAGECONTROLSITF,
    CAMERA_IMAGEEFFECTSITF,
    CAMERA_VIDEOPOSTPROCESSINGITF,
    CAMERA_ITFCOUNT
} XACameraDeviceInterfaces;

/** STRUCTURES **/
/* Specification for CameraDeviceImpl. */
typedef struct XACameraDeviceImpl_
{
    /* Parent for XACameraDeviceImpl */
    XAObjectItfImpl baseObj;

    /* variables */
    XAuint32 deviceID;
#ifdef _GSTREAMER_BACKEND_
    XAAdaptationBaseCtx* adaptationCtx;
#endif    
} XACameraDeviceImpl;


/** METHODS **/

/* base object XAObjectItfImpl methods */
XAresult    XACameraDeviceImpl_DoRealize(XAObjectItf self);
XAresult    XACameraDeviceImpl_DoResume(XAObjectItf self);
void        XACameraDeviceImpl_FreeResources(XAObjectItf self);

/* CameraDeviceImpl -specific methods*/
/* DynamicInterfaceManagement object-specific methods */
XAresult XACameraDeviceImpl_DoAddItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  );
XAresult XACameraDeviceImpl_DoResumeItf(XAObjectItf self, XAObjItfMapEntry *mapEntry );
XAresult XACameraDeviceImpl_DoRemoveItf(XAObjectItf self, XAObjItfMapEntry *mapEntry );



#endif /* XACAMERADEVICE_H */
