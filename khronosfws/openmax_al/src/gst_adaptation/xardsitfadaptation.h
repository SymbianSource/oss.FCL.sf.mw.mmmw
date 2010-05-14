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

#ifndef XARDSITFFADAPTATION_H
#define XARDSOITFFADAPTATION_H

#include "xaadaptationgst.h"
#include "xadebug.h"

/* MACROS */

/* FUNCTIONS */

XAresult XARDSItfAdapt_QueryRDSSignal(XAAdaptationGstCtx *bCtx, XAboolean * isSignal);

XAresult XARDSItfAdapt_GetProgrammeServiceName(XAAdaptationGstCtx *bCtx, XAchar * ps);

XAresult XARDSItfAdapt_GetRadioText(XAAdaptationGstCtx *bCtx, XAchar * rt);

XAresult XARDSItfAdapt_GetRadioTextPlus(XAAdaptationGstCtx *bCtx,
                                       XAuint8 contentType,
                                       XAchar * informationElement,
                                       XAchar * descriptor,
                                       XAuint8 * descriptorContentType);

XAresult XARDSItfAdapt_GetProgrammeType(XAAdaptationGstCtx *bCtx, XAuint32 * pty);

XAresult XARDSItfAdapt_GetProgrammeTypeString(XAAdaptationGstCtx *bCtx,
                                             XAboolean isLengthMax16,
                                             XAchar * pty);

XAresult XARDSItfAdapt_GetProgrammeIdentificationCode(XAAdaptationGstCtx *bCtx, XAint16 * pi);

XAresult XARDSItfAdapt_GetClockTime(XAAdaptationGstCtx *bCtx, XAtime * dateAndTime);

XAresult XARDSItfAdapt_GetTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean * ta);

XAresult XARDSItfAdapt_GetTrafficProgramme(XAAdaptationGstCtx *bCtx, XAboolean * tp);

XAresult XARDSItfAdapt_SeekByProgrammeType(XAAdaptationGstCtx *bCtx,
                                          XAuint32 pty,
                                          XAboolean upwards);

XAresult XARDSItfAdapt_SeekTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean upwards);

XAresult XARDSItfAdapt_SeekTrafficProgramme(XAAdaptationGstCtx *bCtx, XAboolean upwards);

XAresult XARDSItfAdapt_SetAutomaticSwitching(XAAdaptationGstCtx *bCtx, XAboolean automatic);

XAresult XARDSItfAdapt_GetAutomaticSwitching(XAAdaptationGstCtx *bCtx, XAboolean * automatic);

XAresult XARDSItfAdapt_SetAutomaticTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean automatic);

XAresult XARDSItfAdapt_GetAutomaticTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean * automatic);

XAresult XARDSItfAdapt_GetODAGroup(XAAdaptationGstCtx *bCtx, XAint16 AID);

XAresult XARDSItfAdapt_SubscribeODAGroup(XAAdaptationGstCtx *bCtx,
                                        XAint16 group,
                                        XAboolean useErrorCorrection);

XAresult XARDSItfAdapt_UnsubscribeODAGroup(XAAdaptationGstCtx *bCtx, XAint16 group);

XAresult XARDSItfAdapt_ListODAGroupSubscriptions(XAAdaptationGstCtx *bCtx,
                                                XAint16* pGroups,
                                                XAuint32* pLength);

void XARDSItfAdapt_Free(XAAdaptationBaseCtx *bCtx);

#endif /* XARDSITFADAPTATION_H */
