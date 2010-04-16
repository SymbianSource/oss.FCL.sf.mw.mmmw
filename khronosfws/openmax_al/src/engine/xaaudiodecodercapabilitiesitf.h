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

#ifndef XAAUDIODECODERCAPABILITIESITF_H
#define XAAUDIODECODERCAPABILITIESITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"

/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAAudioDecoderCapabilitiesItf implementation */
typedef struct XAAudioDecoderCapabilitiesItfImpl_
{
    /* parent interface */
    struct XAAudioDecoderCapabilitiesItf_ itf;
    /* pointer to self */
    struct XAAudioDecoderCapabilitiesItfImpl_* self;
    /* variables */
    XAuint32 numCodecs;
} XAAudioDecoderCapabilitiesItfImpl;

/* Base interface XAAudioDecoderCapabilitiesItf implementation */
XAresult XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoders(
                            XAAudioDecoderCapabilitiesItf self,
                            XAuint32* pNumDecoders,
                            XAuint32* pDecoderIds);

XAresult XAAudioDecoderCapabilitiesItfImpl_GetAudioDecoderCapabilities(
                            XAAudioDecoderCapabilitiesItf self,
                            XAuint32 decoderId,
                            XAuint32* pIndex,
                            XAAudioCodecDescriptor* pDescriptor);

/* XAAudioDecoderCapabilitiesItfImpl -specific methods */
XAAudioDecoderCapabilitiesItfImpl* XAAudioDecoderCapabilitiesItfImpl_Create(void);
void XAAudioDecoderCapabilitiesItfImpl_Free(XAAudioDecoderCapabilitiesItfImpl* self);

#endif /* XAAUDIODECODERCAPABILITIESITF_H */
