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
 * Description: Play Itf Implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "xaplayitf.h"

#include "xaplayitfadaptation.h"

#include "xaplayitfadaptationmmf.h"
#include "xathreadsafety.h"
#include <string.h>

void* vfHandle;

/* XAPlayItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAPlayItfImpl* GetImpl(XAPlayItf self)
    {
    if (self)
        {
        XAPlayItfImpl* impl = (XAPlayItfImpl*) (*self);
        if (impl && impl == impl->self)
            {
            return impl;
            }
        }
    return NULL;
    }

/**
 * Base interface XAPlayItf implementation
 */

/**
 * XAresult XAPlayItfImpl_SetPlayState(XAPlayItf self, XAuint32 state)
 * Description: Requests a transition of the player into the given play state.
 **/
XAresult XAPlayItfImpl_SetPlayState(XAPlayItf self, XAuint32 state)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);
    DEBUG_API_A1("->XAPlayItfImpl_SetPlayState %s",PLAYSTATENAME(state));

    if (!impl || state < XA_PLAYSTATE_STOPPED || state > XA_PLAYSTATE_PLAYING)
        {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_SetPlayState");
        return XA_RESULT_PARAMETER_INVALID;
        }

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_SetPlayState(impl->pObjImpl->curAdaptCtx,
                state);
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
        DEBUG_API("<-XAPlayItfImpl_SetPlayState");
        return ret;
        }

    /* check is play state changed, if not do nothing */
    if (state != impl->playbackState)
        {
        if (state == XA_PLAYSTATE_PLAYING)
            {
            XAPlayItfAdaptGST_GetPosition(
                    (XAAdaptationGstCtx*) impl->adapCtx,
                    &(impl->lastPosition));
            }
        ret = XAPlayItfAdaptGST_SetPlayState(impl->adapCtx, state);

        if (ret == XA_RESULT_SUCCESS)
            {
            impl->playbackState = state;
            if (state == XA_PLAYSTATE_STOPPED || state == XA_PLAYSTATE_PAUSED)
                {
                impl->isMarkerPosCbSend = XA_BOOLEAN_FALSE;
                impl->lastPosition = 0;
                }
            }
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
    DEBUG_API("<-XAPlayItfImpl_SetPlayState");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_GetPlayState(XAPlayItf self, XAuint32 *pState)
 * Description: Gets the player's current play state.
 **/
XAresult XAPlayItfImpl_GetPlayState(XAPlayItf self, XAuint32 *pState)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_GetPlayState");

    if (!impl || !pState)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_GetPlayState");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_GetPlayState(impl->pObjImpl->curAdaptCtx,
                pState);
        }
    else
        {
        *pState = impl->playbackState;
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );

    DEBUG_API_A1("<-XAPlayItfImpl_GetPlayState: %s",PLAYSTATENAME(impl->playbackState));
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_GetDuration(XAPlayItf self, XAmillisecond *pMsec)
 * Description: Gets the duration of the current content, in milliseconds.
 **/
XAresult XAPlayItfImpl_GetDuration(XAPlayItf self, XAmillisecond *pMsec)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAPlayItfImpl_GetDuration");

    if (!impl || !pMsec)
        {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_GetDuration");
        return XA_RESULT_PARAMETER_INVALID;
        }

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_GetDuration(impl->pObjImpl->curAdaptCtx,
                pMsec);
        }
    else
        {
        ret = XAPlayItfAdaptGST_GetDuration(
                (XAAdaptationGstCtx*) impl->adapCtx, pMsec);
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );

    DEBUG_API("<-XAPlayItfImpl_GetDuration");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_GetPosition(XAPlayItf self, XAmillisecond *pMsec)
 * Description: Returns the current position of the playback head relative
 * to the beginning of the content.
 **/
XAresult XAPlayItfImpl_GetPosition(XAPlayItf self, XAmillisecond *pMsec)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_GetPosition");

    if (!impl || !pMsec)
        {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_GetPosition");
        return XA_RESULT_PARAMETER_INVALID;
        }

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_GetPosition(impl->pObjImpl->curAdaptCtx,
                pMsec);
        }
    else
        {

        if (impl->playbackState == XA_PLAYSTATE_STOPPED)
            {
            *pMsec = 0;
            DEBUG_API("<-XAPlayItfImpl_GetPosition");
            XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
            return XA_RESULT_SUCCESS;
            }
        ret = XAPlayItfAdaptGST_GetPosition(
                (XAAdaptationGstCtx*) impl->adapCtx, pMsec);
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
    DEBUG_API("<-XAPlayItfImpl_GetPosition");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_RegisterCallback(XAPlayItf self, xaPlayCallback callback,
 *                                         void *pContext)
 * Description: Sets the playback callback function.
 **/
XAresult XAPlayItfImpl_RegisterCallback(XAPlayItf self,
        xaPlayCallback callback, void *pContext)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_RegisterCallback");
    if (!impl)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_RegisterCallback");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    /* callback may be NULL (to remove callback) */
    impl->callback = callback;
    impl->cbcontext = pContext;
    impl->cbPtrToSelf = self;

    // No need to do anything else if radio:
    if (((XAMediaPlayerAdaptationMMFCtx*) impl->pObjImpl->curAdaptCtx)->isForRadio
            == XA_BOOLEAN_TRUE)
        {
        DEBUG_API("<-XAPlayItfImpl_RegisterCallback");
        return ret;
        }

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_RegisterCallback(impl->pObjImpl->curAdaptCtx,
                callback);
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );

    DEBUG_API("<-XAPlayItfImpl_RegisterCallback");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_SetCallbackEventsMask(XAPlayItf self, XAuint32 eventFlags)
 * Description: Enables/disables notification of playback events.
 **/
XAresult XAPlayItfImpl_SetCallbackEventsMask(XAPlayItf self,
        XAuint32 eventFlags)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_SetCallbackEventsMask");

    if (!impl || (eventFlags > (XA_PLAYEVENT_HEADATEND
            | XA_PLAYEVENT_HEADATMARKER | XA_PLAYEVENT_HEADATNEWPOS
            | XA_PLAYEVENT_HEADMOVING | XA_PLAYEVENT_HEADSTALLED)))
        {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");DEBUG_API("<-XAPlayItfImpl_SetCallbackEventsMask");
        return XA_RESULT_PARAMETER_INVALID;
        }

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    impl->eventFlags = eventFlags;

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_SetCallbackEventsMask(
                impl->pObjImpl->curAdaptCtx, eventFlags);

        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
        DEBUG_API("<-XAPlayItfImpl_SetCallbackEventsMask");
        return ret;
        }

    /* enable position tracking if client wants so */
    if ((eventFlags & (XA_PLAYEVENT_HEADATMARKER | XA_PLAYEVENT_HEADATNEWPOS))
            && impl->adapCtx && !impl->positionupdateOn)
        {
        ret = XAPlayItfAdapt_EnablePositionTracking(
                (XAAdaptationGstCtx*) impl->adapCtx, XA_BOOLEAN_TRUE);
        if (ret == XA_RESULT_SUCCESS)
            {
            impl->positionupdateOn = XA_BOOLEAN_TRUE;
            }
        }
    else if (!(eventFlags & (XA_PLAYEVENT_HEADATMARKER
            | XA_PLAYEVENT_HEADATNEWPOS)) && impl->adapCtx
            && impl->positionupdateOn)
        {
        ret = XAPlayItfAdapt_EnablePositionTracking(
                (XAAdaptationGstCtx*) impl->adapCtx, XA_BOOLEAN_FALSE);
        if (ret == XA_RESULT_SUCCESS)
            {
            impl->positionupdateOn = XA_BOOLEAN_FALSE;
            }
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
    DEBUG_API("<-XAPlayItfImpl_SetCallbackEventsMask");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_GetCallbackEventsMask(XAPlayItf self, XAuint32 *pEventFlags)
 * Description: Queries for the notification state (enabled/disabled) of playback events.
 **/
XAresult XAPlayItfImpl_GetCallbackEventsMask(XAPlayItf self,
        XAuint32 *pEventFlags)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_GetCallbackEventsMask");
    if (!impl || !pEventFlags)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_GetCallbackEventsMask");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    *pEventFlags = impl->eventFlags;

    DEBUG_API("<-XAPlayItfImpl_GetCallbackEventsMask");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_SetMarkerPosition(XAPlayItf self, XAmillisecond mSec)
 * Description: Sets the position of the playback marker.
 **/
XAresult XAPlayItfImpl_SetMarkerPosition(XAPlayItf self, XAmillisecond mSec)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAmillisecond duration = 0;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API_A1("->XAPlayItfImpl_SetMarkerPosition: %lu ms", mSec);

    /* Get duration of the content */
    if (XAPlayItfImpl_GetDuration(self, &duration) != XA_RESULT_SUCCESS)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_SetMarkerPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    if (!impl || mSec > duration)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_SetMarkerPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    impl->markerPosition = mSec;
    impl->isMarkerPosCbSend = XA_BOOLEAN_FALSE;

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_SetMarkerPosition(
                impl->pObjImpl->curAdaptCtx, mSec);
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );

    DEBUG_API("<-XAPlayItfImpl_SetMarkerPosition");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_ClearMarkerPosition(XAPlayItf self)
 * Description: Clears marker.
 **/
XAresult XAPlayItfImpl_ClearMarkerPosition(XAPlayItf self)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_ClearMarkerPosition");
    if (!impl)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_ClearMarkerPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    impl->isMarkerPosCbSend = XA_BOOLEAN_FALSE;
    impl->markerPosition = NO_POSITION;

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_ClearMarkerPosition(
                impl->pObjImpl->curAdaptCtx);
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );

    DEBUG_API("<-XAPlayItfImpl_ClearMarkerPosition");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_GetMarkerPosition(XAPlayItf self, XAmillisecond *pMsec)
 * Description: Queries the position of playback marker.
 **/
XAresult XAPlayItfImpl_GetMarkerPosition(XAPlayItf self, XAmillisecond *pMsec)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_GetMarkerPosition");
    if (!impl || !pMsec)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_GetMarkerPosition");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    if (impl->markerPosition == NO_POSITION)
        {
        DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED");
        DEBUG_API("<-XAPlayItfImpl_GetMarkerPosition");
        /*marker is not set */
        return XA_RESULT_PRECONDITIONS_VIOLATED;
        }

    *pMsec = impl->markerPosition;

    DEBUG_API("<-XAPlayItfImpl_GetMarkerPosition");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_SetPositionUpdatePeriod(XAPlayItf self, XAmillisecond mSec)
 * Description: Sets the interval between periodic position notifications.
 **/
XAresult XAPlayItfImpl_SetPositionUpdatePeriod(XAPlayItf self,
        XAmillisecond mSec)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API_A1("->XAPlayItfImpl_SetPositionUpdatePeriod, %lu mSec", mSec);
    if (!impl)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_SetPositionUpdatePeriod");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }
    impl->positionUpdatePeriod = mSec;

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        ret = XAPlayItfAdaptMMF_SetPositionUpdatePeriod(
                impl->pObjImpl->curAdaptCtx, mSec);
        }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );

    DEBUG_API("<-XAPlayItfImpl_SetPositionUpdatePeriod");
    return ret;
    }

/**
 * XAresult XAPlayItfImpl_GetPositionUpdatePeriod(XAPlayItf self, XAmillisecond *pMsec)
 * Description: Queries the interval between periodic position notifications.
 **/
XAresult XAPlayItfImpl_GetPositionUpdatePeriod(XAPlayItf self,
        XAmillisecond *pMsec)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAPlayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAPlayItfImpl_GetPositionUpdatePeriod");
    if (!impl || !pMsec)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlayItfImpl_GetPositionUpdatePeriod");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
        }

    *pMsec = impl->positionUpdatePeriod;

    DEBUG_API("<-XAPlayItfImpl_GetPositionUpdatePeriod");
    return ret;
    }

/**
 * XAPlayItfImpl -specific methods
 **/

/**
 * XAPlayItfImpl* XAPlayItfImpl_Create()
 * Description: Allocate and initialize PlayItfImpl
 **/
XAPlayItfImpl* XAPlayItfImpl_Create(XAMediaPlayerImpl *impl)
    {
    XAPlayItfImpl *self;

    DEBUG_API("->XAPlayItfImpl_Create");
    self = (XAPlayItfImpl*) calloc(1, sizeof(XAPlayItfImpl));
    if (self)
        {
        /* init itf default implementation */
        self->itf.ClearMarkerPosition = XAPlayItfImpl_ClearMarkerPosition;
        self->itf.GetCallbackEventsMask = XAPlayItfImpl_GetCallbackEventsMask;
        self->itf.GetDuration = XAPlayItfImpl_GetDuration;
        self->itf.GetMarkerPosition = XAPlayItfImpl_GetMarkerPosition;
        self->itf.GetPlayState = XAPlayItfImpl_GetPlayState;
        self->itf.GetPosition = XAPlayItfImpl_GetPosition;
        self->itf.GetPositionUpdatePeriod
                = XAPlayItfImpl_GetPositionUpdatePeriod;
        self->itf.RegisterCallback = XAPlayItfImpl_RegisterCallback;
        self->itf.SetCallbackEventsMask = XAPlayItfImpl_SetCallbackEventsMask;
        self->itf.SetMarkerPosition = XAPlayItfImpl_SetMarkerPosition;
        self->itf.SetPlayState = XAPlayItfImpl_SetPlayState;
        self->itf.SetPositionUpdatePeriod
                = XAPlayItfImpl_SetPositionUpdatePeriod;

        /* init variables */
        self->callback = NULL;
        self->cbcontext = NULL;
        self->playbackState = XA_PLAYSTATE_STOPPED;
        self->eventFlags = 0;
        self->markerPosition = NO_POSITION;
        self->positionUpdatePeriod = PLAYITF_DEFAULT_UPDATE_PERIOD;
        self->lastPosition = START_POSITION;
        /*self->adapCtx = impl->curAdaptCtx;*/
        self->pObjImpl = impl;
        self->cbPtrToSelf = NULL;
        self->isMarkerPosCbSend = XA_BOOLEAN_FALSE;

        /*        XAAdaptationBase_AddEventHandler( self->adapCtx, &XAPlayItfImpl_AdaptCb, XA_PLAYITFEVENTS, self );*/
        XAAdaptationBase_AddEventHandler(self->pObjImpl->curAdaptCtx,
                &XAPlayItfImpl_AdaptCb, XA_PLAYITFEVENTS, self);

        self->self = self;
        }

    DEBUG_API("<-XAPlayItfImpl_Create");
    return self;
    }

/* void XAPlayItfImpl_Free(XAPlayItfImpl* self)
 * Description: Free all resources reserved at XAPlayItfImpl_Create
 */
void XAPlayItfImpl_Free(XAPlayItfImpl* self)
    {
    DEBUG_API("->XAPlayItfImpl_Free");
    assert(self==self->self);
    /*    XAAdaptationBase_RemoveEventHandler( self->adapCtx, &XAPlayItfImpl_AdaptCb );*/
    XAAdaptationBase_RemoveEventHandler(self->pObjImpl->curAdaptCtx,
            &XAPlayItfImpl_AdaptCb);
    free(self);
    DEBUG_API("<-XAPlayItfImpl_Free");
    }

/* void XAPlayItfImpl_AdaptCb
 * Description: Listen changes in adaptation
 */
void XAPlayItfImpl_AdaptCb(void *pHandlerCtx, XAAdaptEvent *event)
    {
    XAPlayItfImpl* impl = (XAPlayItfImpl*) pHandlerCtx;
    XAuint32 newpos = 0;

    DEBUG_API("->XAPlayItfImpl_AdaptCb");
    XA_IMPL_THREAD_SAFETY_ENTRY_FOR_VOID_FUNCTIONS( XATSMediaPlayer );
    if (!impl)
        {
        DEBUG_ERR("XAPlayItfImpl_AdaptCb, invalid context pointer!");
        DEBUG_API("<-XAPlayItfImpl_AdaptCb");
        XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS( XATSMediaPlayer );
        return;
        }
    assert(event);

    if (impl->pObjImpl->curAdaptCtx->fwtype == FWMgrFWMMF)
        {
        impl->callback(impl->cbPtrToSelf, impl->cbcontext, event->eventid);
        DEBUG_API("<-XAPlayItfImpl_AdaptCb");
        XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS( XATSMediaPlayer );
        return;
        }

    /* check position update events */
    if (event->eventid == XA_ADAPT_POSITION_UPDATE_EVT)
        {
        assert(event->data);
        newpos = *((XAuint32*) (event->data));
        DEBUG_API_A1("Position update from adaptation: new position %lu ms",newpos);

        /* Check is looping start file playing before marker position */
        if (newpos < impl->markerPosition || impl->lastPosition > newpos)
            {
            DEBUG_INFO("Restart looping, clear marker position callback flag.");
            impl->isMarkerPosCbSend = XA_BOOLEAN_FALSE;
            }

        /* check if marker passed and callback needed */
        if ((impl->markerPosition != NO_POSITION) && (impl->eventFlags
                & XA_PLAYEVENT_HEADATMARKER))
            {
            if (impl->callback && (((impl->lastPosition
                    < impl->markerPosition)
                    && (newpos > impl->markerPosition)) || (newpos
                    == impl->markerPosition)))
                {
                /* Check is callback already send */
                if (impl->isMarkerPosCbSend != XA_BOOLEAN_TRUE)
                    {
                    impl->callback(impl->cbPtrToSelf, impl->cbcontext,
                            XA_PLAYEVENT_HEADATMARKER);
                    impl->isMarkerPosCbSend = XA_BOOLEAN_TRUE;
                    }
                }
            }
        /* check if update period passed and callback needed */
        if ((impl->positionUpdatePeriod > 0) && (impl->eventFlags
                & XA_PLAYEVENT_HEADATNEWPOS) && impl->callback)
            {
            if ((XAuint32) ((impl->lastPosition)
                    / (impl->positionUpdatePeriod)) < (XAuint32) (newpos
                    / (impl->positionUpdatePeriod)))
                {
                impl->callback(impl->cbPtrToSelf, impl->cbcontext,
                        XA_PLAYEVENT_HEADATNEWPOS);
                }
            }
        /* store position */
        impl->lastPosition = newpos;
        }

    /* check other events */
    else if (event->eventid == XA_PLAYEVENT_HEADATEND)
        {
        impl->playbackState = XA_PLAYSTATE_STOPPED;
        impl->lastPosition = 0;
        /* send callback if needed */
        if ((XA_PLAYEVENT_HEADATEND & impl->eventFlags) && impl->callback)
            {
            impl->callback(impl->cbPtrToSelf, impl->cbcontext,
                    XA_PLAYEVENT_HEADATEND);
            }
        }
    else if (event->eventid == XA_PLAYEVENT_HEADSTALLED)
        {
        impl->playbackState = XA_PLAYSTATE_PAUSED;
        if (impl->adapCtx->fwtype == FWMgrFWMMF)
            {
            //XAPlayItfAdaptMMF_GetPosition((XAAdaptationGstCtx*)impl->adapCtx, &(impl->lastPosition));
            }
        else
            {
            XAPlayItfAdaptGST_GetPosition(
                    (XAAdaptationGstCtx*) impl->adapCtx,
                    &(impl->lastPosition));
            }
        /* send callback if needed */
        if ((XA_PLAYEVENT_HEADSTALLED & impl->eventFlags) && impl->callback)
            {
            impl->callback(impl->cbPtrToSelf, impl->cbcontext,
                    XA_PLAYEVENT_HEADSTALLED);
            }
        }
    else if (event->eventid == XA_PLAYEVENT_HEADMOVING)
        {
        impl->playbackState = XA_PLAYSTATE_PLAYING;
        /* send callback if needed */
        if ((XA_PLAYEVENT_HEADMOVING & impl->eventFlags) && impl->callback)
            {
            impl->callback(impl->cbPtrToSelf, impl->cbcontext,
                    XA_PLAYEVENT_HEADMOVING);
            }
        }
    else
        {
        /* do nothing */
        }

    DEBUG_API("<-XAPlayItfImpl_AdaptCb");
    XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS( XATSMediaPlayer );
    }

