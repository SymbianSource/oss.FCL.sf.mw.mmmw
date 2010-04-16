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

#ifndef XAMEDIARECORDERADAPTCTX_H_
#define XAMEDIARECORDERADAPTCTX_H_

#include "XAAdaptationContextBase.h"


/* TYPEDEFS */
#define XA_RECMODE_STREAM 1
#define XA_RECMODE_STILL 2

typedef struct XAMediaRecorderAdaptationCtx_ XAMediaRecorderAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAMediaRecorderAdapt_Create(  XADataSource* pAudioSrc, XADataSource* pImageVideoSrc, XADataSink* pDataSnk, XAuint8 recModes );
XAresult XAMediaRecorderAdapt_PostInit( XAAdaptationBaseCtx* bCtx );
void XAMediaRecorderAdapt_Destroy( XAAdaptationBaseCtx* bCtx );
XAresult XAMediaRecorderAdapt_InitContentPipeSink(XAMediaRecorderAdaptationCtx* ctx);
void*  XAMediaRecorderAdapt_RecordEventThr(void* ctx);

#endif /* XAMEDIARECORDERADAPTCTX_H_ */
