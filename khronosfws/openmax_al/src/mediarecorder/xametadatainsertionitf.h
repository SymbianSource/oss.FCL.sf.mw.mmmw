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
 * Description: Metadata Insertion Itf Impl
 *
 */

#ifndef XAMETADATAINSERTIONITF_H
#define XAMETADATAINSERTIONITF_H

#include "xametadataadaptation.h"
#include "xamediarecorder.h"
/** MACROS **/

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAMetadataInsertionItf implementation */
typedef struct XAMetadataInsertionItfImpl_
    {
    /* parent interface */
    struct XAMetadataInsertionItf_ itf;
    /* pointer to self */
    struct XAMetadataInsertionItfImpl_* self;

    /* variables */
    XAAdaptationBaseCtx *adaptCtx;
    XAMetadataImplTagList currentTags;

    /*Callback*/
    xaMetadataInsertionCallback callback;
    void *cbcontext;
    XAMetadataInsertionItf cbPtrToSelf;

    } XAMetadataInsertionItfImpl;

/** METHODS **/

/* Base interface XAMetadataInsertionItf implementation */
XAresult XAMetadataInsertionItfImpl_CreateChildNode(
        XAMetadataInsertionItf self, XAint32 parentNodeID, XAuint32 type,
        XAchar *mimeType, XAint32 *pChildNodeID);

XAresult XAMetadataInsertionItfImpl_GetSupportedKeysCount(
        XAMetadataInsertionItf self, XAint32 nodeID, XAboolean *pFreeKeys,
        XAuint32 *pKeyCount, XAuint32 *pEncodingCount);

XAresult XAMetadataInsertionItfImpl_GetKeySize(XAMetadataInsertionItf self,
        XAint32 nodeID, XAuint32 keyIndex, XAuint32 *pKeySize);

XAresult XAMetadataInsertionItfImpl_GetKey(XAMetadataInsertionItf self,
        XAint32 nodeID, XAuint32 keyIndex, XAuint32 keySize,
        XAMetadataInfo *pKey);

XAresult XAMetadataInsertionItfImpl_GetFreeKeysEncoding(
        XAMetadataInsertionItf self, XAint32 nodeID, XAuint32 encodingIndex,
        XAuint32 *pEncoding);

XAresult XAMetadataInsertionItfImpl_InsertMetadataItem(
        XAMetadataInsertionItf self, XAint32 nodeID, XAMetadataInfo *pKey,
        XAMetadataInfo *pValue, XAboolean overwrite);

XAresult XAMetadataInsertionItfImpl_RegisterCallback(
        XAMetadataInsertionItf self, xaMetadataInsertionCallback callback,
        void *pContext);

/* XAMetadataInsertionItfImpl -specific methods */
XAMetadataInsertionItfImpl* XAMetadataInsertionItfImpl_Create(
        XAMediaRecorderImpl* impl);
void XAMetadataInsertionItfImpl_AdaptCb(void *pHandlerCtx,
        XAAdaptEvent *event);
void XAMetadataInsertionItfImpl_Free(XAMetadataInsertionItfImpl* self);

#endif /* XAMETADATAINSERTIONITF_H */
