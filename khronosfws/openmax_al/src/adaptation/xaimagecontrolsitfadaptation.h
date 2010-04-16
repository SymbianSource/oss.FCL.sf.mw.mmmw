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

#ifndef XAIMAGECONTROLSITFADAPTATION_H
#define XAIMAGECONTROLSITFADAPTATION_H

#include "XAAdaptationContextBase.h"
#include "XADebug.h"
#include "XAThreadSafety.h"
/* MACROS */
#define MIN_GAMMA_VALUE         10
#define MAX_GAMMA_VALUE         10000
#define SCALE_VALUE_BRIGHTNESS  50
#define SCALE_VALUE_CONTRAST    100
#define SCALE_VALUE_GAMMA       1000

/* FUNCTIONS */
XAresult XAImageControlsItfAdapt_SetBrightness(XAAdaptationBaseCtx *bCtx,
                                               XAuint32 brightness);

XAresult XAImageControlsItfAdapt_SetContrast(XAAdaptationBaseCtx *bCtx,
                                             XAint32 contrast);

XAresult XAImageControlsItfAdapt_SetGamma(XAAdaptationBaseCtx *bCtx,
                                          XApermille gamma);

XAresult XAImageControlsItfAdapt_GetSupportedGammaSettings(XAAdaptationBaseCtx *bCtx,
                                                           XApermille *pMinValue,
                                                           XApermille *pMaxValue,
                                                           XAuint32 *pNumSettings,
                                                           XApermille **ppSettings);
XAresult XAImageControlsItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adaptCtx);
XAresult XAImageControlsItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx);
XAresult XAImageControlsItfAdapt_HandleImageType(XAAdaptationBaseCtx *bCtx);
#endif /* XAIMAGECONTROLSITFADAPTATION_H */
