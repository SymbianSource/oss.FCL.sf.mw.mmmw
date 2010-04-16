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

#ifndef XSNAPSHOTITFADAPTATION_H
#define XSNAPSHOTITFADAPTATION_H

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

/* DEFINES*/

/* MACROS */

/* FUNCTIONS */

XAresult XASnapshotItfAdaptation_InitiateSnapshot(XAAdaptationBaseCtx* ctx,
                                            XAuint32 numberOfPictures,
                                            XAuint32 fps,
                                            XAboolean freezeViewFinder,
                                            XADataSink* sink);

XAresult XASnapshotItfAdaptation_TakeSnapshot(XAAdaptationBaseCtx* ctx);

XAresult XASnapshotItfAdaptation_CancelSnapshot(XAAdaptationBaseCtx* ctx);

XAresult XASnapshotItfAdaptation_GetMaxPicsPerBurst(XAAdaptationBaseCtx* ctx,
                                              XAuint32 *maxNumberOfPictures);

XAresult XASnapshotItfAdaptation_GetBurstFPSRange(XAAdaptationBaseCtx* ctx,
                                            XAuint32 *minFPS,
                                            XAuint32 *maxFPS);

XAresult XASnapshotItfAdaptation_SetShutterFeedback(XAAdaptationBaseCtx* ctx,
                                              XAboolean enabled);
#endif /* XSNAPSHOTITFADAPTATION_H */
