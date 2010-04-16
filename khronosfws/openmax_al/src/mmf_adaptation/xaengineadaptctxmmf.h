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

#ifndef XAENGINEADAPTCTXMMF_H_
#define XAENGINEADAPTCTXMMF_H_

#include "xaadaptationcontextbasemmf.h"
//#include "XAStaticCapsAdaptation.h"

/* TYPEDEFS */

typedef struct XAEngineAdaptationMMFCtx_ XAEngineAdaptationMMFCtx;

/* FUNCTIONS */
XAAdaptationBaseMMFCtx* XAEngineAdaptMMF_Create(void);
XAresult XAEngineAdaptMMF_PostInit(XAAdaptationBaseMMFCtx* bCtx);
void XAEngineAdaptMMF_Destroy(XAAdaptationBaseMMFCtx* bCtx);

#endif /* XAENGINEADAPTCTXMMF_H_ */
