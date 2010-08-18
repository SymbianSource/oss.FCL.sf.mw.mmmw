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
 * Description: Metadata Extraction Interface Implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "xametadataextractionitf.h"
#include "xadebug.h"

#include "xametadataadaptation.h"

#include "xaadaptationmmf.h"
#include "xametadataadaptctxmmf.h"
#include "xamediaplayeradaptctxmmf.h"
#include "cmetadatautilityitf.h"

/* XAMetadataExtractionItfImpl* GetImpl(XAMetadataExtractionItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 **/
static XAMetadataExtractionItfImpl* GetImpl(XAMetadataExtractionItf self)
    {
    if (self)
        {
        XAMetadataExtractionItfImpl* impl =
                (XAMetadataExtractionItfImpl*) (*self);
        if (impl && (impl == impl->self))
            {
            return impl;
            }
        }
    return NULL;
    }

static void* GetMetadataUtilityContext(XAAdaptationMMFCtx* adaptCtx)
    {
    if (adaptCtx)
        {
        switch (adaptCtx->baseObj.ctxId)
            {
            case XAMediaPlayerAdaptation:
                return ((XAMediaPlayerAdaptationMMFCtx*) adaptCtx)->mmfMetadataContext;
            case XAMDAdaptation:
                return ((XAMetadataAdaptationMMFCtx*) adaptCtx)->mmfContext;
            default:
                break;
            }

        }
    return NULL;
    }
/*****************************************************************************
 * Base interface XAMetadataExtractionItf implementation
 *****************************************************************************/

/*
 * Returns the number of metadata items within the current scope of the object.
 * @XAuint32 *pItemCount
 *      Number of metadata items.  Must be non-NULL
 */
XAresult XAMetadataExtractionItfImpl_GetItemCount(
        XAMetadataExtractionItf self, XAuint32 *pItemCount)
    {
    XAMetadataExtractionItfImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAMetadataExtractionItfImpl_GetItemCount");

    impl = GetImpl(self);
    /* check parameters */
    if (!impl || !pItemCount)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {

        if (impl->adaptCtx)
            {

            if (impl->adaptCtx->fwtype == FWMgrFWMMF)
                {
                void *mmfCtx = GetMetadataUtilityContext(
                        (XAAdaptationMMFCtx*) impl->adaptCtx);
                if (mmfCtx)
                    {
                    res = mmf_get_item_count(mmfCtx, pItemCount);
                    }
                else
                    {
                    res = XA_RESULT_PARAMETER_INVALID;
                    }
                }
            else
                {
                if (impl->filteringOn)
                    {
                    *pItemCount = impl->filteredcount;
                    }
                else
                    {
                    *pItemCount = impl->currentTags.itemcount;
                    }
                res = XA_RESULT_SUCCESS;
                }
            }
        else
            {
            res = XA_RESULT_INTERNAL_ERROR;
            }

        DEBUG_INFO_A1("itemCount = %d", (int)*pItemCount);
        }

    DEBUG_API_A1("<-XAMetadataExtractionItfImpl_GetItemCount (%d)", (int)res);
    return res;
    }

/*
 * Returns the byte size of a given metadata key
 *
 * @XAuint32 index
 *      Metadata item Index. Range is [0, GetItemCount)
 * @XAuint32 *pKeySize
 *      Address to store key size. size must be greater than 0.  Must be non-NULL
 */
XAresult XAMetadataExtractionItfImpl_GetKeySize(XAMetadataExtractionItf self,
        XAuint32 index, XAuint32 *pKeySize)
    {
    XAMetadataExtractionItfImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 newidx = 0;
    DEBUG_API("->XAMetadataExtractionItfImpl_GetKeySize");

    impl = GetImpl(self);
    if (!impl || !pKeySize)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAMetadataExtractionItfImpl_GetKeySize");
        return XA_RESULT_PARAMETER_INVALID;
        }
    *pKeySize = 0;

    if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWMMF)
        {
        void *mmfCtx = GetMetadataUtilityContext(
                (XAAdaptationMMFCtx*) impl->adaptCtx);
        if (mmfCtx)
            {
            res = mmf_get_key_size(mmfCtx, index, pKeySize);
            }
        else
            {
            res = XA_RESULT_PARAMETER_INVALID;
            }
        }
    else
        {
        /* check index and return unfiltered index */
        if (CheckAndUnfilterIndex(impl, index, &newidx) != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataExtractionItfImpl_GetKeySize");
            return XA_RESULT_PARAMETER_INVALID;
            }

        /* size = size of struct + size of data - 1 (struct size already includes one char) */
        *pKeySize = sizeof(XAMetadataInfo)
                + impl->currentTags.mdeKeys[newidx]->size - 1;
        }

    DEBUG_API_A1("<-XAMetadataExtractionItfImpl_GetKeySize (%d)", (int)res);
    return res;
    }

/*
 * Returns a XAMetadataInfo structure and associated data referenced by the structure for a key.
 * @XAuint32 index
 *      Metadata item Index. Range is [0, GetItemCount())
 * @XAuint32 keySize
 *      Size of the memory block passed as key. Range is [1, GetKeySize].
 * @XAMetadataInfo *pKey
 *      Address to store the key.  Must be non-NULL
 */
XAresult XAMetadataExtractionItfImpl_GetKey(XAMetadataExtractionItf self,
        XAuint32 index, XAuint32 keySize, XAMetadataInfo *pKey)
    {
    XAMetadataExtractionItfImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 newidx = 0;

    XAuint32 neededsize = 0;

    XAuint32 newdatasize = 0;
    DEBUG_API("->XAMetadataExtractionItfImpl_GetKey");

    impl = GetImpl(self);
    if (!impl || !pKey)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAMetadataExtractionItfImpl_GetKey");
        return XA_RESULT_PARAMETER_INVALID;
        }

    memset(pKey, 0, keySize);

    if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWMMF)
        {
        void *mmfCtx = GetMetadataUtilityContext(
                (XAAdaptationMMFCtx*) impl->adaptCtx);
        if (mmfCtx)
            {
            res = mmf_get_key(mmfCtx, index, keySize, pKey);
            }
        else
            {
            res = XA_RESULT_PARAMETER_INVALID;
            }
        }
    else
        {

        /* check index and return unfiltered index */
        if (CheckAndUnfilterIndex(impl, index, &newidx) != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataExtractionItfImpl_GetKey");
            return XA_RESULT_PARAMETER_INVALID;
            }

        /* needed size = size of struct + size of data - 1 (struct size already includes one char) */
        neededsize = sizeof(XAMetadataInfo)
                + impl->currentTags.mdeKeys[newidx]->size - 1;
        if (keySize < neededsize)
            { /* cannot fit all of key data */
            newdatasize = impl->currentTags.mdeKeys[newidx]->size
                    - (neededsize - keySize);
            DEBUG_ERR("XA_RESULT_BUFFER_INSUFFICIENT");
            res = XA_RESULT_BUFFER_INSUFFICIENT;
            }
        else
            {
            newdatasize = impl->currentTags.mdeKeys[newidx]->size;
            res = XA_RESULT_SUCCESS;
            }
        /* copy data up to given size */
        memcpy(pKey, impl->currentTags.mdeKeys[newidx], keySize - 1);
        /* ensure null-termination */

        memset(pKey->data + newdatasize - 1, 0, 1);
        pKey->size = newdatasize;
        }

    DEBUG_API_A1("<-XAMetadataExtractionItfImpl_GetKey (%d)", (int)res);
    return res;
    }

/*
 * Returns the byte size of a given metadata value
 * @XAuint32 index
 *      Metadata item Index. Range is [0, GetItemCount())
 * @XAuint32 *pValueSize
 *      Address to store value size. size must be greater than 0.  Must be non-NULL
 */
XAresult XAMetadataExtractionItfImpl_GetValueSize(
        XAMetadataExtractionItf self, XAuint32 index, XAuint32 *pValueSize)
    {
    XAMetadataExtractionItfImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 newidx = 0;
    DEBUG_API("->XAMetadataExtractionItfImpl_GetValueSize");

    impl = GetImpl(self);
    if (!impl || !pValueSize)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAMetadataExtractionItfImpl_GetValueSize");
        return XA_RESULT_PARAMETER_INVALID;
        }
    *pValueSize = 0;

    if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWMMF)
        {
        void *mmfCtx = GetMetadataUtilityContext(
                (XAAdaptationMMFCtx*) impl->adaptCtx);
        if (mmfCtx)
            {
            res = mmf_get_value_size(mmfCtx, index, pValueSize);
            }
        else
            {
            res = XA_RESULT_PARAMETER_INVALID;
            }
        }
    else
        {
        /* check index and return unfiltered index */
        if (CheckAndUnfilterIndex(impl, index, &newidx) != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataExtractionItfImpl_GetValueSize");
            return XA_RESULT_PARAMETER_INVALID;
            }

        /* size = size of struct + size of data - 1 (struct size already includes one char) */
        *pValueSize = sizeof(XAMetadataInfo)
                + impl->currentTags.mdeValues[newidx]->size - 1;
        }

    DEBUG_API_A1("<-XAMetadataExtractionItfImpl_GetValueSize (%d)", (int)res);
    return res;
    }

/*
 * Returns a XAMetadataInfo structure and associated data referenced by the structure for a value.
 *  @XAuint32 index
 *      Metadata item Index. Range is [0, GetItemCount())
 *  @XAuint32 valueSize
 *      Size of the memory block passed as value. Range is [0, GetValueSize]
 *  @XAMetadataInfo *pValue
 *      Address to store the value.  Must be non-NULL
 */
XAresult XAMetadataExtractionItfImpl_GetValue(XAMetadataExtractionItf self,
        XAuint32 index, XAuint32 valueSize, XAMetadataInfo *pValue)
    {
    XAMetadataExtractionItfImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 newidx = 0;

    XAuint32 neededsize = 0;

    XAuint32 newdatasize = 0;
    DEBUG_API("->XAMetadataExtractionItfImpl_GetValue");

    impl = GetImpl(self);
    if (!impl || !pValue)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAMetadataExtractionItfImpl_GetValue");
        return XA_RESULT_PARAMETER_INVALID;
        }

    memset(pValue, 0, valueSize);

    if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWMMF)
        {
        void *mmfCtx = GetMetadataUtilityContext(
                (XAAdaptationMMFCtx*) impl->adaptCtx);
        if (mmfCtx)
            {
            res = mmf_get_value(mmfCtx, index, valueSize, pValue);
            }
        else
            {
            res = XA_RESULT_PARAMETER_INVALID;
            }
        }
    else
        {
        /* check index and return unfiltered index */
        if (CheckAndUnfilterIndex(impl, index, &newidx) != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataExtractionItfImpl_GetValue");
            return XA_RESULT_PARAMETER_INVALID;
            }

        /* needed size = size of struct + size of data - 1 (struct size already includes one char) */
        neededsize = sizeof(XAMetadataInfo)
                + impl->currentTags.mdeValues[newidx]->size - 1;
        if (valueSize < neededsize)
            { /* cannot fit all of key data */
            newdatasize = impl->currentTags.mdeValues[newidx]->size
                    - (neededsize - valueSize);
            DEBUG_ERR("XA_RESULT_BUFFER_INSUFFICIENT");
            res = XA_RESULT_BUFFER_INSUFFICIENT;
            }
        else
            {
            newdatasize = impl->currentTags.mdeValues[newidx]->size;
            res = XA_RESULT_SUCCESS;
            }
        /* copy data up to given size */
        memcpy(pValue, impl->currentTags.mdeValues[newidx], valueSize - 1);
        /* ensure null-termination */

        memset(pValue->data + newdatasize - 1, 0, 1);

        pValue->size = newdatasize;
        }

    DEBUG_API_A1("<-XAMetadataExtractionItfImpl_GetValue (%d)",(int)res);
    return res;
    }

/*
 * Adds a filter for a specific key
 * @XAuint32 keySize
 *      Size in bytes, of the pKey parameter. Ignored if filtering by key is disabled
 * @const void *pKey
 *      The key to filter by. The entire key must match. Ignored if filtering by key is disabled.
 * @XAuint32 keyEncoding
 *      Character encoding of the pKey parameter. Ignored if filtering by key is disabled
 * @const XAchar *pValueLangCountry
 *      Language / country code of the value to filter by. Ignored if filtering by language / country is disabled
 * @XAuint32 valueEncoding
 *      Encoding of the value to filter by. Ignored if filtering by encoding is disabled
 * @XAuint8 filterMask
 *      Bitmask indicating which criteria to filter by. Should be one of the XA_METADATA_FILTER macros
 */
XAresult XAMetadataExtractionItfImpl_AddKeyFilter(
        XAMetadataExtractionItf self, XAuint32 keySize, const void *pKey,
        XAuint32 keyEncoding, const XAchar *pValueLangCountry,
        XAuint32 valueEncoding, XAuint8 filterMask)
    {
    XAresult res = XA_RESULT_SUCCESS;

    XAuint32 idx = 0;
    XAuint8 matchMask = 0;

    XAMetadataExtractionItfImpl *impl = NULL;
    const XAchar* parsedkey;
    impl = GetImpl(self);

    DEBUG_API("->XAMetadataExtractionItfImpl_AddKeyFilter");

    if (!impl)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAMetadataExtractionItfImpl_AddKeyFilter");
        return XA_RESULT_PARAMETER_INVALID;
        }
    else
        {

        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWMMF)
            {
            DEBUG_API("<-XAMetadataExtractionItfImpl_AddKeyFilter Not Supported in MMF");
            res = XA_RESULT_PARAMETER_INVALID;
            }
        else
            {
            impl->filteringOn = XA_BOOLEAN_TRUE;
            for (idx = 0; idx < impl->currentTags.itemcount; idx++)
                {
                if ((filterMask & XA_METADATA_FILTER_KEY) && pKey)
                    {
                    parsedkey = XAMetadataAdapt_ParseKhronosKey(pKey);
                    if (strcmp((char*) parsedkey,
                            (char*) impl->currentTags.mdeKeys[idx]->data)
                            == 0)
                        {
                        matchMask |= XA_METADATA_FILTER_KEY;
                        }
                    }
                if (filterMask & XA_METADATA_FILTER_LANG && pValueLangCountry)
                    {
                    if (strcmp(
                            (char*) pValueLangCountry,
                            (char*) impl->currentTags.mdeKeys[idx]->langCountry)
                            == 0)
                        {
                        matchMask |= XA_METADATA_FILTER_LANG;
                        }
                    }
                if (filterMask & XA_METADATA_FILTER_ENCODING)
                    {
                    if (keyEncoding
                            == impl->currentTags.mdeKeys[idx]->encoding)
                        {
                        matchMask |= XA_METADATA_FILTER_ENCODING;
                        }
                    if (valueEncoding
                            == impl->currentTags.mdeValues[idx]->encoding)
                        {
                        matchMask |= XA_METADATA_FILTER_ENCODING;
                        }
                    }
                /* check if all filters apply */
                if (filterMask == matchMask)
                    {
                    if (impl->tagmatchesfilter[idx] == XA_BOOLEAN_FALSE)
                        {
                        impl->tagmatchesfilter[idx] = XA_BOOLEAN_TRUE;
                        impl->filteredcount++;
                        }
                    }
                /*reset matchmask*/
                matchMask = 0;
                }
            }
        }
    DEBUG_API_A1("<-XAMetadataExtractionItfImpl_AddKeyFilter (%d)", (int)res);
    return res;
    }

/*
 * Clears the key filter
 */
XAresult XAMetadataExtractionItfImpl_ClearKeyFilter(
        XAMetadataExtractionItf self)
    {
    XAMetadataExtractionItfImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;

    XAuint32 idx = 0;

    DEBUG_API("->XAMetadataExtractionItfImpl_ClearKeyFilter");
    impl = GetImpl(self);
    if (!impl)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
        }
    else
        {

        if (impl->adaptCtx && impl->adaptCtx->fwtype == FWMgrFWMMF)
            {
            DEBUG_API("<-XAMetadataExtractionItfImpl_ClearKeyFilter Not Supported in MMF");
            res = XA_RESULT_PARAMETER_INVALID;
            }
        else
            {
            if (impl->tagmatchesfilter)
                {

                for (idx = 0; idx < impl->currentTags.itemcount; idx++)
                    {
                    impl->tagmatchesfilter[idx] = XA_BOOLEAN_FALSE;
                    }

                }
            impl->filteredcount = 0;
            impl->filteringOn = XA_BOOLEAN_FALSE;
            }
        }

    DEBUG_API_A1("<-XAMetadataExtractionItfImpl_ClearKeyFilter (%d)", (int)res);
    return res;
    }

/*****************************************************************************
 * XAMetadataExtractionItfImpl -specific methods
 *****************************************************************************/

/* XAMetadataExtractionItfImpl* XAMetadataExtractionItfImpl_Create()
 * Description: Allocate and initialize XAMetadataExtractionItfImpl
 */
XAMetadataExtractionItfImpl* XAMetadataExtractionItfImpl_Create(
        XAAdaptationBaseCtx *adaptCtx)
    {
    XAMetadataExtractionItfImpl *self = NULL;
    DEBUG_API("->XAMetadataExtractionItfImpl_Create");

    self = (XAMetadataExtractionItfImpl*) calloc(1,
            sizeof(XAMetadataExtractionItfImpl));

    if (self)
        {
        /* init itf default implementation */
        self->itf.GetItemCount = XAMetadataExtractionItfImpl_GetItemCount;
        self->itf.GetKeySize = XAMetadataExtractionItfImpl_GetKeySize;
        self->itf.GetKey = XAMetadataExtractionItfImpl_GetKey;
        self->itf.GetValueSize = XAMetadataExtractionItfImpl_GetValueSize;
        self->itf.GetValue = XAMetadataExtractionItfImpl_GetValue;
        self->itf.AddKeyFilter = XAMetadataExtractionItfImpl_AddKeyFilter;
        self->itf.ClearKeyFilter = XAMetadataExtractionItfImpl_ClearKeyFilter;

        /* init variables */
        self->filteredcount = 0;
        self->filteringOn = XA_BOOLEAN_FALSE;

        self->adaptCtx = adaptCtx;

        if (self->adaptCtx->fwtype != FWMgrFWMMF)
            {
            XAAdaptationBase_AddEventHandler(adaptCtx,
                    &XAMetadataExtractionItfImp_AdaptCb, XA_METADATAEVENTS,
                    self);
            }

        self->self = self;
        }

    DEBUG_API("<-XAMetadataExtractionItfImpl_Create");
    return self;
    }

/* void XAMetadataExtractionItfImpl_Free(XAMetadataExtractionItfImpl* self)
 * Description: Free all resources reserved at XAMetadataExtractionItfImpl_Create
 */
void XAMetadataExtractionItfImpl_Free(XAMetadataExtractionItfImpl* self)
    {
    DEBUG_API("->XAMetadataExtractionItfImpl_Free");
    assert(self==self->self);

    if (self->adaptCtx->fwtype != FWMgrFWMMF)
        {
        XAAdaptationBase_RemoveEventHandler(self->adaptCtx,
                &XAMetadataExtractionItfImp_AdaptCb);
        XAMetadataAdapt_FreeImplTagList(&(self->currentTags), XA_BOOLEAN_TRUE);

        if (self->tagmatchesfilter)
            {
            free(self->tagmatchesfilter);
            }
        }

    free(self);
    DEBUG_API("<-XAMetadataExtractionItfImpl_Free");
    }

/* With this method, adaptation infroms that new tags are found (e.g. if source,
 * has changed, live stream contains metadata...)
 */
void XAMetadataExtractionItfImp_AdaptCb(void *pHandlerCtx,
        XAAdaptEvent *event)
    {
    XAMetadataExtractionItfImpl* impl = NULL;
    DEBUG_API("->XAMetadataExtractionItfImp_AdaptCb");
    impl = (XAMetadataExtractionItfImpl*) pHandlerCtx;
    if (!impl)
        {
        DEBUG_ERR("XAMetadataExtractionItfImp_AdaptCb, invalid context pointer!");
        DEBUG_API("<-XAMetadataExtractionItfImp_AdaptCb");
        return;
        }
    if (event && event->eventid == XA_ADAPT_MDE_TAGS_AVAILABLE)
        {
        /* get the tag list */
        XAMetadataExtractionItfAdapt_FillTagList(
                (XAAdaptationGstCtx*) impl->adaptCtx, &(impl->currentTags));
        if (impl->tagmatchesfilter)
            {
            free(impl->tagmatchesfilter);
            }
        impl->tagmatchesfilter = calloc(impl->currentTags.itemcount,
                sizeof(XAboolean));
        impl->filteredcount = 0;
        }
    else
        {
        DEBUG_INFO("unhandled");
        }
    DEBUG_API("<-XAMetadataExtractionItfImp_AdaptCb");
    }

/* For given index over filtered array, return index over whole array
 */
XAresult CheckAndUnfilterIndex(XAMetadataExtractionItfImpl *impl,
        XAuint32 oldidx, XAuint32 *newidx)
    {
    DEBUG_API("->CheckAndUnfilterIndex");

    if (impl->filteringOn)
        {
        XAint16 i = -1;
        if (oldidx >= impl->filteredcount)
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-CheckAndUnfilterIndex");
            return XA_RESULT_PARAMETER_INVALID;
            }
        *newidx = 0;
        while (*newidx < impl->currentTags.itemcount)
            {
            if (impl->tagmatchesfilter[*newidx])
                i++;
            if (i < oldidx)
                (*newidx)++;
            else
                break;
            }
        if (*newidx == impl->currentTags.itemcount)
            {
            /* should not end up here */
            *newidx = 0;
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-CheckAndUnfilterIndex");
            return XA_RESULT_PARAMETER_INVALID;
            }
        }
    else
        {
        if (oldidx >= impl->currentTags.itemcount)
            {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-CheckAndUnfilterIndex");
            return XA_RESULT_PARAMETER_INVALID;
            }
        *newidx = oldidx;
        }

    DEBUG_API("<-CheckAndUnfilterIndex");
    return XA_RESULT_SUCCESS;
    }

