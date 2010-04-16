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
#include "xasnapshotitf.h"
#ifdef _GSTREAMER_BACKEND_
#include "XASnapShotItfAdaptation.h"
#endif
#include "xathreadsafety.h"
#include "xaplayitf.h"
#include "xaglobals.h"

extern void* vfHandle;

/**
 * static XASnapshotItfImpl* GetImpl(XASnapshotItf self)
 * Description: Validated interface pointer and cast it to implementations pointer.
 **/
static XASnapshotItfImpl* GetImpl(XASnapshotItf self)
{
    if( self )
    {
        XASnapshotItfImpl* impl = (XASnapshotItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XASnapshotItf implementation
 */

/*
    This method prepares the device for snapshot to shorten the actual shooting
    delay with TakeSnapshot() method. The various settings for snapshot are
    set with this method: the specified number of snapshots, the output location
    either to the XADataSink (if it is specified) or to memory (if no XADataSink
    is specified) and then calls xaSnapshotInitiatedCallback() method.
    Asynchronous.
    Second call of this method before the call to TakeSnapshot() method will
    reinitialize the shooting with the new parameter values.
*/
XAresult XASnapshotItfImpl_InitiateSnapshot(XASnapshotItf self,
                                            XAuint32 numberOfPictures,
                                            XAuint32 fps,
                                            XAboolean freezeViewFinder,
                                            XADataSink sink,
                                            xaSnapshotInitiatedCallback initiatedCallback,
                                            xaSnapshotTakenCallback takenCallback,
                                            void * pContext)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 minF = 0, maxF = 0, maxP = 0;

    DEBUG_API("->XASnapshotItfImpl_InitiateSnapshot");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);

    if(!impl)
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfImpl_InitiateSnapshot");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_
    XASnapshotItfAdaptation_GetBurstFPSRange(impl->adapCtx,&minF,&maxF);
    XASnapshotItfAdaptation_GetMaxPicsPerBurst(impl->adapCtx,&maxP);
#endif
    /*Check attributes and initialize local variables*/
    /*NOTE: bug in spec? - should sink attribute be a pointer?*/
    if( numberOfPictures > maxP ||
        (numberOfPictures!=1 && !(sink.pLocator) ) )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_ERR("XASnapshotItfImpl_InitiateSnapshot -"
                        "XA_RESULT_PARAMETER_INVALID (numberOfPictures)");
        DEBUG_API("<-XASnapshotItfImpl_InitiateSnapshot");
        return XA_RESULT_PARAMETER_INVALID;
    }
    if( numberOfPictures!=1 && (fps<minF || fps>maxF) )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_ERR("XASnapshotItfImpl_InitiateSnapshot -"
                        "XA_RESULT_PARAMETER_INVALID (fps)");
        DEBUG_API("<-XASnapshotItfImpl_InitiateSnapshot");
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->numberofpictures = numberOfPictures;
    impl->usercontext = pContext;
    impl->initcallback = initiatedCallback;
    impl->takencallback = takenCallback;
    impl->cbPtrToSelf = self;
    impl->freezevf = freezeViewFinder;

#ifdef _GSTREAMER_BACKEND_

    if( impl->initongoing ||
        impl->initialized ||
        impl->snapshotongoing )
    {
        res = XASnapshotItfAdaptation_CancelSnapshot(impl->adapCtx);
        impl->initongoing = XA_BOOLEAN_FALSE;
        impl->snapshotongoing = XA_BOOLEAN_FALSE;
    }

    /* Initialize snapshot to adaptation */
    if( impl->adapCtx )
    {
        impl->initongoing = XA_BOOLEAN_TRUE;
        res = XASnapshotItfAdaptation_InitiateSnapshot(impl->adapCtx,
                                                       numberOfPictures,
                                                       fps,
                                                       freezeViewFinder,
                                                       &sink);
    }
    else
    {
        res = XA_RESULT_INTERNAL_ERROR;
    }
#endif
    XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    DEBUG_API_A1("<-XASnapshotItfImpl_InitiateSnapshot (%d)", (int)res);
    return res;
}

/*
    This method takes the specified number of snapshots, stores them either to
    the XADataSink (if it is specified by InitiateSnapshot()) or to memory (if
    no XADataSink is specified) and then calls xaSnapshotTakenCallback()
    method.
 */
XAresult XASnapshotItfImpl_TakeSnapshot(XASnapshotItf self)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XASnapshotItfImpl_TakeSnapshot");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);
    if(!impl)
    {
        DEBUG_ERR("XASnapshotItfImpl_TakeSnapshot -"
                "XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        res = XA_RESULT_PARAMETER_INVALID;
    }

    if( impl->initongoing ||
        impl->snapshotongoing ||
        !impl->initialized )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_ERR("XASnapshotItfImpl_TakeSnapshot -"
                        "XA_RESULT_PRECONDITIONS_VIOLATED");
        DEBUG_API("<-XASnapshotItfImpl_TakeSnapshot");
        return XA_RESULT_PRECONDITIONS_VIOLATED;
    }

    if( impl->numberofpictures == 0 )
    {
        if( impl->takencallback )
        {
            impl->takencallback(impl->cbPtrToSelf, impl->usercontext,
                                impl->numberofpictures, NULL);
        }
        res = XA_RESULT_SUCCESS;
    }
#ifdef _GSTREAMER_BACKEND_
    /*Inform adaptation to take snapshot*/
    else if( impl->adapCtx )
    {
        impl->snapshotongoing = XA_BOOLEAN_TRUE;
        res = XASnapshotItfAdaptation_TakeSnapshot(impl->adapCtx);
        if( res!=XA_RESULT_SUCCESS )
        {
            DEBUG_ERR_A1("Adaptation returns error %d taking snapshot!", (int)res);
            impl->snapshotongoing = XA_BOOLEAN_FALSE;
        }
    }
#endif    
    else
    {
        res = XA_RESULT_INTERNAL_ERROR;
        DEBUG_ERR("XASnapshotItfImpl_TakeSnapshot"
                "-XA_RESULT_INTERNAL_ERROR ");
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    DEBUG_API("<-XASnapshotItfImpl_TakeSnapshot");
    return res;
}

/*
    This method cancels an ongoing shooting session. Snapshooting needs to be
    initiated again after calling this method with InitiateSnapshot method.

    Synchronous.
*/
XAresult XASnapshotItfImpl_CancelSnapshot(XASnapshotItf self)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XASnapshotItfImpl_CancelSnapshot");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);

    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfImpl_CancelSnapshot");
        /* invalid parameter */
        res = XA_RESULT_PARAMETER_INVALID;
    }

    if( impl->initongoing ||
        impl->snapshotongoing ||
        impl->initialized )
    {
#ifdef _GSTREAMER_BACKEND_
        if( impl->adapCtx )
        {
            res = XASnapshotItfAdaptation_CancelSnapshot(impl->adapCtx);
        }
        else
        {
            DEBUG_ERR("No adaptation context!!");
            res = XA_RESULT_INTERNAL_ERROR;
        }
#endif        
        impl->initongoing = XA_BOOLEAN_FALSE;
        impl->snapshotongoing = XA_BOOLEAN_FALSE;
        impl->initialized = XA_BOOLEAN_FALSE;
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    DEBUG_API_A1("<-XASnapshotItfImpl_CancelSnapshot (%d)", (int)res);
    return res;
}

/*
     This method releases the given buffer.
 */
XAresult XASnapshotItfImpl_ReleaseBuffers(XASnapshotItf self,
                                          XADataSink *image)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XASnapshotItfImpl_ReleaseBuffers");
    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        res =  XA_RESULT_PARAMETER_INVALID;
    }
    if( image && (image->pLocator) &&
        *((XAuint32*)(image->pLocator)) == XA_DATALOCATOR_ADDRESS )
    {
        if( ((XADataLocator_Address*)(image->pLocator))->pAddress )
        {
            free(((XADataLocator_Address*)(image->pLocator))->pAddress);
        }
        image->pLocator = NULL;
        image->pFormat = NULL;
        DEBUG_API("<-XASnapshotItfImpl_ReleaseBuffers- buffers released");
        res = XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR_A1("<-XASnapshotItfImpl_ReleaseBuffers- "
                "INVALID XADataSink at 0x%x", (int)image);
        res = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XASnapshotItfImpl_ReleaseBuffers");
    return res;
}

/*
    This method tells how many pictures it is possible to
    be taken during single burst.
*/
XAresult XASnapshotItfImpl_GetMaxPicsPerBurst(XASnapshotItf self,
                                              XAuint32 *maxNumberOfPictures)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XASnapshotItfImpl_GetMaxPicsPerBurst");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);
    if( !impl || !maxNumberOfPictures )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfImpl_GetMaxPicsPerBurst");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_
    if( impl->adapCtx )
    {
        res = XASnapshotItfAdaptation_GetMaxPicsPerBurst(impl->adapCtx, maxNumberOfPictures);
    }
    else
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_API("<-XASnapshotItfImpl_GetMaxPicsPerBurst");
        return XA_RESULT_INTERNAL_ERROR;
    }
#endif
    XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    DEBUG_API("<-XASnapshotItfImpl_GetMaxPicsPerBurst");
    return res;
}

/*
    This method tells the range of shooting rates possible in burst shooting
    mode. Please note that these rates might be different depending on which
    encoder and which resolution has been chosen; not all the rates can
    necessarily be reached with every resolution or encoder.
*/
XAresult XASnapshotItfImpl_GetBurstFPSRange(XASnapshotItf self,
                                            XAuint32 *minFPS,
                                            XAuint32 *maxFPS)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XASnapshotItfImpl_GetBurstFPSRange");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);
    if( !impl || !minFPS || !maxFPS )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfImpl_GetBurstFPSRange");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_
    if( impl->adapCtx )
    {
        res = XASnapshotItfAdaptation_GetBurstFPSRange(impl->adapCtx,minFPS,maxFPS);
    }
    else
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_API("<-XASnapshotItfImpl_GetBurstFPSRange");
        return XA_RESULT_INTERNAL_ERROR;
    }
#endif
    XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    DEBUG_API("<-XASnapshotItfImpl_GetBurstFPSRange");
    return res;
}

/*
    Toggles the shutter feedback (such as shutter sound or some visual
    feedback while taking a snapshot).
*/
XAresult XASnapshotItfImpl_SetShutterFeedback(XASnapshotItf self,
                                              XAboolean enabled)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XASnapshotItfImpl_SetShutterFeedback");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);
    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        res = XA_RESULT_PARAMETER_INVALID;
    }
    impl->shutterfeedback = enabled;
    
#ifdef _GSTREAMER_BACKEND_
    if( impl->adapCtx )
    {
        /*Inform adaptation about shutter feedback */
        res = XASnapshotItfAdaptation_SetShutterFeedback(impl->adapCtx, enabled);
    }
#endif
    XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    DEBUG_API("<-XASnapshotItfImpl_SetShutterFeedback");
    return res;
}

/*
    This method tells if the shutter feedback (such as shutter sound or some
    visual feedback while taking a snapshot) is enabled.
 */
XAresult XASnapshotItfImpl_GetShutterFeedback(XASnapshotItf self,
                                              XAboolean *enabled)
{
    XASnapshotItfImpl *impl = GetImpl(self);
    DEBUG_API("->XASnapshotItfImpl_GetShutterFeedback");
    if( !impl || !enabled )
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XASnapshotItfImpl_GetShutterFeedback");
        return  XA_RESULT_PARAMETER_INVALID;
    }
    enabled = (XAboolean*)impl->shutterfeedback;

    DEBUG_API("<-XASnapshotItfImpl_GetShutterFeedback");
    return XA_RESULT_SUCCESS;
}

/**
 * XASnapshotItfImpl -specific methods
 **/


/**
 * XASnapshotItfImpl* XASnapshotItfImpl_Create()
 * Description: Allocate and initialize SnapshotItfImpl
 **/
XASnapshotItfImpl* XASnapshotItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBaseCtx *adaptCtx
#endif
        )
{
    XASnapshotItfImpl* self = (XASnapshotItfImpl*)
        calloc(1,sizeof(XASnapshotItfImpl));
    DEBUG_API("->XASnapshotItfImpl_Create");
    if( self )
    {
        /* init itf default implementation */
        self->itf.CancelSnapshot = XASnapshotItfImpl_CancelSnapshot;
        self->itf.GetBurstFPSRange = XASnapshotItfImpl_GetBurstFPSRange;
        self->itf.GetMaxPicsPerBurst = XASnapshotItfImpl_GetMaxPicsPerBurst;
        self->itf.GetShutterFeedback = XASnapshotItfImpl_GetShutterFeedback;
        self->itf.InitiateSnapshot = XASnapshotItfImpl_InitiateSnapshot;
        self->itf.ReleaseBuffers = XASnapshotItfImpl_ReleaseBuffers;
        self->itf.SetShutterFeedback = XASnapshotItfImpl_SetShutterFeedback;
        self->itf.TakeSnapshot = XASnapshotItfImpl_TakeSnapshot;

        /* init variables */
#ifdef _GSTREAMER_BACKEND_
        self->adapCtx = adapCtx;
#endif
        self->self = self;
        self->cbPtrToSelf = NULL;
    }

#ifdef _GSTREAMER_BACKEND_
    /* Add call back function to eventhandler. */
    XAAdaptationBase_AddEventHandler( adapCtx, &XASnapshotItfImpl_AdaptCb, XA_SNAPSHOTITFEVENTS, self );
#endif
    DEBUG_API("<-XASnapshotItfImpl_Create");
    return self;
}

/**
 * void XASnapshotItfImpl_Free(XASnapshotItfImpl* self)
 * Description: Free all resources reserved at XASnapshotItfImpl_Create()
 **/
void XASnapshotItfImpl_Free(XASnapshotItfImpl* self)
{
    DEBUG_API("->XASnapshotItfImpl_Free");
    assert( self==self->self );
    /*
     * Free all resources reserved at XASnapshotItfImpl_Create
     */
#ifdef _GSTREAMER_BACKEND_
    if( self->adapCtx )
    {
        XASnapshotItfAdaptation_CancelSnapshot(self->adapCtx);
    }
    XAAdaptationBase_RemoveEventHandler( self->adapCtx, &XASnapshotItfImpl_AdaptCb );
#endif    
    free( self );
    DEBUG_API("<-XASnapshotItfImpl_Free");
}
#ifdef _GSTREAMER_BACKEND_
/* void XASnapshotItfImpl_AdaptCb
 * Description:
 * Listen adaptation callBacks from camera adaptation
 */
void XASnapshotItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event )
{
    XASnapshotItfImpl* impl = (XASnapshotItfImpl*)pHandlerCtx;
    DEBUG_API("->XASnapshotItfImpl_AdaptCb");

    if(!impl)
    {
        DEBUG_ERR("XASnapshotItfImpl_AdaptCb, invalid context pointer!");
        return;
    }
    assert(event);

    if( event->eventid == XA_ADAPT_SNAPSHOT_TAKEN )
    {
        DEBUG_INFO("**Snapshot taken**");
        impl->snapshotongoing = XA_BOOLEAN_FALSE;
        if( impl->takencallback )
        {
        	if ( vfHandle && impl->freezevf )
        	{
        		XAresult ret = XA_RESULT_SUCCESS;
        		XAPlayItfImpl* vf = (XAPlayItfImpl*)vfHandle;
        		DEBUG_INFO_A1("Using global view finder handle from %x to freeze view finder", vf);
        		ret = XAPlayItfImpl_SetPlayState( (XAPlayItf)&vf, XA_PLAYSTATE_PAUSED);
        		if ( ret != XA_RESULT_SUCCESS )
        		{
        			DEBUG_ERR("Could not change view finder to pause");
        		}
        	}
            /* datasize used for "number taken", the pointer to
             * data sink structure itself contains the data size */
            impl->takencallback(impl->cbPtrToSelf, impl->usercontext,
                                event->datasize, (XADataSink*)event->data);
        }
    }
    else if( event->eventid == XA_ADAPT_SNAPSHOT_INITIATED )
    {
        DEBUG_INFO("**Snapshot initiated**");
        impl->initongoing = XA_BOOLEAN_FALSE;
        impl->initialized = XA_BOOLEAN_TRUE;
        if( impl->initcallback )
        {
            impl->initcallback(impl->cbPtrToSelf, impl->usercontext);
        }
    }
    DEBUG_API("<-XASnapshotItfImpl_AdaptCb");
}
#endif
/*End of file*/
