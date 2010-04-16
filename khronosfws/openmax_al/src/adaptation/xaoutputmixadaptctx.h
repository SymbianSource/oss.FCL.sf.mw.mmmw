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

#include "XAAdaptationContextBase.h"

/* TYPEDEFS */

typedef struct XAOutputMixAdaptationCtx_ XAOutputMixAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAOutputMixAdapt_Create();
XAresult XAOutputMixAdapt_PostInit(XAAdaptationBaseCtx* bCtx);
void XAOutputMixAdapt_Destroy(XAAdaptationBaseCtx* bCtx);

#endif /* XAOUTPUTMIXADAPTCTX_H */
