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

#ifndef XASEEKITF_H
#define XASEEKITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XASeekItf implementation */
typedef struct XASeekItfImpl_
{
    /* parent interface */
    struct XASeekItf_ itf;
    /* pointer to self */
    struct XASeekItfImpl_* self;

    /* variables */
    XAmillisecond playbackPosition;
    XAuint32 seekMode;
    XAboolean loopEnable;
    XAmillisecond startPos;
    XAmillisecond endPos;

#ifdef _GSTREAMER_BACKEND_
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
#endif
}XASeekItfImpl;

/** METHODS **/

/* Base interface XASeekItf implementation */
XAresult XASeekItfImpl_SetPosition(XASeekItf self, XAmillisecond pos,
                                   XAuint32 seekMode);

XAresult XASeekItfImpl_SetLoop(XASeekItf self, XAboolean loopEnable,
                               XAmillisecond startPos,
                               XAmillisecond endPos);

XAresult XASeekItfImpl_GetLoop(XASeekItf self, XAboolean *pLoopEnabled,
                               XAmillisecond *pStartPos,
                               XAmillisecond *pEndPos);

#ifdef _GSTREAMER_BACKEND_
/* XASeekItfImpl -specific methods */
XASeekItfImpl* XASeekItfImpl_Create( XAAdaptationBaseCtx *adapCtx );
#endif
void XASeekItfImpl_Free(XASeekItfImpl* self);

#endif /* XASEEKITF_H */
