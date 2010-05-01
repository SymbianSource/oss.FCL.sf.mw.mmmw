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
#include "xaequalizeritf.h"

#include "xaequalizeritfadaptation.h"

static const XAuint16 equalizerNumOfPresets = 0;

/**
 * XAEqualizerItfImpl* GetImpl(XAEqualizerItf self)
 * Description: Validated interface pointer and cast it to implementations pointer.
 **/
static XAEqualizerItfImpl* GetImpl(XAEqualizerItf self)
{
    if(self)
    {
        XAEqualizerItfImpl *impl = (XAEqualizerItfImpl*)(*self);
        if(impl && impl == impl->self)
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAEqualizerItf implementation
 */

/**
 * XAresult XAEqualizerItfImpl_SetEnabled(XAEqualizerItf self,XAboolean enabled)
 * Description: Enables the effect.
 **/
XAresult XAEqualizerItfImpl_SetEnabled(XAEqualizerItf self,XAboolean enabled)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAEqualizerItfImpl* impl = GetImpl(self);
    XAuint16 index = 0;
    DEBUG_API("->XAEqualizerItfImpl_SetEnabled");

    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_SetEnabled");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }


    if(enabled && !(impl->enabled))
    {
        for(index = 0; index < EQUALIZER_NUM_OF_BANDS; index++)
        {
            if(impl->changeLevel[index])
            {
                ret = XAEqualizerItfAdapt_SetBandLevel((XAAdaptationGstCtx*)impl->adapCtx, index, impl->levels[index]);

                if(XA_RESULT_SUCCESS == ret)
                {
                    impl->changeLevel[index] = XA_BOOLEAN_FALSE;
                }
            }
        }
    }
    else if(!enabled && impl->enabled)
    {
        for(index = 0; index < EQUALIZER_NUM_OF_BANDS; index++)
        {
            ret = XAEqualizerItfAdapt_SetBandLevel((XAAdaptationGstCtx*)impl->adapCtx, index, EQUALIZER_DEFAULT_BAND_LEVEL);

            if(XA_RESULT_SUCCESS == ret)
            {
                impl->changeLevel[index] = XA_BOOLEAN_FALSE;
            }
        }
    }
    else
    {
        /* do nothing */
    }

    if(ret == XA_RESULT_SUCCESS)
    {
        impl->enabled = enabled;
    }

    DEBUG_API("<-XAEqualizerItfImpl_SetEnabled");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_IsEnabled(XAEqualizerItf self, XAboolean *pEnabled)
 * Description: Gets the enabled status of the effect.
 **/
XAresult XAEqualizerItfImpl_IsEnabled(XAEqualizerItf self, XAboolean *pEnabled)
{
    XAresult ret = XA_RESULT_SUCCESS;

    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_IsEnabled");

    if(!impl || !pEnabled)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_IsEnabled");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pEnabled = impl->enabled;

    DEBUG_API("<-XAEqualizerItfImpl_IsEnabled");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetNumberOfBands(XAEqualizerItf self,
 *                                              XAuint16 *pNumBands)
 * Description: Gets the number of frequency bands that the equalizer supports.
 * A valid equalizer must have at least two bands.
 **/
XAresult XAEqualizerItfImpl_GetNumberOfBands(XAEqualizerItf self,
                                             XAuint16 *pNumBands)
{
    XAresult ret = XA_RESULT_SUCCESS;

    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetNumberOfBands");

    if(!impl || !pNumBands)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetNumberOfBands");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pNumBands = EQUALIZER_NUM_OF_BANDS;

    DEBUG_API("<-XAEqualizerItfImpl_GetNumberOfBands");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetBandLevelRange(XAEqualizerItf self,
 *                                               XAmillibel *pMin,
 *                                               XAmillibel *pMax)
 * Description: Returns the minimun and maximun band levels supported.
 **/
XAresult XAEqualizerItfImpl_GetBandLevelRange(XAEqualizerItf self,
                                              XAmillibel *pMin,
                                              XAmillibel *pMax)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetBandLevelRange");

    if(!impl || (!pMin && !pMax)) /* other may be NULL */
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetBandLevelRange");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAEqualizerItfImpl_GetBandLevelRange");
        return ret;
    }
    ret = XAEqualizerItfAdapt_GetBandLevelRange((XAAdaptationGstCtx*)impl->adapCtx, pMin, pMax);
    XAAdaptationBase_ThreadExit(impl->adapCtx);

    DEBUG_API("<-XAEqualizerItfImpl_GetBandLevelRange");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_SetBandLevel(XAEqualizerItf self, XAuint16 band,
 *                                          XAmillibel level)
 * Description: Sets the given equalizer band to the given gain value.
 **/
XAresult XAEqualizerItfImpl_SetBandLevel(XAEqualizerItf self, XAuint16 band,
                                         XAmillibel level)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint16 numOfBands = 0;
    XAmillibel minLevel = 0;
    XAmillibel maxLevel = 0;
    XAEqualizerItfImpl* impl = NULL;

    DEBUG_API("->XAEqualizerItfImpl_SetBandLevel");
    impl = GetImpl(self);

    /* Get number of bands */
    if(XA_RESULT_SUCCESS != XAEqualizerItfImpl_GetNumberOfBands(self, &numOfBands))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_SetBandLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Get minimum and maximum level */
    if(XA_RESULT_SUCCESS != XAEqualizerItfImpl_GetBandLevelRange(self, &minLevel, &maxLevel))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_SetBandLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(!impl || band > (numOfBands-1) || level < minLevel || level > maxLevel)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_SetBandLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
 
    ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID  || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAEqualizerItfImpl_SetBandLevel");
        return ret;
    }

    if(impl->enabled)
    {
        ret = XAEqualizerItfAdapt_SetBandLevel((XAAdaptationGstCtx*)impl->adapCtx, band, level);
        if(XA_RESULT_SUCCESS == ret)
        {
            impl->levels[band] = level;
        }
    }
    else
    {
        impl->changeLevel[band] = XA_BOOLEAN_TRUE;
        impl->levels[band] = level;
    }

    XAAdaptationBase_ThreadExit(impl->adapCtx);

    DEBUG_API("<-XAEqualizerItfImpl_SetBandLevel");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetBandLevel(XAEqualizerItf self, XAuint16 band,
 *                                          XAmillibel *pLevel)
 * Description: Gets the gain set for the given equalizer band.
 **/
XAresult XAEqualizerItfImpl_GetBandLevel(XAEqualizerItf self, XAuint16 band,
                                         XAmillibel *pLevel)
{
    XAresult ret = XA_RESULT_SUCCESS;
    
    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetBandLevel");

    if(!impl || !pLevel ||  band >= EQUALIZER_NUM_OF_BANDS)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetBandLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pLevel = impl->levels[band];
    DEBUG_API("<-XAEqualizerItfImpl_GetBandLevel");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetCenterFreq(XAEqualizerItf self, XAuint16 band,
 *                                           XAmilliHertz *pCenter)
 * Description: Gets the center frequency of the given band.
 **/
XAresult XAEqualizerItfImpl_GetCenterFreq(XAEqualizerItf self, XAuint16 band,
                                          XAmilliHertz *pCenter)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetCenterFreq");

    if(!impl || !pCenter)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetCenterFreq");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAEqualizerItfImpl_GetCenterFreq");
        return ret;
    }
    ret = XAEqualizerItfAdapt_GetCenterFreq((XAAdaptationGstCtx*)impl->adapCtx, band, pCenter);

    XAAdaptationBase_ThreadExit(impl->adapCtx);

    DEBUG_API("<-XAEqualizerItfImpl_GetCenterFreq");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetBandFreqRange(XAEqualizerItf self, XAuint16 band,
 *                                              XAmilliHertz *pMin,
 *                                              XAmilliHertz *pMax)
 * Description: Gets the frequency range of the given frequency band.
 **/
XAresult XAEqualizerItfImpl_GetBandFreqRange(XAEqualizerItf self, XAuint16 band,
                                             XAmilliHertz *pMin,
                                             XAmilliHertz *pMax)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint16 numOfBands = 0;
    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetBandFreqRange");

    /* Get number of bands */
    if(XA_RESULT_SUCCESS != XAEqualizerItfImpl_GetNumberOfBands(self, &numOfBands))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetBandFreqRange");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(!impl || (!pMin && !pMax) || band > numOfBands) /* pMin or pMax may be NULL */
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetBandFreqRange");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

 
    ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAEqualizerItfImpl_GetBandFreqRange");
        return ret;
    }
    ret = XAEqualizerItfAdapt_GetBandFreqRange((XAAdaptationGstCtx*)impl->adapCtx, band, pMin, pMax);

    XAAdaptationBase_ThreadExit(impl->adapCtx);
  
    DEBUG_API("<-XAEqualizerItfImpl_GetBandFreqRange");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetBand(XAEqualizerItf self, XAmilliHertz frequency,
 *                                     XAuint16 *pBand)
 *
 * Description: Gets the band that has the most effect on the given frequency.
 * If no band has an effect on the given frequency, XA_EQUALIZER_UNDEFINED is returned.
 **/
XAresult XAEqualizerItfImpl_GetBand(XAEqualizerItf self, XAmilliHertz frequency,
                                    XAuint16 *pBand)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetBand");

    if(!impl || !pBand)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetBand");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
  
    ret = XAAdaptationBase_ThreadEntry(impl->adapCtx);
    if( ret == XA_RESULT_PARAMETER_INVALID || ret == XA_RESULT_PRECONDITIONS_VIOLATED )
    {
        DEBUG_API("<-XAEqualizerItfImpl_GetBand");
        return ret;
    }
    ret = XAEqualizerItfAdapt_GetBand((XAAdaptationGstCtx*)impl->adapCtx, frequency, pBand);

    XAAdaptationBase_ThreadExit(impl->adapCtx);

    DEBUG_API("<-XAEqualizerItfImpl_GetBand");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetCurrentPreset(XAEqualizerItf self,
 *                                              XAuint16 *pPreset)
 * Description: Gets the current preset.
 **/
XAresult XAEqualizerItfImpl_GetCurrentPreset(XAEqualizerItf self,
                                             XAuint16 *pPreset)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetCurrentPreset");

    if(!impl || !pPreset)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetCurrentPreset");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* no presets defined */

    if(ret == XA_RESULT_SUCCESS)
    {
        *pPreset = impl->preset;
    }

    DEBUG_API("<-XAEqualizerItfImpl_GetCurrentPreset");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_UsePreset(XAEqualizerItf self, XAuint16 index)
 * Description: Sets the equalizer according to the given preset
 **/
XAresult XAEqualizerItfImpl_UsePreset(XAEqualizerItf self, XAuint16 index)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint16 numOfPresets = 0;
    XAEqualizerItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAEqualizerItfImpl_UsePreset");

    /* get number of presets */
    if( XA_RESULT_SUCCESS != XAEqualizerItfImpl_GetNumberOfPresets(self, &numOfPresets))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_UsePreset");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(!impl || index >= numOfPresets )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_UsePreset");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->preset = index;

    DEBUG_API("<-XAEqualizerItfImpl_UsePreset");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetNumberOfPresets(XAEqualizerItf self,
 *                                                XAuint16 *pNumPresets)
 * Description: Gets the total number of presets the equalizer supports.
 **/
XAresult XAEqualizerItfImpl_GetNumberOfPresets(XAEqualizerItf self,
                                               XAuint16 *pNumPresets)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAEqualizerItfImpl* impl = GetImpl(self);
    DEBUG_API("->XAEqualizerItfImpl_GetNumberOfPresets");

    if(!impl || !pNumPresets)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetNumberOfPresets");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* No presets defined. */
    *pNumPresets = equalizerNumOfPresets;

    DEBUG_API("<-XAEqualizerItfImpl_GetNumberOfPresets");
    return ret;
}

/**
 * XAresult XAEqualizerItfImpl_GetPresetName(XAEqualizerItf self, XAuint16 index,
 *                                           const XAchar **ppName)
 * Description: Gets the preset name based on the index.
 **/
XAresult XAEqualizerItfImpl_GetPresetName(XAEqualizerItf self, XAuint16 index,
                                          const XAchar **ppName)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint16 numOfPresets = 0;
    XAEqualizerItfImpl* impl = GetImpl(self);

    DEBUG_API("->XAEqualizerItfImpl_GetPresetName");

    /* get number of presets */
    if( XA_RESULT_SUCCESS != XAEqualizerItfImpl_GetNumberOfPresets(self, &numOfPresets))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetPresetName");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(!impl || !ppName || index > (numOfPresets-1))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEqualizerItfImpl_GetPresetName");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Implementation placeholder here for presets when defined.
       Currently always return XA_RESULT_PARAMETER_INVALID */

    DEBUG_API("<-XAEqualizerItfImpl_GetPresetName");
    return ret;
}

/**
 * XAEqualizerItfImpl -specific methods
 **/
 

/**
 * XAEqualizerItfImplImpl* XAEqualizerItfImpl_Create()
 * @return  XAEqualizerItfImplImpl* - Pointer to  EqualizerItf interface implementation
 **/
XAEqualizerItfImpl* XAEqualizerItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
{
    XAuint16 index = 0;

    XAEqualizerItfImpl *self = (XAEqualizerItfImpl*)
        calloc(1,sizeof(XAEqualizerItfImpl));

    DEBUG_API("->XAEqualizerItfImpl_Create");

    if(self)
    {
        /* init itf default implementation */
        self->itf.GetBand = XAEqualizerItfImpl_GetBand;
        self->itf.GetBandFreqRange = XAEqualizerItfImpl_GetBandFreqRange;
        self->itf.GetBandLevel = XAEqualizerItfImpl_GetBandLevel;
        self->itf.GetBandLevelRange = XAEqualizerItfImpl_GetBandLevelRange;
        self->itf.GetCenterFreq = XAEqualizerItfImpl_GetCenterFreq;
        self->itf.GetCurrentPreset = XAEqualizerItfImpl_GetCurrentPreset;
        self->itf.GetNumberOfBands = XAEqualizerItfImpl_GetNumberOfBands;
        self->itf.GetNumberOfPresets = XAEqualizerItfImpl_GetNumberOfPresets;
        self->itf.GetPresetName = XAEqualizerItfImpl_GetPresetName;
        self->itf.IsEnabled = XAEqualizerItfImpl_IsEnabled;
        self->itf.SetBandLevel = XAEqualizerItfImpl_SetBandLevel;
        self->itf.SetEnabled = XAEqualizerItfImpl_SetEnabled;
        self->itf.UsePreset = XAEqualizerItfImpl_UsePreset;

        /* init variables */
        self->enabled = XA_BOOLEAN_FALSE;
        self->preset = XA_EQUALIZER_UNDEFINED;

        for(index = 0; index < EQUALIZER_NUM_OF_BANDS; index++)
        {
            self->levels[index] = EQUALIZER_DEFAULT_BAND_LEVEL;
            self->changeLevel[index] = XA_BOOLEAN_FALSE;
        }

        self->adapCtx = adapCtx;

        self->self = self;
    }

    DEBUG_API("<-XAEqualizerItfImpl_Create");
    return self;
}

/**
 * void XAEqualizerItfImpl_Free(XAEqualizerItfImpl* self)
 * @param  XAEqualizerItfImpl* self -
 **/
void XAEqualizerItfImpl_Free(XAEqualizerItfImpl* self)
{
    DEBUG_API("->XAEqualizerItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAEqualizerItfImpl_Free");
}
