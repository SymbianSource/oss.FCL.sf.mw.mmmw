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

#ifndef XAENGINEADAPTCTX_H_
#define XAENGINEADAPTCTX_H_

#include "XAAdaptationContextBase.h"
#include "XAStaticCapsAdaptation.h"

/* TYPEDEFS */

typedef struct XAEngineAdaptationCtx_ XAEngineAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAEngineAdapt_Create();
XAresult XAEngineAdapt_PostInit(XAAdaptationBaseCtx* bCtx);
void XAEngineAdapt_Destroy(XAAdaptationBaseCtx* bCtx);

#endif /* XAENGINEADAPTCTX_H_ */
