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

#ifndef XARECORDITFADAPTATION_H
#define XARECORDITFADAPTATION_H

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

#ifdef _DEBUG
/*parse state names for debug prints*/
static const char* recordstatenames[3] =
{
    "XA_RECORDSTATE_STOPPED",
    "XA_RECORDSTATE_PAUSED",
    "XA_RECORDSTATE_RECORDING",
};
#define RECORDSTATENAME(i) ((i>0&&i<4)?recordstatenames[i-1]:"INVALID")
#endif /*_DEBUG*/

/* FUNCTIONS */

XAresult XARecordItfAdapt_SetRecordState(XAAdaptationBaseCtx *ctx, XAuint32 state );
XAresult XARecordItfAdapt_SetDurationLimit( XAAdaptationBaseCtx *ctx, XAmillisecond msec );
XAresult XARecordItfAdapt_GetPosition( XAAdaptationBaseCtx *ctx, XAmillisecond *pMsec );
XAresult XARecordItfAdapt_EnablePositionTracking(XAAdaptationBaseCtx *ctx, XAboolean enable);
XAresult XARecordItfAdapt_GetRecordState(XAAdaptationBaseCtx *bCtx, XAuint32 *state);

#endif /* XARECORDITFADAPTATION_H */
