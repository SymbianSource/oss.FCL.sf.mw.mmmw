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

#ifndef XAEQUALIZERITFADAPTATION_H
#define XAEQUALIZERITFADAPTATION_H

#include "xaadaptationgst.h"
#include "xathreadsafety.h"

/* MACROS */
#define EQUALIZER_NUM_OF_BANDS 10
#define EQUALIZER_DEFAULT_BAND_LEVEL 0

/* FUNCTIONS */
XAresult XAEqualizerItfAdapt_GetBandLevelRange(XAAdaptationGstCtx *bCtx,
                                               XAmillibel *pMin,
                                               XAmillibel *pMax);

XAresult XAEqualizerItfAdapt_SetBandLevel(XAAdaptationGstCtx *bCtx,
                                          XAuint16 band,
                                          XAmillibel level);

XAresult XAEqualizerItfAdapt_GetCenterFreq(XAAdaptationGstCtx *bCtx,
                                           XAuint16 band,
                                           XAmilliHertz *pCenter);

XAresult XAEqualizerItfAdapt_GetBandFreqRange(XAAdaptationGstCtx *bCtx,
                                              XAuint16 band,
                                              XAmilliHertz *pMin,
                                              XAmilliHertz *pMax);

XAresult XAEqualizerItfAdapt_GetBand(XAAdaptationGstCtx *bCtx,
                                     XAmilliHertz frequency,
                                     XAuint16 *pBand);

XAresult XAEqualizerItfAdapt_GetDefaultBandLevel(XAAdaptationGstCtx *bCtx,
                                                 XAuint16 index,
                                                 XAint16 *pLevel);

#endif /* XAEQUALIZERITFADAPTATION_H */
