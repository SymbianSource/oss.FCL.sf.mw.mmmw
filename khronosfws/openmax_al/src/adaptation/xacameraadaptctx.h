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

#ifndef XACAMERAADAPTCTX_H
#define XACAMERAADAPTCTX_H

#include "XAAdaptationContextBase.h"

/* TYPEDEFS */

typedef struct XACameraAdaptationCtx_ XACameraAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XACameraAdapt_Create(XAuint32 deviceID);
XAresult XACameraAdapt_PostInit(XAAdaptationBaseCtx* bCtx);
void XACameraAdapt_Destroy(XAAdaptationBaseCtx* bCtx);

#endif /* XACAMERAADAPTCTX_H */
