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

#include "xaoutputmixitf.h"

#include "xaoutputmixitfadaptation.h"

#include <assert.h>
#include "xathreadsafety.h"
#include "e32def.h"
/**
 * XAOutputMixtfImpl* GetImpl(XAOutputMixtfImpl self)
 * Description: Validated interface pointer and cast it to implementations pointer.
 **/
static XAOutputMixItfImpl* GetImpl(XAOutputMixItf self)
{
    if(self)
    {
        XAOutputMixItfImpl* impl = (XAOutputMixItfImpl*)(*self);
        if(impl && impl == impl->self)
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAOutputMixtf implementation
 */

/*
 * XAresult XAOutputMixItfImpl_GetDestinationOutputDeviceIDs( XAOutputMixItf self,
 *                                                          XAint32 * pNumDevices,
 *                                                          XAuint32 * pDeviceIDs )
 * Description: Retrieves the device IDs of the destination output devices currently
 * associated with the output mix.
 */
XAresult XAOutputMixItfImpl_GetDestinationOutputDeviceIDs( XAOutputMixItf self, XAint32 * pNumDevices, XAuint32 * pDeviceIDs )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAOutputMixItfImpl* impl = GetImpl(self);
    DEBUG_API_A1("->XAOutputMixItfImpl_GetDestinationOutputDeviceIDs - pNumDevices %ld",*pNumDevices);
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSOutputMix);
    if(!impl || !pNumDevices  )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAOutputMixItfImpl_GetDestinationOutputDeviceIDs");
        return XA_RESULT_PARAMETER_INVALID;
    }


    ret = XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs((XAAdaptationGstCtx*)impl->adapCtx, pNumDevices, pDeviceIDs );

    XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
    DEBUG_API_A1("<-XAOutputMixItfImpl_GetDestinationOutputDeviceIDs - pNumDevices %ld",*pNumDevices);
    return ret;
}

/*
 * XAresult XAOutputMixItfImpl_RegisterDeviceChangeCallback( XAOutputMixItf self,
 *                                                          xaMixDeviceChangeCallback callback,
 *                                                          void * pContext)
 * Description: Registers a callback to notify client when there are changes to the
 * device IDs associated with the output mix.
 */
XAresult XAOutputMixItfImpl_RegisterDeviceChangeCallback( XAOutputMixItf self, xaMixDeviceChangeCallback callback, void * pContext)
{
   XAresult ret = XA_RESULT_SUCCESS;
   XAOutputMixItfImpl* impl = GetImpl(self);
   DEBUG_API("->XAOutputMixItfImpl_RegisterDeviceChangeCallback");
   if(!impl)
   {
       /* invalid parameter */
       DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
       DEBUG_API("<-XAOutputMixItfImpl_RegisterDeviceChangeCallback");
       return XA_RESULT_PARAMETER_INVALID;
   }

   /* callback may be NULL (to remove callback) */
   impl->callback = callback;
   impl->cbContext = pContext;
   impl->cbPtrToSelf = self;

   if(callback)
   {   /* start listening */
       XAAdaptationBase_AddEventHandler( impl->adapCtx, &XAOutputMixItfImpl_AdaptCb, XA_OUTPUTMIXITFEVENTS, (void*)self );
   }
   else
   {   /* stop listening */
       XAAdaptationBase_RemoveEventHandler( impl->adapCtx, &XAOutputMixItfImpl_AdaptCb );
   }


   DEBUG_API("<-XAOutputMixItfImpl_RegisterDeviceChangeCallback");
   return ret;
}

/*
 * XAresult XAOutputMixItfImpl_ReRoute( XAOutputMixItf self,
 *                                      XAint32 numOutputDevices,
 *                                      XAuint32 * pOutputDeviceIDs)
 * Description: Requests a change to the specified set of output devices on an output mix.
 */
XAresult XAOutputMixItfImpl_ReRoute( XAOutputMixItf self, XAint32 numOutputDevices, XAuint32 * pOutputDeviceIDs)
{
   XAresult ret = XA_RESULT_SUCCESS;
   XAOutputMixItfImpl* impl = GetImpl(self);

   DEBUG_API("->XAOutputMixItfImpl_ReRoute");
   XA_IMPL_THREAD_SAFETY_ENTRY(XATSOutputMix);

   if(!impl || ( numOutputDevices > 0 && !pOutputDeviceIDs) )
   {
       /* invalid parameter */
       XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
       DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
       DEBUG_API("<-XAOutputMixItfImpl_ReRoute");
       return XA_RESULT_PARAMETER_INVALID;
   }


   ret = XAOutputMixItfAdapt_ReRoute((XAAdaptationGstCtx*)impl->adapCtx, numOutputDevices, pOutputDeviceIDs );


   XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
   DEBUG_API("<-XAOutputMixItfImpl_ReRoute");
   return ret;

}

/**
 * XAVolumeItfImpl -specific methods
 **/


/*
 * XAOutputMixItfImpl* XAOutputMixItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
 * Description: Creates new Output mix itf implmementation
 */
XAOutputMixItfImpl* XAOutputMixItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
{
   XAOutputMixItfImpl *self = (XAOutputMixItfImpl*)
        calloc(1,sizeof(XAOutputMixItfImpl));
   DEBUG_API("->XAVolumeItfImpl_Create");

    if(self)
    {
        /* init itf default implementation */
        self->itf.GetDestinationOutputDeviceIDs = XAOutputMixItfImpl_GetDestinationOutputDeviceIDs;
        self->itf.RegisterDeviceChangeCallback = XAOutputMixItfImpl_RegisterDeviceChangeCallback;
        self->itf.ReRoute = XAOutputMixItfImpl_ReRoute;

        /* init variables */
        self->adapCtx = adapCtx;
        self->callback = NULL;
        self->cbContext = NULL;
        self->cbPtrToSelf = NULL;

        self->self = self;
    }

    DEBUG_API("<-XAVolumeItfImpl_Create");
    return self;
}

/*
 * void XAOutputMixItfImpl_Free(XAOutputMixItfImpl* self)
 * Description: Frees XAOutputMixItfImpl
 */
void XAOutputMixItfImpl_Free(XAOutputMixItfImpl* self)
{
    DEBUG_API("->XAOutputMixItfImpl_Free");
    assert(self==self->self);
    if( self->callback )
    {
        XAAdaptationBase_RemoveEventHandler( self->adapCtx, &XAOutputMixItfImpl_AdaptCb );
    }
    free(self);
    DEBUG_API("<-XAOutputMixItfImpl_Free");
}



/* void XAOutputMixItfImpl_AdaptCb( void *pHandlerCtx,
 *                                  XAAdaptEvent *event )
 * Description: Listen changes in adaptation
 */
void XAOutputMixItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event )
{
     XAOutputMixItfImpl* impl = (XAOutputMixItfImpl*)pHandlerCtx;
     DEBUG_API("->XAOutputMixItfImpl_AdaptCb");

     if(!impl)
     {
        DEBUG_ERR("XAOutputMixItfImpl_AdaptCb, invalid context pointer!");
        return;
     }
     assert(event);

     /* send callback if needed.
      * Check event-id to avoid sending incorrect events. */
     if( (event->eventid==XA_ADAPT_OMIX_DEVICESET_CHANGED) && impl->callback )
     {
        impl->callback(impl->cbPtrToSelf, impl->cbContext);
     }

     DEBUG_API("<-XAOutputMixItfImpl_AdaptCb");
}

