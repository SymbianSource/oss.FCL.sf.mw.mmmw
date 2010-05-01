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

#ifndef XAVIDEOPOSTPROSESSINGITFADAPTATION_H
#define XAVIDEOPOSTPROSESSINGITFADAPTATION_H

#include "xaadaptationgst.h"
#include "xathreadsafety.h"

/* MACROS */
#define BLACK_BG_COLOR_MASK  0x00FFFFFF
#define GREEN_BG_COLOR_MASK  0x0000FF00
#define BLUE_BG_COLOR_MASK   0x000000FF
#define ALPHA_VALUE_MASK     0x000000FF

#define TEST_VIDEO_WIDTH     640
#define TEST_VIDEO_HEIGHT    480

#define ROTATION_RATIO       360000
#define ROTATION_RATIO_NEG   -360000

/* FUNCTIONS */
XAresult XAVideoPostProcessingItfAdapt_IsArbitraryRotationSupported(XAAdaptationGstCtx *bCtx,
                                                                    XAboolean *pSupported);

XAresult XAVideoPostProcessingItfAdapt_Commit(XAAdaptationGstCtx *bCtx,
                                              XAmillidegree rotation,
                                              XAuint32 scaleOptions,
                                              XAuint32 backgroundColor,
                                              XAuint32 renderingHints,
                                              const XARectangle *pSrcRect,
                                              const XARectangle *pDestRect,
                                              XAuint32 mirror,
                                              XAboolean isMirror,
                                              XAboolean isRotate,
                                              XAboolean isDestRect,
                                              XAboolean isSrcRect,
                                              XAboolean isScaleOptions
                                              );

#endif /* XAVIDEOPOSTPROSESSINGITFADAPTATION_H */
