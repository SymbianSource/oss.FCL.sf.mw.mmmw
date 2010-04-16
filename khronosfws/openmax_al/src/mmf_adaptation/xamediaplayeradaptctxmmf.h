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

#ifndef XAMEDIAPLAYERADAPTCTXMMF_H
#define XAMEDIAPLAYERADAPTCTXMMF_H

#include "xaadaptationcontextbasemmf.h"

/* TYPEDEFS */

typedef struct XAMediaPlayerAdaptationMMFCtx_ XAMediaPlayerAdaptationMMFCtx;

/* FUNCTIONS */
XAAdaptationBaseMMFCtx* XAMediaPlayerAdaptMMF_Create( XADataSource *pDataSrc, XADataSource *pBankSrc,
                                                       XADataSink *pAudioSnk, XADataSink *pImageVideoSnk,
                                                       XADataSink *pVibra, XADataSink *pLEDArray);
XAresult XAMediaPlayerAdaptMMF_PostInit( XAAdaptationBaseMMFCtx* bCtx );
void XAMediaPlayerAdaptMMF_Destroy( XAAdaptationBaseMMFCtx* bCtx );

//XAresult XAMediaPlayerAdapt_InitContentPipeSrc(XAMediaPlayerAdaptationCtx* ctx);
#endif /*XAMEDIAPLAYERADAPTCTXMMF_H*/

