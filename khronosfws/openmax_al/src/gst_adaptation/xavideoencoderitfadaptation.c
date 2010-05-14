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
#include "xamediarecorderadaptctx.h"
#include "xavideoencoderitfadaptation.h"
#include "xaadaptationgst.h"


/*
 * XAresult XAVideoEncoderITfAdapt_SetVideoSettings( XAAdaptationGstCtx *bCtx,
 *                                                   XAVideoSettings *pSettings )
 * @param XAAdaptationGstCtx *ctx - pointer to Media Recorer adaptation context
 * @param XAVideoSettings *pSettings - Settings for encoder
 * @return XAresult ret - return success value
 * Description: Sets preferred encoder settings for pipeline.
 */
XAresult XAVideoEncoderItfAdapt_SetVideoSettings( XAAdaptationGstCtx *bCtx,
                                                  XAVideoSettings *pSettings )
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAVideoEncoderAdapt_SetVideoSettings");
    if( !bCtx || (bCtx->baseObj.ctxId != XAMediaRecorderAdaptation) || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVideoEncoderAdapt_SetVideoSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;

    ret = XAMediaRecorderAdapt_CheckCodec(mCtx,XACAP_VIDEO,pSettings->encoderId);
    if(ret==XA_RESULT_SUCCESS)
    {
        /*change of settings - pipeline must be regenerated**/
        mCtx->encodingchanged = XA_BOOLEAN_TRUE;
        memcpy(&mCtx->videoEncSettings, pSettings, sizeof(XAVideoSettings));
    }
    DEBUG_API("<-XAVideoEncoderAdapt_SetVideoSettings");
    return ret;
}

/*
 * XAresult XAVideoEncoderItfAdapt_GetVideoSettings( XAAdaptationGstCtx *bCtx, XAVideoSettings *pSettings )
 */
XAresult XAVideoEncoderItfAdapt_GetVideoSettings( XAAdaptationGstCtx *bCtx,
                                                  XAVideoSettings *pSettings )
{
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAVideoEncoderItfAdapt_GetVideoSettings");

    if( !bCtx || (bCtx->baseObj.ctxId != XAMediaRecorderAdaptation) || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVideoEncoderItfAdapt_GetVideoSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    memcpy(pSettings, &(mCtx->videoEncSettings), sizeof(XAVideoSettings));
    DEBUG_API("<-XAVideoEncoderItfAdapt_GetVideoSettings");
    return ret;
}
