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

#ifndef XAVIDEOENCODERCAPABILITIESITF_H
#define XAVIDEOENCODERCAPABILITIESITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"

/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAVideoEncoderCapabilitiesItf implementation */
typedef struct XAVideoEncoderCapabilitiesItfImpl_
{
    /* parent interface */
    struct XAVideoEncoderCapabilitiesItf_ itf;
    /* pointer to self */
    struct XAVideoEncoderCapabilitiesItfImpl_* self;
    /* variables */
    XAuint32 numCodecs;
} XAVideoEncoderCapabilitiesItfImpl;

/* Base interface XAVideoEncoderCapabilitiesItf implementation */
XAresult XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoders(
                            XAVideoEncoderCapabilitiesItf self,
                            XAuint32* pNumEncoders,
                            XAuint32* pEncoderIds);

XAresult XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoderCapabilities(
                            XAVideoEncoderCapabilitiesItf self,
                            XAuint32 decoderId,
                            XAuint32* pIndex,
                            XAVideoCodecDescriptor* pDescriptor);

/* XAVideoEncoderCapabilitiesItfImpl -specific methods */
XAVideoEncoderCapabilitiesItfImpl* XAVideoEncoderCapabilitiesItfImpl_Create(void);
void XAVideoEncoderCapabilitiesItfImpl_Free(XAVideoEncoderCapabilitiesItfImpl* self);

#endif /* XAVIDEOENCODERCAPABILITIESITF_H */
