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

#ifndef XAIMAGEENCODERCAPABILITIESITF_H
#define XAIMAGEENCODERCAPABILITIESITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"

/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAImageEncoderCapabilitiesItf implementation */
typedef struct XAImageEncoderCapabilitiesItfImpl_
{
    /* parent interface */
    struct XAImageEncoderCapabilitiesItf_ itf;
    /* pointer to self */
    struct XAImageEncoderCapabilitiesItfImpl_* self;
    /* variables */
    XAuint32 numCodecs;
} XAImageEncoderCapabilitiesItfImpl;

/* Base interface XAImageEncoderCapabilitiesItf implementation */
XAresult XAImageEncoderCapabilitiesItfImpl_GetImageEncoderCapabilities(
                            XAImageEncoderCapabilitiesItf self,
                            XAuint32* pEncoderId,
                            XAImageCodecDescriptor* pDescriptor);

XAresult XAImageEncoderCapabilitiesItfImpl_QueryColorFormats(
                            const XAImageEncoderCapabilitiesItf self,
                            XAuint32* pIndex,
                            XAuint32* pColorFormats);

/* XAImageEncoderCapabilitiesItfImpl -specific methods */
XAImageEncoderCapabilitiesItfImpl* XAImageEncoderCapabilitiesItfImpl_Create(void);
void XAImageEncoderCapabilitiesItfImpl_Free(XAImageEncoderCapabilitiesItfImpl* self);

#endif /* XAIMAGEENCODERCAPABILITIESITF_H */
