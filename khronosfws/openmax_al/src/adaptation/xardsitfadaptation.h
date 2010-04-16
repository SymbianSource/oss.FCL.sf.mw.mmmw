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

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

/* MACROS */

/* FUNCTIONS */

XAresult XARDSItfAdapt_QueryRDSSignal(XAAdaptationBaseCtx *bCtx, XAboolean * isSignal);

XAresult XARDSItfAdapt_GetProgrammeServiceName(XAAdaptationBaseCtx *bCtx, XAchar * ps);

XAresult XARDSItfAdapt_GetRadioText(XAAdaptationBaseCtx *bCtx, XAchar * rt);

XAresult XARDSItfAdapt_GetRadioTextPlus(XAAdaptationBaseCtx *bCtx,
                                       XAuint8 contentType,
                                       XAchar * informationElement,
                                       XAchar * descriptor,
                                       XAuint8 * descriptorContentType);

XAresult XARDSItfAdapt_GetProgrammeType(XAAdaptationBaseCtx *bCtx, XAuint32 * pty);

XAresult XARDSItfAdapt_GetProgrammeTypeString(XAAdaptationBaseCtx *bCtx,
                                             XAboolean isLengthMax16,
                                             XAchar * pty);

XAresult XARDSItfAdapt_GetProgrammeIdentificationCode(XAAdaptationBaseCtx *bCtx, XAint16 * pi);

XAresult XARDSItfAdapt_GetClockTime(XAAdaptationBaseCtx *bCtx, XAtime * dateAndTime);

XAresult XARDSItfAdapt_GetTrafficAnnouncement(XAAdaptationBaseCtx *bCtx, XAboolean * ta);

XAresult XARDSItfAdapt_GetTrafficProgramme(XAAdaptationBaseCtx *bCtx, XAboolean * tp);

XAresult XARDSItfAdapt_SeekByProgrammeType(XAAdaptationBaseCtx *bCtx,
                                          XAuint32 pty,
                                          XAboolean upwards);

XAresult XARDSItfAdapt_SeekTrafficAnnouncement(XAAdaptationBaseCtx *bCtx, XAboolean upwards);

XAresult XARDSItfAdapt_SeekTrafficProgramme(XAAdaptationBaseCtx *bCtx, XAboolean upwards);

XAresult XARDSItfAdapt_SetAutomaticSwitching(XAAdaptationBaseCtx *bCtx, XAboolean automatic);

XAresult XARDSItfAdapt_GetAutomaticSwitching(XAAdaptationBaseCtx *bCtx, XAboolean * automatic);

XAresult XARDSItfAdapt_SetAutomaticTrafficAnnouncement(XAAdaptationBaseCtx *bCtx, XAboolean automatic);

XAresult XARDSItfAdapt_GetAutomaticTrafficAnnouncement(XAAdaptationBaseCtx *bCtx, XAboolean * automatic);

XAresult XARDSItfAdapt_GetODAGroup(XAAdaptationBaseCtx *bCtx, XAint16 AID);

XAresult XARDSItfAdapt_SubscribeODAGroup(XAAdaptationBaseCtx *bCtx,
                                        XAint16 group,
                                        XAboolean useErrorCorrection);

XAresult XARDSItfAdapt_UnsubscribeODAGroup(XAAdaptationBaseCtx *bCtx, XAint16 group);

XAresult XARDSItfAdapt_ListODAGroupSubscriptions(XAAdaptationBaseCtx *bCtx,
                                                XAint16* pGroups,
                                                XAuint32* pLength);

void XARDSItfAdapt_Free(XAAdaptationBaseCtx *bCtx);

#endif /* XARDSITFADAPTATION_H */
