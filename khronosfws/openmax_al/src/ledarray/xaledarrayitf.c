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
#include "xaledarrayitf.h"
  
#include "xaledarrayitfadaptation.h"

#include "xathreadsafety.h"

/* XALEDArrayItfImpl* GetImpl(XALEDArrayItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XALEDArrayItfImpl* GetImpl(XALEDArrayItf self)
{
    if( self )
    {
        XALEDArrayItfImpl* impl = (XALEDArrayItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XALEDArrayItf implementation
 *****************************************************************************/
/*
 * XAresult XALEDArrayItfImpl_ActivateLEDArray ( XALEDArrayItf self,
 *                                               XAuint32 lightMask )
 * Description: Activates or deactivates individual LEDs in an array of LEDs.
 */
XAresult XALEDArrayItfImpl_ActivateLEDArray ( XALEDArrayItf self,
                                              XAuint32 lightMask )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XALEDArrayItfImpl* impl = GetImpl(self);

    DEBUG_API("->XALEDArrayItfImpl_ActivateLEDArray");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSLEDArray);

    if( !impl || lightMask > LED_COUNT)
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayItfImpl_ActivateLEDArray");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is ligtMask mode changed */
    if( impl->lightMask != lightMask )
    {
  
        ret = XALEDArrayItfAdapt_ActivateLEDArray( (XAAdaptationGstCtx*)impl->adapCtx, lightMask );

        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->lightMask = lightMask;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
    DEBUG_API("<-XALEDArrayItfImpl_ActivateLEDArray");
    return ret;
}
/*
 * XAresult XALEDArrayItfImpl_IsLEDArrayActivated ( XALEDArrayItf self,
 *                                                  XAuint32 * pLightMask )
 * Description: Returns the state of each LED in an array of LEDs.
 */
XAresult XALEDArrayItfImpl_IsLEDArrayActivated ( XALEDArrayItf self,
                                                 XAuint32 * pLightMask )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XALEDArrayItfImpl* impl = GetImpl(self);
    DEBUG_API("->XALEDArrayItfImpl_IsLEDArrayActivated");

    if( !impl || !pLightMask)
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayItfImpl_IsLEDArrayActivated");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pLightMask = impl->lightMask;

    if( *pLightMask )
    {
        DEBUG_API("The LED is on.");
    }
    else
    {
        DEBUG_API("The LED is off.");
    }

    DEBUG_API("<-XALEDArrayItfImpl_IsLEDArrayActivated");
    return ret;
}
/*
 * XAresult XALEDArrayItfImpl_SetColor ( XALEDArrayItf self,
 *                                       XAuint8 index,
 *                                       const XAHSL * pColor )
 * Description: Sets the color of an individual LED.
 */
XAresult XALEDArrayItfImpl_SetColor ( XALEDArrayItf self,
                                      XAuint8 index,
                                      const XAHSL * pColor )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XALEDArrayItfImpl* impl = GetImpl(self);
    XAboolean support = XA_BOOLEAN_FALSE;

    DEBUG_API("->XALEDArrayItfImpl_SetColor");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSLEDArray);

    if( !impl || index > LED_COUNT - 1 || !pColor )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayItfImpl_SetColor");
        return XA_RESULT_PARAMETER_INVALID;
    }

    support = ( COLOR_MASK >> index ) & 0x1;
    if( !support ) /* Not supporting color*/
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
        DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED: not supporting color");
        DEBUG_API("<-XALEDArrayItfImpl_SetColor");
        return XA_RESULT_PRECONDITIONS_VIOLATED;
    }
    else
    {
  
        ret = XALEDArrayItfAdapt_SetColor( (XAAdaptationGstCtx*)impl->adapCtx, index, pColor);

        if ( ret == XA_RESULT_SUCCESS )
        {
            impl->color = *pColor;
        }
    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
    DEBUG_API("<-XALEDArrayItfImpl_SetColor");
    return ret;
}
/*
 * XAresult XALEDArrayItfImpl_GetColor ( XALEDArrayItf self,
 *                                       XAuint8 index,
 *                                       const XAHSL * pColor )
 * Description: Returns the color of an individual LED.
 */
XAresult XALEDArrayItfImpl_GetColor ( XALEDArrayItf self,
                                      XAuint8 index,
                                      XAHSL * pColor )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XALEDArrayItfImpl* impl = GetImpl(self);
    XAboolean support = XA_BOOLEAN_FALSE;
    DEBUG_API("->XALEDArrayItfImpl_GetColor");

    if( !impl || index > LED_COUNT - 1 || !pColor )
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayItfImpl_GetColor");
        return XA_RESULT_PARAMETER_INVALID;
    }


    support = ( COLOR_MASK >> index ) & 0x1;
    if( !support ) /* Not supporting color*/
    {
        DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED: not supporting color");
        DEBUG_API("<-XALEDArrayItfImpl_GetColor");
        return XA_RESULT_PRECONDITIONS_VIOLATED;
    }
    else
    {
        *pColor = impl->color;
    }

    DEBUG_API_A3("<-XALEDArrayItfImpl_GetColor: hue = %ld, saturation = %d, lightness = %d", pColor->hue, pColor->saturation, pColor->lightness);
    return ret;
}

/*****************************************************************************
 * XALEDArrayItfImpl -specific methods
 *****************************************************************************/
  

/* XALEDArrayItfImpl* XALEDArrayItfImpl_Create()
 * Description: Allocate and initialize LEDArrayItfImpl
 */
XALEDArrayItfImpl* XALEDArrayItfImpl_Create( XAAdaptationBaseCtx *adapCtx )
{
    XALEDArrayItfImpl* self = (XALEDArrayItfImpl*)
        calloc(1,sizeof(XALEDArrayItfImpl));
    DEBUG_API("->XALEDArrayItfImpl_Create");
    if( self )
    {
        /* init itf default implementation */
        self->itf.ActivateLEDArray = XALEDArrayItfImpl_ActivateLEDArray;
        self->itf.IsLEDArrayActivated = XALEDArrayItfImpl_IsLEDArrayActivated;
        self->itf.SetColor = XALEDArrayItfImpl_SetColor;
        self->itf.GetColor = XALEDArrayItfImpl_GetColor;

        /* init variables */
        self->lightMask = 0;
        self->color.hue = 0;
        self->color.saturation = 0;
        self->color.lightness = 0;
        self->adapCtx = adapCtx;

        self->self = self;
    }

    DEBUG_API("<-XALEDArrayItfImpl_Create");
    return self;
}

/* void XALEDArrayItfImpl_Free(XALEDArrayItfImpl* self)
 * Description: Free all resources reserved at XALEDArrayItfImpl_Create
 */
void XALEDArrayItfImpl_Free(XALEDArrayItfImpl* self)
{
    DEBUG_API("->XALEDArrayItfImpl_Free");
    assert( self==self->self );
    free( self );
    DEBUG_API("<-XALEDArrayItfImpl_Free");
}
