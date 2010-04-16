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
#include "xaimagecontrolsitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAImageControlsItfAdaptation.h"
#endif
/**
 * XAImageControlsItfImpl* GetImpl(XAVolumeItf self)
 * Description: Validated interface pointer and cast it to implementations pointer.
 **/
static XAImageControlsItfImpl* GetImpl(XAImageControlsItf self)
{
    if(self)
    {
        XAImageControlsItfImpl* impl = (XAImageControlsItfImpl*)(*self);
        if(impl && (impl == impl->self))
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAImageControlsItf implementation
 */

/**
 * XAresult XAImageControlsItfImpl_SetBrightness(XAImageControlsItf self,
 *                                               XAuint32 brightness)
 * Description: Sets the brightness level.
 **/
XAresult XAImageControlsItfImpl_SetBrightness(XAImageControlsItf self,
                                              XAuint32 brightness)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageControlsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageControlsItfImpl_SetBrightness");

    if(!impl || brightness > MAX_BRIGHTNESS_VALUE)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfImpl_SetBrightness");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageControlsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAImageControlsItfImpl_SetBrightness");
        return ret;
    }
    ret = XAImageControlsItfAdapt_SetBrightness(impl->adapCtx, brightness);

    if(ret == XA_RESULT_SUCCESS)
    {
        impl->brightness = brightness;
    }

    XAImageControlsItfAdapt_ThreadExit(impl->adapCtx);
#endif
    DEBUG_API("<-XAImageControlsItfImpl_SetBrightness");
    return ret;
}

/**
 * XAresult XAImageControlsItfImpl_SetBrightness(XAImageControlsItf self,
 *                                               XAuint32 *pBrightness)
 * Description: Gets the current brightness level.
 **/
XAresult XAImageControlsItfImpl_GetBrightness(XAImageControlsItf self,
                                              XAuint32 *pBrightness)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageControlsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageControlsItfImpl_GetBrightness");

    if(!impl || !pBrightness)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfImpl_GetBrightness");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pBrightness = impl->brightness;

    DEBUG_API("<-XAImageControlsItfImpl_GetBrightness");
    return ret;
}

/**
 * XAresult XAImageControlsItfImpl_SetContrast(XAImageControlsItf self,
 *                                             XAint32 contrast)
 * Description: Sets the contrast level.
 **/
XAresult XAImageControlsItfImpl_SetContrast(XAImageControlsItf self,
                                            XAint32 contrast)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageControlsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageControlsItfImpl_SetContrast");

    if(!impl || (contrast < MIN_CONTRAST_VALUE || contrast > MAX_CONTRAST_VALUE))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfImpl_SetContrast");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageControlsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED)
    {
        DEBUG_API("<-XAImageControlsItfImpl_SetContrast");
        return ret;
    }
    ret = XAImageControlsItfAdapt_SetContrast(impl->adapCtx, contrast);

    if(ret == XA_RESULT_SUCCESS)
    {
        impl->contrast = contrast;
    }

    XAImageControlsItfAdapt_ThreadExit(impl->adapCtx);
#endif    
    DEBUG_API("<-XAImageControlsItfImpl_SetContrast");
    return ret;
}

/**
 * XAresult XAImageControlsItfImpl_GetContrast(XAImageControlsItf self,
 *                                             XAint32 *pContrast)
 * Description: Gets the contrast level.
 **/
XAresult XAImageControlsItfImpl_GetContrast(XAImageControlsItf self,
                                            XAint32 *pContrast)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageControlsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageControlsItfImpl_GetContrast");

    if(!impl || !pContrast)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfImpl_GetContrast");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pContrast = impl->contrast;

    DEBUG_API("<-XAImageControlsItfImpl_GetContrast");
    return ret;
}

/**
 * XAresult XAImageControlsItfImpl_SetGamma(XAImageControlsItf self,
 *                                          XApermille gamma)
 * Description: Sets the gamma level.
 **/
XAresult XAImageControlsItfImpl_SetGamma(XAImageControlsItf self,
                                         XApermille gamma)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageControlsItfImpl* impl = GetImpl(self);
    XApermille  minGammaValue = 0;
    XApermille  maxGammaValue = 0;
    XAuint32    numSettings = 0;
    DEBUG_API("->XAImageControlsItfImpl_SetGamma");

    /* check supported min and max gamma levels */
    ret = XAImageControlsItfImpl_GetSupportedGammaSettings(self, &minGammaValue,
                                                           &maxGammaValue, &numSettings, NULL);

    if(!impl || (gamma < minGammaValue || gamma > maxGammaValue) ||
       ret != XA_RESULT_SUCCESS)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfImpl_SetGamma");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageControlsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAImageControlsItfImpl_SetGamma");
        return ret;
    }
    ret = XAImageControlsItfAdapt_SetGamma(impl->adapCtx, gamma);

    if(ret == XA_RESULT_SUCCESS)
    {
        impl->gamma = gamma;
    }

    XAImageControlsItfAdapt_ThreadExit(impl->adapCtx);
#endif
    DEBUG_API("<-XAImageControlsItfImpl_SetGamma");
    return ret;
}

/**
 * XAresult XAImageControlsItfImpl_GetGamma(XAImageControlsItf self,
 *                                          XApermille *pGamma)
 * Description: Gets the gamma level.
 **/
XAresult XAImageControlsItfImpl_GetGamma(XAImageControlsItf self,
                                         XApermille *pGamma)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageControlsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageControlsItfImpl_GetGamma");

    if(!impl || !pGamma)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfImpl_GetGamma");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pGamma = impl->gamma;

    DEBUG_API("<-XAImageControlsItfImpl_GetGamma");
    return ret;
}

/**
 * XAresult XAImageControlsItfImpl_GetSupportedGammaSettings(XAImageControlsItf self,
 *                                                           XApermille *pMinValue,
 *                                                           XApermille *pMaxValue,
 *                                                           XAuint32 *pNumSettings,
 *                                                           XApermille **ppSettings)
 * Description: This method gets the supported gamma settings.
 **/
XAresult XAImageControlsItfImpl_GetSupportedGammaSettings(XAImageControlsItf self,
                                                          XApermille *pMinValue,
                                                          XApermille *pMaxValue,
                                                          XAuint32 *pNumSettings,
                                                          XApermille **ppSettings)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAImageControlsItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAImageControlsItfImpl_GetSupportedGammaSettings");

    if(!impl || !pMinValue || !pMaxValue || !pNumSettings)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XAImageControlsItfImpl_GetSupportedGammaSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }

#ifdef _GSTREAMER_BACKEND_  
    ret = XAImageControlsItfAdapt_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAImageControlsItfImpl_GetSupportedGammaSettings");
        return ret;
    }
    /* if ppSettings is non-null and non-continuos from min and max gamma value is
     * supported then pnunSettings is length of the ppSettings array. Else
     * pNumSettings returns the number of supported gamma settings. */
    if( ppSettings && pNumSettings != 0 )
    {
        /* solve array of supported gamma settings */
        ret = XAImageControlsItfAdapt_GetSupportedGammaSettings(impl->adapCtx,
                                                                pMinValue, pMaxValue,
                                                                pNumSettings, ppSettings);
    }
    else
    {
        /* Solve min and max values and numSettings */
        ret = XAImageControlsItfAdapt_GetSupportedGammaSettings(impl->adapCtx,
                                                                pMinValue, pMaxValue,
                                                                pNumSettings, NULL);
    }

    XAImageControlsItfAdapt_ThreadExit(impl->adapCtx);
#endif
    DEBUG_API("<-XAImageControlsItfImpl_GetSupportedGammaSettings");
    return ret;
}

/**
 * XAImageControlsItfImpl -specific methods
 **/
#ifdef _GSTREAMER_BACKEND_  

/**
 * XAImageControlsItfImplImpl* XAImageControlsItfImpl_Create()
 * @return  XAImageControlsItfImplImpl* - Pointer to  ImageControlsItf interface implementation
 **/
XAImageControlsItfImpl* XAImageControlsItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
{
    XAImageControlsItfImpl* self = (XAImageControlsItfImpl*)
        calloc(1,sizeof(XAImageControlsItfImpl));
    DEBUG_API("->XAImageControlsItfImpl_Create");

    if(self)
    {
        /* init itf default implementation */
        self->itf.GetBrightness = XAImageControlsItfImpl_GetBrightness;
        self->itf.GetContrast = XAImageControlsItfImpl_GetContrast;
        self->itf.GetGamma = XAImageControlsItfImpl_GetGamma;
        self->itf.GetSupportedGammaSettings = XAImageControlsItfImpl_GetSupportedGammaSettings;
        self->itf.SetBrightness = XAImageControlsItfImpl_SetBrightness;
        self->itf.SetContrast = XAImageControlsItfImpl_SetContrast;
        self->itf.SetGamma = XAImageControlsItfImpl_SetGamma;

        /* init variables */
        self->brightness = DEFAULT_BRIGHTNESS_VALUE;
        self->contrast = DEFAULT_CONTRAST_VALUE;
        self->gamma = DEFAULT_GAMMA_VALUE;

        self->adapCtx = adapCtx;

        self->self = self;
    }
    DEBUG_API("<-XAImageControlsItfImpl_Create");
    return self;
}
#endif
/**
 * void XAImageControlsItfImpl_Free(XAImageControlsItfImpl* self)
 * @param  XAImageControlsItfImpl* self -
 **/
void XAImageControlsItfImpl_Free(XAImageControlsItfImpl* self)
{
    DEBUG_API("->XAImageControlsItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAImageControlsItfImpl_Free");
}
