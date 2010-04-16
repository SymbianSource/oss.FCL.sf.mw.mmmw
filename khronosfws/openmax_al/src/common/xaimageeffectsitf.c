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
#include "xaimageeffectsitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAImageEffectsItfAdaptation.h"
#endif
static XAImageEffectsItfImpl* GetImpl(XAImageEffectsItf self)
{
    if(self)
    {
        XAImageEffectsItfImpl* impl = (XAImageEffectsItfImpl*)(*self);
        if(impl && (impl == impl->self))
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAImageEffectsItf implementation
 */

/**
 * XAresult XAImageEffectsItfImpl_QuerySupportedImageEffects(XAImageEffectsItf self,
 *                                                           XAuint32 index,
 *                                                           XAuint32 *pImageEffectId)
 * Description: Queries image effects supported.
 **/
XAresult XAImageEffectsItfImpl_QuerySupportedImageEffects(XAImageEffectsItf self,
                                                          XAuint32 index,
                                                          XAuint32 *pImageEffectId)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageEffectsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageEffectsItfImpl_QuerySupportedImageEffects");

    if(!impl || !pImageEffectId)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageEffectsItfImpl_QuerySupportedImageEffects");
        return XA_RESULT_PARAMETER_INVALID;
    }
#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageEffectsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAImageEffectsItfImpl_QuerySupportedImageEffects");
        return ret;
    }

    ret = XAImageEffectsItfAdapt_QuerySupportedImageEffects(impl->adapCtx, index,
                                                            pImageEffectId);

    if( ret == XA_RESULT_SUCCESS )
    {
        impl->index = index;
    }

    XAImageEffectsItfAdapt_ThreadExit(impl->adapCtx);
#endif
    DEBUG_API("<-XAImageEffectsItfImpl_QuerySupportedImageEffects");
    return ret;
}

/**
 * XAresult XAImageEffectsItfImpl_EnableImageEffect(XAImageEffectsItf self,
 *                                                  XAuint32 imageEffectID
 * Description: Enables an image effect.
 **/
XAresult XAImageEffectsItfImpl_EnableImageEffect(XAImageEffectsItf self,
                                                 XAuint32 imageEffectID)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageEffectsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageEffectsItfImpl_EnableImageEffect");

    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageEffectsItfImpl_EnableImageEffect");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageEffectsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAImageEffectsItfImpl_EnableImageEffect");
        return ret;
    }

    ret = XAImageEffectsItfAdapt_EnableImageEffect(impl->adapCtx, imageEffectID);

    if( ret == XA_RESULT_SUCCESS )
    {
        impl->imageEffectID = imageEffectID;
    }

    XAImageEffectsItfAdapt_ThreadExit(impl->adapCtx);
#endif
    DEBUG_API("<-XAImageEffectsItfImpl_EnableImageEffect");
    return ret;
}

/**
 * XAresult XAImageEffectsItfImpl_DisableImageEffect(XAImageEffectsItf self,
 *                                                   XAuint32 imageEffectID)
 * Description: Disable an image effect.
 **/
XAresult XAImageEffectsItfImpl_DisableImageEffect(XAImageEffectsItf self,
                                                  XAuint32 imageEffectID)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageEffectsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageEffectsItfImpl_DisableImageEffect");

    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageEffectsItfImpl_DisableImageEffect");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageEffectsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAImageEffectsItfImpl_DisableImageEffect");
        return ret;
    }

    ret = XAImageEffectsItfAdapt_DisableImageEffect(impl->adapCtx, imageEffectID);

    if( ret == XA_RESULT_SUCCESS )
    {
        impl->imageEffectID = NO_IMAGE_EFFECTS;
    }

    XAImageEffectsItfAdapt_ThreadExit(impl->adapCtx);
#endif
    DEBUG_API("<-XAImageEffectsItfImpl_DisableImageEffect");
    return ret;
}

/**
 * XAresult XAImageEffectsItfImpl_IsImageEffectEnabled(XAImageEffectsItf self,
 *                                                     XAuint32 imageEffectID,
 *                                                     XAboolean *pEnabled)
 * Description: Checks to see if an image effect is enabled.
 **/
XAresult XAImageEffectsItfImpl_IsImageEffectEnabled(XAImageEffectsItf self,
                                                    XAuint32 imageEffectID,
                                                    XAboolean *pEnabled)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageEffectsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageEffectsItfImpl_IsImageEffectEnabled");

    if(!impl || !pEnabled)
    {
        DEBUG_ERR("XA_RESUT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageEffectsItfImpl_IsImageEffectEnabled");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageEffectsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAImageEffectsItfImpl_IsImageEffectEnabled");
        return ret;
    }

    ret = XAImageEffectsItfAdapt_IsImageEffectEnabled(impl->adapCtx, imageEffectID,
                                                      pEnabled);

    XAImageEffectsItfAdapt_ThreadExit(impl->adapCtx);
#endif
    DEBUG_API("<-XAImageEffectsItfImpl_IsImageEffectEnabled");
    return ret;
}

/**
 * XAImageEffectsItfImpl -specific methods
 **/
#ifdef _GSTREAMER_BACKEND_  

/**
 * XAImageEffectsItfImplImpl* XAImageEffectsItfImpl_Create()
 * @return  XAImageEffectsItfImplImpl* - Pointer to  ImageEffectsItf interface implementation
 **/
XAImageEffectsItfImpl* XAImageEffectsItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
{
    XAImageEffectsItfImpl* self = (XAImageEffectsItfImpl*)
        calloc(1,sizeof(XAImageEffectsItfImpl));
    DEBUG_API("->XAImageEffectsItfImpl_Create");
    if(self)
    {
        /* init itf default implementation */
        self->itf.DisableImageEffect = XAImageEffectsItfImpl_DisableImageEffect;
        self->itf.EnableImageEffect = XAImageEffectsItfImpl_EnableImageEffect;
        self->itf.IsImageEffectEnabled = XAImageEffectsItfImpl_IsImageEffectEnabled;
        self->itf.QuerySupportedImageEffects = XAImageEffectsItfImpl_QuerySupportedImageEffects;

        /* init variables */
        self->enabled = XA_BOOLEAN_FALSE;
        self->index = 0;
        self->imageEffectID = NO_IMAGE_EFFECTS;
        self->adapCtx = adapCtx;

        self->self = self;
    }
    DEBUG_API("<-XAImageEffectsItfImpl_Create");
    return self;
}
#endif
/**
 * void XAImageEffectsItfImpl_Free(XAImageEffectsItfImpl* self)
 * @param  XAImageEffectsItfImpl* self -
 **/
void XAImageEffectsItfImpl_Free(XAImageEffectsItfImpl* self)
{
    DEBUG_API("->XAImageEffectsItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAImageEffectsItfImpl_Free");
}
