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

#include "xaadaptationgst.h"

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

XAresult XAPlayItfAdaptGST_SetPlayState(XAAdaptationBaseCtx *bCtx, XAuint32 state);
XAresult XAPlayItfAdaptGST_GetDuration(XAAdaptationGstCtx *bCtx, XAmillisecond *pMsec);
XAresult XAPlayItfAdaptGST_GetPosition(XAAdaptationGstCtx *bCtx, XAmillisecond *pMsec);
XAresult XAPlayItfAdapt_EnablePositionTracking(XAAdaptationGstCtx *bCtx, XAboolean enable);


#endif /* XAPLAYITFADAPTATION_H */
