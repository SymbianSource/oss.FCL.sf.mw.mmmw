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

#ifndef XAOUTPUTMIXADAPTCTX_H
#define XAOUTPUTMIXADAPTCTX_H

#include "xaadaptationgst.h"

/* TYPEDEFS */

typedef struct XAOutputMixAdaptationCtx_ XAOutputMixAdaptationCtx;

typedef struct XAOutputMixAdaptationCtx_
{
    /* Parent*/
    XAAdaptationGstCtx_ baseObj;
    /* Internal variables */
    XAboolean   isStereoPosition;
    XAboolean   mute;

    XAuint32    currentrouting;

    GArray*     availableDevices;
    GArray*     connectedObjects;
} XAOutputMixAdaptationCtx_;

typedef struct XAOMixAdaptConnObj_
{
    /*pointer to connected context*/
    XAAdaptationGstCtx* ctx;
    /*pointer to sink currently used by connected ctx*/
    GstElement* currentSink;
} XAOMixAdaptConnObj;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAOutputMixAdapt_Create(void);
XAresult XAOutputMixAdapt_PostInit(XAAdaptationGstCtx* bCtx);
void XAOutputMixAdapt_Destroy(XAAdaptationGstCtx* bCtx);


GstElement* XAOutputMixAdapt_GetSink(XAAdaptationGstCtx* bCtx);
XAresult XAOutputMixAdapt_ConnectObject(XAAdaptationGstCtx* omCtx, XAAdaptationGstCtx* bCtx, GstElement* usedMix);
XAresult XAOutputMixAdapt_DisconnectObject(XAAdaptationGstCtx* omCtx, XAAdaptationGstCtx* bCtx);

#endif /* XAOUTPUTMIXADAPTCTX_H */
