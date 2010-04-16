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

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

/* FUNCTIONS */

XAresult XASeekItfAdapt_SetPosition(XAAdaptationBaseCtx *ctx, XAmillisecond pos,
                                        XAuint32 seekMode);

XAresult XASeekItfAdapt_SetLoop(XAAdaptationBaseCtx *ctx, XAboolean loopEnable,
                                XAmillisecond startPos, XAmillisecond endPos);
#endif /* XASEEKITFADAPTATION_H */
