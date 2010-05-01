/**
 * src\adaptation\XAStreamInformationItfAdaptation.h
 * Part of OpenMAX AL Implementation
 * Copyright (c) 2009 Nokia Inc.
 *
 * This file contains implementation of XAStreamInformationItfAdaptation
 **/
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
