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

#ifndef XAPLAYITFADAPTATIONMMF_H
#define XAPLAYITFADAPTATIONMMF_H

#include "xaadaptationcontextbasemmf.h"
#include "xadebug.h"

#ifdef _DEBUG
/*parse state names for debug prints*/
static const char* playstatenamemmf[3] =
{
    "XA_PLAYSTATE_STOPPED",
    "XA_PLAYSTATE_PAUSED",
    "XA_PLAYSTATE_PLAYING"
};
#define PLAYSTATENAME(i) (const char*)((i>0&&i<4)?playstatenamesmmf[i-1]:"INVALID")
#endif /*_DEBUG*/


/* FUNCTIONS */

XAresult XAPlayItfAdaptMMF_SetPlayState(XAAdaptationBaseMMFCtx *bCtx, XAuint32 state);
XAresult XAPlayItfAdaptMMF_GetDuration(XAAdaptationBaseMMFCtx *bCtx, XAmillisecond *pMsec);
XAresult XAPlayItfAdaptMMF_GetPosition(XAAdaptationBaseMMFCtx *bCtx, XAmillisecond *pMsec);
XAresult XAPlayItfAdaptMMF_EnablePositionTracking(XAAdaptationBaseMMFCtx *bCtx, XAboolean enable);


#endif /* XAPLAYITFADAPTATIONMMF_H */
