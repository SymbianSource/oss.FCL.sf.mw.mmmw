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

#ifndef XAVIBRAADAPTCTX_H
#define XAVIBRAADAPTCTX_H

#include "XAAdaptationContextBase.h"

/* TYPEDEFS */

typedef struct XAVibraAdaptationCtx_ XAVibraAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAVibraAdapt_Create(XAuint32 deviceID);

XAresult XAVibraAdapt_PostInit(XAAdaptationBaseCtx* bCtx);
void XAVibraAdapt_Destroy(XAAdaptationBaseCtx* bCtx);

#endif /* XAVIBRAADAPTCTX_H */
