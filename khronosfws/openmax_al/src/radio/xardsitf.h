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

#ifndef XARDSITF_H
#define XARDSITF_H

#include "xaadptbasectx.h"
/** MACROS **/
/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAEqualizerItf implementation */
typedef struct XARDSItfImpl_
{
    /* parent interface */
    struct XARDSItf_ itf;
    /* pointer to self */
    struct XARDSItfImpl_* self;

    /* variables */

    xaRDSCallback           rdsCallback;
    void                   *rdsContext;
    XARDSItf               rdsCbPtrToSelf;

    xaGetODAGroupCallback   odaGroupCallback;
    void                   *odaGroupContext;
    XARDSItf               odaGroupCbPtrToSelf;

    xaNewODADataCallback    odaDataCallback;
    void                   *odaDataContext;
    XARDSItf               odaDataCbPtrToSelf;

    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
} XARDSItfImpl;

/** METHODS **/

/* Base interface XARDSItf implementation */

XAresult XARDSItfImpl_QueryRDSSignal(XARDSItf self, XAboolean * isSignal);

XAresult XARDSItfImpl_GetProgrammeServiceName(XARDSItf self, XAchar * ps);

XAresult XARDSItfImpl_GetRadioText(XARDSItf self, XAchar * rt);

XAresult XARDSItfImpl_GetRadioTextPlus(XARDSItf self,
                                       XAuint8 contentType,
                                       XAchar * informationElement,
                                       XAchar * descriptor,
                                       XAuint8 * descriptorContentType);

XAresult XARDSItfImpl_GetProgrammeType(XARDSItf self, XAuint32 * pty);

XAresult XARDSItfImpl_GetProgrammeTypeString(XARDSItf self,
                                             XAboolean isLengthMax16,
                                             XAchar * pty);

XAresult XARDSItfImpl_GetProgrammeIdentificationCode(XARDSItf self, XAint16 * pi);

XAresult XARDSItfImpl_GetClockTime(XARDSItf self, XAtime * dateAndTime);

XAresult XARDSItfImpl_GetTrafficAnnouncement(XARDSItf self, XAboolean * ta);

XAresult XARDSItfImpl_GetTrafficProgramme(XARDSItf self, XAboolean * tp);

XAresult XARDSItfImpl_SeekByProgrammeType(XARDSItf self,
                                          XAuint32 pty,
                                          XAboolean upwards);

XAresult XARDSItfImpl_SeekTrafficAnnouncement(XARDSItf self, XAboolean upwards);

XAresult XARDSItfImpl_SeekTrafficProgramme(XARDSItf self, XAboolean upwards);

XAresult XARDSItfImpl_SetAutomaticSwitching(XARDSItf self, XAboolean automatic);

XAresult XARDSItfImpl_GetAutomaticSwitching(XARDSItf self, XAboolean * automatic);

XAresult XARDSItfImpl_SetAutomaticTrafficAnnouncement(XARDSItf self, XAboolean automatic);

XAresult XARDSItfImpl_GetAutomaticTrafficAnnouncement(XARDSItf self, XAboolean * automatic);

XAresult XARDSItfImpl_GetODAGroup(XARDSItf self,
                                  XAuint16 AID,
                                  xaGetODAGroupCallback callback,
                                  void * pContext);

XAresult XARDSItfImpl_SubscribeODAGroup(XARDSItf self,
                                        XAint16 group,
                                        XAboolean useErrorCorrection);

XAresult XARDSItfImpl_UnsubscribeODAGroup(XARDSItf self, XAint16 group);

XAresult XARDSItfImpl_ListODAGroupSubscriptions(XARDSItf self,
                                                XAint16* pGroups,
                                                XAuint32* pLength);

XAresult XARDSItfImpl_RegisterRDSCallback(XARDSItf self,
                                          xaRDSCallback callback,
                                          void * pContext);

XAresult XARDSItfImpl_RegisterODADataCallback(XARDSItf self,
                                              xaNewODADataCallback callback,
                                              void * pContext);


/* XARDSItfImpl -specific methods */
XARDSItfImpl* XARDSItfImpl_Create(XAAdaptationBaseCtx *adapCtx);

void XARDSItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event );
void XARDSItfImpl_Free(XARDSItfImpl* self);

#endif /* XARDSITF_H */
