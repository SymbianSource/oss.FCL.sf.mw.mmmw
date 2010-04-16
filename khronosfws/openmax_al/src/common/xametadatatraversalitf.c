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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "xametadatatraversalitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAMetadataAdaptation.h"
#endif
/* XAMetadataTraversalImpl* GetImpl(XAMetadataTraversalItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAMetadataTraversalImpl* GetImpl(XAMetadataTraversalItf self)
{
    if( self )
    {
        XAMetadataTraversalImpl* impl = (XAMetadataTraversalImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XAMetadataTraversalItf implementation
 *****************************************************************************/

/* XAresult XAMetadataTraversalItfImpl_SetMode
 * Description: Sets the metadata traversal mode
 */
XAresult XAMetadataTraversalItfImpl_SetMode(XAMetadataTraversalItf self,
                                            XAuint32 mode)
{
    XAMetadataTraversalImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataTraversalItfImpl_SetMode");
    impl = GetImpl(self);
    /* check parameters */
    if( !impl  ||
       (mode!=XA_METADATATRAVERSALMODE_NODE &&
        mode!=XA_METADATATRAVERSALMODE_ALL))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else if(impl->traversemode != mode)
    {
#ifdef _GSTREAMER_BACKEND_  
        res =XAMetadataTraversalItfAdapt_SetMode(impl->adaptCtx, mode);
#endif
        if( res == XA_RESULT_SUCCESS )
        {
            impl->traversemode = mode;
        }
    }
    else
    {
        /* do nothing */
    }

    DEBUG_API("<-XAMetadataTraversalItfImpl_SetMode");
    return res;
}

/* XAresult XAMetadataTraversalItfImpl_GetChildCount
 * Description: Returns the number of children (nodes, streams, etc.) within the current scope
 */
XAresult XAMetadataTraversalItfImpl_GetChildCount(XAMetadataTraversalItf self,
                                                  XAuint32 *pCount)
{
    XAMetadataTraversalImpl *impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataTraversalItfImpl_GetChildCount");
    impl = GetImpl(self);
    if( !impl || !pCount )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if(impl->traversemode==XA_METADATATRAVERSALMODE_ALL)
        {
            /* for this mode, child count is always 0 */
            *pCount = 0;
            res = XA_RESULT_SUCCESS;
        }
        else
        {
#ifdef _GSTREAMER_BACKEND_  
            res = XAMetadataTraversalItfAdapt_GetChildCount(impl->adaptCtx, pCount);
#endif
        }
    }

    DEBUG_API("<-XAMetadataTraversalItfImpl_GetChildCount");
    return res;
}

/* XAresult XAMetadataTraversalItfImpl_GetChildMIMETypeSize
 * Description: Returns the size in bytes needed to store the MIME type of a child
 */
XAresult XAMetadataTraversalItfImpl_GetChildMIMETypeSize(XAMetadataTraversalItf self,
                                                         XAuint32 index,
                                                         XAuint32 *pSize)
{
    XAMetadataTraversalImpl *impl = NULL;
#ifdef _GSTREAMER_BACKEND_  
    XAuint32 chCount = 0;
#endif    
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataTraversalItfImpl_GetChildMIMETypeSize");
    impl = GetImpl(self);

    if( !impl || !pSize)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
#ifdef _GSTREAMER_BACKEND_  
        res = XAMetadataTraversalItfAdapt_GetChildCount(impl->adaptCtx, &chCount);
        if(index >= chCount || res != XA_RESULT_SUCCESS)
        {
            /* out of bounds */
            res = XA_RESULT_PARAMETER_INVALID;
        }
        res = XAMetadataTraversalItfAdapt_GetChildMIMETypeSize(impl->adaptCtx, index, pSize);
#endif
    }

    DEBUG_API("<-XAMetadataTraversalItfImpl_GetChildMIMETypeSize");
    return res;
}

/* XAresult XAMetadataTraversalItfImpl_GetChildInfo
 * Description: Returns information about a child
 */
XAresult XAMetadataTraversalItfImpl_GetChildInfo(XAMetadataTraversalItf self,
                                                 XAuint32 index,
                                                 XAint32 *pNodeID,
                                                 XAuint32 *pType,
                                                 XAuint32 size,
                                                 XAchar *pMimeType)
{
    XAMetadataTraversalImpl *impl = NULL;
#ifdef _GSTREAMER_BACKEND_  
    XAuint32 chCount = 0;
#endif
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataTraversalItfImpl_GetChildInfo");
    impl = GetImpl(self);
    if( !impl || !pNodeID || !pType )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
#ifdef _GSTREAMER_BACKEND_  
        res = XAMetadataTraversalItfAdapt_GetChildCount(impl->adaptCtx, &chCount);
        if(index >= chCount || res != XA_RESULT_SUCCESS)
        {
            /* out of bounds */
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAMetadataTraversalItfImpl_GetChildInfo");
            return XA_RESULT_PARAMETER_INVALID;
        }
        res = XAMetadataTraversalItfAdapt_GetChildInfo(impl->adaptCtx, index,
                                                pNodeID, pType, size, pMimeType);
#endif
    }

    DEBUG_API("<-XAMetadataTraversalItfImpl_GetChildInfo");
    return res;
}

/* XAresult XAMetadataTraversalItfImpl_SetActiveNode
 * Description: Sets the scope to a child node
 */
XAresult XAMetadataTraversalItfImpl_SetActiveNode(XAMetadataTraversalItf self,
                                                  XAuint32 index)
{
    XAMetadataTraversalImpl *impl = NULL;
#ifdef _GSTREAMER_BACKEND_  
    XAuint32 chCount = 0;
#endif
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataTraversalItfImpl_SetActiveNode");
    impl = GetImpl(self);
    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
#ifdef _GSTREAMER_BACKEND_  
        res = XAMetadataTraversalItfAdapt_GetChildCount(impl->adaptCtx, &chCount);
        if( res == XA_RESULT_SUCCESS )
        {
            if((impl->nodedepth==0 && index==XA_NODE_PARENT) ||
                (index >= chCount && index!=XA_NODE_PARENT))
            {
                /* try to ascend from root or descend to nonexistend child node */
                res = XA_RESULT_PARAMETER_INVALID;
            }
            else
            {
                /* update node and childs */
                res = XAMetadataTraversalItfAdapt_SetActiveNode(impl->adaptCtx, index);
                if( res == XA_RESULT_SUCCESS )
                {
                    if(index==XA_NODE_PARENT)
                    {
                        impl->nodedepth++;
                    }
                    else
                    {
                        impl->nodedepth--;
                    }
                }
            }
        }
        else
        {
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("<-XAMetadataTraversalItfImpl_SetActiveNode");
            return XA_RESULT_INTERNAL_ERROR;
        }
#endif        
    }

    DEBUG_API("<-XAMetadataTraversalItfImpl_SetActiveNode");
    return res;
}

/*****************************************************************************
 * XAMetadataTraversalImpl -specific methods
 *****************************************************************************/
#ifdef _GSTREAMER_BACKEND_  

/* XAMetadataTraversalImpl* XAMetadataTraversalItfImpl_Create()
 * Description: Allocate and initialize XAMetadataTraversalImpl
 */
XAMetadataTraversalImpl* XAMetadataTraversalItfImpl_Create( XAAdaptationBaseCtx *adaptCtx )
{
    XAMetadataTraversalImpl *self = NULL;
    DEBUG_API("->XAMetadataTraversalItfImpl_Create");

    self = (XAMetadataTraversalImpl*) calloc(1,sizeof(XAMetadataTraversalImpl));

    if( self )
    {
        /* init itf default implementation */
        self->itf.SetMode = XAMetadataTraversalItfImpl_SetMode;
        self->itf.GetChildCount = XAMetadataTraversalItfImpl_GetChildCount;
        self->itf.GetChildMIMETypeSize = XAMetadataTraversalItfImpl_GetChildMIMETypeSize;
        self->itf.GetChildInfo = XAMetadataTraversalItfImpl_GetChildInfo;
        self->itf.SetActiveNode = XAMetadataTraversalItfImpl_SetActiveNode;

        /* init variables */
        self->adaptCtx = adaptCtx;

        self->self = self;
    }
    DEBUG_API("<-XAMetadataTraversalItfImpl_Create");
    return self;
}
#endif
/* void XAMetadataTraversalItfImpl_Free(XAMetadataTraversalImpl* self)
 * Description: Free all resources reserved at XAMetadataTraversalItfImpl_Create
 */
void XAMetadataTraversalItfImpl_Free(XAMetadataTraversalImpl* self)
{
    DEBUG_API("->XAMetadataTraversalItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAMetadataTraversalItfImpl_Free");
}
