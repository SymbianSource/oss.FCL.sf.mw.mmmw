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

#ifndef XARADIOITFFADAPTATION_H
#define XARADIOITFFADAPTATION_H

#include "xaadaptationgst.h"
#include "xadebug.h"

/* MACROS */
#define RADIO_DEFAULT_STEREO_MODE XA_STEREOMODE_AUTO
#define RADIO_NUM_OF_PRESETS 10

extern const char* RadioPresetDefaultName;

/* FUNCTIONS */
XAresult XARadioItfAdapt_SetFreqRange(XAAdaptationGstCtx *bCtx, XAuint8 range);

XAresult XARadioItfAdapt_GetFreqRange(XAAdaptationGstCtx *bCtx,
                                      XAuint8 * pFreqRange);

XAresult XARadioItfAdapt_IsFreqRangeSupported(XAAdaptationGstCtx *bCtx,
                                             XAuint8 range,
                                             XAboolean * pSupported);

XAresult XARadioItfAdapt_GetFreqRangeProperties(XAAdaptationGstCtx *bCtx,
                                               XAuint8 range,
                                               XAuint32 * pMinFreq,
                                               XAuint32 * pMaxFreq,
                                               XAuint32 * pFreqInterval);

XAresult XARadioItfAdapt_SetFrequency(XAAdaptationGstCtx *bCtx, XAuint32 freq);

XAresult XARadioItfAdapt_GetFrequency(XAAdaptationGstCtx *bCtx,
                                      XAuint32 * pFrequency);

XAresult XARadioItfAdapt_CancelSetFrequency(XAAdaptationGstCtx *bCtx);

XAresult XARadioItfAdapt_SetSquelch(XAAdaptationGstCtx *bCtx, XAboolean squelch);

XAresult XARadioItfAdapt_SetStereoMode(XAAdaptationGstCtx *bCtx, XAuint32 mode);

XAresult XARadioItfAdapt_GetSignalStrength(XAAdaptationGstCtx *bCtx, XAuint32 * pStrength);

XAresult XARadioItfAdapt_Seek(XAAdaptationGstCtx *bCtx, XAboolean upwards);

XAresult XARadioItfAdapt_StopSeeking(XAAdaptationGstCtx *bCtx);

void XARadioItfAdapt_Free(XAAdaptationBaseCtx *bCtx);

#endif /* XARADIOITFADAPTATION_H */
