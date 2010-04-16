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

#ifndef XAMETADATATRAVERSALITF_H
#define XAMETADATATRAVERSALITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAMetadataTraversalItf implementation */
typedef struct XAMetadataTraversalImpl_
{
    /* parent interface */
    struct XAMetadataTraversalItf_ itf;
    /* pointer to self */
    struct XAMetadataTraversalImpl_* self;

    /* pointer to metadata variables */
    XAuint32 traversemode;
    XAuint32 index;
    XAuint32 nodedepth;

#ifdef _GSTREAMER_BACKEND_  
    XAAdaptationBaseCtx *adaptCtx;
#endif
} XAMetadataTraversalImpl;

/** METHODS **/

/* Base interface XAMetadataTraversalItf implementation */
XAresult XAMetadataTraversalItfImpl_SetMode(XAMetadataTraversalItf self,
                                          XAuint32 mode);

XAresult XAMetadataTraversalItfImpl_GetChildCount(XAMetadataTraversalItf self,
                                                XAuint32 *pCount);

XAresult XAMetadataTraversalItfImpl_GetChildMIMETypeSize(XAMetadataTraversalItf self,
                                                       XAuint32 index,
                                                       XAuint32 *pSize);

XAresult XAMetadataTraversalItfImpl_GetChildInfo(XAMetadataTraversalItf self,
                                               XAuint32 index,
                                               XAint32 *pNodeID,
                                               XAuint32 *pType,
                                               XAuint32 size,
                                               XAchar *pMimeType);

XAresult XAMetadataTraversalItfImpl_SetActiveNode(XAMetadataTraversalItf self,
                                                XAuint32 index);

#ifdef _GSTREAMER_BACKEND_  
/* XAMetadataExtractionItfImpl -specific methods */
XAMetadataTraversalImpl* XAMetadataTraversalItfImpl_Create(XAAdaptationBaseCtx *adapCtx);
#endif
void XAMetadataTraversalItfImpl_Free(XAMetadataTraversalImpl* self);

#endif /* XAMETADATATRAVERSALITF_H */
