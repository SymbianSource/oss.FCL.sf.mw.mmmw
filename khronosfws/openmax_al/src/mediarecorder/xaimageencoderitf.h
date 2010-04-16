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

#ifndef XAIMAGEENCODERITF_H
#define XAIMAGEENCODERITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAImageEncoderItf implementation */
typedef struct XAImageEncoderItfImpl_
{
    /* parent interface */
    struct XAImageEncoderItf_ itf;
    /* pointer to self */
    struct XAImageEncoderItfImpl_* self;
#ifdef _GSTREAMER_BACKEND_
    /* variables */
    XAAdaptationBaseCtx *adaptCtx;
#endif
} XAImageEncoderItfImpl;

/** METHODS **/

/* Base interface XAImageEncoderItf implementation
 * See API Specification for method documentation
 */
XAresult XAImageEncoderItfImpl_SetImageSettings(XAImageEncoderItf self,
                                                const XAImageSettings *pSettings);

XAresult XAImageEncoderItfImpl_GetImageSettings(XAImageEncoderItf self,
                                                XAImageSettings *pSettings);

XAresult XAImageEncoderItfImpl_GetSizeEstimate(XAImageEncoderItf self,
                                               XAuint32 * pSize);


/* XAImageEncoderItfImpl -specific methods */
XAImageEncoderItfImpl* XAImageEncoderItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBaseCtx *adaptCtx,
#endif
        void
        );

void XAImageEncoderItfImpl_Free(XAImageEncoderItfImpl* self);

#endif /* XAIMAGEENCODERITF_H */
