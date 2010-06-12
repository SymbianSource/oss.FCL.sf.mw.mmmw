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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "xametadatainsertionitf.h"

#include "xametadataadaptation.h"

#define MAX_TAGS 255
/* XAMetadataInsertionItfImpl* GetImpl(XAMetadataInsertionItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAMetadataInsertionItfImpl* GetImpl(XAMetadataInsertionItf self)
    {
    if (self)
        {
        XAMetadataInsertionItfImpl* impl =
                (XAMetadataInsertionItfImpl*) (*self);
        if (impl && (impl == impl->self))
            {
            return impl;
            }
        }
    return NULL;
    }

/*****************************************************************************
 * Base interface XAMetadataInsertionItf implementation
 *****************************************************************************/

/* XAMetadataInsertionItfImpl_CreateChildNode
 * Description: Creates a new child node for the given parent.
 */
XAresult XAMetadataInsertionItfImpl_CreateChildNode(
        XAMetadataInsertionItf self, XAint32 parentNodeID, XAuint32 type,
        XAchar *mimeType, XAint32 *pChildNodeID)
    {
    XAMetadataInsertionItfImpl *impl = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfImpl_CreateChildNode");
    impl = GetImpl(self);
    if (!impl || !mimeType || !pChildNodeID)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        ret = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {

        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWGST)
            {
            if (impl->adaptCtx)
                {
                ret = XAMetadataInsertionItfAdapt_CreateChildNode(
                        (XAAdaptationGstCtx*) impl->adaptCtx, parentNodeID,
                        type, mimeType, pChildNodeID);
                }
            else
                {
                ret = XA_RESULT_INTERNAL_ERROR;
                }

            }
        else
            {
            DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
            DEBUG_API("<-XAMetadataInsertionItfImpl_CreateChildNode");
            return XA_RESULT_FEATURE_UNSUPPORTED;
            }
        }DEBUG_API("<-XAMetadataInsertionItfImpl_CreateChildNode");
    return ret;
    }

/* XAresult XAMetadataInsertionItfImpl_GetSupportedKeysCount
 * Description: A query method to tell if the metadata keys (for writing metadata)
 * can be freely chosen by the application or if they are fixed (for the given node).
 */
XAresult XAMetadataInsertionItfImpl_GetSupportedKeysCount(
        XAMetadataInsertionItf self, XAint32 nodeID, XAboolean *pFreeKeys,
        XAuint32 *pKeyCount, XAuint32 *pEncodingCount)
    {
    XAMetadataInsertionItfImpl *impl = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfImpl_GetSupportedKeysCount");
    impl = GetImpl(self);
    if (!impl || !pFreeKeys || !pKeyCount || !pEncodingCount)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        ret = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {
        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWGST)
            {
            if (impl->adaptCtx)
                {
                ret = XAMetadataInsertionItfAdapt_GetSupportedKeysCount(
                        (XAAdaptationGstCtx*) impl->adaptCtx, nodeID,
                        pFreeKeys, pKeyCount, pEncodingCount);
                }
            else
                {
                ret = XA_RESULT_INTERNAL_ERROR;
                }
            }
        else
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataInsertionItfImpl_GetSupportedKeysCount");
            return XA_RESULT_PARAMETER_INVALID;
            }
        }DEBUG_API("<-XAMetadataInsertionItfImpl_GetSupportedKeysCount");
    return ret;
    }

/* XAresult XAMetadataInsertionItfImpl_GetKeySize
 * Description: Returns the byte size required for a supported metadata
 * key pointed by the given index
 */
XAresult XAMetadataInsertionItfImpl_GetKeySize(XAMetadataInsertionItf self,
        XAint32 nodeID, XAuint32 keyIndex, XAuint32 *pKeySize)
    {
    XAMetadataInsertionItfImpl *impl = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfImpl_GetKeySize");
    impl = GetImpl(self);
    if (!impl || !pKeySize)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        ret = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {
        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWGST)
            {

            if (impl->adaptCtx)
                {
                ret = XAMetadataInsertionItfAdapt_GetKeySize(
                        (XAAdaptationGstCtx*) impl->adaptCtx, nodeID,
                        keyIndex, pKeySize);
                }
            else
                {
                ret = XA_RESULT_INTERNAL_ERROR;
                }
            }
        else
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataInsertionItfImpl_GetKeySize");
            return XA_RESULT_PARAMETER_INVALID;
            }
        }DEBUG_API("<-XAMetadataInsertionItfImpl_GetKeySize");
    return ret;
    }

/* XAresult XAMetadataInsertionItfImpl_GetKey
 * Description; Returns a XAMetadataInfo structure and associated data
 * referenced by the structure for a supported key
 */
XAresult XAMetadataInsertionItfImpl_GetKey(XAMetadataInsertionItf self,
        XAint32 nodeID, XAuint32 keyIndex, XAuint32 keySize,
        XAMetadataInfo *pKey)
    {
    XAMetadataInsertionItfImpl *impl = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfImpl_GetKey");
    impl = GetImpl(self);
    if (!impl || !pKey)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        ret = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {
        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWGST)
            {

            if (impl->adaptCtx)
                {
                ret = XAMetadataInsertionItfAdapt_GetKey(
                        (XAAdaptationGstCtx*) impl->adaptCtx, nodeID,
                        keyIndex, keySize, pKey);
                }
            else
                {
                ret = XA_RESULT_INTERNAL_ERROR;
                }
            }
        else
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataInsertionItfImpl_GetKey");
            return XA_RESULT_PARAMETER_INVALID;
            }

        }DEBUG_API("<-XAMetadataInsertionItfImpl_GetKey");
    return ret;
    }

/* XAresult XAMetadataInsertionItfImpl_GetFreeKeysEncoding
 * Description: A method to be used in case implementation supports
 * free keys for metadata insertion.
 */
XAresult XAMetadataInsertionItfImpl_GetFreeKeysEncoding(
        XAMetadataInsertionItf self, XAint32 nodeID, XAuint32 encodingIndex,
        XAuint32 *pEncoding)
    {
    XAMetadataInsertionItfImpl *impl = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfImpl_GetFreeKeysEncoding");
    impl = GetImpl(self);
    if (!impl || !pEncoding)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        ret = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {
        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWGST)
            {

            if (impl->adaptCtx)
                {
                ret = XAMetadataInsertionItfAdapt_GetFreeKeysEncoding(
                        (XAAdaptationGstCtx*) impl->adaptCtx, nodeID,
                        encodingIndex, pEncoding);
                }
            else
                {
                ret = XA_RESULT_INTERNAL_ERROR;
                }
            }
        else
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataInsertionItfImpl_GetFreeKeysEncoding");
            return XA_RESULT_PARAMETER_INVALID;
            }
        }DEBUG_API("<-XAMetadataInsertionItfImpl_GetFreeKeysEncoding");
    return ret;
    }

/* XAresult XAMetadataInsertionItfImpl_InsertMetadataItem
 * Description: Inserts the key/value pair to the specified node of the metadata tree.
 */
XAresult XAMetadataInsertionItfImpl_InsertMetadataItem(
        XAMetadataInsertionItf self, XAint32 nodeID, XAMetadataInfo *pKey,
        XAMetadataInfo *pValue, XAboolean overwrite)
    {
    XAMetadataInsertionItfImpl *impl = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfImpl_InsertMetadataItem");
    impl = GetImpl(self);
    if (!impl || !pKey || !pValue)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        ret = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {
        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWGST)
            {

            if (impl->adaptCtx)
                {
                ret = XAMetadataInsertionItfAdapt_InsertMetadataItem(
                        (XAAdaptationGstCtx*) impl->adaptCtx, nodeID, pKey,
                        pValue, overwrite);
                impl->currentTags.mdeKeys[impl->currentTags.itemcount] = pKey;
                impl->currentTags.mdeValues[impl->currentTags.itemcount]
                        = pValue;
                impl->currentTags.itemcount++;
                }
            else
                {
                ret = XA_RESULT_INTERNAL_ERROR;
                }
            }
        else
            {
            DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
            DEBUG_API("<-XAMetadataInsertionItfImpl_GetFreeKeysEncoding");
            return XA_RESULT_FEATURE_UNSUPPORTED;
            }
        }DEBUG_API("<-XAMetadataInsertionItfImpl_InsertMetadataItem");
    return ret;
    }

/* XAresult XAMetadataInsertionItfImpl_RegisterCallback
 * Description: Registers a callback on the object that executes after each of
 * the actual writings of metadata key/value pairs takes place.
 */
XAresult XAMetadataInsertionItfImpl_RegisterCallback(
        XAMetadataInsertionItf self, xaMetadataInsertionCallback callback,
        void *pContext)
    {
    XAMetadataInsertionItfImpl *impl = NULL;

    DEBUG_API("->XAMetadataInsertionItfImpl_RegisterCallback");
    impl = GetImpl(self);
    if (!impl)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAMetadataInsertionItfImpl_RegisterCallback");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    if (callback)
        {
        XAAdaptationBase_AddEventHandler(impl->adaptCtx,
                &XAMetadataInsertionItfImpl_AdaptCb, XA_METADATAEVENTS, impl);
        impl->callback = callback;
        impl->cbcontext = pContext;
        impl->cbPtrToSelf = self;
        }
    else
        {
        XAAdaptationBase_RemoveEventHandler(impl->adaptCtx,
                &XAMetadataInsertionItfImpl_AdaptCb);
        }

    DEBUG_API("<-XAMetadataInsertionItfImpl_RegisterCallback");
    return XA_RESULT_SUCCESS;
    }

/*****************************************************************************
 * XAMetadataTraversalImpl -specific methods
 *****************************************************************************/

/* XAMetadataInsertionItfImpl* XAMetadataInsertionItfImpl_Create()
 * Description: Allocate and initialize MetadataInsertionItfImpl
 */
XAMetadataInsertionItfImpl* XAMetadataInsertionItfImpl_Create(
        XAMediaRecorderImpl* impl)
    {
    XAMetadataInsertionItfImpl* self = (XAMetadataInsertionItfImpl*) calloc(
            1, sizeof(XAMetadataInsertionItfImpl));
    //XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*)(impl->adaptationCtx);

    DEBUG_API("->XAMetadataInsertionItfImpl_Create");
    if (self)
        {
        //if(mCtx->fwtype == FWMgrFWGST)
            {
            /* init itf default implementation */
            self->itf.CreateChildNode
                    = XAMetadataInsertionItfImpl_CreateChildNode;
            self->itf.GetFreeKeysEncoding
                    = XAMetadataInsertionItfImpl_GetFreeKeysEncoding;
            self->itf.GetKey = XAMetadataInsertionItfImpl_GetKey;
            self->itf.GetKeySize = XAMetadataInsertionItfImpl_GetKeySize;
            self->itf.GetSupportedKeysCount
                    = XAMetadataInsertionItfImpl_GetSupportedKeysCount;
            self->itf.InsertMetadataItem
                    = XAMetadataInsertionItfImpl_InsertMetadataItem;
            self->itf.RegisterCallback
                    = XAMetadataInsertionItfImpl_RegisterCallback;
            }

        /* init variables*/

        self->callback = NULL;
        self->cbcontext = NULL;
        self->cbPtrToSelf = NULL;
        self->adaptCtx = impl->adaptationCtx;
        self->currentTags.mdeKeys = calloc(MAX_TAGS, sizeof(XAMetadataInfo*));
        self->currentTags.mdeValues = calloc(MAX_TAGS,
                sizeof(XAMetadataInfo*));
        self->self = self;

        }DEBUG_API("<-XAMetadataInsertionItfImpl_Create");
    return self;
    }

/* void XAMetadataInsertionItfImpl_Free(XAMetadataInsertionItfImpl* self)
 * Description: Free all resources reserved at XAMetadataInsertionItfImpl_Create()
 */
void XAMetadataInsertionItfImpl_Free(XAMetadataInsertionItfImpl* self)
    {
    DEBUG_API("->XAMetadataInsertionItfImpl_Free");
    assert( self==self->self );
    if (self->callback)
        {
        XAAdaptationBase_RemoveEventHandler(self->adaptCtx,
                &XAMetadataInsertionItfImpl_AdaptCb);
        }
    XAMetadataAdapt_FreeImplTagList(&(self->currentTags), XA_BOOLEAN_FALSE);
    free(self);
    DEBUG_API("<-XAMetadataInsertionItfImpl_Free");
    }

/* With this method, adaptation infroms that tags are written to stream
 */
void XAMetadataInsertionItfImpl_AdaptCb(void *pHandlerCtx,
        XAAdaptEvent *event)
    {
    XAMetadataInsertionItfImpl* impl = NULL;
    XAuint32 idx = 0, nodeID = XA_ROOT_NODE_ID;
    XAboolean result = XA_BOOLEAN_TRUE;
    DEBUG_API("->XAMetadataInsertionItfImpl_AdaptCb");
    impl = (XAMetadataInsertionItfImpl*) pHandlerCtx;
    if (!impl)
        {
        DEBUG_ERR("XAMetadataInsertionItfImpl_AdaptCb, invalid context pointer!");
        DEBUG_API("<-XAMetadataInsertionItfImpl_AdaptCb");
        return;
        }
    if (event && event->eventid == XA_ADAPT_MDE_TAGS_WRITTEN)
        {
        /*here datasize field is used for return value*/
        if (event->datasize != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR_A1("Adaptation failed to write metadata (code %x)!",(int)event->datasize);
            result = XA_BOOLEAN_FALSE;
            }
        /*all tags written, send cb for client to free the memory*/
        for (idx = 0; idx < impl->currentTags.itemcount; idx++)
            {
            if (impl->callback)
                {
                impl->callback(impl->cbPtrToSelf, impl->cbcontext,
                        impl->currentTags.mdeKeys[idx],
                        impl->currentTags.mdeValues[idx], nodeID, result);
                }
            impl->currentTags.mdeKeys[idx] = NULL;
            impl->currentTags.mdeValues[idx] = NULL;
            }
        impl->currentTags.itemcount = 0;
        }
    else
        {
        DEBUG_INFO("unhandled");
        }
    DEBUG_API("<-XAMetadataInsertionItfImpl_AdaptCb");
    }

