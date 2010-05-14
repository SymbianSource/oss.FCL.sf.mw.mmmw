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

#ifndef XALEDARRAYADAPTCTX_H
#define XALEDARRAYADAPTCTX_H

#include "xaadaptationgst.h"

/* TYPEDEFS */

typedef struct XALEDArrayAdaptationCtx_ XALEDArrayAdaptationCtx;

/*
 * Structure for Ledarray specific gst-adaptation variables
 */
typedef struct XALEDArrayAdaptationCtx_ {

    /* Parent*/
    XAAdaptationGstCtx_ baseObj;

    /* OMX-AL Variables */
    XAuint32    deviceID;

    /* GST Variables */

} XALedarrayAdaptationCtx_;

/* FUNCTIONS */
XAAdaptationBaseCtx* XALEDArrayAdapt_Create(XAuint32 deviceID);
XAresult XALEDArrayAdapt_PostInit(XAAdaptationGstCtx* bCtx);
void XALEDArrayAdapt_Destroy(XAAdaptationGstCtx* bCtx);

#endif /* XALEDARRAYADAPTCTX_H */
