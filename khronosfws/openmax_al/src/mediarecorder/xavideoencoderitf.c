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
#include <string.h>
#include "xavideoencoderitf.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAVideoEncoderItfAdaptation.h"
#include "XAStaticCapsAdaptation.h"
#include "XARecordItfAdaptation.h"
#endif
#include "xathreadsafety.h"


/* static XAVideoEncoderItfImpl* GetImpl(XAVideoEncoderItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAVideoEncoderItfImpl* GetImpl(XAVideoEncoderItf self)
{
    if( self )
    {
        XAVideoEncoderItfImpl* impl = (XAVideoEncoderItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XAVideoEncoderItf implementation
 *****************************************************************************/

/* XAresult XAVideoEncoderItfImpl_SetVideoSettings
 * Description: Set video encoder settings.
 */
XAresult XAVideoEncoderItfImpl_SetVideoSettings(XAVideoEncoderItf self,
                                                XAVideoSettings *pSettings)
{
    XAresult res = XA_RESULT_SUCCESS;
#ifdef _GSTREAMER_BACKEND_
    XAStaticCapsData temp;
    XAuint32 recState = XA_RECORDSTATE_STOPPED;
    XAVideoEncoderItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAVideoEncoderItfImpl_SetVideoSettings");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);

    if( !impl || !pSettings )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVideoEncoderItfImpl_SetVideoSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }

    res = XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_VIDEO, pSettings->encoderId, &temp);
    if( res == XA_RESULT_SUCCESS )
    {
        res = XARecordItfAdapt_GetRecordState( impl->adaptCtx, &recState );
        if( res == XA_RESULT_SUCCESS )
        {
            if( XA_RECORDSTATE_STOPPED == recState )
            {
                res = XAVideoEncoderItfAdapt_SetVideoSettings(impl->adaptCtx, pSettings);
            }
            else
            {
                DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED");
                DEBUG_API("<-XAVideoEncoderItfImpl_SetVideoSettings");
                return XA_RESULT_PRECONDITIONS_VIOLATED;
            }
        }
    }
    else
    {
        DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
        DEBUG_API("<-XAVideoEncoderItfImpl_SetVideoSettings");
        return XA_RESULT_FEATURE_UNSUPPORTED;
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
#endif
    DEBUG_API("<-XAVideoEncoderItfImpl_SetVideoSettings");
    return res;
}

/* XAresult XAVideoEncoderItfImpl_GetVideoSettings
 * Description: Get video encoder settings.
 */
XAresult XAVideoEncoderItfImpl_GetVideoSettings(XAVideoEncoderItf self,
                                                XAVideoSettings *pSettings)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAVideoEncoderItfImpl *impl = GetImpl(self);
    DEBUG_API("->XAVideoEncoderItfImpl_GetVideoSettings");
    if( !impl || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVideoEncoderItfImpl_GetVideoSettings");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
#ifdef _GSTREAMER_BACKEND_
    res = XAVideoEncoderItfAdapt_GetVideoSettings(impl->adaptCtx, pSettings);
#endif
    DEBUG_API("<-XAVideoEncoderItfImpl_GetVideoSettings");
    return res;
}

/*****************************************************************************
 * XAVideoEncoderItfImpl -specific methods
 *****************************************************************************/


/* XAVideoEncoderItfImpl* XAVideoEncoderItfImpl_Create()
 * Description: Allocate and initialize VideoEncoderItfImpl
 */
XAVideoEncoderItfImpl* XAVideoEncoderItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBaseCtx *adaptCtx
#endif
        )
{
    XAVideoEncoderItfImpl* self = (XAVideoEncoderItfImpl*)
        calloc(1,sizeof(XAVideoEncoderItfImpl));
    DEBUG_API("->XAVideoEncoderItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.GetVideoSettings = XAVideoEncoderItfImpl_GetVideoSettings;
        self->itf.SetVideoSettings = XAVideoEncoderItfImpl_SetVideoSettings;

#ifdef _GSTREAMER_BACKEND_
        /* init variables */
        self->adaptCtx = adaptCtx;
#endif
        self->self = self;
    }
    DEBUG_API("<-XAVideoEncoderItfImpl_Create");
    return self;
}

/* void XAVideoEncoderItfImpl_Free(XAVideoEncoderItfImpl* self)
 * Description: Free all resources reserved at XAVideoEncoderItfImpl_Create()
 */
void XAVideoEncoderItfImpl_Free(XAVideoEncoderItfImpl* self)
{
    DEBUG_API("->XAVideoEncoderItfImpl_Free");
    assert( self==self->self );
    free( self );
    DEBUG_API("<-XAVideoEncoderItfImpl_Free");
}

