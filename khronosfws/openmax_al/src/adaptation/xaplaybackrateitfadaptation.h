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

#ifndef XADYNAMICSOURCEITFADAPTATION_H
#define XADYNAMICSOURCEITFADAPTATION_H

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

/* FUNCTIONS */

XAresult XAPlaybackRateItfAdapt_SetRate(XAAdaptationBaseCtx *bCtx, XApermille rate);

XAresult XAPlaybackRateItfAdapt_SetPropertyConstraints(XAAdaptationBaseCtx *bCtx,
                                                        XAuint32 constraints);

XAresult XAPlaybackRateItfAdapt_GetProperties(XAAdaptationBaseCtx *bCtx,
                                               XAuint32 *pProperties);

XAresult XAPlaybackRateItfAdapt_GetCapabilitiesOfRate(XAAdaptationBaseCtx *bCtx,
                                                       XApermille rate,
                                                       XAuint32 *pCapabilities);

XAresult XAPlaybackRateItfAdapt_GetRateRange(XAAdaptationBaseCtx *bCtx,
                                              XAuint8 index,
                                              XApermille *pMinRate,
                                              XApermille *pMaxRate,
                                              XApermille *pStepSize,
                                              XAuint32 *pCapabilities);

#endif /* XADYNAMICSOURCEITFADAPTATION_H */
