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

#ifndef XAIMAGEEFFECTSITFADAPTATION_H
#define XAIMAGEEFFECTSITFADAPTATION_H

#include "xaadaptationgst.h"
#include "xathreadsafety.h"

/* MACROS */
#define NUM_SUPPORTED_EFFECTS   7
#define NO_IMAGEEFFECT          0
#define GST_NO_IMAGEEFFECT      1
#define IMAGEEFFECT_MONOCHROME  0
#define IMAGEEFFECT_NEGATIVE    0.2
#define IMAGEEFFECT_SEPIA       0.3
#define IMAGEEFFECT_EMBOSS      1.4
#define IMAGEEFFECT_PAINTBRUSH  1.5
#define IMAGEEFFECT_SOLARIZE    1.6
#define IMAGEEFFECT_CARTOON     2

/* FUNCTIONS */
XAresult XAImageEffectsItfAdapt_QuerySupportedImageEffects(XAAdaptationGstCtx *bCtx,
                                                           XAuint32 index,
                                                           XAuint32 *pImageEffectId);

XAresult XAImageEffectsItfAdapt_EnableImageEffect(XAAdaptationGstCtx *bCtx,
                                                  XAuint32 imageEffectID);

XAresult XAImageEffectsItfAdapt_DisableImageEffect(XAAdaptationGstCtx *bCtx,
                                                   XAuint32 imageEffectID);

XAresult XAImageEffectsItfAdapt_IsImageEffectEnabled(XAAdaptationGstCtx *bCtx,
                                                     XAuint32 imageEffectID,
                                                     XAboolean *pEnabled);

XAresult XAImageEffectsItfAdapt_HandleImageType(XAAdaptationGstCtx *bCtx);
#endif /* XAIMAGEEFFECTSITFADAPTATION_H */
