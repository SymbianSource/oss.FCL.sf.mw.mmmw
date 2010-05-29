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

#ifndef XACONFIGEXTENSIONSITF_H
#define XACONFIGEXTENSIONSITF_H

#include "xaadptbasectx.h"

/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAConfigExtensionsItf implementation */
typedef struct XAConfigExtensionsItfImpl_
{
    /* parent interface */
    struct XAConfigExtensionsItf_ itf;
    /* pointer to self */
    struct XAConfigExtensionsItfImpl_* self;
    /* variables */
    XAuint8 testbufferconf;
   
    /*Adaptation variables*/
    XAAdaptationBaseCtx *ctx;
  
} XAConfigExtensionsItfImpl;

/* Base interface XAConfigExtensionsItf implementation */
XAresult XAConfigExtensionsItfImpl_SetConfiguration(
                            XAConfigExtensionsItf self,
                            const XAchar* configKey,
                            XAuint32 valueSize,
                            const void* pConfigValue);

XAresult XAConfigExtensionsItfImpl_GetConfiguration(
                            XAConfigExtensionsItf self,
                            const XAchar* configKey,
                            XAuint32* pValueSize,
                            void* pConfigValue);

XAresult XAConfigExtensionsItfImpl_SetContext(XAConfigExtensionsItfImpl* self,XAAdaptationBaseCtx* ctx);

/* XAConfigExtensionsItfImpl -specific methods */
XAConfigExtensionsItfImpl* XAConfigExtensionsItfImpl_Create(void);
void XAConfigExtensionsItfImpl_Free(XAConfigExtensionsItfImpl* self);

#endif /* XACONFIGEXTENSIONSITF_H */