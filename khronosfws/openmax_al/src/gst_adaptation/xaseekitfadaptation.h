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

#ifndef XASEEKITFADAPTATION_H
#define XASEEKITFADAPTATION_H

#include "xaadaptationgst.h"
#include "xadebug.h"

/* FUNCTIONS */

XAresult XASeekItfAdapt_SetPosition(XAAdaptationGstCtx *ctx, XAmillisecond pos,
                                        XAuint32 seekMode);

XAresult XASeekItfAdapt_SetLoop(XAAdaptationGstCtx *ctx, XAboolean loopEnable,
                                XAmillisecond startPos, XAmillisecond endPos);
#endif /* XASEEKITFADAPTATION_H */
