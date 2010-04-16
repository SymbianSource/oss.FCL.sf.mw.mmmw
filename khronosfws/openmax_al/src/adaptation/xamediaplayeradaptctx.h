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

#ifndef XAMEDIAPLAYERADAPTCTX_H
#define XAMEDIAPLAYERADAPTCTX_H

#include "XAAdaptationContextBase.h"

/* TYPEDEFS */

typedef struct XAMediaPlayerAdaptationCtx_ XAMediaPlayerAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAMediaPlayerAdapt_Create( XADataSource *pDataSrc, XADataSource *pBankSrc,
                                                       XADataSink *pAudioSnk, XADataSink *pImageVideoSnk,
                                                       XADataSink *pVibra, XADataSink *pLEDArray);
XAresult XAMediaPlayerAdapt_PostInit( XAAdaptationBaseCtx* bCtx );
void XAMediaPlayerAdapt_Destroy( XAAdaptationBaseCtx* bCtx );

XAresult XAMediaPlayerAdapt_InitContentPipeSrc(XAMediaPlayerAdaptationCtx* ctx);
#endif /*XAMEDIAPLAYERADAPTCTX_H*/

