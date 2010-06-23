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
 * Description: StreamInformation Itf Adaptation header
 *
 */
 
#ifndef XASTREAMINFORMATIONITFADAPTATION_H
#define XASTREAMINFORMATIONITFADAPTATION_H

#include "openmaxalwrapper.h"
#include "xaadptbasectx.h"
/* MACROS */

/* FUNCTIONS */
XAresult XAStreamInformationItfAdapt_QueryMediaContainerInformation(
    XAAdaptationBaseCtx *adaptCtx,
    XAuint32 *containerType,
    XAmillisecond *mediaDuration,
    XAuint32 *numStreams);

XAresult XAStreamInformationItfAdapt_QueryStreamType(
                XAAdaptationBaseCtx *adaptCtx,
                XAuint32 streamIndex, 
                XAuint32 *domain);

XAresult XAStreamInformationItfAdapt_QueryStreamInformation(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 streamIndex,
                void * info);

XAresult XAStreamInformationItfAdapt_QueryStreamName(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 streamIndex,
                XAuint16 * pNameSize,
                XAchar * pName);

XAresult XAStreamInformationItfAdapt_QueryActiveStreams(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 *numStreams,
                XAboolean *activeStreams);

XAresult XAStreamInformationItfAdapt_SetActiveStream(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 streamNum,
                XAboolean active, 
                XAboolean commitNow);


#endif /* XASTREAMINFORMATIONITFADAPTATION_H */
