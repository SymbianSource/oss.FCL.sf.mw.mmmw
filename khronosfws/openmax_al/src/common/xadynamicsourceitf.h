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

#ifndef XADYNAMICSOURCEITF_H
#define XADYNAMICSOURCEITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_   
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XADynamicSourceItf implementation */
typedef struct XADynamicSourceItfImpl_
{
    /* parent interface */
    struct XADynamicSourceItf_ itf;
    /* pointer to self */
    struct XADynamicSourceItfImpl_* self;
#ifdef _GSTREAMER_BACKEND_   
    /* variables */
    XAAdaptationBaseCtx *adaptCtx;
#endif
} XADynamicSourceItfImpl;

/** METHODS **/

/* Base interface XADynamicSourceItf implementation
 * See API Specification for method documentation
 */
XAresult XADynamicSourceItfImpl_SetSource(XADynamicSourceItf self,
                                        XADataSource *pDataSource);

#ifdef _GSTREAMER_BACKEND_   
/* XADynamicSourceItfImpl -specific methods */
XADynamicSourceItfImpl* XADynamicSourceItfImpl_Create(XAAdaptationBaseCtx *adaptCtx);
#endif
void XADynamicSourceItfImpl_Free(XADynamicSourceItfImpl* self);

#endif /* XADYNAMICSOURCEITF_H */
