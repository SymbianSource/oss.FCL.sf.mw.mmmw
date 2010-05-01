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

#ifndef XAMETADATAADAPTCTX_H
#define XAMETADATAADAPTCTX_H

#include "xamediaplayeradaptctx.h"
/* TYPEDEFS */

/* use media player context with NULL sinks
 * (no specific MDE context needed, because in GStreamer,
 *  playback resources are needed for getting stream tags)
 */
typedef struct XAMediaPlayerAdaptationCtx_ XAMetadataAdaptationCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAMetadataAdaptCtx_Create(XADataSource* pDataSource);
XAresult XAMetadataAdaptCtx_PostInit(XAAdaptationGstCtx* bCtx);
void XAMetadataAdaptCtx_Destroy(XAAdaptationGstCtx* bCtx);


#endif /* XAMETADATAADAPTCTX_H */
