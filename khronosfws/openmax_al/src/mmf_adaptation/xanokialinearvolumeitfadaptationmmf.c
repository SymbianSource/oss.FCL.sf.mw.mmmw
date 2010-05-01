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


//#include <mixer.h>
#include "xaadaptationmmf.h"
#include "xanokialinearvolumeitfadaptationmmf.h"
#include "xamediaplayeradaptctxmmf.h"
#include "cmmfbackendengine.h"
/*
 * XAresult XANokiaLinearVolumeItfAdapt_SetVolumeLevel(void *ctx, AdaptationContextIDS ctx->baseObj.ctxId,
 *                                          XAuint32 percentage)
 * @param void *ctx - Adaptation context, this will be casted to correct type regarding to contextID
 * XAuint32 percentage - Requested volume level, in between MIN_VOLUME_LEVEL and MAX_VOLUME_LEVEL
 * @return XAresult ret - Success value
 */
XAresult XANokiaLinearVolumeItfAdapt_SetVolumeLevel(XAAdaptationMMFCtx *ctx, XAuint32 percentage)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 maxvol;
    XAuint32 vol;
    DEBUG_API_A1("->XANokiaLinearVolumeItfAdapt_SetVolumeLevel to: %d", percentage);
    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation &&
                 ctx->baseObj.ctxId != XAMediaRecorderAdaptation &&
                 ctx->baseObj.ctxId != XARadioAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XANokiaLinearVolumeItfAdapt_SetVolumeLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    
    if(ctx->baseObj.ctxId == XAMediaPlayerAdaptation)
        {
        mmf_volumeitf_get_maxvolume(((XAMediaPlayerAdaptationMMFCtx*)ctx)->mmfContext,
                                        &maxvol); 
        
        vol = (percentage * maxvol) / MAX_PERCENTAGE_VOLUME;
        
        mmf_volumeitf_set_volume(((XAMediaPlayerAdaptationMMFCtx*)ctx)->mmfContext,
                                    vol);
        }
    DEBUG_API("<-XANokiaLinearVolumeItfAdapt_SetVolumeLevel");
    return res;
}

XAresult XANokiaLinearVolumeItfAdapt_GetStepCount(XAAdaptationMMFCtx *ctx,
                                                    XAuint32* stepcount)
    {
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API_A1("->XANokiaLinearVolumeItfAdapt_SetVolumeLevel to: %d",
                                                            percentage);
    if(!ctx || ( ctx->baseObj.ctxId != XAMediaPlayerAdaptation &&
                 ctx->baseObj.ctxId != XAMediaRecorderAdaptation &&
                 ctx->baseObj.ctxId != XARadioAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XANokiaLinearVolumeItfAdapt_SetVolumeLevel");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(ctx->baseObj.ctxId == XAMediaPlayerAdaptation)
        {
        mmf_volumeitf_get_maxvolume(((XAMediaPlayerAdaptationMMFCtx*)ctx)->mmfContext,
                                        stepcount);
        }
    
    DEBUG_API("<-XANokiaLinearVolumeItfAdapt_SetVolumeLevel");
    return res;    
    }

void XANokiaLinearVolumeItfAdapt_VolumeChange(XAAdaptationBaseCtx *ctx, TInt aVolume)
{
   	XAAdaptEvent event = {XA_NOKIALINEARVOLITFEVENTS, XA_ADAPT_VOLUME_VOLUME_CHANGED,1,0};    	
    if (ctx)
 		{
 			event.data = &aVolume;    
 			XAAdaptationBase_SendAdaptEvents(ctx, &event );
   	}   
}
