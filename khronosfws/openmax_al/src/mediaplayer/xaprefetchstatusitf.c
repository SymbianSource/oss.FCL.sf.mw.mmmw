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

#include "xaprefetchstatusitf.h"

#include "xaadaptationgst.h"


static XAPrefetchStatusItfImpl* GetImpl(XAPrefetchStatusItf self)
{
    if(self)
    {
        XAPrefetchStatusItfImpl* impl = (XAPrefetchStatusItfImpl*)(*self);
        if(impl && impl == impl->self)
        {
            return impl;
        }
    }
    return NULL;
}

XAresult XAPrefetchStatusItfImpl_GetPrefetchStatus(XAPrefetchStatusItf self,
                                                   XAuint32 *pStatus)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAPrefetchStatusItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPrefetchStatusItfImpl_GetPrefetchStatus");

    if(!impl || !pStatus )
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPrefetchStatusItfImpl_GetPrefetchStatus");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pStatus = impl->pStatus;

    DEBUG_API("<-XAPrefetchStatusItfImpl_GetPrefetchStatus");
    return ret;
}

XAresult XAPrefetchStatusItfImpl_GetFillLevel(XAPrefetchStatusItf self,
                                              XApermille *pLevel)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAPrefetchStatusItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPrefetchStatusItfImpl_GetFillLevel");

    if(!impl || !pLevel)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPrefetchStatusItfImpl_GetFillLevel");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pLevel = impl->pLevel;

    DEBUG_API("<-XAPrefetchStatusItfImpl_GetFillLevel");
    return ret;
}

XAresult XAPrefetchStatusItfImpl_RegisterCallback(XAPrefetchStatusItf self,
                                                  xaPrefetchCallback callback,
                                                  void *pContext)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAPrefetchStatusItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPrefetchStatusItfImpl_RegisterCallback");

    if(!impl)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPrefetchStatusItfImpl_RegisterCallback");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Can be null to reset callback */
    impl->callback = callback;
    impl->cbcontext = pContext;
    impl->cbPtrToSelf = self;

    DEBUG_API("<-XAPrefetchStatusItfImpl_RegisterCallback");
    return ret;
}

XAresult XAPrefetchStatusItfImpl_SetCallbackEventsMask(XAPrefetchStatusItf self,
                                                       XAuint32 eventFlags)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAPrefetchStatusItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPrefetchStatusItfImpl_SetCallbackEventsMask");

    if(!impl  )
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPrefetchStatusItfImpl_SetCallbackEventsMask");
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->eventFlags = eventFlags;

    DEBUG_API("<-XAPrefetchStatusItfImpl_SetCallbackEventsMask");
    return ret;
}

XAresult XAPrefetchStatusItfImpl_GetCallbackEventsMask(XAPrefetchStatusItf self,
                                                       XAuint32 *pEventFlags)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAPrefetchStatusItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPrefetchStatusItfImpl_GetCallbackEventsMask");

    if(!impl || !pEventFlags)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPrefetchStatusItfImpl_GetCallbackEventsMask");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pEventFlags = impl->eventFlags;

    DEBUG_API("<-XAPrefetchStatusItfImpl_GetCallbackEventsMask");
    return ret;
}

XAresult XAPrefetchStatusItfImpl_SetFillUpdatePeriod(XAPrefetchStatusItf self,
                                                     XApermille period)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAPrefetchStatusItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPrefetchStatusItfImpl_SetFillUpdatePeriod");

    if(!impl || period == 0 || period > 1000 )
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPrefetchStatusItfImpl_SetFillUpdatePeriod");
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->fillUpdatePeriod = period;

    DEBUG_API("<-XAPrefetchStatusItfImpl_SetFillUpdatePeriod");
    return ret;
}

XAresult XAPrefetchStatusItfImpl_GetFillUpdatePeriod(XAPrefetchStatusItf self,
                                                     XApermille *pPeriod)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAPrefetchStatusItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPrefetchStatusItfImpl_GetFillUpdatePeriod");

    if(!impl || !pPeriod)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPrefetchStatusItfImpl_GetFillUpdatePeriod");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pPeriod = impl->fillUpdatePeriod;

    DEBUG_API("<-XAPrefetchStatusItfImpl_GetFillUpdatePeriod");
    return ret;
}

/*
 * implementation-specific methods
 */
XAPrefetchStatusItfImpl* XAPrefetchStatusItfImpl_Create( XAMediaPlayerImpl* impl )
{
    XAPrefetchStatusItfImpl *self = (XAPrefetchStatusItfImpl*)
        calloc(1,sizeof(XAPrefetchStatusItfImpl));
    DEBUG_API("->XAPrefetchStatusItfImpl_Create");

    if(self)
    {
        /* init itf default implementation */
        self->itf.GetCallbackEventsMask = XAPrefetchStatusItfImpl_GetCallbackEventsMask;
        self->itf.GetFillLevel = XAPrefetchStatusItfImpl_GetFillLevel;
        self->itf.GetFillUpdatePeriod = XAPrefetchStatusItfImpl_GetFillUpdatePeriod;
        self->itf.GetPrefetchStatus = XAPrefetchStatusItfImpl_GetPrefetchStatus;
        self->itf.RegisterCallback = XAPrefetchStatusItfImpl_RegisterCallback;
        self->itf.SetCallbackEventsMask = XAPrefetchStatusItfImpl_SetCallbackEventsMask;
        self->itf.SetFillUpdatePeriod = XAPrefetchStatusItfImpl_SetFillUpdatePeriod;

        self->callback = NULL;
        self->cbcontext = NULL;
        self->eventFlags = 0;
        self->fillUpdatePeriod = PREFETCHSTATUSITF_DEFAULT_UPDATE_PERIOD;
        self->pStatus = XA_PREFETCHSTATUS_SUFFICIENTDATA;
        self->adaptationCtx = impl->curAdaptCtx;
        self->cbPtrToSelf = NULL;

        XAAdaptationBase_AddEventHandler( self->adaptationCtx, &XAPrefetchStatusItfImpl_AdaptCb, XA_PREFETCHITFEVENTS, self );

        self->self = self;
    }

    DEBUG_API("<-XAPrefetchStatusItfImpl_Create");
    return self;
}


void XAPrefetchStatusItfImpl_Free(XAPrefetchStatusItfImpl* self)
{
    DEBUG_API("->XAPrefetchStatusItfImpl_Free");
    assert(self==self->self);
    XAAdaptationBase_RemoveEventHandler( (XAAdaptationBaseCtx*)self->adaptationCtx, &XAPrefetchStatusItfImpl_AdaptCb );
    free(self);
    DEBUG_API("<-XAPrefetchStatusItfImpl_Free");
}

/* void XAPrefetchStatusItfImpl_AdaptCb
 * Description: Listen changes in adaptation
 */
void XAPrefetchStatusItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event )
{
    XAPrefetchStatusItfImpl* impl = (XAPrefetchStatusItfImpl*)pHandlerCtx;
    DEBUG_API("->XAPrefetchStatusItfImpl_AdaptCb");
    if(!impl)
    {
        DEBUG_ERR("XAPrefetchStatusItfImpl_AdaptCb, invalid context pointer!");
        DEBUG_API("<-XAPrefetchStatusItfImpl_AdaptCb");
        return;
    }
    assert(event);
    if( event->eventid == XA_ADAPT_BUFFERING )
    {
        /* Adaptation sends percents convert it to permille */
        impl->fillUpdatePeriod = ((*(XAint32*)(event->data))*10);

        /*Check do we have to send event */
        if( impl->callback && impl->eventFlags & XA_PREFETCHEVENT_FILLLEVELCHANGE )
        {
            if ( (*(XAint32*)event->data) >  impl->fillUpdatePeriod / 10 )
            {
                impl->callback(impl->cbPtrToSelf, impl->cbcontext, XA_PREFETCHEVENT_FILLLEVELCHANGE);
            }
        }
    }

    DEBUG_API("<-XAPrefetchStatusItfImpl_AdaptCb");
}

