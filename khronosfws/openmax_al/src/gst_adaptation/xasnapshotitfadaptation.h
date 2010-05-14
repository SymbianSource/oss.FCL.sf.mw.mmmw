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

#include "xaadaptationgst.h"


/* DEFINES*/

/* MACROS */

/* FUNCTIONS */

XAresult XASnapshotItfAdaptation_InitiateSnapshot(XAAdaptationGstCtx* ctx,
                                            XAuint32 numberOfPictures,
                                            XAuint32 fps,
                                            XAboolean freezeViewFinder,
                                            XADataSink* sink);

XAresult XASnapshotItfAdaptation_TakeSnapshot(XAAdaptationGstCtx* ctx);

XAresult XASnapshotItfAdaptation_CancelSnapshot(XAAdaptationGstCtx* ctx);

XAresult XASnapshotItfAdaptation_GetMaxPicsPerBurst(XAAdaptationGstCtx* ctx,
                                              XAuint32 *maxNumberOfPictures);

XAresult XASnapshotItfAdaptation_GetBurstFPSRange(XAAdaptationGstCtx* ctx,
                                            XAuint32 *minFPS,
                                            XAuint32 *maxFPS);

XAresult XASnapshotItfAdaptation_SetShutterFeedback(XAAdaptationGstCtx* ctx,
                                              XAboolean enabled);
#endif /* XSNAPSHOTITFADAPTATION_H */
