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

#ifndef XAOUTPUTMIXITF_H_
#define XAOUTPUTMIXITF_H_

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAOutputMixItf implementation */
typedef struct XAOutputMixItfImpl_
{
    /* parent interface */
    struct XAOutputMixItf_ itf;
    /* pointer to self */
    struct XAOutputMixItfImpl_* self;

    /* Variables */

#ifdef _GSTREAMER_BACKEND_
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
#endif
    XAuint32 eventFlags;
    xaMixDeviceChangeCallback callback;
    void    *cbContext;
    XAOutputMixItf cbPtrToSelf;

} XAOutputMixItfImpl;

/** METHODS **/

/* Base interface XAOutputMixItf implementation */
XAresult XAOutputMixItfImpl_GetDestinationOutputDeviceIDs( XAOutputMixItf self, XAint32 * pNumDevices, XAuint32 * pDeviceIDs );
XAresult XAOutputMixItfImpl_RegisterDeviceChangeCallback( XAOutputMixItf self, xaMixDeviceChangeCallback callback, void * pContext);
XAresult XAOutputMixItfImpl_ReRoute( XAOutputMixItf self, XAint32 numOutputDevices, XAuint32 * pOutputDeviceIDs);

#ifdef _GSTREAMER_BACKEND_
/* XAOutputMixItfImpl -specific methods */
XAOutputMixItfImpl* XAOutputMixItfImpl_Create(XAAdaptationBaseCtx *adapCtx);
void XAOutputMixItfImpl_Free(XAOutputMixItfImpl* self);
void XAOutputMixItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event );
#endif

#endif /* XAOUTPUTMIXITF_H_ */
