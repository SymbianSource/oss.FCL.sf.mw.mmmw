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

#ifndef XAPLAYITFADAPTATION_H
#define XAPLAYITFADAPTATION_H

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

#ifdef _DEBUG
/*parse state names for debug prints*/
static const char* playstatenames[3] =
{
    "XA_PLAYSTATE_STOPPED",
    "XA_PLAYSTATE_PAUSED",
    "XA_PLAYSTATE_PLAYING"
};
#define PLAYSTATENAME(i) (const char*)((i>0&&i<4)?playstatenames[i-1]:"INVALID")
#endif /*_DEBUG*/


/* FUNCTIONS */

XAresult XAPlayItfAdapt_SetPlayState(XAAdaptationBaseCtx *bCtx, XAuint32 state);
XAresult XAPlayItfAdapt_GetDuration(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec);
XAresult XAPlayItfAdapt_GetPosition(XAAdaptationBaseCtx *bCtx, XAmillisecond *pMsec);
XAresult XAPlayItfAdapt_EnablePositionTracking(XAAdaptationBaseCtx *bCtx, XAboolean enable);


#endif /* XAPLAYITFADAPTATION_H */
