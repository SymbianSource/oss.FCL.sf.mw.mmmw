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

#ifndef XAPLAYBACKRATEITF_H
#define XAPLAYBACKRATEITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAPlaybackRateItf implementation */
typedef struct XAPlaybackRateItfImpl_
{
    /* parent interface */
    struct XAPlaybackRateItf_ itf;
    /* pointer to self */
    struct XAPlaybackRateItfImpl_* self;

#ifdef _GSTREAMER_BACKEND_
    /* variables */
    XAAdaptationBaseCtx *adaptCtx;
#endif
    XApermille currentRate;

} XAPlaybackRateItfImpl;

/** METHODS **/

/* Base interface XAPlaybackRateItf implementation */
XAresult XAPlaybackRateItfImpl_SetRate(XAPlaybackRateItf self, XApermille rate);

XAresult XAPlaybackRateItfImpl_GetRate(XAPlaybackRateItf self, XApermille *pRate);

XAresult XAPlaybackRateItfImpl_SetPropertyConstraints(XAPlaybackRateItf self,
                                                        XAuint32 constraints);

XAresult XAPlaybackRateItfImpl_GetProperties(XAPlaybackRateItf self,
                                               XAuint32 *pProperties);

XAresult XAPlaybackRateItfImpl_GetCapabilitiesOfRate(XAPlaybackRateItf self,
                                                       XApermille rate,
                                                       XAuint32 *pCapabilities);

XAresult XAPlaybackRateItfImpl_GetRateRange(XAPlaybackRateItf self,
                                              XAuint8 index,
                                              XApermille *pMinRate,
                                              XApermille *pMaxRate,
                                              XApermille *pStepSize,
                                              XAuint32 *pCapabilities);

#ifdef _GSTREAMER_BACKEND_

/* XAPlaybackRateItfImpl -specific methods */
XAPlaybackRateItfImpl* XAPlaybackRateItfImpl_Create( XAAdaptationBaseCtx *adaptCtx );
#endif
void XAPlaybackRateItfImpl_Free(XAPlaybackRateItfImpl* self);

#endif /* XAPLAYBACKRATEITF_H */
