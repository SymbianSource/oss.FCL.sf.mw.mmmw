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
#ifdef _GSTREAMER_BACKEND_  
#include "XAAdaptationContextBase.h"
#endif
/* TYPEDEFS */

typedef struct XALEDArrayAdaptationCtx_ XALEDArrayAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XALEDArrayAdapt_Create(XAuint32 deviceID);
XAresult XALEDArrayAdapt_PostInit(XAAdaptationBaseCtx* bCtx);
void XALEDArrayAdapt_Destroy(XAAdaptationBaseCtx* bCtx);

#endif /* XALEDARRAYADAPTCTX_H */
