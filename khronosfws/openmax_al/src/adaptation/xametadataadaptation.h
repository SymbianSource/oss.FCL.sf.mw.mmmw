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

#ifndef XAMETADATAADAPTATION_H
#define XAMETADATAADAPTATION_H

#include "XADebug.h"
#include "XAAdaptationContextBase.h"
#include "XAMediaRecorderAdaptCtx.h"

/* FUNCTIONS */
typedef struct XAMetadataImplTagList
{
    XAuint32         itemcount; /* number of items in all three following arrays */
    XAMetadataInfo** mdeKeys;   /* array of tag keys */
    XAMetadataInfo** mdeValues; /* array of tag values */
} XAMetadataImplTagList;

XAresult XAMetadataAdapt_PreInit(XAAdaptationBaseCtx *bCtx);
XAresult XAMetadataAdapt_PostInit(XAAdaptationBaseCtx *bCtx);
const XAchar* XAMetadataAdapt_ParseKhronosKey(const XAchar* KKey);
void XAMetadataAdapt_FreeImplTagList(XAMetadataImplTagList *list, XAboolean freeItems);

XAresult XAMetadataExtractionItfAdapt_FillTagList(XAAdaptationBaseCtx *bCtx,
                                                  XAMetadataImplTagList* tagList);


XAresult XAMetadataTraversalItfAdapt_SetMode(XAAdaptationBaseCtx *bCtx, XAuint32 mode);
XAresult XAMetadataTraversalItfAdapt_GetChildCount(XAAdaptationBaseCtx *bCtx, XAuint32 *pCount);
XAresult XAMetadataTraversalItfAdapt_GetChildMIMETypeSize(XAAdaptationBaseCtx *bCtx,
                                                          XAuint32 index,
                                                          XAuint32 *pSize);
XAresult XAMetadataTraversalItfAdapt_GetChildInfo(XAAdaptationBaseCtx *bCtx,
                                                  XAuint32 index,
                                                  XAint32 *pNodeID,
                                                  XAuint32 *pType,
                                                  XAuint32 size,
                                                  XAchar *pMimeType);
XAresult XAMetadataTraversalItfAdapt_SetActiveNode(XAAdaptationBaseCtx *bCtx,
                                                   XAuint32 index);

XAresult XAMetadataInsertionItfAdapt_CreateChildNode(XAAdaptationBaseCtx *bCtx,
                                                    XAint32 parentNodeID,
                                                    XAuint32 type,
                                                    XAchar *mimeType,
                                                    XAint32 *pChildNodeID);
XAresult XAMetadataInsertionItfAdapt_GetSupportedKeysCount(XAAdaptationBaseCtx *bCtx,
                                                          XAint32 nodeID,
                                                          XAboolean *pFreeKeys,
                                                          XAuint32 *pKeyCount,
                                                          XAuint32 *pEncodingCount);
XAresult XAMetadataInsertionItfAdapt_GetKeySize(XAAdaptationBaseCtx *bCtx,
                                               XAint32 nodeID,
                                               XAuint32 keyIndex,
                                               XAuint32 *pKeySize);
XAresult XAMetadataInsertionItfAdapt_GetKey(XAAdaptationBaseCtx *bCtx,
                                           XAint32 nodeID,
                                           XAuint32 keyIndex,
                                           XAuint32 keySize,
                                           XAMetadataInfo *pKey);
XAresult XAMetadataInsertionItfAdapt_GetFreeKeysEncoding(XAAdaptationBaseCtx *bCtx,
                                                        XAint32 nodeID,
                                                        XAuint32 encodingIndex,
                                                        XAuint32 *pEncoding);
XAresult XAMetadataInsertionItfAdapt_InsertMetadataItem(XAAdaptationBaseCtx *bCtx,
                                                       XAint32 nodeID,
                                                       XAMetadataInfo *pKey,
                                                       XAMetadataInfo *pValue,
                                                       XAboolean overwrite);
#endif /* XAMETADATAADAPTATION_H */
