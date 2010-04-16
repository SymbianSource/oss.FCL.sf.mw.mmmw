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

#include <gst.h>
#include "XAMediaRecorderAdaptCtx.h"
#include "XAImageEncoderItfAdaptation.h"
#include "XAAdaptation.h"
#include "XAStaticCapsAdaptation.h"

/*
 * XAresult XAImageEncoderItfAdapt_SetImageSettings( XAAdaptationBaseCtx *bCtx,
 *                                                   XAImageSettings *pSettings )
 * @param XAAdaptationBaseCtx *ctx - pointer to Media Recorer adaptation context
 * @param XAImageSettings *pSettings - Settings for encoder
 * @return XAresult ret - return success value
 * Description: Sets preferred encoder settings for pipeline.
 */
XAresult XAImageEncoderItfAdapt_SetImageSettings( XAAdaptationBaseCtx *bCtx,
                                                  const XAImageSettings *pSettings )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XAStaticCapsData temp;

    DEBUG_API("->XAImageEncoderAdapt_SetImageSettings");

    if( !bCtx || (bCtx->ctxId != XAMediaRecorderAdaptation) || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    if(XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_IMAGE, mCtx->imageEncSettings.encoderId, &temp) != XA_RESULT_SUCCESS)
    {   /* no such codec */
        return XA_RESULT_FEATURE_UNSUPPORTED;
    }
    /*Just copy - image settings are applied in InitiateSnapshot*/
    memcpy(&mCtx->imageEncSettings, pSettings, sizeof(XAImageSettings));

    DEBUG_API("<-XAImageEncoderAdapt_SetImageSettings");
    return ret;
}

XAresult XAImageEncoderItfAdapt_GetImageSettings( XAAdaptationBaseCtx *bCtx,
                                                  XAImageSettings *pSettings )
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAImageEncoderItfAdapt_GetImageSettings");
    if( !bCtx || (bCtx->ctxId != XAMediaRecorderAdaptation) || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    memcpy(pSettings, &(mCtx->imageEncSettings), sizeof(XAImageSettings));
    DEBUG_API("<-XAImageEncoderItfAdapt_GetImageSettings");
    return ret;
}
