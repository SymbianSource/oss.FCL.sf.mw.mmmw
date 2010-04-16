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

#ifndef XAIMAGEDECODERCAPABILITIESITF_H
#define XAIMAGEDECODERCAPABILITIESITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"

/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAImageDecoderCapabilitiesItf implementation */
typedef struct XAImageDecoderCapabilitiesItfImpl_
{
    /* parent interface */
    struct XAImageDecoderCapabilitiesItf_ itf;
    /* pointer to self */
    struct XAImageDecoderCapabilitiesItfImpl_* self;
    /* variables */
    XAuint32 numCodecs;
} XAImageDecoderCapabilitiesItfImpl;

/* Base interface XAImageDecoderCapabilitiesItf implementation */
XAresult XAImageDecoderCapabilitiesItfImpl_GetImageDecoderCapabilities(
                            XAImageDecoderCapabilitiesItf self,
                            XAuint32* pDecoderId,
                            XAImageCodecDescriptor* pDescriptor);

XAresult XAImageDecoderCapabilitiesItfImpl_QueryColorFormats(
                            const XAImageDecoderCapabilitiesItf self,
                            XAuint32* pIndex,
                            XAuint32* pColorFormats);

/* XAImageDecoderCapabilitiesItfImpl -specific methods */
XAImageDecoderCapabilitiesItfImpl* XAImageDecoderCapabilitiesItfImpl_Create(void);
void XAImageDecoderCapabilitiesItfImpl_Free(XAImageDecoderCapabilitiesItfImpl* self);

#endif /* XAIMAGEDECODERCAPABILITIESITF_H */
