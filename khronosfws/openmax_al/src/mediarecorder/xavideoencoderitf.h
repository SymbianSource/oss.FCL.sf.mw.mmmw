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

#ifndef XAVIDEOENCODERITF_H
#define XAVIDEOENCODERITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAVideoEncoderItf implementation */
typedef struct XAVideoEncoderItfImpl_
{
    /* parent interface */
    struct XAVideoEncoderItf_ itf;
    /* pointer to self */
    struct XAVideoEncoderItfImpl_* self;
#ifdef _GSTREAMER_BACKEND_
    /* variables */
    XAAdaptationBaseCtx *adaptCtx;
#endif
} XAVideoEncoderItfImpl;

/** METHODS **/

/* Base interface XAVideoEncoderItf implementation
 * See API Specification for method documentation
 */
XAresult XAVideoEncoderItfImpl_SetVideoSettings(XAVideoEncoderItf self,
                                                XAVideoSettings *pSettings);

XAresult XAVideoEncoderItfImpl_GetVideoSettings(XAVideoEncoderItf self,
                                                XAVideoSettings *pSettings);


/* XAVideoEncoderItfImpl -specific methods */
XAVideoEncoderItfImpl* XAVideoEncoderItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBaseCtx *adaptCtx,
#else
        void
#endif
        );

void XAVideoEncoderItfImpl_Free(XAVideoEncoderItfImpl* self);

#endif /* XAVIDEOENCODERITF_H */
