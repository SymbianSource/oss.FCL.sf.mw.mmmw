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

#ifndef XAPLAYITF_H
#define XAPLAYITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
#include "xaadaptationcontextbasemmf.h"
#include "xamediaplayer.h"


/** MACROS **/
#define NO_POSITION XA_TIME_UNKNOWN
#define PLAYITF_DEFAULT_UPDATE_PERIOD 1000
#define START_POSITION 0

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAPlayItf implementation */
typedef struct XAPlayItfImpl_
{
    /* parent interface */
    struct XAPlayItf_ itf;
    /* pointer to self */
    struct XAPlayItfImpl_* self;

    /* variables */
    XAPlayItf cbPtrToSelf;
    XAuint32 playbackState;
    xaPlayCallback callback;
    void *cbcontext;
    XAuint32 eventFlags;
    XAmillisecond markerPosition;
    XAmillisecond positionUpdatePeriod;
    XAboolean     positionupdateOn;
    XAmillisecond lastPosition;
    XAboolean isMarkerPosCbSend;

#ifdef _GSTREAMER_BACKEND_
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
#endif
    XAAdaptationBaseMMFCtx *adaptCtxMMF;
    XAboolean isMMFPlayback;

} XAPlayItfImpl;

/** METHODS **/

/* Base interface XAPlayItf implementation */
XAresult XAPlayItfImpl_SetPlayState(XAPlayItf self, XAuint32 state);

XAresult XAPlayItfImpl_GetPlayState(XAPlayItf self, XAuint32 *pState);

XAresult XAPlayItfImpl_GetDuration(XAPlayItf self, XAmillisecond *pMsec);

XAresult XAPlayItfImpl_GetPosition(XAPlayItf self, XAmillisecond *pMsec);

XAresult XAPlayItfImpl_RegisterCallback(XAPlayItf self, xaPlayCallback callback,
                                        void *pContext);

XAresult XAPlayItfImpl_SetCallbackEventsMask(XAPlayItf self, XAuint32 eventFlags);

XAresult XAPlayItfImpl_GetCallbackEventsMask(XAPlayItf self, XAuint32 *pEventFlags);

XAresult XAPlayItfImpl_SetMarkerPosition(XAPlayItf self, XAmillisecond mSec);

XAresult XAPlayItfImpl_ClearMarkerPosition(XAPlayItf self);

XAresult XAPlayItfImpl_GetMarkerPosition(XAPlayItf self, XAmillisecond *pMsec);

XAresult XAPlayItfImpl_SetPositionUpdatePeriod(XAPlayItf self, XAmillisecond mSec);

XAresult XAPlayItfImpl_GetPositionUpdatePeriod(XAPlayItf self, XAmillisecond *pMsec);


/*
 * implementation-specific methods
 */
XAPlayItfImpl* XAPlayItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBaseCtx *adapCtx,
#endif
        XAAdaptationBaseMMFCtx *adaptationCtxMMF );
#ifdef _GSTREAMER_BACKEND_
void XAPlayItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event );
#endif
XAresult XAPlayItfImpl_DeterminePlaybackEngine(XAPlayItf self, XADataLocator_URI *uri);
void XAPlayItfImpl_Free(XAPlayItfImpl* self);


#endif /* XAPLAYITF_H */
