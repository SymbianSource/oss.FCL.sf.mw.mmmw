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
#include "xaglobals.h"
#include "xadevicevolumeitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XADeviceVolumeItfAdaptation.h"
#endif

static XADeviceVolumeItfImpl* GetImpl(XADeviceVolumeItf self)
{
    if( self )
    {
        XADeviceVolumeItfImpl* impl = (XADeviceVolumeItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XADeviceVolumeItf implementation
 */

/* XAresult XADeviceVolumeItfImpl_GetVolumeScale( XADeviceVolumeItf self,
 *                                                XAuint32 deviceID,
 *                                                XAint32* pMinValue,
 *                                                XAint32* pMaxValue,
 *                                                XAboolean* pIsMillibelScale)
 * Description: Gets the properties of the volume scale supported by the given device.
 */
XAresult XADeviceVolumeItfImpl_GetVolumeScale( XADeviceVolumeItf self,
                                               XAuint32 deviceID,
                                               XAint32* pMinValue,
                                               XAint32* pMaxValue,
                                               XAboolean* pIsMillibelScale)
{
    XADeviceVolumeItfImpl* impl = GetImpl(self);
    XAresult ret = XA_RESULT_SUCCESS;
    XAboolean supported = XA_BOOLEAN_FALSE;
    DEBUG_API("->XADeviceVolumeItfImpl_GetVolumeScale");
    if( !impl || !pMinValue || !pMaxValue || !pIsMillibelScale )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfImpl_GetVolumeScale");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XADeviceVolumeItfAdapt_IsDeviceIDSupported(impl->adapCtx, deviceID, &supported);
#endif
    if( ret != XA_RESULT_SUCCESS || supported != XA_BOOLEAN_TRUE )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfImpl_GetVolumeScale");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pMinValue = impl->minVolume;
    *pMaxValue = impl->maxVolume;
    *pIsMillibelScale = impl->ismBscale;

    DEBUG_API("<-XADeviceVolumeItfImpl_GetVolumeScale");
    return ret;
}

/* XAresult XADeviceVolumeItfImpl_SetVolume( XADeviceVolumeItf self,
 *                                           XAuint32 deviceID,
 *                                           XAint32 volume)
 * Description: Sets the device's volume.
 */
XAresult XADeviceVolumeItfImpl_SetVolume( XADeviceVolumeItf self,
                                          XAuint32 deviceID,
                                          XAint32 volume)
{
    XADeviceVolumeItfImpl* impl = GetImpl(self);
    XAresult ret = XA_RESULT_SUCCESS;
#ifdef _GSTREAMER_BACKEND_  
    XAboolean supported = XA_BOOLEAN_FALSE;
#endif
    DEBUG_API("->XADeviceVolumeItfImpl_SetVolume");

    if( !impl || volume < impl->minVolume || volume > impl->maxVolume )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfImpl_SetVolume");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XADeviceVolumeItfAdapt_IsDeviceIDSupported(impl->adapCtx, deviceID, &supported);
    if( ret != XA_RESULT_SUCCESS || supported != XA_BOOLEAN_TRUE )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfImpl_SetVolume");
        return XA_RESULT_PARAMETER_INVALID;
    }

    ret = XADeviceVolumeItfAdapt_SetVolume(impl->adapCtx, deviceID, volume);

    if( ret == XA_RESULT_SUCCESS )
    {
        impl->curVolume = volume;
    }
#endif
    DEBUG_API("<-XADeviceVolumeItfImpl_SetVolume");
    return ret;
}

/* XAresult XADeviceVolumeItfImpl_GetVolume( XADeviceVolumeItf self,
 *                                           XAuint32 deviceID,
 *                                           XAint32 * pVolume)
 * Description: Gets the device's volume.
 */
XAresult XADeviceVolumeItfImpl_GetVolume( XADeviceVolumeItf self,
                                          XAuint32 deviceID,
                                          XAint32 * pVolume)
{
    XADeviceVolumeItfImpl* impl = GetImpl(self);
    XAresult ret = XA_RESULT_SUCCESS;
    XAboolean supported = XA_BOOLEAN_FALSE;
    DEBUG_API("->XADeviceVolumeItfImpl_GetVolume");

    if( !impl || !pVolume )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfImpl_GetVolume");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XADeviceVolumeItfAdapt_IsDeviceIDSupported(impl->adapCtx, deviceID, &supported);
#endif
    if( ret != XA_RESULT_SUCCESS || supported != XA_BOOLEAN_TRUE )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfImpl_GetVolume");
        return XA_RESULT_PARAMETER_INVALID;
    }
    *pVolume = impl->curVolume;
    DEBUG_API("<-XADeviceVolumeItfImpl_GetVolume");
    return XA_RESULT_SUCCESS;
}

/**
 * XADeviceVolumeItfImpl -specific methods
 **/
#ifdef _GSTREAMER_BACKEND_  
/**
 * XADeviceVolumeItfImpl* XADeviceVolumeItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
 * Description: Allocate and initialize DeviceVolumeImpl
 **/
XADeviceVolumeItfImpl* XADeviceVolumeItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
{
    XADeviceVolumeItfImpl* self = (XADeviceVolumeItfImpl*)
        calloc(1,sizeof(XADeviceVolumeItfImpl));
    DEBUG_API("->XADeviceVolumeItfImpl_Create");

    if(self)
    {
        /* init itf default implementation */
        self->itf.GetVolumeScale =
            XADeviceVolumeItfImpl_GetVolumeScale;
        self->itf.GetVolume =
            XADeviceVolumeItfImpl_GetVolume;
        self->itf.SetVolume =
            XADeviceVolumeItfImpl_SetVolume;

        /* init variables */
        /* these values are replaced by adaptation in init phase */
        self->curVolume = DEFAULTDEVICEVOLUME;
        self->minVolume = DEFAULTDEVICEMINVOLUME;
        self->maxVolume = DEFAULTDEVICEMAXVOLUME;
        self->ismBscale = DEFAULTSCALEISMILLIBEL;

        self->adapCtx = adapCtx;
        self->self = self;
    }
    DEBUG_API("<-XADeviceVolumeItfImpl_Create");
    return self;
}
#endif
/* void XADeviceVolumeItfImpl_Free(XADeviceVolumeItfImpl* self)
 * Description: Free all resources reserved at XADeviceVolumeItfImpl_Create
 */
void XADeviceVolumeItfImpl_Free(XADeviceVolumeItfImpl* self)
{
    DEBUG_API("->XADeviceVolumeItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XADeviceVolumeItfImpl_Free");
}
