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

#ifndef XARECORDITFADAPTATIONMMF_H
#define XARECORDITFADAPTATIONMMF_H

#include "xaadaptationcontextbasemmf.h"
#include "xadebug.h"
#include <glib.h>


#ifdef _DEBUG
/*parse state names for debug prints*/
static const char* recordstatenamesmmf[3] =
{
    "XA_RECORDSTATE_STOPPED",
    "XA_RECORDSTATE_PAUSED",
    "XA_RECORDSTATE_RECORDING",
};
#define RECORDSTATENAME(i) ((i>0&&i<4)?recordstatenamesmmf[i-1]:"INVALID")
#endif /*_DEBUG*/

/*forward declaration of position updater callback*/
gboolean XARecordItfAdaptMMF_PositionUpdate(gpointer ctx, XAuint64 position);


/* FUNCTIONS */

XAresult XARecordItfAdaptMMF_SetRecordState(XAAdaptationBaseMMFCtx *ctx, XAuint32 state );
XAresult XARecordItfAdaptMMF_GetPosition( XAAdaptationBaseMMFCtx *ctx, XAmillisecond *pMsec );
XAresult XARecordItfAdaptMMF_GetRecordState(XAAdaptationBaseMMFCtx *bCtx, XAuint32 *state);
XAresult XARecordItfAdaptMMF_EnablePositionTracking(XAAdaptationBaseMMFCtx *ctx, XAboolean enable);
XAresult XARecordItfAdaptMMF_SetPositionUpdatePeriod(XAAdaptationBaseMMFCtx *ctx, XAmillisecond pMsec);

#endif /* XARECORDITFADAPTATIONMMF_H */
