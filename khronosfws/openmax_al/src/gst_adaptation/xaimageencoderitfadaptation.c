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

#include "xamediarecorderadaptctx.h"
#include "xaimageencoderitfadaptation.h"
#include "xacapabilitiesmgr.h"


/*
 * XAresult XAImageEncoderItfAdapt_SetImageSettings( XAAdaptationGstCtx *bCtx,
 *                                                   XAImageSettings *pSettings )
 * @param XAAdaptationGstCtx *ctx - pointer to Media Recorer adaptation context
 * @param XAImageSettings *pSettings - Settings for encoder
 * @return XAresult ret - return success value
 * Description: Sets preferred encoder settings for pipeline.
 */
XAresult XAImageEncoderItfAdapt_SetImageSettings( XAAdaptationGstCtx *bCtx,
                                                  const XAImageSettings *pSettings )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XACapabilities temp;

    DEBUG_API("->XAImageEncoderAdapt_SetImageSettings");

    if( !bCtx || (bCtx->baseObj.ctxId != XAMediaRecorderAdaptation) || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    if(XACapabilitiesMgr_GetCapsById(NULL, (XACapsType)(XACAP_ENCODER|XACAP_IMAGE), mCtx->imageEncSettings.encoderId, &temp) != XA_RESULT_SUCCESS)
    {   /* no such codec */
        return XA_RESULT_FEATURE_UNSUPPORTED;
    }
    /*Just copy - image settings are applied in InitiateSnapshot*/
    memcpy(&mCtx->imageEncSettings, pSettings, sizeof(XAImageSettings));

    DEBUG_API("<-XAImageEncoderAdapt_SetImageSettings");
    return ret;
}

XAresult XAImageEncoderItfAdapt_GetImageSettings( XAAdaptationGstCtx *bCtx,
                                                  XAImageSettings *pSettings )
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAImageEncoderItfAdapt_GetImageSettings");
    if( !bCtx || (bCtx->baseObj.ctxId != XAMediaRecorderAdaptation) || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    memcpy(pSettings, &(mCtx->imageEncSettings), sizeof(XAImageSettings));
    DEBUG_API("<-XAImageEncoderItfAdapt_GetImageSettings");
    return ret;
}
