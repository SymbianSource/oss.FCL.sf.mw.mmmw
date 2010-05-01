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

#include "xaadaptationgst.h"


/* FUNCTIONS */
typedef struct XAMetadataImplTagList
{
    XAuint32         itemcount; /* number of items in all three following arrays */
    XAMetadataInfo** mdeKeys;   /* array of tag keys */
    XAMetadataInfo** mdeValues; /* array of tag values */
} XAMetadataImplTagList;


typedef struct XAMetadataAdaptVars_
{

    XAuint32        childcount;
    XAuint32        nodedepth;
    XAuint32        traversemode;

    GstTagList*     generaltags;
    GstStructure*   audiotags;
    GstStructure*   videotags;
    GstStructure**   currentchild; /*points to one of previous 3*/

} XAMetadataAdaptVars;

XAresult XAMetadataAdapt_PreInit(XAAdaptationGstCtx *bCtx);
XAresult XAMetadataAdapt_PostInit(XAAdaptationGstCtx *bCtx);
const XAchar* XAMetadataAdapt_ParseKhronosKey(const XAchar* KKey);
void XAMetadataAdapt_FreeImplTagList(XAMetadataImplTagList *list, XAboolean freeItems);
void XAMetadataAdapt_FreeVars(XAMetadataAdaptVars *vars);
XAresult XAMetadataAdapt_TryWriteTags(XAAdaptationGstCtx* bCtx, GstBin* binToWriteTo);

XAresult XAMetadataExtractionItfAdapt_FillTagList(XAAdaptationGstCtx *bCtx,
                                                  XAMetadataImplTagList* tagList);


XAresult XAMetadataTraversalItfAdapt_SetMode(XAAdaptationGstCtx *bCtx, XAuint32 mode);
XAresult XAMetadataTraversalItfAdapt_GetChildCount(XAAdaptationGstCtx *bCtx, XAuint32 *pCount);
XAresult XAMetadataTraversalItfAdapt_GetChildMIMETypeSize(XAAdaptationGstCtx *bCtx,
                                                          XAuint32 index,
                                                          XAuint32 *pSize);
XAresult XAMetadataTraversalItfAdapt_GetChildInfo(XAAdaptationGstCtx *bCtx,
                                                  XAuint32 index,
                                                  XAint32 *pNodeID,
                                                  XAuint32 *pType,
                                                  XAuint32 size,
                                                  XAchar *pMimeType);
XAresult XAMetadataTraversalItfAdapt_SetActiveNode(XAAdaptationGstCtx *bCtx,
                                                   XAuint32 index);

XAresult XAMetadataInsertionItfAdapt_CreateChildNode(XAAdaptationGstCtx *bCtx,
                                                    XAint32 parentNodeID,
                                                    XAuint32 type,
                                                    XAchar *mimeType,
                                                    XAint32 *pChildNodeID);
XAresult XAMetadataInsertionItfAdapt_GetSupportedKeysCount(XAAdaptationGstCtx *bCtx,
                                                          XAint32 nodeID,
                                                          XAboolean *pFreeKeys,
                                                          XAuint32 *pKeyCount,
                                                          XAuint32 *pEncodingCount);
XAresult XAMetadataInsertionItfAdapt_GetKeySize(XAAdaptationGstCtx *bCtx,
                                               XAint32 nodeID,
                                               XAuint32 keyIndex,
                                               XAuint32 *pKeySize);
XAresult XAMetadataInsertionItfAdapt_GetKey(XAAdaptationGstCtx *bCtx,
                                           XAint32 nodeID,
                                           XAuint32 keyIndex,
                                           XAuint32 keySize,
                                           XAMetadataInfo *pKey);
XAresult XAMetadataInsertionItfAdapt_GetFreeKeysEncoding(XAAdaptationGstCtx *bCtx,
                                                        XAint32 nodeID,
                                                        XAuint32 encodingIndex,
                                                        XAuint32 *pEncoding);
XAresult XAMetadataInsertionItfAdapt_InsertMetadataItem(XAAdaptationGstCtx *bCtx,
                                                       XAint32 nodeID,
                                                       XAMetadataInfo *pKey,
                                                       XAMetadataInfo *pValue,
                                                       XAboolean overwrite);
#endif /* XAMETADATAADAPTATION_H */
