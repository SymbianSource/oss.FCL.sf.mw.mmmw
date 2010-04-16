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
#include "xavibraitf.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAVibraItfAdaptation.h"
#endif
#include "xathreadsafety.h"
/* XAVibraItfImpl* GetImpl(XAVibraItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
*/
static XAVibraItfImpl* GetImpl(XAVibraItf self)
{
    if( self )
    {
        XAVibraItfImpl* impl = (XAVibraItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XAVibraItf implementation
 *****************************************************************************/
/*
 * XAresult XAVibraItfImpl_Vibrate ( XAVibraItf self, XAboolean vibrate )
 * Description: Activates or deactivates vibration for the I/O device.
 */
XAresult XAVibraItfImpl_Vibrate ( XAVibraItf self,
                                  XAboolean vibrate )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAVibraItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAVibraItfImpl_Vibrate");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSVibra);

    if( !impl )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSVibra);
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfImpl_Vibrate");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is vibrate mode changed */
    if( impl->vibrate != vibrate )
    {
#ifdef _GSTREAMER_BACKEND_
        ret = XAVibraItfAdapt_Vibrate( impl->adapCtx, vibrate );
#endif
        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->vibrate = vibrate;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSVibra);
    DEBUG_API("<-XAVibraItfImpl_Vibrate");
    return ret;
}
/*
 * XAresult XAVibraItfImpl_IsVibrating ( XAVibraItf self, XAboolean * pVibrating )
 * Description: Returns whether the I/O device is vibrating.
 */
XAresult XAVibraItfImpl_IsVibrating ( XAVibraItf self, XAboolean * pVibrating )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAVibraItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAVibraItfImpl_IsVibrating");

    if( !impl || !pVibrating)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfImpl_IsVibrating");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pVibrating = impl->vibrate;

    if( *pVibrating )
    {
        DEBUG_API("It's vibrating");
    }
    else
    {
        DEBUG_API("Vibrate is off.");
    }

    DEBUG_API("<-XAVibraItfImpl_IsVibrating");
    return ret;
}
/*
 * XAresult XAVibraItfImpl_SetFrequency ( XAVibraItf self,  XAmilliHertz frequency )
 * Description: Sets the vibration frequency of the I/O device.
 */
XAresult XAVibraItfImpl_SetFrequency ( XAVibraItf self, XAmilliHertz frequency )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAVibraItfImpl* impl = GetImpl(self);

    DEBUG_API_A1("->XAVibraItfImpl_SetFrequency %ld", frequency);
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSVibra);

    if( !impl || frequency < MIN_FREQUENCY || frequency > MAX_FREQUENCY)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSVibra);
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfImpl_SetFrequency");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is vibration frequency changed */
    if( impl->frequency != frequency )
    {
#ifdef _GSTREAMER_BACKEND_
        ret = XAVibraItfAdapt_SetFrequency( impl->adapCtx, frequency );
#endif
        if( ret == XA_RESULT_SUCCESS )
        {
            impl->frequency = frequency;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSVibra);
    DEBUG_API("<-XAVibraItfImpl_SetFrequency");
    return ret;
}
/*
 * XAresult XAVibraItfImpl_GetFrequency ( XAVibraItf self,  XAmilliHertz * pFrequency )
 * Description: Returns the vibration frequency of the I/O device.
 */
XAresult XAVibraItfImpl_GetFrequency ( XAVibraItf self, XAmilliHertz * pFrequency )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAVibraItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAVibraItfImpl_GetFrequency");
    if( !impl || !pFrequency)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfImpl_GetFrequency");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pFrequency = impl->frequency;

    DEBUG_API_A1("Frequency is %ld", *pFrequency);
    DEBUG_API("<-XAVibraItfImpl_GetFrequency");
    return ret;
}
/*
 * XAresult XAVibraItfImpl_SetIntensity ( XAVibraItf self,  XApermille intensity )
 * Description: Sets the vibration intensity of the Vibra I/O device.
 */
XAresult XAVibraItfImpl_SetIntensity ( XAVibraItf self, XApermille intensity )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAVibraItfImpl* impl = GetImpl(self);
    DEBUG_API_A1("->XAVibraItfImpl_SetIntensity %d", intensity);
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSVibra);

    if( !impl || intensity < MIN_INTENSITY || intensity > MAX_INTENSITY )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSVibra);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfImpl_SetIntensity");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is vibration intensity changed */
    if( impl->intensity != intensity )
    {
#ifdef _GSTREAMER_BACKEND_
        ret = XAVibraItfAdapt_SetIntensity( impl->adapCtx, intensity );
#endif
        if( ret == XA_RESULT_SUCCESS )
        {
            impl->intensity = intensity;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSVibra);
    DEBUG_API("<-XAVibraItfImpl_SetIntensity");
    return ret;
}
/*
 * XAresult XAVibraItfImpl_GetIntensity ( XAVibraItf self, XApermille * pIntensity )
 * Description: Returns the vibration intensity of the Vibra I/O device.
 */
XAresult XAVibraItfImpl_GetIntensity ( XAVibraItf self, XApermille * pIntensity )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAVibraItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAVibraItfImpl_GetIntensity");

    if( !impl || !pIntensity)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfImpl_GetIntensity");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pIntensity = impl->intensity;

    DEBUG_API_A1("Intensity is %d", *pIntensity);
    DEBUG_API("<-XAVibraItfImpl_GetIntensity");
    return ret;
}

#ifdef _GSTREAMER_BACKEND_

/*****************************************************************************
 * XAVibraItfImpl -specific methods
 *****************************************************************************/
/* XAVibraItfImpl* XAVibraItfImpl_Create()
 * Description: Allocate and initialize VibraItfImpl
 */
XAVibraItfImpl* XAVibraItfImpl_Create( XAAdaptationBaseCtx *adapCtx )
{
    XAVibraItfImpl* self = (XAVibraItfImpl*) calloc(1,sizeof(XAVibraItfImpl));
    DEBUG_API("->XAVibraItfImpl_Create");
    if( self )
    {
        /* init itf default implementation*/
        self->itf.Vibrate = XAVibraItfImpl_Vibrate;
        self->itf.IsVibrating = XAVibraItfImpl_IsVibrating;
        self->itf.SetFrequency = XAVibraItfImpl_SetFrequency;
        self->itf.GetFrequency = XAVibraItfImpl_GetFrequency;
        self->itf.SetIntensity = XAVibraItfImpl_SetIntensity;
        self->itf.GetIntensity = XAVibraItfImpl_GetIntensity;

        /* init variables*/
        self->vibrate = XA_BOOLEAN_FALSE;
        self->frequency = 0;
        self->intensity = 0;
        self->adapCtx = adapCtx;

        self->self = self;
    }

    DEBUG_API("<-XAVibraItfImpl_Create");
    return self;
}
#endif
/* void XAVibraItfImpl_Free(XAVibraItfImpl* self)
 * Description: Free all resources reserved at XAVibraItfImpl_Create
 */
void XAVibraItfImpl_Free(XAVibraItfImpl* self)
{
    DEBUG_API("->XAVibraItfImpl_Free");
    assert( self==self->self );
    free( self );
    DEBUG_API("<-XAVibraItfImpl_Free");
}

