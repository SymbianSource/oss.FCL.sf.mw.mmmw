/**
 * src\common\XAStreamInformationItf.c
 * Part of OpenMAX AL Implementation
 * Copyright (c) 2009 Nokia Inc.
 *
 * This file contains implementation of XAStreamInformationItf
 **/

#include <assert.h>

#include "xastreaminformationitf.h"
#include "xastreaminformationitfadaptation.h"
#include "xastreaminformationitfadaptationmmf.h"

/* XAStreamInformationItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAStreamInformationItfImpl* GetImpl(XAStreamInformationItf self)
{
    if(self)
    {
        XAStreamInformationItfImpl* impl = (XAStreamInformationItfImpl*)(*self);
        if(impl && impl == impl->self)
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAPlayItf implementation
 */


XAresult XAStreamInformationItfImpl_QueryMediaContainerInformation(
                XAStreamInformationItf self,
                XAMediaContainerInformation * info)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAStreamInformationItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAStreamInformationItfImpl_QueryMediaContainerInformation");
    if(!impl || !info)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfImpl_QueryMediaContainerInformation");
        return XA_RESULT_PARAMETER_INVALID;
    }


    if(impl->adapCtx->fwtype == FWMgrFWGST)
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryMediaContainerInformation");
            return ret;
        }
        ret = XAStreamInformationItfAdapt_QueryMediaContainerInformation(
                    impl->adapCtx,
                    &(info->containerType),
                    &(info->mediaDuration),
                    &(info->numStreams));
        XAAdaptationBase_ThreadExit(impl->adapCtx);
        }
    else
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryMediaContainerInformation");
            return ret;
        }
        ret = XAStreamInformationItfAdaptMMF_QueryMediaContainerInformation(
                    impl->adapCtx,
                    &(info->containerType),
                    &(info->mediaDuration),
                    &(info->numStreams));
        XAAdaptationBase_ThreadExit(impl->adapCtx);;
        }

    DEBUG_API("-<XAStreamInformationItfImpl_QueryMediaContainerInformation");
    return ret;
}

XAresult XAStreamInformationItfImpl_QueryStreamType(
                XAStreamInformationItf self,
                XAuint32 streamIndex, 
                XAuint32 *domain)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAStreamInformationItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAStreamInformationItfImpl_QueryStreamType");
    if(!impl || !domain || (streamIndex == 0))
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfImpl_QueryStreamType");
        return XA_RESULT_PARAMETER_INVALID;
    }
    if(impl->adapCtx->fwtype == FWMgrFWGST)
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryStreamType");
            return ret;
        }
    
        ret = XAStreamInformationItfAdapt_QueryStreamType(
                    impl->adapCtx,
                    streamIndex,
                    domain);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);
        }
    else
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryStreamType");
            return ret;
        }
    
        ret = XAStreamInformationItfAdaptMMF_QueryStreamType(
                    impl->adapCtx,
                    streamIndex,
                    domain);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);;
        }
    DEBUG_API("-<XAStreamInformationItfImpl_QueryStreamType");
    return ret;
}

XAresult XAStreamInformationItfImpl_QueryStreamInformation(
                XAStreamInformationItf self, 
                XAuint32 streamIndex,
                void * info)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAStreamInformationItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAStreamInformationItfImpl_QueryStreamInformation");
    if(!impl || !info || (streamIndex == 0))
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfImpl_QueryStreamInformation");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(impl->adapCtx->fwtype == FWMgrFWGST)
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryStreamInformation");
            return ret;
        }
    
        ret = XAStreamInformationItfAdapt_QueryStreamInformation(
                    impl->adapCtx,
                    streamIndex,
                    info);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);
        }
    else
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryStreamInformation");
            return ret;
        }
    
        ret = XAStreamInformationItfAdaptMMF_QueryStreamInformation(
                    impl->adapCtx,
                    streamIndex,
                    info);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);;
        }

    DEBUG_API("-<XAStreamInformationItfImpl_QueryStreamInformation");
    return ret;
}

XAresult XAStreamInformationItfImpl_QueryStreamName(
                XAStreamInformationItf self, 
                XAuint32 streamIndex, 
                XAuint16 * pNameSize,
                XAchar * pName)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAStreamInformationItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAStreamInformationItfImpl_QueryStreamName");
    if(!impl || (streamIndex == 0) || !pNameSize)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfImpl_QueryStreamName");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(impl->adapCtx->fwtype == FWMgrFWGST)
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryStreamName");
            return ret;
        }
    
        ret = XAStreamInformationItfAdapt_QueryStreamName(
                    impl->adapCtx,
                    streamIndex,
                    pNameSize,
                    pName);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);
        }
    else
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryStreamName");
            return ret;
        }
    
        ret = XAStreamInformationItfAdaptMMF_QueryStreamName(
                    impl->adapCtx,
                    streamIndex,
                    pNameSize,
                    pName);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);;
        }
    DEBUG_API("-<XAStreamInformationItfImpl_QueryStreamName");
    return ret;
}

XAresult XAStreamInformationItfImpl_RegisterStreamChangeCallback(
                XAStreamInformationItf self,
                xaStreamEventChangeCallback callback,
                void * pContext)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAStreamInformationItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAStreamInformationItfImpl_RegisterStreamChangeCallback");
    if(!impl)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfImpl_RegisterStreamChangeCallback");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* callback may be NULL (to remove callback) */
    impl->callback = callback;
    impl->cbcontext = pContext;
    impl->cbPtrToSelf = self;

    DEBUG_API("-<XAStreamInformationItfImpl_RegisterStreamChangeCallback");
    return ret;
}

XAresult XAStreamInformationItfImpl_QueryActiveStreams(
                XAStreamInformationItf self,
                XAuint32 *numStreams,
                XAboolean *activeStreams)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAStreamInformationItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAStreamInformationItfImpl_QueryActiveStreams");
    if(!impl || !numStreams)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfImpl_QueryActiveStreams");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(impl->adapCtx->fwtype == FWMgrFWGST)
        {    
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryActiveStreams");
            return ret;
        }
    
        ret = XAStreamInformationItfAdapt_QueryActiveStreams(
                    impl->adapCtx,
                    numStreams,
                    activeStreams);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);
        }
    else
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_QueryActiveStreams");
            return ret;
        }
    
        ret = XAStreamInformationItfAdaptMMF_QueryActiveStreams(
                    impl->adapCtx,
                    numStreams,
                    activeStreams);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);;
        }

    DEBUG_API("-<XAStreamInformationItfImpl_QueryActiveStreams");
    return ret;
}

XAresult XAStreamInformationItfImpl_SetActiveStream(
                XAStreamInformationItf self, 
                XAuint32 streamNum,
                XAboolean active, 
                XAboolean commitNow)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAStreamInformationItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAStreamInformationItfImpl_SetActiveStream");
    if(!impl)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfImpl_SetActiveStream");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(impl->adapCtx->fwtype == FWMgrFWGST)
        {      
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_SetActiveStream");
            return ret;
        }
    
        ret = XAStreamInformationItfAdapt_SetActiveStream(
                    impl->adapCtx,
                    streamNum,
                    active, 
                    commitNow);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);
        }
    else
        {
        ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
        if( ret == XA_RESULT_PARAMETER_INVALID )
        {
            DEBUG_API("<-XAStreamInformationItfImpl_SetActiveStream");
            return ret;
        }
    
        ret = XAStreamInformationItfAdaptMMF_SetActiveStream(
                    impl->adapCtx,
                    streamNum,
                    active, 
                    commitNow);
    
        XAAdaptationBase_ThreadExit(impl->adapCtx);;
        }

    DEBUG_API("-<XAStreamInformationItfImpl_SetActiveStream");
    return ret;
}

/**
 * XAStreamInformationItfImpl -specific methods
 **/

/**
 * XAStreamInformationItfImpl* XAStreamInformationItfImpl_Create()
 * Description: Allocate and initialize PlayItfImpl
 **/
XAStreamInformationItfImpl* XAStreamInformationItfImpl_Create( XAAdaptationBaseCtx *adapCtx )
{
    XAStreamInformationItfImpl *self;

    DEBUG_API("->XAStreamInformationItfImpl_Create");
    self = (XAStreamInformationItfImpl*)calloc(1,sizeof(XAStreamInformationItfImpl));
    if(self)
    {
        /* init itf default implementation */
        self->itf.QueryMediaContainerInformation = XAStreamInformationItfImpl_QueryMediaContainerInformation;
        self->itf.QueryStreamType = XAStreamInformationItfImpl_QueryStreamType;
        self->itf.QueryStreamInformation = XAStreamInformationItfImpl_QueryStreamInformation;
        self->itf.QueryStreamName = XAStreamInformationItfImpl_QueryStreamName;
        self->itf.RegisterStreamChangeCallback = XAStreamInformationItfImpl_RegisterStreamChangeCallback;
        self->itf.QueryActiveStreams = XAStreamInformationItfImpl_QueryActiveStreams;
        self->itf.SetActiveStream = XAStreamInformationItfImpl_SetActiveStream;

        /* init variables */
        self->cbPtrToSelf = NULL;
        self->callback = NULL;
        self->cbcontext = NULL;
        self->adapCtx = adapCtx;

        self->self = self;
    }

    DEBUG_API("<-XAStreamInformationItfImpl_Create");
    return self;
}

/* void XAStreamInformationItfImpl_Free(XAStreamInformationItfImpl* self)
 * Description: Free all resources reserved at XAStreamInformationItfImpl_Create
 */
void XAStreamInformationItfImpl_Free(XAStreamInformationItfImpl* self)
{
    DEBUG_API("->XAStreamInformationItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAStreamInformationItfImpl_Free");
}


