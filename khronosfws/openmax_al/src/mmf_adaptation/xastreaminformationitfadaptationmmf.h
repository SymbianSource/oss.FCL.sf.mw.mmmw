/**
 * src\adaptation\XAStreamInformationItfAdaptation.h
 * Part of OpenMAX AL Implementation
 * Copyright (c) 2009 Nokia Inc.
 *
 * This file contains implementation of XAStreamInformationItfAdaptation
 **/
#ifndef XASTREAMINFORMATIONITFADAPTATIONMMF_H
#define XASTREAMINFORMATIONITFADAPTATIONMMF_H

#include "openmaxalwrapper.h"
#include "xaadptbasectx.h"
/* MACROS */

/* FUNCTIONS */
XAresult XAStreamInformationItfAdaptMMF_QueryMediaContainerInformation(
    XAAdaptationBaseCtx *adaptCtx,
    XAuint32 *containerType,
    XAmillisecond *mediaDuration,
    XAuint32 *numStreams);

XAresult XAStreamInformationItfAdaptMMF_QueryStreamType(
                XAAdaptationBaseCtx *adaptCtx,
                XAuint32 streamIndex, 
                XAuint32 *domain);

XAresult XAStreamInformationItfAdaptMMF_QueryStreamInformation(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 streamIndex,
                void * info);

XAresult XAStreamInformationItfAdaptMMF_QueryStreamName(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 streamIndex,
                XAuint16 * pNameSize,
                XAchar * pName);

XAresult XAStreamInformationItfAdaptMMF_QueryActiveStreams(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 *numStreams,
                XAboolean *activeStreams);

XAresult XAStreamInformationItfAdaptMMF_SetActiveStream(
                XAAdaptationBaseCtx *adaptCtx, 
                XAuint32 streamNum,
                XAboolean active, 
                XAboolean commitNow);

#endif /* XASTREAMINFORMATIONITFADAPTATION_H */
