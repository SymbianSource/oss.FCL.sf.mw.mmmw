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

#ifndef XAMEDIARECORDERADAPTCTXMMF_H_
#define XAMEDIARECORDERADAPTCTXMMF_H_

#include "xaadaptationcontextbasemmf.h"


/* TYPEDEFS */
#define XA_RECMODE_STREAM 1
#define XA_RECMODE_STILL 2

typedef struct XAMediaRecorderAdaptationMMFCtx_ XAMediaRecorderAdaptationMMFCtx;

/* FUNCTIONS */
XAAdaptationBaseMMFCtx* XAMediaRecorderAdaptMMF_Create(  XADataSource* pAudioSrc, XADataSource* pImageVideoSrc, XADataSink* pDataSnk, XAuint8 recModes );
XAresult XAMediaRecorderAdaptMMF_PostInit( XAAdaptationBaseMMFCtx* bCtx );
void XAMediaRecorderAdaptMMF_Destroy( XAAdaptationBaseMMFCtx* bCtx );


#endif /* XAMEDIARECORDERADAPTCTXMMF_H_ */
