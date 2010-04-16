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

#ifndef XAVOLUMEITFADAPTATION_H
#define XAVOLUMEITFADAPTATION_H

#include "XAAdaptationContextBase.h"
#include "XADebug.h"
#include "XAThreadSafety.h"

/* MACROS */
#define STEREO_POSITION_RATIO    1000
#define MAX_SUPPORT_VOLUME_LEVEL 9000
#define MIN_SUPPORT_VOLUME_LEVEL -1000
#define VOLUME_LEVEL_RATIO       1000

/* FUNCTIONS */

XAresult XAVolumeItfAdapt_SetVolumeLevel(XAAdaptationBaseCtx *ctx,
                                         XAmillibel level);

XAresult XAVolumeItfAdapt_GetMaxVolumeLevel(XAAdaptationBaseCtx *ctx,
                                            XAmillibel *pMaxLevel);

XAresult XAVolumeItfAdapt_SetMute(XAAdaptationBaseCtx *ctx,  XAboolean mute);

XAresult XAVolumeItfAdapt_EnableStereoPosition(XAAdaptationBaseCtx *ctx,
                                               XAboolean enable);

XAresult XAVolumeItfAdapt_SetStereoPosition(XAAdaptationBaseCtx *ctx,
                                            XApermille stereoPosition);

XAresult XAVolumeItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adaptCtx);
XAresult XAVolumeItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx);

#endif /* XAVOLUMEITFADAPTATION_H */
