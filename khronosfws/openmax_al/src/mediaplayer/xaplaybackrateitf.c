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
#include "xaplaybackrateitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAPlaybackRateItfAdaptation.h"
#endif
#include "xathreadsafety.h"

static XAPlaybackRateItfImpl* GetImpl(XAPlaybackRateItf self)
{
    if(self)
    {
        XAPlaybackRateItfImpl *impl = (XAPlaybackRateItfImpl*)(*self);
        if(impl && impl == impl->self)
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAPlaybackRateItf implementation
 */
XAresult XAPlaybackRateItfImpl_SetRate(XAPlaybackRateItf self, XApermille rate)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAPlaybackRateItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPlaybackRateItfImpl_SetRate");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );

    if( !impl )
    {

        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlaybackRateItfImpl_SetRate");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_
    res = XAPlaybackRateItfAdapt_SetRate(impl->adaptCtx, rate);
#endif
    if(res == XA_RESULT_SUCCESS)
    {
        impl->currentRate = rate;
    }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
    DEBUG_API("<-XAPlaybackRateItfImpl_SetRate");
    return res;
}

XAresult XAPlaybackRateItfImpl_GetRate(XAPlaybackRateItf self, XApermille *pRate)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAPlaybackRateItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPlaybackRateItfImpl_GetRate");

    if( !impl || !pRate )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlaybackRateItfImpl_GetRate");
        return XA_RESULT_PARAMETER_INVALID;
    }
    *pRate = impl->currentRate;

    DEBUG_API("<-XAPlaybackRateItfImpl_GetRate");
    return res;
}

XAresult XAPlaybackRateItfImpl_SetPropertyConstraints(XAPlaybackRateItf self,
                                                        XAuint32 constraints)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAPlaybackRateItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPlaybackRateItfImpl_SetPropertyConstraints");

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );
    if( !impl )
    {
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlaybackRateItfImpl_SetPropertyConstraints");
        return XA_RESULT_PARAMETER_INVALID;
    }
    
#ifdef _GSTREAMER_BACKEND_
    /* set to adaptation */
    res = XAPlaybackRateItfAdapt_SetPropertyConstraints(impl->adaptCtx, constraints);
#endif
    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
    DEBUG_API("<-XAPlaybackRateItfImpl_SetPropertyConstraints");
    return res;
}

XAresult XAPlaybackRateItfImpl_GetProperties(XAPlaybackRateItf self,
                                               XAuint32 *pProperties)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAPlaybackRateItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPlaybackRateItfImpl_GetProperties");

    if( !impl || !pProperties )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlaybackRateItfImpl_GetProperties");
        return XA_RESULT_PARAMETER_INVALID;
    }
   
#ifdef _GSTREAMER_BACKEND_
    /* needs to be queried from adaptation */
    res = XAPlaybackRateItfAdapt_GetProperties(impl->adaptCtx, pProperties);
#endif
    DEBUG_API("<-XAPlaybackRateItfImpl_GetProperties");
    return res;
}

XAresult XAPlaybackRateItfImpl_GetCapabilitiesOfRate(XAPlaybackRateItf self,
                                                       XApermille rate,
                                                       XAuint32 *pCapabilities)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAPlaybackRateItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPlaybackRateItfImpl_GetCapabilitiesOfRate");

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );
    if( !impl || !pCapabilities )
    {

        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlaybackRateItfImpl_GetCapabilitiesOfRate");
        return XA_RESULT_PARAMETER_INVALID;
    }
    
#ifdef _GSTREAMER_BACKEND_
    /* needs to be queried from adaptation */
    res = XAPlaybackRateItfAdapt_GetCapabilitiesOfRate(impl->adaptCtx, rate, pCapabilities);
#endif
    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
    DEBUG_API("<-XAPlaybackRateItfImpl_GetCapabilitiesOfRate");
    return res;
}

XAresult XAPlaybackRateItfImpl_GetRateRange(XAPlaybackRateItf self,
                                              XAuint8 index,
                                              XApermille *pMinRate,
                                              XApermille *pMaxRate,
                                              XApermille *pStepSize,
                                              XAuint32 *pCapabilities)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAPlaybackRateItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAPlaybackRateItfImpl_GetRateRange");

    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaPlayer );
    if( !impl || !pMinRate || !pMaxRate || !pStepSize || !pCapabilities )
    {
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAPlaybackRateItfImpl_GetRateRange");
        return XA_RESULT_PARAMETER_INVALID;
    }
    
#ifdef _GSTREAMER_BACKEND_
    /* needs to be queried from adaptation */
    res = XAPlaybackRateItfAdapt_GetRateRange(impl->adaptCtx, index, pMinRate,
                                              pMaxRate,pStepSize, pCapabilities);
#endif
    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaPlayer );
    DEBUG_API("<-XAPlaybackRateItfImpl_GetRateRange");
    return res;
}

/**
 * XAPlaybackRateItfImpl -specific methods
 **/
#ifdef _GSTREAMER_BACKEND_

/**
 * XAPlaybackRateItfImpl* XAPlaybackRateItfImpl_Create();
 * @return  XAPlaybackRateItfImpl* - Pointer to  PlaybackRateItf interface implementation
 **/
XAPlaybackRateItfImpl* XAPlaybackRateItfImpl_Create( XAAdaptationBaseCtx *adaptCtx )
{
    XAPlaybackRateItfImpl *self = (XAPlaybackRateItfImpl*)
        calloc(1,sizeof(XAPlaybackRateItfImpl));
    DEBUG_API("->XAPlaybackRateItfImpl_Create");

    if(self)
    {
        /* init itf default implementation */
        self->itf.GetCapabilitiesOfRate = XAPlaybackRateItfImpl_GetCapabilitiesOfRate;
        self->itf.GetProperties = XAPlaybackRateItfImpl_GetProperties;
        self->itf.GetRate = XAPlaybackRateItfImpl_GetRate;
        self->itf.GetRateRange = XAPlaybackRateItfImpl_GetRateRange;
        self->itf.SetPropertyConstraints = XAPlaybackRateItfImpl_SetPropertyConstraints;
        self->itf.SetRate = XAPlaybackRateItfImpl_SetRate;

        /* init variables */
        self->adaptCtx = adaptCtx;
        self->currentRate = 1000;
        self->self = self;
    }

    DEBUG_API("<-XAPlaybackRateItfImpl_Create");
    return self;
}
#endif
/**
 * void XAPlaybackRateItfImpl_Free(XAPlaybackRateItfImpl* self);
 * @param  XAPlaybackRateItfImpl* self -
 **/
void XAPlaybackRateItfImpl_Free(XAPlaybackRateItfImpl* self)
{
    DEBUG_API("->XAPlaybackRateItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAPlaybackRateItfImpl_Free");
}
