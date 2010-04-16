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

#ifndef XAVIDEODECODERCAPABILITIESITF_H
#define XAVIDEODECODERCAPABILITIESITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"

/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAVideoDecoderCapabilitiesItf implementation */
typedef struct XAVideoDecoderCapabilitiesItfImpl_
{
    /* parent interface */
    struct XAVideoDecoderCapabilitiesItf_ itf;
    /* pointer to self */
    struct XAVideoDecoderCapabilitiesItfImpl_* self;
    /* variables */
    XAuint32 numCodecs;
} XAVideoDecoderCapabilitiesItfImpl;

/* Base interface XAVideoDecoderCapabilitiesItf implementation */
XAresult XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoders(
                            XAVideoDecoderCapabilitiesItf self,
                            XAuint32* pNumDecoders,
                            XAuint32* pDecoderIds);

XAresult XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoderCapabilities(
                            XAVideoDecoderCapabilitiesItf self,
                            XAuint32 decoderId,
                            XAuint32* pIndex,
                            XAVideoCodecDescriptor* pDescriptor);

/* XAVideoDecoderCapabilitiesItfImpl -specific methods */
XAVideoDecoderCapabilitiesItfImpl* XAVideoDecoderCapabilitiesItfImpl_Create(void);
void XAVideoDecoderCapabilitiesItfImpl_Free(XAVideoDecoderCapabilitiesItfImpl* self);

#endif /* XAVIDEODECODERCAPABILITIESITF_H */
